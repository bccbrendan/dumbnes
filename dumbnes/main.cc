/*
 * main.cc
 * dummy application to play around with dumbnes in dev.
 */

#include "dumbnes_includes.h"
#include "dumbnes_config.h"
#include "cpu6502/nes_6502.h"
#include "memory/flat_memory.h"
#include "ppu/ppu.h"

int main(int argc, char* argv[])
{
    using std::cout;
    using std::endl;

    cout << argv[0] << " Version " << Dumbnes_VERSION_MAJOR
         << "." << Dumbnes_VERSION_MINOR << endl;

    auto mem = std::make_shared<dumbnes::memory::FlatMemory>();
    auto cpu = std::make_shared<dumbnes::cpu6502::Nes6502>(mem);
    std::cout << "Resetting CPU regs!" << std::endl;
    cpu->Reset();
    (*mem)[0x0] = 0xa9; // LDA 
    (*mem)[0x1] = 0xc0; // 
    cpu->Step();
    std::cout << std::hex
              << "Cpu state dump: " << std::endl
              << " PC: 0x" << cpu->PC() << std::endl
              << "  A: 0x" << cpu->A() << std:: endl
              ;
    // TODO provide some kinda callback to the PPU for user GUI clicks?
    auto ppu = std::make_shared<dumbnes::ppu::Ppu>();
    ppu->StartGraphics();

    std::cout << "hit enter to terminate!" << std::endl;
    std::string input;
    std::cin >> input;
}
