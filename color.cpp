#include "color.h"

sf::Color randomColor(mt19937& mt)
{
	array<int, 3> rgb;
	int max = 0;

	for (int i = 0; i < 3; i++)
	{
		rgb[i] = mt() % 1024;
		if (rgb[i] > max)
			max = rgb[i];
	}

	// scale to max color
	return sf::Color(
		static_cast<uint8_t>(rgb[0] * 255 / max),
		static_cast<uint8_t>(rgb[1] * 255 / max),
		static_cast<uint8_t>(rgb[2] * 255 / max)
	);

}