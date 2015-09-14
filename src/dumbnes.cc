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
    auto mem = std::make_shared<dumbnes::FlatMemory>();
    auto cpu = std::make_shared<dumbnes::Nes6502>(mem);
    std::cout << "Resetting CPU regs!" << std::endl;
    cpu->Reset();
    (*mem)[0x0] = 0xa9; // LDA 
    (*mem)[0x1] = 0xc0; // 
    cpu->Step();
    std::cout << std::hex
              << "Cpu state dump: << " << std::endl
              << " PC: 0x" << cpu->PC() << std::endl
              << "  A: 0x" << cpu->A() << std:: endl
              ;

    std::cout << "Goodbye!" << std::endl;
}
