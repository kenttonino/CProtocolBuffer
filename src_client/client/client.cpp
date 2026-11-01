#include <chrono>
#include <memory>
#include <cmath>
#include <random>
#include <string>
#include <iostream>
#include <thread>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/tls_certificate_provider.h>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include "../../utils_protos/gen/route.pb.h"
#include "../../utils_protos/gen/route.grpc.pb.h"
#include "../../utils_database/utils_database.hpp"
#include "grpcpp/client_context.h"
#include "grpcpp/support/sync_stream.h"

route::Point MakePoint(long latitude, long longitude) {
  route::Point point;
  point.set_latitude(latitude);
  point.set_longitude(longitude);
  return point;
}

class RouteClientService {
  public:
    RouteClientService(
      std::shared_ptr<grpc::Channel> channel,
      const std::string& database
    ) : _stub(route::Route::NewStub(channel)) {
      utils_database::ParseDatabase(database, &_feature_list);
    }

    void GetFeature() {
      route::Point point;
      route::Feature feature;
      point = MakePoint(409146138, -746188906);
      GetOneFeature(point, &feature);
      point = MakePoint(0, 0);
      GetOneFeature(point, &feature);
    }

    void ListFeatures() {
      route::Rectangle rectangle;
      route::Feature feature;
      grpc::ClientContext context;

      rectangle.mutable_lo()->set_latitude(400000000);
      rectangle.mutable_lo()->set_longitude(-750000000);
      rectangle.mutable_hi()->set_latitude(420000000);
      rectangle.mutable_hi()->set_longitude(-730000000);
      std::cout << "Looking for features between, 40, -75 and 42, -73" << std::endl;

      std::unique_ptr<grpc::ClientReader<route::Feature>> reader(_stub->ListFeatures(&context, rectangle));
      while (reader->Read(&feature)) {
        std::cout << "Found feature called " << feature.name() << " at "
                  << feature.location().latitude() / _kCoordinateFactor << ", "
                  << feature.location().longitude() / _kCoordinateFactor << std::endl;
      }

      grpc::Status status = reader->Finish();
      if (status.ok()) {
        std::cout << "ListFeatures rpc succeeded." << std::endl;
      } else {
        std::cout << "ListFeatures rpc failed." << std::endl;
      }
    }

    void RecordRoute() {
      route::Point point;
      route::RouteSummary stats;
      grpc::ClientContext context;
      const int kPoints = 10;
      unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

      std::default_random_engine generator(seed);
      std::uniform_int_distribution<int> feature_distribution(0, _feature_list.size() - 1);
      std::uniform_int_distribution<int> delay_distribution(500, 1500);

      std::unique_ptr<grpc::ClientWriter<route::Point>> writer(_stub->RecordRoute(&context, &stats));
      for (int i = 0; i < kPoints; i++) {
        const route::Feature& feature = _feature_list[feature_distribution(generator)];
        std::cout << "Visiting point "
                  << feature.location().latitude() / _kCoordinateFactor
                  << ", "
                  << feature.location().latitude() / _kCoordinateFactor
                  << std::endl;

        if (!writer->Write(feature.location())) {
          break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(delay_distribution(generator)));
      }

      writer->WritesDone();
      grpc::Status status = writer->Finish();
      if (status.ok()) {
        std::cout << "Finished trip with "
                  << stats.point_count()
                  << " points\n"
                  << "Passed "
                  << stats.feature_count()
                  << " feature\n"
                  << "Travelled "
                  << stats.distance()
                  << " meters\n"
                  << "It took "
                  << stats.elapsed_time()
                  << " seconds"
                  << std::endl;
      } else {
        std::cout << "RecordRoute rpc failed." << std::endl;
      }
    }

  private:
    bool GetOneFeature(
      const route::Point& point,
      route::Feature* feature
    ) {
      grpc::ClientContext context;
      grpc::Status status = _stub->GetFeature(&context, point, feature);

      if (!status.ok()) {
        std::cout << "GetFeature RPC failed." << std::endl;
        return false;
      }

      if (!feature->has_location()) {
        std::cout << "Server returns incomplete feature." << std::endl;
        return false;
      }

      if (feature->name().empty()) {
        std::cout << "Found no feature at "
                  << feature->location().latitude() / _kCoordinateFactor
                  << ", "
                  << feature->location().longitude() / _kCoordinateFactor
                  << std::endl;
      }

      return true;
    }

    const float _kCoordinateFactor = 10000000.0;
    std::unique_ptr<route::Route::Stub> _stub;
    std::vector<route::Feature> _feature_list;
};
