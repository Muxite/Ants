#pragma once
#include "world.h"
#include <SFML/Graphics.hpp>

class Entity
{
public:
	// virtual is overridden in derived classes like Ant
	virtual ~Entity() = default;
	virtual void update(World& world) = 0;
	virtual void display(sf::RenderWindow& renderWindow, uint8_t upscale) = 0;
};