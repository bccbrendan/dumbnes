/*
 * gui_sfml.h
 * Window manager implemented with SFML
 */
#pragma once
#ifndef __gui_sfml_h
#define __gui_sfml_h
#include "gui/gui.h"
#include <atomic>
#include <mutex>
#include <thread>
#include <SFML/Graphics.hpp>

namespace dumbnes { namespace gui {

    class GuiSfml : public IGui
    {
      private:
        std::thread gfx_thread_;
        std::atomic<bool> gfx_thread_kill_;
        std::shared_ptr<sf::RenderWindow> window_;
        std::mutex sprite_mutex_;
        sf::Texture texture_;
        sf::Sprite sprite_;

      void GfxThread(void);

      public:
        GuiSfml(void);
        ~GuiSfml(void);
        virtual void UpdatePixels(uint8_t *pixels) override;
        void StartGraphics(void) override;
        void HandleEvents(void) override;
        bool IsOpen(void) override;
        void Close(void) override;
    };
}}

#endif /* __gui_sfml_h */

