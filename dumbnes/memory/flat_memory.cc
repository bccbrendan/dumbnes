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
#include "memory/flat_memory.h"

using namespace dumbnes::memory;

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

void FlatMemory::W(const uint16_t addr, uint8_t data)
{
    _contents[addr] = data;
}

uint8_t FlatMemory::R(const uint16_t addr) const
{
    return _contents[addr];
}


