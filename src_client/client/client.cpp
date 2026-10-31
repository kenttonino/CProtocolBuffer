#include <memory>
#include <cmath>
#include <string>
#include <iostream>
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
        std::cout << "Found no feature at " << feature->location().latitude() / _kCoordinateFactor << ", " << feature->location().longitude() / _kCoordinateFactor << std::endl;
      }

      return true;
    }

    const float _kCoordinateFactor = 10000000.0;
    std::unique_ptr<route::Route::Stub> _stub;
    std::vector<route::Feature> _feature_list;
};
