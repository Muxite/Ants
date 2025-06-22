#pragma once
#include <SFML/Graphics.hpp>
#include "world.h"
#include "entity.h"
#include <random>
#include <ctime>
#include <iostream>
#include <cmath>
#include "color.h"

using namespace std;


class Ant : public Entity
{
public:
	Ant(const position position, const int patience);
	void update(World& world) override;
	void move(Map& map);
	void display(sf::RenderWindow& renderWindow, uint8_t upscale) override;
	void teleport(const position& newPos);
	path randomWalk(Map& map, int dist);
	void reroute(Map& map);
	void teleportValid(Map& map);

private:
	position pos;
	bool moving = false;
	bool wantMove = true;
	bool onPath = false;
	uint8_t patience;
	uint8_t maxPatience;
	vector<position> directive;
	mt19937 mt;
	sf::Color color = sf::Color::Red;
};
