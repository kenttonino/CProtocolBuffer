#include "absl/flags/parse.h"
#include "absl/log/initialize.h"
#include "../utils_database/utils_database.hpp"
#include "./client/client.cpp"
#include "./client/client.hpp"

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();

  std::string database = utils_database::get_database_file_content(argc, argv);
  run_client(database);

  return 0;
}
