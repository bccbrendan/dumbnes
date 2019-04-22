/*
 * gui_sfml.cc
 * Window manager implemented with SFML
 */
#include "dumbnes_includes.h"
#include "gui/gui_sfml.h"
#include "ppu/ppu.h" // for pixels size

namespace dumbnes { namespace gui {

GuiSfml::GuiSfml(void)
    : gfx_thread_()
    , gfx_thread_kill_(false)
    , window_(nullptr)
    , sprite_mutex_()
    , texture_()
    , sprite_()
{
    LOG(DEBUG) << "Creating texture";
    texture_.create(ppu::SCREEN_WIDTH, ppu::SCREEN_HEIGHT);
    LOG(DEBUG) << "Creating texture";
    sprite_.setTexture(texture_);
    LOG(DEBUG) << "GuiSfml constructed";
} 

GuiSfml::~GuiSfml(void) {
    gfx_thread_kill_ = true;
    if (gfx_thread_.joinable())
    {
        gfx_thread_.join();
    }
}

void GuiSfml::UpdatePixels(uint8_t *pixels) {
    std::unique_lock<std::mutex>(sprite_mutex_);
    texture_.update(pixels);
    sprite_.setTexture(texture_);
    uint32_t pixel_info = pixels[0] << 24 | pixels[1] << 16 | pixels[2] << 8 | pixels[3];
    LOG(DEBUG) << "updated sprite. pixel[0]: " << std::hex << pixel_info;
}

void GuiSfml::GfxThread(void)
{
    window_->setActive(true);
    while (!gfx_thread_kill_ && window_->isOpen())
    {
        window_->clear();
        std::unique_lock<std::mutex>(sprite_mutex_);
        window_->draw(sprite_);
        window_->display();
    }
}

void GuiSfml::StartGraphics(void)
{
    LOG(DEBUG) << "Starting graphics";
    auto v_mode = sf::VideoMode(200, 200);
    auto title = "DumbNes";
    window_ = std::make_shared<sf::RenderWindow>(v_mode, title);
    window_->setActive(false);
    window_->setSize(sf::Vector2u(ppu::SCREEN_WIDTH, ppu::SCREEN_HEIGHT));
    LOG(DEBUG) << "created window";
    gfx_thread_ = std::thread(&GuiSfml::GfxThread,this);
    LOG(DEBUG) << "Started graphics thread";
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

