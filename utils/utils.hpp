#ifndef UTILS_HPP
#include <string>
#include <absl/flags/declare.h>

// Defined macros.
// Macros are commonly used to define constants and create reusable code snippets.
ABSL_DECLARE_FLAG(std::string, db_path);

// Defined namespace.
namespace utils_database {
  std::string GetDatabaseFileContent(int argc, char** argv);
}

#endif
