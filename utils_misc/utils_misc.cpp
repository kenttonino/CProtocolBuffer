#include <cmath>
#include <string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/tls_certificate_provider.h>
#include <grpc/grpc.h>
#include "../../utils_protos/gen/route.pb.h"

float convert_to_radians(float num) {
  return num * 3.1415926 / 180;
}

// ------------------------------------------------------------------------ //
// Formula is based on: http://mathforum.org/library/drmath/view/51879.html //
// ------------------------------------------------------------------------ //
float get_distance(const route::Point& start, const route::Point& end) {
  const float kCoordinateFactor = 10000000.0;
  float lat_1 = start.latitude() / kCoordinateFactor;
  float lat_2 = end.latitude() / kCoordinateFactor;
  float lon_1 = start.longitude() / kCoordinateFactor;
  float lon_2 = end.longitude() / kCoordinateFactor;
  float lat_rad_1 = convert_to_radians(lat_1);
  float lat_rad_2 = convert_to_radians(lat_2);
  float delta_lat_rad = convert_to_radians(lat_2 - lat_1);
  float delta_lon_rad = convert_to_radians(lon_2 - lon_1);

  float a = pow(sin(delta_lat_rad / 2), 2) * cos(lat_rad_1) * cos(lat_rad_2) * pow(sin(delta_lon_rad / 2), 2);
  float c = 2 * atan2(sqrt(a), sqrt(1 - a));
  int R = 6371000;

  return R * c;
}

std::string get_feature_name(
  const route::Point& point,
  const std::vector<route::Feature>& feature_list
) {
  for (const route::Feature& f : feature_list) {
    if (f.location().latitude() == point.latitude() && f.location().longitude() == point.longitude()) {
      return f.name();
    }
  }

  return "";
}

route::Point set_point(long latitude, long longitude) {
  route::Point point;
  point.set_latitude(latitude);
  point.set_longitude(longitude);
  return point;
}

route::RouteNote set_route_note(
  const std::string& message,
  long latitude,
  long longitude
) {
  route::RouteNote note;
  note.set_message(message);
  note.mutable_location()->CopyFrom(set_point(latitude, longitude));
  return note;
}

std::string time_point_to_string(
  std::chrono::system_clock::time_point tp,
  const std::string& fmt = "%Y-%m-%d %H:%M:%S"
) {
    auto t = std::chrono::system_clock::to_time_t(tp);
    std::ostringstream os_string_stream;
    os_string_stream << std::put_time(std::localtime(&t), fmt.c_str());
    return os_string_stream.str();
}
