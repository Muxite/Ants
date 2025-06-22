#include <SFML/Graphics.hpp>
#include "world.h"
#include "ant.h"
#include "entity.h"
#include <random>
#include <chrono>
#include <iostream>

int main()
{
    const int upscale = 16;
    const int width = 1920 / 16;
    const int height = 1080 / 16;
	World world(width, height);
	vector<unique_ptr<Entity>> entities;
    sf::RenderWindow window(sf::VideoMode({ width*upscale, height*upscale }), "Ant House");
    sf::Clock frameClock;
    const sf::Time targetFrameTime = sf::seconds(1.0f / 20.0f);

    
    // info for this test
    world.map.noiseWall(18.0f, 0.2f, -2);
    world.map.noiseWall(4.0f, 0.4f, 1);
    for (int i = 0; i < 32; i++)
    {
        Ant ant(position{i*1, i*1}, 4);
		ant.teleportValid(world.map);
        entities.push_back(make_unique<Ant>(ant));
    }

    // main loop
    while (window.isOpen())
    {
        frameClock.restart();

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();
        for (auto& entity : entities)
        {
            entity->update(world);
        }

		world.map.display(window, upscale);

        for (auto& entity : entities)
        {
            entity->display(window, upscale);
        }


        window.display();

        sf::Time elapsed = frameClock.getElapsedTime();
        if (elapsed < targetFrameTime)
        {
            sf::sleep(targetFrameTime - elapsed);
        }

    }

    return 0;
}