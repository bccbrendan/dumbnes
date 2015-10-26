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

typedef std::function<void(std::vector<std::string> const&, std::ostream&)> HandlerFunc;

class Console
{
private:
    std::thread console_thread_;
    std::atomic<bool> console_thread_kill_;
    std::shared_ptr<dumbnes::cpu6502::Nes6502> cpu_;
    std::shared_ptr<dumbnes::memory::IMemory> memory_;
    std::shared_ptr<dumbnes::ppu::Ppu> ppu_;
//    std::map<std::string, HandlerFunc> handlers_;
    std::map<std::string, HandlerFunc> handlers_;

    void ConsoleThread(void);
    
    void PrintHeader(std::ostream &out) const;
    // CommandHandler GetHandler const(std::string);
 
public:

    Console(std::shared_ptr<dumbnes::cpu6502::Nes6502> cpu,
            std::shared_ptr<dumbnes::memory::IMemory> memory,
            std::shared_ptr<dumbnes::ppu::Ppu> ppu);

    void StartPrompt(void);
    bool PromptRunning(void) const;

    /* Handlers */
    void HandleHelp(std::vector<std::string> const& tokens, std::ostream& out) const;
    void HandleQuit(std::vector<std::string> const& tokens, std::ostream& out);
    void HandleRegisters(std::vector<std::string> const& tokens, std::ostream& out) const;
    void HandleStep(std::vector<std::string> const& tokens, std::ostream& out) const;
    void HandleSetReg(std::vector<std::string> const& tokens, std::ostream& out) const;
    void HandleMemdump(std::vector<std::string> const& tokens, std::ostream& out) const;
    void HandleMemset(std::vector<std::string> const& tokens, std::ostream& out) const;



    ~Console(void);
};


/* simple tokenizer */
std::vector<std::string> Tokenize(std::string const& line);

    
/* convenience function for readline.
 * Read a string, and return a pointer to it.  Returns NULL on EOF.
 */
char* rl_gets (void);


}
}
 

#endif /* __console_h */
