#pragma once

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>

#include "bar_helper.hh"

#include "mix.grpc.pb.h"
#include "health.grpc.pb.h"
#include <google/protobuf/empty.pb.h>

using grpc::Channel;
using grpc::CompletionQueue;

// RPCs: frontend <-> sound analyzation service
using mouthwash::Mixer;
using grpc::health::v1::Health;

namespace mouthwash {

class MixerClient {
    public:
    explicit MixerClient(std::shared_ptr<Channel> channel)
        : healthStub_(Health::NewStub(channel)), mixerStub_(Mixer::NewStub(channel)) {}

    void MixANewOne(const std::string& instrument);

    void HealthCheck(const std::string& service);

    void HealthWatch(const std::string& service);

    void AsyncCompleteRpc();

    void Run(BarHelper& barHelper);

    private:
    std::unique_ptr<Mixer::Stub> mixerStub_;
    std::unique_ptr<Health::Stub> healthStub_;
    CompletionQueue cq_;
};

} // mouthwash
