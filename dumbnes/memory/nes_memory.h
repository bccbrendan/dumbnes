#pragma once
#ifndef __NES_MEMORY_H
#define __NES_MEMORY_H
/*
 * nes_memory.h
 *
 * class declarations for NES memory
 * brendan long
 * Feb 2015
 */

// https://wiki.nesdev.com/w/index.php/CPU_memory_map
#include "dumbnes_includes.h"
#include "memory/memory_interface.h"
#include "rom/ines_rom.h"
#include "ppu/ppu.h"

namespace dumbnes { namespace memory {
class NesMemory : public IMemory
{
    /*
    Address range	Size	Device
    $0000-$07FF	$0800	2KB internal RAM
    $0800-$0FFF	$0800	Mirrors of $0000-$07FF
    $1000-$17FF	$0800
    $1800-$1FFF	$0800
    $2000-$2007	$0008	NES PPU registers
    $2008-$3FFF	$1FF8	Mirrors of $2000-2007 (repeats every 8 bytes)
    $4000-$4017	$0018	NES APU and I/O registers
    $4018-$401F	$0008	APU and I/O functionality that is normally disabled. See CPU Test Mode.
    $4020-$FFFF	$BFE0	Cartridge space: PRG ROM, PRG RAM, and mapper registers (See Note)
    */
    constexpr static unsigned int RAM_START = 0x0000;
    constexpr static unsigned int RAM_END = 0x07FF;
    constexpr static unsigned int RAM_SIZE = RAM_END - RAM_START + 1;
    // repeats 3 times, every 0x800 bytes
    constexpr static unsigned int RAM_MIRRORS_START = 0x0800;
    constexpr static unsigned int RAM_MIRRORS_END = 0x1FFF;
    constexpr static unsigned int RAM_MIRRORS_SIZE = RAM_MIRRORS_END - RAM_MIRRORS_START + 1;
    // PPU
    constexpr static unsigned int PPU_REGISTERS_START = 0x2000;
    constexpr static unsigned int PPU_REGISTERS_END = 0x2007;
    constexpr static unsigned int PPU_REGISTERS_SIZE = PPU_REGISTERS_END - PPU_REGISTERS_START + 1;
    // PPU mirrors - repeats every 8 bytes
    constexpr static unsigned int PPU_REGISTERS_MIRRORS_START = 0x2008;
    constexpr static unsigned int PPU_REGISTERS_MIRRORS_END = 0x3FFF;
    constexpr static unsigned int PPU_REGISTERS_MIRRORS_SIZE = PPU_REGISTERS_MIRRORS_END - PPU_REGISTERS_MIRRORS_START + 1;
    // APU
    constexpr static unsigned int APU_START = 0x4000;
    constexpr static unsigned int APU_END = 0x4017;
    constexpr static unsigned int APU_SIZE = APU_END - APU_START + 1;
    // APU/IO normaly disabled
    constexpr static unsigned int TEST_MODE_START = 0x4018;
    constexpr static unsigned int TEST_MODE_END = 0x401F;
    constexpr static unsigned int TEST_MODE_SIZE = TEST_MODE_END - TEST_MODE_START + 1;
    // cartridge boundaries defined in ines_rom.h

    uint8_t ram_[RAM_SIZE];
    std::shared_ptr<dumbnes::ppu::Ppu> ppu_;
    std::shared_ptr<dumbnes::rom::iNesRom> rom_;

public:
    NesMemory(void) = delete;
    NesMemory(std::shared_ptr<dumbnes::rom::iNesRom> rom,
            std::shared_ptr<dumbnes::ppu::Ppu> ppu);
    virtual ~NesMemory(void);
    void InitializeBlock(void /*TODO*/) override;
    virtual void DumpBlock(/*TODO*/) override;
    virtual void W(const uint16_t addr, uint8_t data) override;
    virtual uint8_t R(const uint16_t addr) const override;
    virtual uint16_t R16(const uint16_t addr) const override;
    };

}}  // namespace dumbnes::memory

#endif /* __NES_MEMORY_H */

