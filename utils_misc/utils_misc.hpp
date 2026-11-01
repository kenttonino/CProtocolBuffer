#ifndef UTILS_MISC_HPP
#define UTILS_MISC_HPP
#include <cmath>
#include <string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/tls_certificate_provider.h>
#include <grpc/grpc.h>
#include "../../utils_protos/gen/route.pb.h"

// ------------------- //
// ASCII style colors. //
// ------------------- //
#define BLUE "\e[0;34m"
#define GREEN "\e[0;32m"
#define RESET "\e[0m"

// ------------------------ //
// Functions use by server. //
// ------------------------ //
float convert_to_radians(float num);
float get_distance(const route::Point& start, const route::Point& end);
std::string get_feature_name(const route::Point& point, const std::vector<route::Feature>& feature_list);

#endif
