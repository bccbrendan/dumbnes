/*
 * main.cc
 * dummy application to play around with dumbnes in dev.
 */

#include "dumbnes_includes.h"
#include "cpu6502/nes_6502.h"
#include "memory/nes_memory.h"
#include "gui/gui_sfml.h"
#include "gui/gui_stub.h"
#include "ppu/ppu.h"
#include "console/console.h"
#include "rom/ines_rom.h"
#include <X11/Xlib.h> // include last -- name conflict with None

INITIALIZE_EASYLOGGINGPP

int main(int argc, char* argv[])
{
    START_EASYLOGGINGPP(argc, argv);
    el::Configurations log_conf("log-conf.conf");
    el::Loggers::reconfigureAllLoggers(log_conf);
    LOG(INFO) << "My first log!";
    if (argc < 2) {
        std::cout << "usage: dumbnes_emu <rom_file>" << std::endl;
        return 1;
    }

    auto rom = std::make_shared<dumbnes::rom::iNesRom>(argv[1]);
    LOG(INFO) << "Loaded Rom file!";

    // apparently XInitThreads is needed with SFML if rendering is 
    // done in a background thread.
    // http://en.sfml-dev.org/forums/index.php?topic=16882.0
    // TODO make portable - X11 might not be as available on windows.
    XInitThreads();

    // construct application window
    // TODO fix X11 issues
    // auto gui = std::make_shared<dumbnes::gui::GuiSfml>();
    auto gui = std::make_shared<dumbnes::gui::GuiStub>();
    LOG(INFO) << "constructed GUI";
    gui->StartGraphics();
    LOG(INFO) << "Started graphics";

    // construct Picture Processing Unit and start render thread
    std::weak_ptr<dumbnes::cpu6502::Nes6502> placeholder_cpu;

    auto ppu = std::make_shared<dumbnes::ppu::Ppu>(gui, placeholder_cpu);

    auto mem = std::make_shared<dumbnes::memory::NesMemory>(rom, ppu);
    LOG(INFO) << "Initialized memory";

    auto cpu = std::make_shared<dumbnes::cpu6502::Nes6502>(mem);

    ppu->SetCpu(cpu);
    cpu->Reset();
    ppu->Powerup();

    // start debug console
    auto console = std::make_shared<dumbnes::console::Console>(cpu, mem, ppu);
    // TODO optionally enable?
    //console->StartPrompt();
    LOG(INFO) << "Started prompt";

    float target_fps = 60.0;
    auto target_frame_duration = std::chrono::duration<double, std::milli>(1000.0 / target_fps);
    int steps_to_take = 10000000;
    // handle window event loop.
    while (gui->IsOpen() && (steps_to_take > 0))
    {
        auto frame_start = std::clock();
        gui->HandleEvents();
        if (!console->PromptRunning())
        {
            gui->Close();
        }
        
        // execute enough cycles for one frame refresh
        bool new_frame = false;
        while(!new_frame && (steps_to_take-- > 0)) {
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
