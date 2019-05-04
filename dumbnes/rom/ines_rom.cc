#include "rom/ines_rom.h"
#include <iterator>
#include <fstream>


namespace dumbnes { namespace rom {

iNesRom::iNesRom(const char* rom_file_path)
{
    mapper_ = dumbnes::mapper::Mapper::CreateMapper(rom_file_path);
}

iNesRom::~iNesRom(void) {
}

void iNesRom::Write(uint16_t address, uint8_t data)
{
    return mapper_->Write(address, data);
}

uint8_t iNesRom::Read(uint16_t address)
{
    return mapper_->Read(address);
}

}}
