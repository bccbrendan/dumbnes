/*
 * gui_stub.h
 * Window manager - headless
 */
#pragma once
#ifndef __gui_stub_h
#define __gui_stub_h

#include "gui/gui.h"

namespace dumbnes { namespace gui {

    class GuiStub : public IGui
    {
      public:
        virtual void UpdatePixels(uint8_t *pixels) override;
        virtual void StartGraphics(void) override;
        virtual void HandleEvents(void) override;
        virtual bool IsOpen(void) override;
        virtual void Close(void) override;
    };
}}

#endif /* __gui_stub_h */

