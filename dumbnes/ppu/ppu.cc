#include "ppu.h"
#include <cstdlib> // TODO remove (used for rand and srand

namespace dumbnes
{
namespace ppu
{

using dumbnes::memory::IMemory;
using dumbnes::gui::IGui;
using dumbnes::cpu6502::Nes6502;

   
Ppu::Ppu(std::shared_ptr<IGui> gui,
         std::shared_ptr<IMemory> memory,
         std::shared_ptr<Nes6502> cpu)
    : gui_(gui)
    , memory_(memory)
    , odd_frame_(false)
    , cpu_(cpu)
    , cycle_(0)
    , scanline_(0)
{
    srand(time(NULL));
}

Ppu::~Ppu(void)
{
}

void Ppu::Powerup()
{
    // http://wiki.nesdev.com/w/index.php/PPU_power_up_state
    //
    //
    memory_->PPUCTRL(0x00);
    ctrl_.raw = 0x00;
    memory_->PPUMASK(0x00);
    mask_.raw = 0x00;
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
    cycle_ = 0;
    scanline_ = 0;
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

bool Ppu::Step(void) {
    bool new_frame = false;
    // ref https://wiki.nesdev.com/w/index.php/PPU_rendering
    // here we use 0-239 for visible scanlines,
    // 240 for post-render 
    // 241-260 for vblanking
    // and 261 for dummy/pre-render
    // so 261 == -1 on the wiki
    switch (scanline_) {
        case 0 ... 239:
            {
            uint32_t pixel = rand() % 0xFFFFFFFF;
            // TODO fix this
            if (cycle_ > 2 && cycle_ < 250) {
                video_buffer_[scanline_ * 256 + (cycle_ - 2)] = pixel;
            }
            // TODO: visible
            // for now, get a random byte
            
            break;
            }
        case 240:
            // Post-render scanline (240)
            // The PPU just idles during this scanline. 
            // Even though accessing PPU memory from the program would be safe here,
            // the VBlank flag isn't set until after this scanline.
            if (cycle_ == 0) {
                // in the first cycle of the post-render scanline, we'll update our gui
                gui_->UpdatePixels(reinterpret_cast<uint8_t*>(video_buffer_));
                new_frame = true;
            }
            break;
        case 241:
            // Vertical blanking lines (241-260)
            // The VBlank flag of the PPU is set at tick 1 (the second tick)
            // of scanline 241, where the VBlank NMI also occurs.
            // The PPU makes no memory accesses during these scanlines, 
            // so PPU memory can be freely accessed by the program.
            if (cycle_ == 1) {
                cpu_->NMI();
            }
            break;
        case 261:

            // TODO: pre-render scanline
            break;
    }
    ++cycle_;
    if (cycle_ == PPU_CYCLES) {
        // std::cout << "Ppu::Step " << std::dec << " scanline " << scanline_ << std::endl;
        // wrap around
        cycle_ %= PPU_CYCLES;
        ++scanline_;
        if (scanline_ == SCANLINES) {
            scanline_ = 0;
            odd_frame_ = !odd_frame_;
        }
    }
    return new_frame;
}

}
}

