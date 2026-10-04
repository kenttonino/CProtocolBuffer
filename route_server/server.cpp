#include <cstdio>
#include "absl/flags/parse.h"
#include "absl/log/initialize.h"
#include "../utils/utils.hpp"

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  absl::InitializeLog();

  // Expect only arg: --db_path=./utils/database.json
  std::string database = utils_database::GetDatabaseFileContent(argc, argv);

  return 0;
}
