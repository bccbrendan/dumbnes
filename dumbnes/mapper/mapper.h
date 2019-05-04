#pragma once
#ifndef __mapper_h
#define __mapper_h

#include "dumbnes_includes.h"

namespace dumbnes { namespace mapper {


class Mapper
{
  protected:
    bool uses_chr_ram_;
    std::vector<char> trainer_;
    std::vector<char> prg_rom_;
    std::vector<char> chr_rom_;
    std::vector<char> prg_ram_;

    Mapper(void) = delete;

    Mapper(char *header,
           std::vector<char> trainer,
           std::vector<char> prg_rom,
           std::vector<char> chr_rom,
           std::vector<char> prg_ram);

  public:
    static std::unique_ptr<Mapper> CreateMapper(const char *rom_file_path);

    virtual ~Mapper(void);
    
    virtual uint8_t Read(uint16_t address) = 0;
    virtual void Write(uint16_t address, uint8_t data) = 0;
};

}}
#endif /*__mapper_h*/
