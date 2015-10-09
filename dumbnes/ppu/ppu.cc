#include "ppu.h"
#include <SFML/Graphics.hpp>

namespace dumbnes
{
namespace ppu
{

void Ppu::GfxThread(void)
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "DumbNes");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Magenta);

    while (!gfx_thread_kill_ && window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        window.clear();
        window.draw(shape);
        window.display();
    }
}
    
Ppu::Ppu(void)
    : gfx_thread_()
    , gfx_thread_kill_(false)
{
}

void Ppu::StartGraphics(void)
{
    gfx_thread_ = std::thread(&Ppu::GfxThread,this);
}

Ppu::~Ppu(void)
{
    gfx_thread_kill_ = true;
    if (gfx_thread_.joinable())
    {
        gfx_thread_.join();
    }
}

}
}

