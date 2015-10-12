#include "console.h"
#include "dumbnes_config.h"
#include "dumbnes_includes.h"

namespace dumbnes
{
namespace console
{
    
using dumbnes::memory::IMemory;

void Console::ConsoleThread(void)
{

    cout << "Dumbnes Version " << Dumbnes_VERSION_MAJOR
         << "." << Dumbnes_VERSION_MINOR << endl;


    (*memory_)[0x0] = 0xa9; // LDA 
    (*memory_)[0x1] = 0xc0; // 
    cpu_->Step();
    std::cout << std::hex
              << "Cpu state dump: " << std::endl
              << " PC: 0x" << cpu_->PC() << std::endl
              << "  A: 0x" << cpu_->A() << std::endl
              ;

    while (!console_thread_kill_)
    {
        char *command = rl_gets();
        cout << "Right-o, I'll get to that asap" << endl;
    }

}

Console::Console(std::shared_ptr<dumbnes::cpu6502::Nes6502> cpu,
                 std::shared_ptr<dumbnes::memory::IMemory> memory,
                 std::shared_ptr<dumbnes::ppu::Ppu> ppu)
    : console_thread_()
    , console_thread_kill_(false)
    , cpu_(cpu)
    , memory_(memory)
    , ppu_(ppu)
{
}

    
void Console::StartPrompt(void)
{
    console_thread_ = std::thread(&Console::ConsoleThread,this);
}


Console::~Console(void)
{
    console_thread_kill_ = true;
    if (console_thread_.joinable())
    {
        console_thread_.join();
    }
}


/* Read a string, and return a pointer to it.  Returns NULL on EOF.
 * proudly stolen from http://web.mit.edu/gnu/doc/html/rlman_2.html
 * modified slighty*/
char *
rl_gets ()
{
    /* A static variable for holding the line. */
    static char *line_read = (char *)NULL;
    /* If the buffer has already been allocated, return the memory
    to the free pool. */
    if (line_read)
    {
        free (line_read);
        line_read = (char *)NULL;
    }

    /* Get a line from the user. */
    line_read = readline ("");

    /* If the line has any text in it, save it on the history. */
    if (line_read && *line_read)
    {
        add_history (line_read);
    }

    return (line_read);
}

}
}

