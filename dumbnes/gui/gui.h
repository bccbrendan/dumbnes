/*
 * gui.h
 * Window manager
 */
#pragma once
#ifndef __gui_h
#define __gui_h

#include <cstdint>

namespace dumbnes { namespace gui {

    class IGui
    {
      public:
        virtual void UpdatePixels(uint8_t *pixels) = 0;
        virtual void StartGraphics(void) = 0;
        virtual void HandleEvents(void) = 0;
        virtual bool IsOpen(void) = 0;
        virtual void Close(void) = 0;
    };
}}

#endif /* __gui_h */

