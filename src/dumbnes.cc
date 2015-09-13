/*
 * dumbnes.cc
 * dummy application to play around with dumbnes in dev.
 */

#include "dumbnes_includes.h"

int main(int argc, char* argv[])
{
    using std::cout;
    using std::endl;

    if (argc < 2)
    {
        cout << argv[0] << " Version " << Dumbnes_VERSION_MAJOR
             << "." << Dumbnes_VERSION_MINOR << endl;
        return 1;
    }
    auto mem = std::make_shared<FlatMemory>();
    auto cpu = std::make_shared<Nes6502>(mem);
    std::cout << "Resetting CPU regs!" << std::endl;
    cpu->Reset();
    std::cout << "Goodbye!" << std::endl;
}
