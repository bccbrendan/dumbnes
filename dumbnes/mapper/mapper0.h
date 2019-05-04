#pragma once
#ifndef __mapper0_h
#define __mapper0_h

#include "dumbnes_includes.h"
#include "mapper/mapper.h"

namespace dumbnes { namespace mapper {


class Mapper0 : public Mapper
{
    protected:
        Mapper0(void) = delete;


  public:
    Mapper0(char *header,
       std::vector<char> trainer,
       std::vector<char> prg_rom,
       std::vector<char> chr_rom,
       std::vector<char> prg_ram);


    virtual ~Mapper0(void);
    virtual uint8_t Read(uint16_t address) override;
    virtual void Write(uint16_t address, uint8_t data) override;
};

}}
#endif /*__mapper0_h*/
