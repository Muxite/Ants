#include "map.h"

int heuristic(const position& a, const position& b)
{
	return static_cast<int> (sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2)));
}

int heuristic(const node& a, const node& b)
{
	return static_cast<int> (sqrt(pow(a.pos.x - b.pos.x, 2) + pow(a.pos.y - b.pos.y, 2)));
}

Map::Map(int width, int height)
	: width(width)
	, height(height), size(width * height), map(width * height, 1), entityMap(width * height, false)
{
}

int Map::get(int x, int y) const
{
	if (x < 0 || x >= width || y < 0 || y >= height) 
	{
		return -1;
	}
	return map[y * width + x];
}

int Map::get(const position& pos) const
{
	if (pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height)
	{
		return -1;
	}
	return map[pos.y * width + pos.x];
}

bool Map::entityCheck(const position& pos) const
{
	if (pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height)
	{
		return true;
	}
	return entityMap[pos.y * width + pos.x];
}

path Map::pathfind(const position& start, const position& end, const bool includeEntities)
{
	std::array<position, 4> directions =
	{
		position{1, 0}, // right
		position{-1, 0}, // left
		position{0, 1}, // down
		position{0, -1} // up
	};

	unordered_map<position, int> gs;
	unordered_map<position, position> parents;
	unordered_set<position> closed;
	MinHeap open;

	// initialize start node
	node start_node;
	start_node.pos = start;
	start_node.g = 0;
	start_node.h = heuristic(start, end);
	open.push(start_node);

	while (!open.empty())
	{

		// get the node with the lowest f value
		node current = open.top();
		open.pop();
		closed.insert(current.pos);

		// check if we reached the end
		if (current.pos == end)
		{
			// reconstruct path
			path result;
			position pos = end;
			result.cost = current.g;
			result.trace.reserve(closed.size());

			// check if end reached fr
			if (parents.find(pos) == parents.end() && pos != start)
			{
				// No path found
				return path{};
			}

			while (pos != start)
			{
				result.trace.emplace_back(pos);
				pos = parents[pos];
			}
			result.trace.emplace_back(start);

			return result;
		}

		// check neighbors
		for (position& direction : directions)
		{
			position neighbor_position = current.pos + direction;
			// skip if out of bounds, unwalkable, or already closed
			if (get(neighbor_position) < 0 || closed.count(neighbor_position))
			{
				continue;
			}

			if (includeEntities && entityCheck(neighbor_position))
			{
				continue;
			}

			int path_length = current.g + get(neighbor_position);

			// open the neighbor if not already open, or update if new path is shorter.
			if (!gs.count(neighbor_position) || path_length < gs[neighbor_position])
			{
				parents[neighbor_position] = current.pos;
				gs[neighbor_position] = path_length;

				// make a new node (might be duplicate position) with recalculated values.
				node neighbor_node;
				neighbor_node.pos = neighbor_position;
				neighbor_node.g = path_length;
				neighbor_node.h = heuristic(neighbor_node.pos, end);
				open.push(neighbor_node);
			}
		}
	}

	return path{};
}

void Map::display(sf::RenderWindow& window, uint8_t upscale)
{
	// vertex array renders faster than many rect.
	sf::VertexArray grid(sf::PrimitiveType::Triangles, size * 6);

	int idx = 0;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			uint8_t value = 0;

			switch (get(x, y))
			{
			case -2:
				value = 255;
			break;
			default:
				value = static_cast<uint8_t>(get(x, y) * 255 / 10); 
			}
			sf::Color color{value, value, value};
			
			float px = x * upscale;
			float py = y * upscale;
			float ux = upscale;

			grid[idx + 0].position = { px, py };
			grid[idx + 1].position = { px + ux, py };
			grid[idx + 2].position = { px + ux, py + ux };

			grid[idx + 3].position = { px, py };
			grid[idx + 4].position = { px + ux, py + ux };
			grid[idx + 5].position = { px, py + ux };

			for (int i = 0; i < 6; ++i) {
				grid[idx + i].color = color;
			}

			idx += 6;
		}
	}

	window.draw(grid);
}

void Map::randomWall(uint8_t num, uint8_t length, bool hori, bool vert)
{
	std::mt19937 mt(std::random_device{}());
	for (int i = 0; i < num; i++)
	{
		int x = mt() % width;
		int y = mt() % height;
		uint16_t currentLength = 0;
		bool pick = 0;
		int8_t dir = (mt() % 2 + 1) * 2 - 3;

		if (hori && vert)
		{
			pick = mt() % 2;
		}
		
		while (currentLength < length && (x >= 0 && x < width && y >= 0 && y < height))
		{
			map[y * width + x] = -2;
			currentLength++;

			if ((hori && !vert) || pick)
			{
				x += dir;
			}
			else
			{
				y += dir;
			}
		}
	}
}

void Map::noiseWall(float frequency, float threshold, int brush)
{
	FastNoiseLite fnl;
	fnl.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			float level = fnl.GetNoise((float)x * frequency, (float)y * frequency);
			if (level > threshold)
			{
				map[y * width + x] = brush;
			}
		}
	}
}

void Map::block(const position pos)
{
	entityMap[pos.y * width + pos.x] = true;
}

void Map::unblock(const position pos)
{
	entityMap[pos.y * width + pos.x] = false;
}