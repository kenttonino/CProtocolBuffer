#include "absl/flags/parse.h"
#include <iostream>
#include "absl/log/initialize.h"
#include "../utils_database/utils_database.hpp"

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();

  std::string database = utils_database::get_database_file_content(argc, argv);

  std::cout << "Hello, from client side!";
  return 0;
}
