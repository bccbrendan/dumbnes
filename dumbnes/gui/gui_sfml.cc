/*
 * gui_sfml.cc
 * Window manager implemented with SFML
 */
#include "gui/gui_sfml.h"
#include "ppu/ppu.h" // for pixels size

namespace dumbnes { namespace gui {

GuiSfml::GuiSfml(void)
    : gfx_thread_()
    , gfx_thread_kill_(false)
{
} 

GuiSfml::~GuiSfml(void) {
    gfx_thread_kill_ = true;
    if (gfx_thread_.joinable())
    {
        gfx_thread_.join();
    }
}

void GuiSfml::UpdatePixels(uint8_t *pixels) {
    texture_.update(pixels);
}

void GuiSfml::GfxThread(void)
{
    window_->setActive(true);
    texture_.create(ppu::SCREEN_WIDTH, ppu::SCREEN_HEIGHT);
    sf::Sprite sprite;
    sprite.setTexture(texture_);

    while (!gfx_thread_kill_ && window_->isOpen())
    {
        window_->clear();
        window_->draw(sprite);
        window_->display();
    }
}

void GuiSfml::StartGraphics(void)
{
    auto v_mode = sf::VideoMode(200, 200);
    auto title = "DumbNes";
    window_ = std::make_shared<sf::RenderWindow>(v_mode, title);
    window_->setActive(false);
    window_->setSize(sf::Vector2u(ppu::SCREEN_WIDTH, ppu::SCREEN_HEIGHT));


    gfx_thread_ = std::thread(&GuiSfml::GfxThread,this);
}

bool GuiSfml::IsOpen(void) {
    return (window_->isOpen());
}

void GuiSfml::HandleEvents(void) {
    sf::Event event;
    while (window_->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
        {
            window_->close();
        }
    }
}

void GuiSfml::Close(void) {
    window_->close();
}


}}

