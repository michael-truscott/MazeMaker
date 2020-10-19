#include "Util.h"
#include <iostream>
#include <algorithm>

void SetPixel(SDL_Surface *surface, int x, int y, Uint32 color)
{
	int offset = surface->w * y + x;
	Uint32 *test = (Uint32*)surface->pixels;
	test += offset;
	*test = color;
}

void PrintMaze(MazeMaker &maze) {
	for (int y = 0; y < maze.Height(); y++) {
		for (int x = 0; x < maze.Width(); x++) {
			char c = '0';
			switch (maze.GetBlock(x, y).Type) {
			case BL_EMPTY: c = ' '; break;
			case BL_SOLID: c = (char)219; break;
			case BL_PLAYERSTART: c = 'P'; break;
			case BL_END: c = 'X'; break;
			}
			std::cout << c;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void RenderMazePreview(MazeMaker &maze, Player &player, SDL_Surface *buffer, int blockSize)
{
	for (int y = 0; y < maze.Height(); y++) {
		for (int x = 0; x < maze.Width(); x++) {
			MazeBlock block = maze.GetBlock(x, y);
			Uint32 color;
			switch (block.Type) {
			case BL_EMPTY: color = SDL_MapRGB(buffer->format, 0x00, 0x00, 0x00); break;
			case BL_SOLID: color = SDL_MapRGB(buffer->format, 0xFF, 0xFF, 0xFF); break;
			case BL_END: color = SDL_MapRGB(buffer->format, 0x00, 0xFF, 0x00); break;
			default: color = SDL_MapRGB(buffer->format, 0x00, 0x00, 0x00); break;
			}
			SDL_Rect rect{ x*blockSize, y*blockSize, blockSize, blockSize };
			SDL_FillRect(buffer, &rect, color);
		}
	}

	// draw player
	SDL_Rect rect{ (int)((player.x - 0.5f) * blockSize), (int)((player.y - 0.5f) * blockSize),
					blockSize, blockSize };
	SDL_FillRect(buffer, &rect, SDL_MapRGB(buffer->format, 0x00, 0x00, 0xFF));

	// draw player view
	Vec2f view = player.GetViewVector();
	DrawLine(Vec2f{ player.x * blockSize, player.y * blockSize },
		Vec2f{ (player.x + view.x) * blockSize, (player.y + view.y) * blockSize },
		buffer, SDL_MapRGB(buffer->format, 0x00, 0x00, 0xFF));
}

void DrawLine(Vec2f v0, Vec2f v1, SDL_Surface *image, Uint32 color)
{
	bool flipped = false;
	if ((std::abs(v1.y - v0.y) > std::abs(v1.x - v0.x)))
	{
		std::swap(v0.x, v0.y);
		std::swap(v1.x, v1.y);
		flipped = true;
	}
	if (v0.x > v1.x)
	{
		std::swap(v0.x, v1.x);
		std::swap(v0.y, v1.y);
	}
	int dx = (int)(v1.x - v0.x);
	int dy = (int)(v1.y - v0.y);
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = (int)v0.y;
	for (int x = (int)v0.x; x <= v1.x; x++) {
		if (flipped)
			SetPixel(image, y, x, color);
		else
			SetPixel(image, x, y, color);
		error2 += derror2;
		if (error2 > dx) {
			y += (v1.y > v0.y ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}
