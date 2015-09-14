/* nes_6502.cpp
 * definitions for 6502 cpu emulation
 * brendan long
 * Feb 2015
 */

#include "nes_6502.h"

dumbnes::Nes6502::Nes6502(const std::shared_ptr<IMemory>& memory)
    :_memory(memory)
{
}

dumbnes::Nes6502::~Nes6502(void)
{
}

void dumbnes::Nes6502::Reset (void)
{
}

int dumbnes::Nes6502::Step(void)
{
    auto next_op = dumbnes::OpInfo::Decode((*_memory)[_regPC]);
    std::cout << "PC[0x" << std::hex << _regPC << "] "
        << next_op << std::endl;
}

int dumbnes::Nes6502::Interrupt( /*TODO code?*/)
{
}

