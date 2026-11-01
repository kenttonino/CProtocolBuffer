#ifndef CLIENT_HPP
#define CLIENT_HPP
#include <memory>
#include <cmath>
#include <string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/tls_certificate_provider.h>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include "../../utils_protos/gen/route.grpc.pb.h"
#include "grpcpp/client_context.h"

class RouteClientService {
  public:
    RouteClientService(
      std::shared_ptr<grpc::Channel> channel,
      const std::string& database
    ) : _stub(route::Route::NewStub(channel)){};

    void GetFeature() {};
    void ListFeatures() {};
    void RecordRoute() {};
    void RouteChat() {};

  private:
    std::unique_ptr<route::Route::Stub> _stub;
};

#endif
