#include "gui/gui_stub.h"

namespace dumbnes { namespace gui {


void GuiStub::UpdatePixels(uint8_t *pixels) {}
void GuiStub::StartGraphics(void) {}
void GuiStub::HandleEvents(void) {}
bool GuiStub::IsOpen(void) { return true; }
void GuiStub::Close(void) {}

}}
