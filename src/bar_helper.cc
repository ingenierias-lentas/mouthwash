#include "bar_helper.hh"

using namespace mouthwash; 

SNDFILE*
BarHelper::Sopen(const char *path, int mode, SF_INFO *fmt)
{
      SNDFILE* file = sf_open(path, mode, fmt);
      const int st  = sf_error(file);
      if (st) {
          Fatal("Failed to open %s (%s)\n", path, sf_error_number(st));
      }
      return file;
}

void
BarHelper::Sclose(const char* path, SNDFILE* file)
{
    int st;
    if (file && (st = sf_close(file))) {
        Fatal("Failed to close %s (%s)\n", path, sf_error_number(st));
    }
}

bool
BarHelper::Sread(SNDFILE* file, unsigned file_chans, float* buf)
{
    const sf_count_t n_read = sf_readf_float(file, buf, 1);
    
    if (n_read != 1) {
        Fatal("Failed to read\n");
    }

    return n_read == 1;
}

void
BarHelper::Fatal(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
    barOk = false;
}
