#include "mapper0.h"

namespace dumbnes { namespace mapper {

Mapper0::Mapper0(char *header,
   std::vector<char> trainer,
   std::vector<char> prg_rom,
   std::vector<char> chr_rom,
   std::vector<char> prg_ram)
    : Mapper(header, std::move(trainer),
             std::move(prg_rom), std::move(chr_rom),
             std::move(prg_ram))
{
}

Mapper0::~Mapper0(void) {}

uint8_t Mapper0::Read(uint16_t address) {
    // https://wiki.nesdev.com/w/index.php/NROM
    uint8_t data = 0;
    uint16_t offset = 0;
    const char* description = "";
    switch (address) {
        case 0x6000 ... 0x7FFF: // family basic only, PRG RAM
            description = "[prg ram]";
            offset = address - 0x6000;
            data = prg_ram_[offset];
            break;
        case 0x8000 ... 0xBFFF: // first 16KB of Rom
            description = "[first 16kb rom]";
            offset = address - 0x8000;
            data = prg_rom_[offset];
            break;
        case 0xC000 ... 0xFFFF: // last 16KB of Rom
            description = "[last 16kb rom]";
            offset = (address - 0xC000) + (prg_rom_.size() - 0x4000);
            data = prg_rom_[offset];
            break;
        default:
            description = "[unmapped]";
            LOG(WARNING) << "access to unmapped memory!";
    }
    LOG(DEBUG) << "addr: " << std::hex << address
               << " " << description
               << " offset: " << offset
               << " data: 0x" << int(data);
    return data;
}

}}


