#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "lilv-0/lilv/lilv.h"

#include "lv2/core/lv2.h"

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


namespace mouthwash {

class BarHelper{
    public:
    static BarHelper& GetInstance()
    {
        static BarHelper instance;
        return instance;
    }

    BarHelper(BarHelper const&)       = delete;
    void operator=(BarHelper const&)   = delete;

    void Fatal(const char* fmt, ...);
    /** Open a sound file with error handling. */
    SNDFILE* Sopen(const char *path, int mode, SF_INFO *fmt);
    /** Close a sound file with error handling. */
    void Sclose(const char* path, SNDFILE* file);
    /** Read a single frame from a file into an interleaved buffer. */
    bool Sread(SNDFILE* file, unsigned file_chans, float* buf);

    bool BarIsOk() { return barOk; };

    private:
    BarHelper();
    ~BarHelper();

    bool barOk;
};

} // mouthwash
