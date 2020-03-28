#include <iostream>

#include "bartender.hh"

using namespace mouthwash;

using grpc::Channel;

Bartender::Bartender()
    : mixer_client_(MixerClient(grpc::CreateChannel("localhost:60061", grpc::InsecureChannelCredentials())))
{
        /*
    mixer_client_ = MixerClient(grpc::CreateChannel(
                "localhost:60061", grpc::InsecureChannelCredentials()));
                */
}

void
Bartender::Run() {
    //TODO thread this and have inter-thread communication queue for determining
    //when to request a new mix
    thread_group_.create_thread(std::bind(&BottleServiceImpl::Run, &bottle_service_, &barHelper_));
    thread_group_.create_thread(std::bind(&MixerClient::Run, &mixer_client_, &barHelper_));
    
    thread_group_.join_all();
}
