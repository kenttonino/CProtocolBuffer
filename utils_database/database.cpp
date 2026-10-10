#include <fstream>
#include <sstream>
#include <string>
#include "absl/flags/flag.h"
#include "absl/log/log.h"
#include "./utils_database.hpp"

ABSL_FLAG(std::string, db_path, "./utils/database.json", "Path to database file.");

namespace utils_database {
  std::string get_database_file_content(int argc, char** argv) {
    std::string database_path = absl::GetFlag(FLAGS_db_path);
    std::ifstream database_file(database_path);
    if (!database_file.is_open()) {
      LOG(ERROR) << "Failed to open." << database_path;
      abort();
    }

    std::stringstream database;
    database << database_file.rdbuf();
    return database.str();
  }
}
