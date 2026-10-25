#include <string>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpc/grpc.h>
#include "../../utils_protos/gen/route.pb.h"
#include "../../utils_database/utils_database.hpp"

class RouteImplementation {
  public:
    explicit RouteImplementation(const std::string& database) {
      utils_database::ParseDatabase(database, &_feature_list);
    }

  private:
    std::vector<route::Feature> _feature_list;
    std::mutex _mutex;
    std::vector<route::RouteNote> _received_notes;
}

void run_server(std::string& database_path) {
  std::string server_address("0.0.0.0:50051");
}
