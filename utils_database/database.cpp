#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include "absl/flags/flag.h"
#include "absl/log/log.h"
#include "./utils_database.hpp"
#include "../utils_protos/gen/route.pb.h"

ABSL_FLAG(std::string, db_path, "./utils_database/database.json", "Path to database file.");

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

  class Parser {
    bool failed_ = false;
    std::string database_;
    size_t current_ = 0;

    public:
      explicit Parser(const std::string& database) : database_(database) {
        if (!Match("[")) {
          SetFailedAndReturnFalse();
        }
      }

    private:
      bool SetFailedAndReturnFalse() {
        failed_ = true;
        return false;
      }
      bool Match(const std::string& prefix) {
        bool equal = database_.substr(current_, prefix.size()) == prefix;
        current_ += prefix.size();
        return equal;
      }
  };

  std::string MinifyJson(const std::string& json) {
    std::regex whitespace_outside_quotes(R"(\s+(?=(?:(?:[^"]*"){2})*[^"]*$))");
    return std::regex_replace(json, whitespace_outside_quotes, "");
  }

  void ParseDatabase(const std::string& database, std::vector<Feature>* feature_list) {
    feature_list -> clear();
    std::string database_content(MinifyJson(database));
  }
}
