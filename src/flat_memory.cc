/*
 * flat_memory.cpp
 *
 * class declarations for simple flat memory
 * mainly for test/development
 *
 * brendan long
 * Feb 2015
 */

#include "dumbnes_includes.h"

dumbnes::FlatMemory::FlatMemory(void)
{
}

void dumbnes::FlatMemory::InitializeBlock(void /*TODO*/)
{
}

void dumbnes::FlatMemory::DumpBlock(/*TODO*/)
{
}

uint8_t& dumbnes::FlatMemory::operator[](const uint16_t addr)
{
    return _contents[addr];
}
