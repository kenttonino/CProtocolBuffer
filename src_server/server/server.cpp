#include <string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/tls_certificate_provider.h>
#include <grpc/grpc.h>
#include "../../utils_protos/gen/route.pb.h"
#include "../../utils_protos/gen/route.grpc.pb.h"
#include "../../utils_database/utils_database.hpp"

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

class RouteImplementation final : public route::Route::Service {
  public:
    explicit RouteImplementation(const std::string& database) {
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

  private:
    std::vector<route::Feature> _feature_list;
    std::mutex _mutex;
    std::vector<route::RouteNote> _received_notes;
};

void run_server(std::string& database_path) {
  std::string server_address("0.0.0.0:50051");
  RouteImplementation service(database_path);

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server -> Wait();
}
