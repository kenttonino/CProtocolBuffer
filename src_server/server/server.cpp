#include <chrono>
#include <cmath>
#include <string>
#include <iostream>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/tls_certificate_provider.h>
#include <grpc/grpc.h>
#include "../../utils_protos/gen/route.pb.h"
#include "../../utils_protos/gen/route.grpc.pb.h"
#include "../../utils_database/utils_database.hpp"

float _convert_to_radians(float num) {
  return num * 3.1415926 / 180;
}

// ------------------------------------------------------------------------ //
// Formula is based on: http://mathforum.org/library/drmath/view/51879.html //
// ------------------------------------------------------------------------ //
float _get_distance(const route::Point& start, const route::Point& end) {
  const float kCoordinateFactor = 10000000.0;
  float lat_1 = start.latitude() / kCoordinateFactor;
  float lat_2 = end.latitude() / kCoordinateFactor;
  float lon_1 = start.longitude() / kCoordinateFactor;
  float lon_2 = end.longitude() / kCoordinateFactor;
  float lat_rad_1 = _convert_to_radians(lat_1);
  float lat_rad_2 = _convert_to_radians(lat_2);
  float delta_lat_rad = _convert_to_radians(lat_2 - lat_1);
  float delta_lon_rad = _convert_to_radians(lon_2 - lon_1);

  float a = pow(sin(delta_lat_rad / 2), 2) * cos(lat_rad_1) * cos(lat_rad_2) * pow(sin(delta_lon_rad / 2), 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  int R = 6371000;

  return R * c;
}

std::string GetFeatureName(
  const route::Point& point,
  const std::vector<route::Feature>& feature_list
) {
  for (const route::Feature& f : feature_list) {
    if (f.location().latitude() == point.latitude() && f.location().longitude() == point.longitude()) {
      return f.name();
    }
  }

  return "";
}

class RouteServerService final : public route::Route::Service {
  public:
    explicit RouteServerService(const std::string& database) {
      utils_database::ParseDatabase(database, &_feature_list);
    }

    grpc::Status GetFeature(
      grpc::ServerContext* context,
      const route::Point* point,
      route::Feature* feature
    ) override {
      feature->set_name(GetFeatureName(*point, _feature_list));
      feature->mutable_location()->CopyFrom(*point);
      return grpc::Status::OK;
    }

    grpc::Status ListFeatures(
      grpc::ServerContext* context,
      const route::Rectangle* rectangle,
      grpc::ServerWriter<route::Feature>* writer
    ) override {
      auto lo = rectangle->lo();
      auto hi = rectangle->hi();
      long left = (std::min)(lo.longitude(), hi.longitude());
      long right = (std::max)(lo.longitude(), hi.longitude());
      long top = (std::max)(lo.latitude(), hi.latitude());
      long bottom = (std::min)(lo.latitude(), hi.latitude());
      for (const route::Feature& f : _feature_list) {
        if (
            f.location().longitude() >= left &&
            f.location().longitude() <= right &&
            f.location().latitude() >= bottom &&
            f.location().latitude() <= top
        ) {
          writer->Write(f);
        }
      }

      return grpc::Status::OK;
    }

    grpc::Status RecordRoute(
      grpc::ServerContext* context,
      grpc::ServerReader<route::Point>* reader,
      route::RouteSummary* summary
    ) override {
      route::Point point;
      int point_count = 0;
      int feature_count = 0;
      float distance = 0.0;
      route::Point previous;

      std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
      while (reader->Read(&point)) {
        point_count++;

        if (!GetFeatureName(point, _feature_list).empty()) {
            feature_count++;
        }

        if (point_count != 1) {
          distance += _get_distance(previous, point);
        }

        previous = point;
      }

      std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();
      summary->set_point_count(point_count);
      summary->set_feature_count(feature_count);
      summary->set_distance(static_cast<long>(distance));
      auto secs = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
      summary->set_elapsed_time(secs.count());

      return grpc::Status::OK;
    }

    grpc::Status RouteChat(
      grpc::ServerContext* context,
      grpc::ServerReaderWriter<route::RouteNote, route::RouteNote>* stream
    ) override {
      route::RouteNote note;

      while (stream->Read(&note)) {
        std::unique_lock<std::mutex> lock(_mutex);

        for (const route::RouteNote& n : _received_notes) {
          if (
            n.location().latitude() == note.location().latitude() &&
            n.location().longitude() == note.location().longitude()
          ) {
            stream->Write(n);
          }

          _received_notes.push_back(note);
        }
      }

      return grpc::Status::OK;
    }

  private:
    std::vector<route::Feature> _feature_list;
    std::mutex _mutex;
    std::vector<route::RouteNote> _received_notes;
};

void run_server(std::string& database_path) {
  std::string server_address("0.0.0.0:50051");
  RouteServerService service(database_path);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server -> Wait();
}
