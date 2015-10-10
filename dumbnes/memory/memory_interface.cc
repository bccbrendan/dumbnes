#include "memory/memory_interface.h"
namespace dumbnes
{
namespace memory
{

IMemory& IMemory::PPUCTRL(uint8_t data)
{
    this->W(0x2000, data);
}

uint8_t IMemory::PPUCTRL() const
{
    return this->R(0x2000);
}

IMemory& IMemory::PPUMASK(uint8_t data)
{
    this->W(0x2001, data);
}

uint8_t IMemory::PPUMASK() const
{
    return this->R(0x2001);
}


IMemory& IMemory::PPUSTATUS(uint8_t data)
{
    this->W(0x2002, data);
}

uint8_t IMemory::PPUSTATUS() const
{
    return this->R(0x2002);
}


IMemory& IMemory::OAMADDR(uint8_t data)
{
    this->W(0x2003, data);
}

uint8_t IMemory::OAMADDR() const
{
    return this->R(0x2003);
}


IMemory& IMemory::PPUSCROLL(uint8_t data)
{
    this->W(0x2005, data);
}

uint8_t IMemory::PPUSCROLL() const
{
    return this->R(0x2005);
}


IMemory& IMemory::PPUADDR(uint8_t data)
{
    this->W(0x2006, data);
}

uint8_t IMemory::PPUADDR() const
{
    return this->R(0x2006);
}


IMemory& IMemory::PPUDATA(uint8_t data)
{
    this->W(0x2007, data);
}

uint8_t IMemory::PPUDATA() const
{
    return this->R(0x2007);
}

}
}
