#include "ant.h"
#include <iostream>

Ant::Ant(const position position, const int maxPatience)
	: mt(std::chrono::system_clock::now().time_since_epoch().count()), maxPatience(maxPatience), pos(position), patience(maxPatience)
{
	color = randomColor(mt);
}

void Ant::update(World& world)
{	
	Map& map = world.map;
	if (!moving)
	{
		directive = randomWalk(map, 30).trace;

		patience = maxPatience;
		moving = true;
	}
	if (moving)
	{
		if (patience > 0)
		{
			move(world.map);
		}
		else
		{
			reroute(world.map);
		}
	}
}

void Ant::move(Map& map)
{
	map.unblock(pos);
	if (!directive.empty())
	{
		position target = directive.back();
		position right = pos + position{ 1, 0 };
		position left = pos + position{ -1, 0 };
		position down = pos + position{ 0, -1 };
		position up = pos + position{ 0, 1 };

		// Move 1 pixel towards target
		if (pos.x < target.x && !map.entityCheck(right))
			pos = right;
		else if (pos.x > target.x && !map.entityCheck(left))
			pos = left;
		else if (pos.y < target.y && !map.entityCheck(up))
			pos = up;
		else if (pos.y > target.y && !map.entityCheck(down))
			pos = down;
		else {
			patience--;
		}

		if (pos == target)
		{
			directive.pop_back();
		}
	}
	else
	{
		moving = false;
	}
	map.block(pos);
}

void Ant::display(sf::RenderWindow& renderWindow, uint8_t upscale)
{
	sf::RectangleShape ant_shape(sf::Vector2f(upscale, upscale));
	ant_shape.setFillColor(color);
	ant_shape.setPosition(sf::Vector2f(static_cast<float> (pos.x * upscale), static_cast<float> (pos.y * upscale)));
	renderWindow.draw(ant_shape);

	int headingLen = 0;
	if (directive.size() > headingLen)
	{
		for (int i = directive.size() - 1; i > directive.size() - 1 - headingLen; i--)
		{
			ant_shape.setPosition(sf::Vector2f(static_cast<float>(directive[i].x * upscale), static_cast<float>(directive[i].y * upscale)));
			renderWindow.draw(ant_shape);
		}
	}
}

void Ant::teleport(const position& newPos)
{
	pos = newPos;
}

void Ant::teleportValid(Map& map)
{
	position target;
	do 
	{
		target.x = mt() % map.width;
		target.y = mt() % map.height;
	} while (map.get(target) < 0 || map.entityCheck(target));
	pos = target;
}

path Ant::randomWalk(Map& map, int dist)
{
	position target;
	path path;

	int min_dx = -min(dist, pos.x);
	int max_dx = min(dist, map.width - pos.x - 1);
	int min_dy = -min(dist, pos.y);
	int max_dy = min(dist, map.height - pos.y - 1);

	// think of a target
	for (int retry = 0; retry < 10; retry++)
	{
		do
		{
			int dx = min_dx + (mt() % (max_dx - min_dx + 1));
			int dy = min_dy + (mt() % (max_dy - min_dy + 1));
			target = pos + position{ dx, dy };

		} while (target == pos || map.get(target) < 0);

		// find path to target
		path = map.pathfind(pos, target, false);

		if (!path.trace.empty())
		{
			break;
		}
	}

	return path;
}

void Ant::reroute(Map& map)
{
	// we are rerouting because the first few didnt work
	if (directive.size() > 3)
	{
		directive.pop_back();
		directive.pop_back();
		directive.pop_back();
	}

	// keep finding a new route, taking entities into account
	while (directive.size() > 0)
	{
		path attempt = map.pathfind(pos, directive.back(), true);
		if (!attempt.trace.empty())
		{
			directive.insert(directive.end(), attempt.trace.begin(), attempt.trace.end());
			patience = maxPatience;
			break;
		}
		directive.pop_back();
	}

	// find a new target
	moving = false;
}