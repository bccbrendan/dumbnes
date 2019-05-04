#include "ppu.h"
#include <cstdlib> // TODO remove (used for rand and srand

namespace dumbnes
{
    namespace ppu
    {

        using dumbnes::gui::IGui;
        using dumbnes::cpu6502::Nes6502;


        Ppu::Ppu(std::shared_ptr<IGui> gui,
                std::weak_ptr<Nes6502> cpu)
            : ctrl_(0x00)
            , mask_(0x00)
            , status_(0xA0)
            , oam_addr_(0x00)
            , address_latch_(false)
            , ppu_scroll_(0x00)
            , ppu_addr_(0x00)
            , ppu_data_(0x00)
            , odd_frame_(false)
            , cycle_(0)
            , scanline_(0)
            , gui_(gui)
            , cpu_(cpu)
        {
            srand(time(NULL)); // TODO remove when no longer using noise image
            std::memset(video_buffer_, 0, 256 * 240 * sizeof(uint32_t));
        }

        Ppu::~Ppu(void)
        {
        }

        void Ppu::Powerup()
        {
            // http://wiki.nesdev.com/w/index.php/PPU_power_up_state
            //
            //
            ctrl_.raw = 0x00;
            mask_.raw = 0x00;
            // 0xa0 from web reference '+0+x xxxx'
            status_.raw = 0xA0;
            oam_addr_ = 0x00;
            ppu_scroll_ = 0x00;
            ppu_addr_ = 0x00;
            ppu_data_ = 0x00;
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
            ctrl_.raw = 0x00;
            mask_.raw = 0x00;
            // 0xa0 from web reference '+0+x xxxx'
            status_.raw = 0xA0;
            oam_addr_ = 0x00;
            ppu_scroll_ = 0x00;
            ppu_addr_ = 0x00;
            ppu_data_ = 0x00;
            odd_frame_ = false;
            // TODO implement OAM http://wiki.nesdev.com/w/index.php/PPU_OAM
            // OAM = pattern
            // NT RAM (external, in Control Deck)     mostly $FF
            // CHR RAM (external, in Game Pak)
            cycle_ = 0;
            scanline_ = 0;
        }


        void Ppu::ScanLineVisible(void) {
            uint32_t pixel = rand() % 0xFFFFFFFF;
            switch(cycle_) {
                // https://wiki.nesdev.com/w/index.php/PPU_rendering#Cycle_0
                case 0: // idle cycle
                    // TODO
                    break;
                case 1 ... 256: // data fetch
                    // TODO fix this
                    video_buffer_[scanline_ * 256 + (cycle_ - 2)] = pixel;
                    break;
                case 257 ... 320: // tile data for sprites on next scanline
                    // TODO
                    break;
                case 321 ... 336:  // tile data for first two tiles of next scanline
                    break;
                case 337 ... 340: // unknown byte fetch
                    break;
            }
        }

        void Ppu::SetCpu(std::weak_ptr<dumbnes::cpu6502::Nes6502> cpu) {
            cpu_ = cpu;
        }

        void Ppu::Write(uint16_t address, uint8_t data) {
            // TODO
        }
    
        uint8_t Ppu::Read(uint16_t address) {
            uint8_t data = 0;
            switch(address) {
                case STATUS_ADDR:
                    data = status_.raw;
                    status_.vblank_started = 0;
                    address_latch_ = false;
                    break;
                case PPU_ADDR_ADDR:
                    data = ppu_addr_;
                    // TODO address_latch? seems odd to read this reg
                    break;
                default:
                    throw std::logic_error("not a PPU Read address!");
            }
            return data;
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
                case 0 ... 239:  // Visible
                    ScanLineVisible();
                    break;
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
                    status_.vblank_started = 1;
                    if (cycle_ == 1) {
                        auto cpu = cpu_.lock();
                        cpu->SetNmiReq(true);
                    }
                    break;
                case 261:
                    // TODO: pre-render scanline
                    status_.vblank_started = 0;
                    break;
            }
            ++cycle_;
            if (cycle_ == PPU_CYCLES) {
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

