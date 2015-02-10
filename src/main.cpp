/*
 * main.cpp
 * dummy application to play around with dumbnes in dev.
 */

#include "dumbnes_includes.h"

int main(int argc, char* args[])
{
    std::cout << "Hello Dumbnes!" << std::endl;
    IMemory *mem = new FlatMemory();
    Nes6502 *cpu = new Nes6502(mem);
    std::cout << "Resetting CPU regs!" << std::endl;
    cpu->Reset();
    std::cout << "Goodbye!" << std::endl;
}
