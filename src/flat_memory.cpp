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

FlatMemory::FlatMemory(void)
{
}

void FlatMemory::InitializeBlock(void /*TODO*/)
{
}

void FlatMemory::DumpBlock(/*TODO*/)
{
}

uint8_t& FlatMemory::operator[](const uint16_t addr)
{
    return _contents[addr];
}
