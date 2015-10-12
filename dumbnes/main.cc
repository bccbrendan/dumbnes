/*
 * main.cc
 * dummy application to play around with dumbnes in dev.
 */

#include "dumbnes_includes.h"
#include "dumbnes_config.h"
#include "cpu6502/nes_6502.h"
#include "memory/flat_memory.h"
#include "ppu/ppu.h"
#include <X11/Xlib.h> // include last -- name conflict with None

int main(int argc, char* argv[])
{
    using std::cout;
    using std::endl;

    // apparently XInitThreads is needed with SFML if rendering is 
    // done in a background thread.
    // http://en.sfml-dev.org/forums/index.php?topic=16882.0
    // TODO make portable - X11 might not be as available on windows.
    XInitThreads();

    cout << argv[0] << " Version " << Dumbnes_VERSION_MAJOR
         << "." << Dumbnes_VERSION_MINOR << endl;

    auto mem = std::make_shared<dumbnes::memory::FlatMemory>();
    auto cpu = std::make_shared<dumbnes::cpu6502::Nes6502>(mem);
    std::cout << "Resetting CPU regs!" << std::endl;
    cpu->Reset();
    (*mem)[0x0] = 0xa9; // LDA 
    (*mem)[0x1] = 0xc0; // 
    cpu->Step();
    std::cout << std::hex
              << "Cpu state dump: " << std::endl
              << " PC: 0x" << cpu->PC() << std::endl
              << "  A: 0x" << cpu->A() << std:: endl
              ;

    auto window = std::make_shared<sf::RenderWindow>(sf::VideoMode(200, 200),
                                                     "DumbNes");
    window->setActive(false);

    auto ppu = std::make_shared<dumbnes::ppu::Ppu>(mem, window);
    ppu->Powerup();
    ppu->StartGraphics();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

//    TODO enable console
//    auto console = std::make_shared<dumbnes::console::Console>(cpu, mem, ppu);
//    console->StartPrompt();

    // handle window event loop.
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window->close();
            }
        }
    }

}
