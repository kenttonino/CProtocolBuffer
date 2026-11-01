#include <cstdio>
#include "absl/flags/parse.h"
#include "absl/log/initialize.h"
#include "../utils_database/utils_database.hpp"
#include "./server/server.hpp"

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();

  std::string database = utils_database::get_database_file_content(argc, argv);
  run_server(database);

  return 0;
}
