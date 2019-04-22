#pragma once
#ifndef __ines_rom_h
#define __ines_rom_h

#include "dumbnes_includes.h"
#include "mapper/mapper.h"

namespace dumbnes { namespace rom {

constexpr static unsigned int CARTRIDGE_START = 0x4020;
constexpr static unsigned int CARTRIDGE_END = 0xFFFF;
constexpr static unsigned int CARTRIDGE_SPACE = CARTRIDGE_END - CARTRIDGE_START + 1;

// https://wiki.nesdev.com/w/index.php/INES
class iNesRom {
    private:
        std::unique_ptr<dumbnes::mapper::Mapper> mapper_;

    public:

    iNesRom(void) = delete; // no default
    iNesRom(const char* rom_file_path);
    ~iNesRom(void);

    void Write(uint16_t address, uint8_t data);
    uint8_t Read(uint16_t address);
};

}}





#endif /* __ines_rom_h */
