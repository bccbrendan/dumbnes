/* nes_6502.cpp
 * definitions for 6502 cpu emulation
 * brendan long
 * Feb 2015
 */

#include "nes_6502.h"

Nes6502::Nes6502(IMemory *memory)
{
    _memory = memory;
}

Nes6502::~Nes6502(void)
{
}

void Nes6502::Reset (void)
{
}

int Nes6502::Step(int num_steps)
{
}

int Nes6502::Interrupt( /*TODO code?*/)
{
}
