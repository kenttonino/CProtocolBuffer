#include "absl/flags/parse.h"
#include <iostream>
#include "absl/log/initialize.h"
#include "../utils_database/utils_database.hpp"
#include "./client/client.cpp"
#include "grpcpp/create_channel.h"
#include "grpcpp/security/credentials.h"

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();

  std::string database = utils_database::get_database_file_content(argc, argv);
  RouteClientService client_service(
    grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()),
    database
  );

  std::cout << "-------------- GetFeature --------------" << std::endl;
  client_service.GetFeature();
  std::cout << "-------------- ListFeatures --------------" << std::endl;
  client_service.ListFeatures();
  std::cout << "-------------- RecordRoute --------------" << std::endl;
  client_service.RecordRoute();
  std::cout << "-------------- RouteChat --------------" << std::endl;
  client_service.RouteChat();

  return 0;
}
