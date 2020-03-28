#pragma once
#include <memory>

#include <boost/thread.hpp>

#include "bar_helper.hh"
#include "bar.hh"
#include "bottle_server.hh"
#include "mixer_client.hh"

namespace mouthwash {

class Bartender {
    public:
    Bartender();

    void Run();

    private:

    BarHelper& barHelper_ = BarHelper::GetInstance();
    Bar& bar_ = Bar::GetInstance();
    BottleServiceImpl bottle_service_;
    MixerClient mixer_client_;
    boost::thread_group thread_group_;

};

} // mouthwash
