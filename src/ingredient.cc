#include "ingredient.hh"

using namespace mouthwash;

Ingredient::Ingredient(
    LilvWorld* world,
    const LilvPlugin* plugin,
    int sampleRate
)
    : world_(world), plugin_(plugin), sampleRate_(sampleRate)
{
    const uint32_t  n_ports = lilv_plugin_get_num_ports(plugin_);
    nPorts_ = n_ports;
    ports_ = (Port*)calloc(nPorts_, sizeof(Port));

    /* Get default values for all ports */
    float* values = (float*)calloc(nPorts_, sizeof(float));
    lilv_plugin_get_port_ranges_float(plugin_, NULL, NULL, values);

    LilvNode* lv2_InputPort             = lilv_new_uri(world_, LV2_CORE__InputPort);
    LilvNode* lv2_OutputPort            = lilv_new_uri(world_, LV2_CORE__OutputPort);
    LilvNode* lv2_AudioPort             = lilv_new_uri(world_, LV2_CORE__AudioPort);
    LilvNode* lv2_ControlPort           = lilv_new_uri(world_, LV2_CORE__ControlPort);
    LilvNode* lv2_AtomPort              = lilv_new_uri(world_, LV2_ATOM__AtomPort);
    LilvNode* lv2_connectionOptional    = lilv_new_uri(world_, LV2_CORE__connectionOptional);

    for (uint32_t i = 0; i < n_ports; ++i) {
        Port*           port    = &ports_[i];
        const LilvPort* lport   = lilv_plugin_get_port_by_index(plugin_, i);

        port->lilvPort_ = lport;
        port->index_     = i;
        port->value_     = isnan(values[i]) ? 0.0f : values[i];
        port->optional_  = lilv_port_has_property(
                plugin_, lport, lv2_connectionOptional);

        /* Check if port is an input or output */
        if (lilv_port_is_a(plugin_, lport, lv2_InputPort)) {
            port->isInput_ = true;
        } else if (!lilv_port_is_a(plugin_, lport, lv2_OutputPort) &&
                !port->optional_) {
            barHelper_.Fatal("Port %d is neither input nor output\n", i);
            break;
        }

        /* Check if port is an audio or control port */
        if (lilv_port_is_a(plugin_, lport, lv2_ControlPort)) {
            port->type_ = TYPE_CONTROL;
        } else if (lilv_port_is_a(plugin_, lport, lv2_AudioPort)) {
            port->type_ = TYPE_AUDIO;
            if (port->isInput_) {
                ++nAudioIn_;
            } else {
                ++nAudioOut_;
            }
        } else if (lilv_port_is_a(plugin_, lport, lv2_AtomPort)) {
            port->type_ = TYPE_ATOM;
            if (port->isInput_) {
                ++nAudioIn_;
            } else {
                ++nAudioOut_;
            }
        } else if (!port->optional_) {
            barHelper_.Fatal("Port %d has unsupported type\n", i);
            break;
        }
    }

    lilv_node_free(lv2_connectionOptional);
    lilv_node_free(lv2_ControlPort);
    lilv_node_free(lv2_AudioPort);
    lilv_node_free(lv2_OutputPort);
    lilv_node_free(lv2_InputPort);
    free(values);

    instance_ = lilv_plugin_instantiate(plugin_, sampleRate_, NULL);
    if (nAudioIn_ == 0) {
        barHelper_.Fatal("Plugin contains no input ports\n");
        return;
    }
}

Ingredient::~Ingredient() {
    lilv_instance_free(instance_);
    free(ports_);
}


void
Ingredient::SetParams(
    std::map<int, std::pair<std::string, float>> params
)
{
    for (int i=0; i<nParams_; i++) {
        const char* paramSymRepr = params[i].first.c_str();
        float paramValue = params[i].second;
        LilvNode* sym = lilv_new_string(world_, paramSymRepr);
        const LilvPort* port = lilv_plugin_get_port_by_symbol(plugin_, sym);
        lilv_node_free(sym);
        if (!port) {
            barHelper_.Fatal("Unknown port `%s`\n", paramSymRepr);
        }

        ports_[lilv_port_get_index(plugin_, port)].value_ = paramValue;
    }
}

void
Ingredient::Prepare(
    std::vector<IoTransportGroup> inputTransportGroups,
    std::vector<IoTransportGroup> outputTransportGroups,
    int samplerate
)
{
    auto CloseAudioFilesHelper = [] (std::vector<IoTransportGroup> grps) {
        for ( auto itr = grps.begin(); itr != grps.end(); itr++) {
            auto el = *itr;
            if (el.isOpen_ && el.type_ == TYPE_AUDIO) {
                barHelper_.Sclose(el.filePath_, el.audioFile_);
            }
        }
    };

    //TODO handle this in IoTransportGroup classes
    // 1) open files and connect ports
    // 2) read files into lilv run function and out into output files
    const uint32_t nPorts = lilv_plugin_get_num_ports(plugin_);
    std::map<int, std::vector<float>> inBuf; // inputs can be one-to-many
    std::map<int, float> outBuf; // outputs can only be one to one
    for ( auto itr = inputTransportGroups.begin(); itr != inputTransportGroups.end(); itr++) {
        itr->ConnectToPlugin(instance_, &ports_);
    }
    for ( auto itr = outputTransportGroups.begin(); itr != outputTransportGroups.end(); itr++) {
        itr->ConnectToPlugin(instance_, &ports_);
    }

    lilv_instance_activate(instance_);
    bool inputAvailable = false;
    for ( auto itr = inputTransportGroups.begin(); itr != inputTransportGroups.end(); itr++ ) {
        if (itr->LoadBuffers()) inputAvailable = true;
    }
    while(inputAvailable && barHelper_.BarIsOk()) {
        inputAvailable = false;
        for ( auto itr = inputTransportGroups.begin(); itr != inputTransportGroups.end(); itr++ ) {
            if (itr->LoadBuffers()) inputAvailable = true;
        }
        lilv_instance_run(instance_, 1);
        for ( auto itr = outputTransportGroups.begin(); itr != outputTransportGroups.end(); itr++ ) {
            if (itr->SaveBuffers()) outputAvailable = true;
        }
    }
    CloseAudioFilesHelper(inputTransportGroups);
    CloseAudioFilesHelper(outputTransportGroups);

    for ( auto itr = inputTransportGroups.begin(); itr != inputTransportGroups.end(); itr++ ) {
        auto el = *itr;
        if (el.type_ == TYPE_ATOM && el.isInput_) {
        //TODO IF MIDI PORT

        } else if (el.type_ == TYPE_AUDIO && el.isInput_) {
        //TODO Apply mappings from iogroups from soundfile channel to port
            SF_INFO inFmt = { 0, 0, 0, 0, 0, 0 };
            SNDFILE* inFile;
            if (!(inFile = barHelper_.Sopen(el.filePath_, SFM_READ, &inFmt))) {
                barHelper_.Fatal("Could not read input file `%s`\n", el.filePath_);
                return;
            }
            el.audioFileInfo_ = std::make_pair<SF_INFO, SNDFILE*>(inFmt, inFile);

            if (inFmt.channels != (int)nAudioIn_ && inFmt.channels != 1) {
                barHelper_.Fatal("Unable to map %d inputs to %d ports\n", inFmt.channels, nAudioIn_);
                return;
            }

            SF_INFO outFmt = inFmt;
            outFmt.channels = nAudioOut_;
            if (!(outFile = barHelper_.Sopen(outPath, SFM_WRITE, &outFmt))) {
                barHelper_.Fatal("Could not setup output file `%s`\n", outPath);
                return;
            }

            instance_ = lilv_plugin_instantiate(plugin_, inFmt.samplerate, NULL);

            for (uint32_t p=0, i=0, o=0; p<nPorts; ++p) {
                if (ports_[p].type == TYPE_CONTROL) {
                    lilv_instance_connect_port(instance_, p, &ports_[p].value);
                } else if (ports_[p].type == TYPE_AUDIO) {
                    if (ports_[p].isInput_) {
                        lilv_instance_connect_port(instance_, p, inBuf + i++);
                    } else {
                        lilv_instance_connect_port(instance_, p, outBuf + o++);
                    }
                } else {
                    lilv_instance_connect_port(instance_, p, NULL);
                }
            }
        } else if (el.type_ == TYPE_CONTROL && el.isInput_) {
        } else if (el.isInput_) {
            CloseAudioFilesHelper(inputTransportGroups);
            CloseAudioFilesHelper(outputTransportGroups);
            barHelper_.Fatal("Unsupported transport group type %d\n", el.type_);
            return;
        }
    }

    lilv_instance_activate(instance_);
    while (barHelper_.Sread(inFile, inFmt.channels, inBuf, nAudioIn_)) {
        lilv_instance_run(instance_, 1);
        if (sf_writef_float(outFile, outBuf, 1) != 1) {
            barHelper_.Fatal("Failed to write to output file `%s`\n", outPath);
            break;
        }
    }

    CloseAudioFilesHelper(ioTransportGroups);
}
