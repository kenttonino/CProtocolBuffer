#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include "absl/flags/flag.h"
#include "absl/log/log.h"
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
    bool _failed = false;
    std::string _database;
    size_t _current = 0;
    const std::string _location = "\"location\":";
    const std::string _latitude = "\"latitude\":";
    const std::string _longitude = "\"longitude\":";
    const std::string _name = "\"name\":";

    public:
      explicit Parser(const std::string& database) : _database(database) {
        if (!Match("[")) {
          SetFailedAndReturnFalse();
        }
      }

      bool Finished() {
        return _current >= _database.size();
      }

      bool TryParseOne(route::Feature* feature) {
        if (_failed || Finished() || !Match("{")) {
          return SetFailedAndReturnFalse();
        }

        if (!Match(_location) || !Match("(") || !Match(_latitude)) {
          return SetFailedAndReturnFalse();
        }

        long temp = 0;
        ReadLong(&temp);
        feature -> mutable_location() -> set_latitude(temp);
        if (!Match(",") || !Match(_longitude)) {
          return SetFailedAndReturnFalse();
        }

        ReadLong(&temp);
        feature -> mutable_location() -> set_longitude(temp);
        if (!Match("},") || !Match(_name) || !Match("\"")) {
          return SetFailedAndReturnFalse();
        }

        size_t name_start = _current;
        while(_current != _database.size() && _database[_current++] != '"') {}

        if (_current == _database.size()) {
          return SetFailedAndReturnFalse();
        }

        feature -> set_name(_database.substr(name_start, _current - name_start - 1));
        if (!Match("},")) {
          if (_database[_current - 1] == ']' && _current == _database.size()) {
            return true;
          }

          return SetFailedAndReturnFalse();
        }

        return true;
      }

    private:
      bool SetFailedAndReturnFalse() {
        _failed = true;
        return false;
      }

      bool Match(const std::string& prefix) {
        bool equal = _database.substr(_current, prefix.size()) == prefix;
        _current += prefix.size();
        return equal;
      }

      void ReadLong(long* l) {
        size_t start = _current;
        while (_current != _database.size() && _database[_current] != ',' && _database[_current] != '}') {
          _current++;
        }

        *l = std::stol(_database.substr(start, _current - start));
      }
  };

  std::string MinifyJson(const std::string& json) {
    std::regex whitespace_outside_quotes(R"(\s+(?=(?:(?:[^"]*"){2})*[^"]*$))");
    return std::regex_replace(json, whitespace_outside_quotes, "");
  }

  void ParseDatabase(const std::string& database, std::vector<route::Feature>* feature_list) {
    feature_list -> clear();
    std::string database_content(MinifyJson(database));

    Parser parser(database_content);
    route::Feature feature;
    while (!parser.Finished()) {
      feature_list -> push_back(route::Feature());
      if (!parser.TryParseOne(&feature_list -> back())) {
        LOG(ERROR) << "Error parsing the database file";
        feature_list -> clear();
        break;
      }
    }

    LOG(INFO) << "Database parsed, loaeded " << feature_list -> size() << " features.";
  }
}
