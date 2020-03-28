#include "bar.hh"
#include "bartender.hh"

#define MOUTHWASH_VERSION "1.0"

using namespace mouthwash;

static void
PrintVersion(void)
{
    printf("mouthwash " MOUTHWASH_VERSION "\n");
}


int main(int argc, char** argv) {
    PrintVersion();
    Bartender bartender;
    bartender.Run();
}
