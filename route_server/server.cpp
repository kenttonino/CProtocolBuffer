#include <cstdio>
#include "absl/flags/parse.h"

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  printf("Hello from gRPC.");
  return 0;
}
