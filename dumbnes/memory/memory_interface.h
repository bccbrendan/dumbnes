#ifndef __MEMORY_INTERFACE_H
#define __MEMORY_INTERFACE_H
/*
 * memory_interface.h
 *
 * class declarations for memory
 * brendan long
 * Feb 2015
 */

#include <dumbnes_includes.h>

namespace dumbnes { namespace memory {

class IMemory
{
public:
    virtual void InitializeBlock(void /*TODO*/) = 0;
    virtual void DumpBlock(/*TODO*/) = 0;
    virtual void W(const uint16_t addr, uint8_t data) = 0;
    virtual uint8_t R(const uint16_t addr) const = 0;
    virtual uint16_t R16(const uint16_t addr) const = 0;

    /*********************/
    /* PPU mapped memory */
    /*********************/
    virtual IMemory& PPUCTRL(uint8_t);
    virtual uint8_t PPUCTRL() const;

    virtual IMemory& PPUMASK(uint8_t);
    virtual uint8_t PPUMASK() const;

    virtual IMemory& PPUSTATUS(uint8_t);
    virtual uint8_t PPUSTATUS() const;

    virtual IMemory& OAMADDR(uint8_t);
    virtual uint8_t OAMADDR() const;

    virtual IMemory& PPUSCROLL(uint8_t);
    virtual uint8_t PPUSCROLL() const;

    virtual IMemory& PPUADDR(uint8_t);
    virtual uint8_t PPUADDR() const;

    virtual IMemory& PPUDATA(uint8_t);
    virtual uint8_t PPUDATA() const;
};

}}
#endif /* __NES_MEMORY_H*/
