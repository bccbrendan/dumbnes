/*
 * nes_memory.cpp
 *
 * class declarations for simple nes memory
 * mainly for test/development
 *
 * brendan long
 * Feb 2015
 */

#include "dumbnes_includes.h"
#include "memory/nes_memory.h"

using namespace dumbnes::memory;
using namespace dumbnes::rom;


NesMemory::NesMemory(void)
{
}

NesMemory::NesMemory(std::shared_ptr<iNesRom> rom) : rom_(rom)
{
}

NesMemory::~NesMemory(void)
{
}

void NesMemory::InitializeBlock(void /*TODO*/)
{
}

void NesMemory::DumpBlock(/*TODO*/)
{
}


void NesMemory::W(const uint16_t addr, uint8_t data)
{
    switch (addr) {
        // mirrors region simply map to the RAM region,
        // so mask off the unused addr bits.
        case RAM_START ... RAM_MIRRORS_END:
            ram_[addr & RAM_END] = data;
            LOG(DEBUG) << "RAM[" << std::hex << (addr & RAM_END) << "] <- " << int(data);
            break;
        case PPU_REGISTERS_START ... PPU_REGISTERS_MIRRORS_END:
            LOG(DEBUG) << "PPU[" << std::hex << (addr & PPU_REGISTERS_END) << "] <- " << int(data);
            ppu_->Write(addr & PPU_REGISTERS_END, data);
            break;
        case APU_START ... APU_END:
            LOG(WARNING) << "Write to APU detected, APU not supported.";
            // TODO
            break;
        case TEST_MODE_START ... TEST_MODE_END:
            LOG(WARNING) << "Write to " << addr << " detected, not supported";
            break;
        case CARTRIDGE_START ... CARTRIDGE_END:
            LOG(DEBUG) << "cartridge[" << std::hex << addr << "] <- " << int(data);
            rom_->Write(addr, data);
            break;
        default:
            LOG(ERROR) << "Write to unmapped " << addr;
    }
}

uint8_t NesMemory::R(const uint16_t addr) const
{
    switch (addr) {
        // mirrors region simply map to the RAM region,
        // so mask off the unused addr bits.
        case RAM_START ... RAM_MIRRORS_END:
            LOG(DEBUG) << "RAM[" << std::hex << (addr & RAM_END) << "] : " << ram_[addr & RAM_END];
            return ram_[addr & RAM_END];
        case PPU_REGISTERS_START ... PPU_REGISTERS_MIRRORS_END:
            LOG(DEBUG) << "PPU[" << std::hex << (addr & PPU_REGISTERS_END) << "]";
            return ppu_->Read(addr & PPU_REGISTERS_END);
        case APU_START ... APU_END:
            LOG(WARNING) << "Read to APU detected, APU not supported.";
            return 0;
        case TEST_MODE_START ... TEST_MODE_END:
            LOG(WARNING) << "Read from " << addr << " detected, not supported.";
            return 0;
        case CARTRIDGE_START ... CARTRIDGE_END:
            LOG(DEBUG) << "cartridge[" << std::hex << addr << "]";
            return rom_->Read(addr);
        default:
            LOG(ERROR) << "Read from unmapped " << addr;
            return 0;
    }

}

uint16_t NesMemory::R16(const uint16_t addr) const
{
    uint16_t data = R(addr) | R(addr+1) << 8;
    LOG(DEBUG) << std::hex << "addr: 0x" << addr << " data: 0x" << data;
    return data;
    
}
 

