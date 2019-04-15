/*
 * ppu.h
 * Picture Processing unit
 */
#pragma once
#ifndef __ppu_h
#define __ppu_h

#include "gui/gui.h"
#include "memory/memory_interface.h"
#include "cpu6502/nes_6502.h"

namespace dumbnes
{
    namespace ppu
    {
        union Ctrl
        {
            uint8_t raw;
            struct {
                uint8_t nametable: 2; // Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
                uint8_t addr_increment: 1; // VRAM address increment per CPU read/write of PPUDATA (0: add 1, going across; 1: add 32, going down)
                uint8_t sprite_pat_table: 1; // Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
                uint8_t background_pat_table: 1; // Background pattern table address (0: $0000; 1: $1000)
                uint8_t sprite_size: 1; // Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
                uint8_t ppu_master_slave: 1; // PPU master/slave select (0: read backdrop from EXT pins; 1: output color on EXT pins)
                uint8_t generate_nmi: 1; // Generate an NMI at the start of the vertical blanking interval (0: off; 1: on)
            };
        };

        union Mask
        {
            uint8_t raw;
            struct {
                uint8_t greyscale: 1; // Greyscale (0: normal color, 1: produce a greyscale display)
                uint8_t show_background_left: 1; // 1: Show background in leftmost 8 pixels of screen, 0: Hide
                uint8_t show_sprites_left: 1; // 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
                uint8_t show_background: 1; // 1: Show background
                uint8_t show_sprites: 1; // 1: Show sprites
                uint8_t emphasize_red: 1; // Emphasize red
                uint8_t emphasize_green: 1; // Emphasize green
                uint8_t emphasize_blue: 1; // Emphasize blue
            };
        };

        union Status
        {
            uint8_t raw;
            struct {
                uint8_t lsb_written_to_ppu: 5; // Least significant bits previously written into a PPU register (due to register not being updated for this address)
                // Sprite overflow. The intent was for this flag to be set
                // whenever more than eight sprites appear on a scanline, but a
                // hardware bug causes the actual behavior to be more complicated
                // and generate false positives as well as false negatives; see
                // PPU sprite evaluation. This flag is set during sprite
                // evaluation and cleared at dot 1 (the second dot) of the
                // pre-render line.
                uint8_t sprite_overflow: 1;
                // Sprite 0 Hit.  Set when a nonzero pixel of sprite 0 overlaps
                // a nonzero background pixel; cleared at dot 1 of the pre-render
                // line.  Used for raster timing.
                uint8_t sprite0_hit: 1;
                // Vertical blank has started (0: not in vblank; 1: in vblank).
                // Set at dot 1 of line 241 (the line *after* the post-render
                // line); cleared after reading $2002 and at dot 1 of the
                // pre-render line.
                uint8_t vblank_started: 1;
            };
        };

        constexpr static unsigned int SCREEN_WIDTH = 256.0;
        constexpr static unsigned int SCREEN_HEIGHT = 240.0;
        // The PPU renders 262 scanlines per frame. 
        // Each scanline lasts for 341 PPU clock cycles 
        // (113.667 CPU clock cycles; 1 CPU cycle = 3 PPU cycles), 
        // with each clock cycle producing one pixel. The line numbers given here correspond to how the internal PPU frame counters count lines.
        constexpr static unsigned int PRE_RENDER_SCANLINE = 261;
        constexpr static unsigned int SCANLINES = 262;
        constexpr static unsigned int PPU_CYCLES = 341;


        class Ppu
        {
            private:
                std::shared_ptr<dumbnes::gui::IGui> gui_;
                std::shared_ptr<dumbnes::memory::IMemory> memory_;
                std::shared_ptr<dumbnes::cpu6502::Nes6502> cpu_;
                bool odd_frame_;
                // memory mapped registers
                Ctrl ctrl_;  // $2000
                Mask mask_;  // $2001
                Status status_; // $2002

                uint32_t video_buffer_[256 * 240];
               int cycle_;
                int scanline_;


            public:
                Ppu(std::shared_ptr<dumbnes::gui::IGui> gui,
                    std::shared_ptr<dumbnes::memory::IMemory> memory,
                    std::shared_ptr<dumbnes::cpu6502::Nes6502> cpu);
                ~Ppu(void);
                void StartGraphics(void);
                void Powerup();
                void Reset();
                void Write(uint16_t address, uint8_t data);
                uint8_t Read(uint16_t address);
                bool Step(void);
        };


    }
}


#endif /* __ppu_h */
