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
    auto ppu = std::make_shared<dumbnes::ppu::Ppu>(gui, mem, cpu);
    ppu->Powerup();

    // start debug console
    auto console = std::make_shared<dumbnes::console::Console>(cpu, mem, ppu);
    console->StartPrompt();

    float target_fps = 60.0;
    auto target_frame_duration = std::chrono::duration<double, std::milli>(1000.0 / target_fps);
    auto pixels_per_frame = dumbnes::ppu::SCREEN_HEIGHT * dumbnes::ppu::SCREEN_WIDTH;

    // handle window event loop.
    while (gui->IsOpen())
    {
        auto frame_start = std::clock();
        gui->HandleEvents();
        if (!console->PromptRunning())
        {
            gui->Close();
        }
        
        // execute enough cycles for one frame refresh
        bool new_frame = false;
        while(!new_frame) {
            auto cpu_cycles = cpu->AsmStep();
            // 1 cpu cycle = 3 ppus
            for (size_t i = 0; i < cpu_cycles * 3; ++i) {
                new_frame |=  ppu->Step();
            }
        }
        auto frame_end = std::clock();
        auto millis_elapsed = std::chrono::duration<double, std::milli>(frame_end - frame_start);
        if (target_frame_duration > millis_elapsed) {
            auto sleep_time = target_frame_duration - millis_elapsed;
            std::this_thread::sleep_for(sleep_time);
        }
    }
}
