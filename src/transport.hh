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

#include "bar_helper.hh"

namespace mouthwash {

/** Control port value set from the command line */
struct Param {
    const char*   sym; ///< Port symbol
    float         value;     ///< Control value
};

/** Port type
 *  TYPE_ATOM used for midi
 */
enum PortType { TYPE_CONTROL, TYPE_AUDIO, TYPE_ATOM };

/** Runtime port information */
struct Port {
    const LilvPort* lilvPort_; ///< Port description
    PortType        type_;             ///< Datatype
    uint32_t        index_;            ///< Port index
    float           value_;               ///< Control value (if applicable)
    bool            isInput_;             ///< True iff an input port
    bool            optional_;             ///< True iff connection optional
};

/** Port mapping and associated input/output file paths */
struct IoTransportGroup {
    public:
    std::vector<int> MappedPorts() {
        std::vector<int> ret;
        for (auto it = portBufMap_.begin(); it != portBufMap_.end(); it++) {
            ret.push_back(it->first);
        }
        return ret;
    }

    float* GetBuffer(int portNum) { 
        auto bufItr = portBufMap_.find(portNum);
        return bufItr == portBufMap_.end() ? NULL : bufItr->second;
    }

    bool ConnectToPlugin(LilvInstance* instance) {
        for (auto itr = portBufMap_.begin(); itr != portBufMap_.end(); itr++) {
            lilv_instance_connect_port(instance, itr->first, itr->second);
        }
        return true;
    }

    PortType                type_;
    bool                    isInput_;
    std::string             filePath_;
    std::map<int, float*>   portBufMap_; // map of port to port buffer
    BarHelper& barHelper_ = BarHelper::GetInstance();
};

struct InputTransportGroup : public IoTransportGroup {
    virtual bool LoadBuffers() = 0;
};

struct OutputTransportGroup : public IoTransportGroup {
    virtual bool SaveBuffers() = 0;
};

struct AudioInputTransportGroup : public InputTransportGroup {
    AudioInputTransportGroup(std::vector<int> ports) {
        for (auto it = ports.begin(); it != ports.end(); it++) {
            portBufMap_[*it] = (float*)calloc(1, sizeof(float));
        }
    }

    bool LoadBuffers(int portNum) {
        if (!file_) {
            if (!(file_ = barHelper_.Sopen(filePath_.c_str(), SFM_READ, &fmt_))) {
                barHelper_.Fatal("Could not open input file `%s`\n", filePath_.c_str());
                return false;
            }
        }
        float buf[fmt_.channels];
        if (!barHelper_.Sread(file_, fmt_.channels, buf)) {
            barHelper_.Fatal("Could not read input file `%s`\n", filePath_.c_str());
            return false;
        }

        for (auto itr = portBufMap_.begin(); itr != portBufMap_.end(); itr++) {
            *itr->second = buf[itr->first];
        }

        return true;
    }

    private:
    ~AudioInputTransportGroup() {
        for (auto it = portBufMap_.begin(); it != portBufMap_.end(); it++) {
            free(it->second);
        }
    }

    std::map<int, int>      channelPortMap_; // map of audio channel to port
    SF_INFO                 fmt_ = { 0, 0, 0, 0, 0, 0 };
    SNDFILE*                file_ = NULL;
};

struct AudioOutputTransportGroup : public OutputTransportGroup {
    AudioOutputTransportGroup(std::vector<int> ports, SF_INFO fmt)
    : fmt_(fmt)
    {
        for (auto it = ports.begin(); it != ports.end(); it++) {
            portBufMap_[*it] = (float*)calloc(1, sizeof(float));
        }
    }

    bool SaveBuffers(int portNum) {
        if (!file_) {
            if (!(file_ = barHelper_.Sopen(filePath_.c_str(), SFM_READ, &fmt_))) {
                barHelper_.Fatal("Could not open output file `%s`\n", filePath_.c_str());
                return false;
            }
        }

        float buf[fmt_.channels];
        for (auto itr = portBufMap_.begin(); itr != portBufMap_.end(); itr++) {
            buf[itr->first] = *itr->second;
        }

        if (sf_writef_float(file_, buf, 1) != 1) {
            barHelper_.Fatal("Could not write output file `%s`\n", filePath_.c_str());
            return false;
        }

        return true;
    }

    private:
    ~AudioOutputTransportGroup() {
        for (auto it = portBufMap_.begin(); it != portBufMap_.end(); it++) {
            free(it->second);
        }
    }

    std::map<int, int>      channelPortMap_; // map of audio channel to port
    SF_INFO                 fmt_ = { 0, 0, 0, 0, 0, 0 };
    SNDFILE*                file_ = NULL;

};

struct MidiInputTransportGroup : public InputTransportGroup {
    MidiInputTransportGroup(std::vector<int> ports ) {
        for (auto it = ports.begin(); it != ports.end(); it++) {
            portBufMap_[*it] = (float*)calloc(1, sizeof(float));
        }
    }

    bool LoadBuffers(int portNum) {
        return false;
    }

    private:
    ~MidiInputTransportGroup() {
        for (auto it = portBufMap_.begin(); it != portBufMap_.end(); it++) {
            free(it->second);
        }
    }

    std::map<int, float*>   portBufMap_; // map of port to port buffer
    MidiFile                file_;
};

struct MidiOutputTransportGroup : public OutputTransportGroup {
    MidiOutputTransportGroup(std::vector<int> ports ) {
        for (auto it = ports.begin(); it != ports.end(); it++) {
            portBufMap_[*it] = (float*)calloc(1, sizeof(float));
        }
    }

    bool SaveBuffers(int portNum) {
        return false;
    }

    private:
    ~MidiOutputTransportGroup() {
        for (auto it = portBufMap_.begin(); it != portBufMap_.end(); it++) {
            free(it->second);
        }
    }

    std::map<int, float*>   portBufMap_; // map of port to port buffer
    MidiFile                file_;
};

struct ControlInputTransportGroup : public InputTransportGroup {
    ControlInputTransportGroup(std::vector<int> ports ) {
        for (auto it = ports.begin(); it != ports.end(); it++) {
            portBufMap_[*it] = (float*)calloc(1, sizeof(float));
        }
    }

    bool LoadBuffers(int portNum) {
        return false;
    }

    private:
    ~ControlInputTransportGroup() {
        for (auto it = portBufMap_.begin(); it != portBufMap_.end(); it++) {
            free(it->second);
        }
    }
 
    std::map<int, float*>   portBufMap_; // map of port to port buffer
    float                   value_;
};

struct ControlOutputTransportGroup : public OutputTransportGroup {
    ControlOutputTransportGroup(std::vector<int> ports ) {
        for (auto it = ports.begin(); it != ports.end(); it++) {
            portBufMap_[*it] = (float*)calloc(1, sizeof(float));
        }
    }

    bool SaveBuffers(int portNum) {
        return false;
    }

    private:
    ~ControlOutputTransportGroup() {
        for (auto it = portBufMap_.begin(); it != portBufMap_.end(); it++) {
            free(it->second);
        }
    }
 
    std::map<int, float*>   portBufMap_; // map of port to port buffer
    float                   value_;
};

} // mouthwash
