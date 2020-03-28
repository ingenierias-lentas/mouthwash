#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "lilv-0/lilv/lilv.h"

#include "lv2/core/lv2.h"
#include "lv2/atom/atom.h"

#ifdef __cplusplus
}
#endif

#include <math.h>
#include <sndfile.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <map>
#include <tuple>
#include <vector>

#include "MidiFile.h"

#include "bar_helper.hh"
#include "transport.hh"

namespace mouthwash {

// Class representing LV2 plugin, including connection to Lilv environment
class Ingredient {
    public:
    // Set up port structures
    Ingredient(
        LilvWorld* world,
        const LilvPlugin* plugin
    );
    ~Ingredient();
    // Set up ingredient for preparation
    void SetParams(std::map<int, std::pair<std::string, float>> params);
    // Prepare ingredient by transforming data on file at inPath to file at
    // outPath
    void Prepare(
        std::vector<IoTransportGroup> inputTransportGroups,
        std::vector<IoTransportGroup> outputTransportGroups
    );

    Param*  params_;
    Port*   ports_;

    const char*         inPath_;
    const char*         outPath_;
    unsigned            nParams_;
    unsigned            nPorts_;
    unsigned            nAudioIn_;
    unsigned            nAudioOut_;
    int                 servicePort_;

    private:
    LilvWorld*          world_;
    const LilvPlugin*   plugin_;
    LilvInstance*       instance_;
    int                 sampleRate_;

    BarHelper& barHelper_ = BarHelper::GetInstance();
};

} // mouthwash
