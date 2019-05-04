#include "mapper/mapper.h"

#include "mapper/mapper0.h"

namespace dumbnes { namespace mapper {

Mapper::Mapper(char *header,
       std::vector<char> trainer,
       std::vector<char> prg_rom,
       std::vector<char> chr_rom,
       std::vector<char> prg_ram)
    : trainer_(std::move(trainer))
    , prg_rom_(std::move(prg_rom))
    , chr_rom_(std::move(chr_rom))
    , prg_ram_(std::move(prg_ram))
{
}



Mapper::~Mapper(void)
{
}

std::unique_ptr<Mapper> Mapper::CreateMapper(const char *rom_file_path) {
    // reference:
    // https://wiki.nesdev.com/w/index.php/INES
    std::ifstream ines_file(rom_file_path, std::ios::binary);
    char header[16];
    ines_file.read(header, 16);
    if (!ines_file) {
        LOG(ERROR) << "rom size too small!";
    }
    // read the header
    if ((header[0] != 'N') || (header[1] != 'E')
       || (header[2] != 'S') || (header[3] != 0x1A)) {
        LOG(ERROR) << "'NES' marker not found in ROM";
    }
    LOG(DEBUG) << "NES marker: " << header[0] << header[1] << header[2];
    size_t prg_rom_size = header[4] * 0x4000; // 16kb chunks
    LOG(DEBUG) << "PRG ROM: " << prg_rom_size << " bytes";
    size_t chr_rom_size = header[5] * 0x2000; // 8kb
    LOG(DEBUG) << "CHR ROM: " << chr_rom_size << " bytes";

    LOG(DEBUG) << "Flags 6: 0x" << std::hex << int(header[6]);
    bool vertical_mirroring = (header[6] & 0x1) != 0;
    bool contains_prg_ram = (header[6] & 0x2) != 0;
    bool contains_trainer = (header[6] & 0x4) != 0; // flag six bit two
    LOG(DEBUG) << "Trainer? " << contains_trainer;
    std::vector<char> trainer(contains_trainer ? 512 : 0);
    if (contains_trainer) {
        ines_file.read(trainer.data(), trainer.size());
        if (!ines_file) {
            LOG(ERROR) << "error reading trainer data from rom file";
        }
    }
    std::vector<char> prg_rom(prg_rom_size);
    ines_file.read(prg_rom.data(), prg_rom_size);
    if (!ines_file) {
        LOG(ERROR) << "error reading prg_rom data from rom file";
    }
    std::vector<char> chr_rom(chr_rom_size);
    if (chr_rom_size > 0) {
        ines_file.read(chr_rom.data(), chr_rom_size);
        if (!ines_file) {
            LOG(ERROR) << "error reading chr_rom data from rom file";
        }
    }
    std::vector<char> prg_ram(8192); // just use 8k ram always
    LOG(DEBUG) << "Flags 7: 0x" << std::hex << int(header[7]);
    bool playchoice_10 = (header[7] & 0x2) != 0;
    LOG(DEBUG) << "playchoice 10?" << playchoice_10;

    bool is_ines2 = ((header[7] & 0xC) >> 2) == 0x2;
    LOG(DEBUG) << "iNES 2.0? " << is_ines2;

    uint8_t mapper_number = (header[6] >> 4) | (header[7] & 0xF0);
    switch (mapper_number) {
        case 0:
            return std::make_unique<Mapper0>(header, std::move(trainer),
                                            std::move(prg_rom),
                                            std::move(chr_rom),
                                            std::move(prg_ram));
            break;
        default:
            LOG(ERROR) << "Unknown mapper number: " << mapper_number;
            return std::unique_ptr<Mapper>(nullptr);
    }
}

}}
