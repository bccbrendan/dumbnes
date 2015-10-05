/*
 * flat_memory.h
 *
 * class declarations for simple flat memory
 * mainly for test/development
 *
 * brendan long
 * Feb 2015
 */
#ifndef __FLAT_MEMORY_H
#define __FLAT_MEMORY_H

#include "dumbnes_includes.h"
#include "memory/memory_interface.h"

namespace dumbnes { namespace memory {

class FlatMemory : public IMemory
{
protected:
    uint8_t _contents[65536];

public:
    FlatMemory(void);
    void InitializeBlock(void /*TODO*/) override;
    virtual void DumpBlock(/*TODO*/) override ;
    virtual uint8_t& operator[](const uint16_t addr) override;
};

}} // namespace dumbnes::memory

#endif /* __FLAT_MEMORY_H */
