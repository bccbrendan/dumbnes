#ifndef __NES_MEMORY_H
#define __NES_MEMORY_H
/*
 * nes_memory.h
 *
 * class declarations for NES memory
 * brendan long
 * Feb 2015
 */

#include "dumbnes_includes.h"
#if 0
class NesMemory : public IMemory
{
public:
    NesMemory(void) {};
    void InitializeBlock(void /*TODO*/) = 0;
    virtual void DumpBlock(/*TODO*/) = 0;
    virtual uint8_t& operator[](const uint16_t addr) = 0; 
    };
#endif
#endif /* __NES_MEMORY_H */

