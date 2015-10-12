#include "ppu.h"
#include <SFML/Graphics.hpp>

namespace dumbnes
{
namespace ppu
{

using dumbnes::memory::IMemory;

void Ppu::GfxThread(void)
{
    window_->setActive(true);
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Magenta);

    while (!gfx_thread_kill_ && window_->isOpen())
    {
        window_->clear();
        window_->draw(shape);
        window_->display();
    }
}
    
Ppu::Ppu(std::shared_ptr<IMemory> memory,
         std::shared_ptr<sf::RenderWindow> window)
    : gfx_thread_()
    , gfx_thread_kill_(false)
    , memory_(memory)
    , odd_frame_(false)
    , window_(window)
{
}

void Ppu::StartGraphics(void)
{
    gfx_thread_ = std::thread(&Ppu::GfxThread,this);
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
    gfx_thread_kill_ = true;
    if (gfx_thread_.joinable())
    {
        gfx_thread_.join();
    }
}

}
}

