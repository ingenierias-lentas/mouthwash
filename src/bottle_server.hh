#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include "bar_helper.hh"

#include "health.grpc.pb.h"
#include "bottle.grpc.pb.h"

using grpc::Server;
using grpc_impl::ServerAsyncResponseWriter;
using grpc_impl::ServerAsyncWriter;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;

// Advertise shot making service to client
using mouthwash::ShotCall;
using mouthwash::Shot;
using mouthwash::Bottle;
// Advertise health of bottle server to client
using grpc::health::v1::HealthCheckRequest;
using grpc::health::v1::HealthCheckResponse;
using grpc::health::v1::Health;

namespace mouthwash {

class BottleServiceImpl final {
    public:
    BottleServiceImpl(){}

    ~BottleServiceImpl();

    void Run(BarHelper& barHelper);

    private:
    void HandleRpcs(BarHelper& barHelper);

    std::vector<std::unique_ptr<ServerCompletionQueue>> cqs_;
    Health::AsyncService healthService_;
    Bottle::AsyncService bottleService_;
    std::unique_ptr<Server> server_;
};

} // mouthwash
