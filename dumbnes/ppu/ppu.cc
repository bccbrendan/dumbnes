#include "ppu.h"

namespace dumbnes
{
namespace ppu
{

using dumbnes::memory::IMemory;
using dumbnes::gui::IGui;

   
Ppu::Ppu(std::shared_ptr<IMemory> memory,
         std::shared_ptr<IGui> gui)
    : gui_(gui)
    , memory_(memory)
    , odd_frame_(false)
{
}

void Ppu::Powerup()
{
    // http://wiki.nesdev.com/w/index.php/PPU_power_up_state
    //
    //
    memory_->PPUCTRL(0x00);
    memory_->PPUMASK(0x00);
    // 0xa0 from web reference '+0+x xxxx'
    memory_->PPUSTATUS(memory_->PPUSTATUS() | 0xA0);
    memory_->OAMADDR(0x00);
    (*memory_)[0x2005] = 0x00;
    (*memory_)[0x2006] = 0x00;
    memory_->PPUSCROLL(0x00);
    memory_->PPUADDR(0x00);
    memory_->PPUDATA(0x00);
    odd_frame_ = false;
    // TODO implement OAM http://wiki.nesdev.com/w/index.php/PPU_OAM
    // OAM = pattern
    // NT RAM (external, in Control Deck)     mostly $FF
    // CHR RAM (external, in Game Pak)
}

void Ppu::Reset()
{
    // http://wiki.nesdev.com/w/index.php/PPU_power_up_state
    memory_->PPUCTRL(0x00);
    memory_->PPUMASK(0x00);
    // memory_->PPUSTATUS(U??x xxxx); 
    // memory_->OAMADDR = unchanged
    (*memory_)[0x2005] = 0x00;
    (*memory_)[0x2006] = 0x00;
    memory_->PPUSCROLL(0x00);
    // memory_->PPUADDR(unchanged);
    memory_->PPUDATA(0x00);
    odd_frame_ = false;
    // TODO implement OAM
    // memory_->OAM = pattern
    // memory_->NT RAM (external, in Control Deck) = unchanged
    // memory_->CHR RAM (external, in Game Pak) =    unchanged 
}

Ppu::~Ppu(void)
{
}

}
}

