/*
 * main.cc
 * dummy application to play around with dumbnes in dev.
 */

#include "dumbnes_includes.h"
#include "cpu6502/nes_6502.h"
#include "memory/flat_memory.h"
#include "gui/gui_sfml.h"
#include "ppu/ppu.h"
#include "console/console.h"
#include <X11/Xlib.h> // include last -- name conflict with None

int main(int argc, char* argv[])
{
    // apparently XInitThreads is needed with SFML if rendering is 
    // done in a background thread.
    // http://en.sfml-dev.org/forums/index.php?topic=16882.0
    // TODO make portable - X11 might not be as available on windows.
    XInitThreads();

    auto mem = std::make_shared<dumbnes::memory::FlatMemory>();
    auto cpu = std::make_shared<dumbnes::cpu6502::Nes6502>(mem);
    cpu->Reset();

    // construct application window
    auto gui = std::make_shared<dumbnes::gui::GuiSfml>();
    gui->StartGraphics();
    // construct Picture Processing Unit and start render thread
    auto ppu = std::make_shared<dumbnes::ppu::Ppu>(mem, gui);
    ppu->Powerup();

    // start debug console
    auto console = std::make_shared<dumbnes::console::Console>(cpu, mem, ppu);
    console->StartPrompt();

    // handle window event loop.
    while (gui->IsOpen())
    {
        gui->HandleEvents();
        if (!console->PromptRunning())
        {
            gui->Close();
        }
    }
}
