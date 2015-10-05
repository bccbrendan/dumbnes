#ifndef __MEMORY_INTERFACE_H
#define __MEMORY_INTERFACE_H
/*
 * memory_interface.h
 *
 * class declarations for memory
 * brendan long
 * Feb 2015
 */

namespace dumbnes { namespace memory {

class IMemory
{
public:
    virtual void InitializeBlock(void /*TODO*/) = 0;
    virtual void DumpBlock(/*TODO*/) = 0;
    virtual uint8_t& operator[](const uint16_t addr) = 0; 
};

}}
#endif /* __NES_MEMORY_H*/
