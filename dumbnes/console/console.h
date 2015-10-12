/*
 * console.h
 * command-line debugger/controller for nes emulator
 */
#pragma once
#ifndef __console_h
#define __console_h

#include "dumbnes_includes.h"

#include "memory/memory_interface.h"
#include "cpu6502/nes_6502.h"
#include "ppu/ppu.h"

namespace dumbnes
{
namespace console
{

using std::cout;
using std::endl;


class Console
{
private:
    std::thread console_thread_;
    std::atomic<bool> console_thread_kill_;
    std::shared_ptr<dumbnes::cpu6502::Nes6502> cpu_;
    std::shared_ptr<dumbnes::memory::IMemory> memory_;
    std::shared_ptr<dumbnes::ppu::Ppu> ppu_;

    void ConsoleThread(void);
    
public:

    Console(std::shared_ptr<dumbnes::cpu6502::Nes6502> cpu,
            std::shared_ptr<dumbnes::memory::IMemory> memory,
            std::shared_ptr<dumbnes::ppu::Ppu> ppu);

    void StartPrompt(void);

    ~Console(void);
};
    
/* convenience function for readline.
 * Read a string, and return a pointer to it.  Returns NULL on EOF.
 */
char* rl_gets (void);


}
}
 

#endif /* __console_h */
