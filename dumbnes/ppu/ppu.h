/*
 * ppu.h
 * Picture Processing unit
 */
#pragma once
#ifndef __ppu_h
#define __ppu_h

#include <thread>
#include <atomic>
#include <SFML/Graphics.hpp>

#include "memory/memory_interface.h"

namespace dumbnes
{
namespace ppu
{

class Ppu
{
private:
    std::thread gfx_thread_;
    std::atomic<bool> gfx_thread_kill_;
    std::shared_ptr<dumbnes::memory::IMemory> memory_;
    std::shared_ptr<sf::RenderWindow> window_;
    bool odd_frame_;

    void GfxThread(void);
    
public:
    Ppu(std::shared_ptr<dumbnes::memory::IMemory> memory,
        std::shared_ptr<sf::RenderWindow> window);
    void StartGraphics(void);
    void Powerup();
    void Reset();
    ~Ppu(void);
};


}
}
 

#endif /* __ppu_h */
