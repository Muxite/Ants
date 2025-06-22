#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <array>
#include <vector>
#include <queue>
#include <cmath>
#include <unordered_set>
#include <random>
#include "FastNoiseLite.h"


using namespace std;

// stores hashable coordinates
struct position
{
	int x, y;
	bool operator==(const position& other) const
	{
		return x == other.x && y == other.y;
	}

	bool operator!=(const position& other) const
	{
		return x != other.x || y != other.y;
	}

	position operator+(const position& other) const
	{
		return { x + other.x, y + other.y };
	}
};

// stores position and traits of that point.
struct node
{
	position pos;
	int g;
	int h;

	int f() const
	{
		return g + h;
	}

	bool operator>(const node& other) const
	{
		return f() > other.f();
	}
};


// make hashable
namespace std
{
	template<>
	struct hash<position> {
		size_t operator()(const position& p) const noexcept {
			return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
		}
	};
}

// calculates the heuristic distance between two positions
int heuristic(const position& a, const position& b);

int heuristic(const node& a, const node& b);

using MinHeap = priority_queue<node, vector<node>, greater<node>>;

struct path
{
	vector<position> trace;
	int cost = 0;
};

class Map {
public:
	Map(int width, int height);
	int get(int x, int y) const;
	int get(const position& pos) const;
	path pathfind(const position& start, const position& end, const bool includeEntities);
	void display(sf::RenderWindow& window, uint8_t upscale);
	bool entityCheck(const position& pos) const;
	void randomWall(uint8_t num, uint8_t length, bool hori, bool vert);
	void noiseWall(float frequency, float threshold, int brush);
	int width, height, size;
	void block(const position pos);
	void unblock(const position pos);

private:
	vector<int> map;
	vector<bool> entityMap;
	unordered_set<position> unwalkable;
};
