/*
 * ppu.h
 * Picture Processing unit
 */
#pragma once
#ifndef __ppu_h
#define __ppu_h

#include <thread>
#include <atomic>

namespace dumbnes
{
namespace ppu
{

class Ppu
{
private:
    std::thread gfx_thread_;
    std::atomic<bool> gfx_thread_kill_;
    std::atomic<bool> gfx_thread_running_;

    void GfxThread(void);
    
public:
    Ppu(void);
    void StartGraphics(void);
    ~Ppu(void);
};


}
}
 

#endif /* __ppu_h */
