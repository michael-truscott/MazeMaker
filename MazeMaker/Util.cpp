#include "Util.h"
#include <iostream>
#include <algorithm>
#include <cmath>

void SetPixel(SDL_Surface *surface, int x, int y, Uint32 color)
{
	int offset = surface->w * y + x;
	Uint32 *test = (Uint32*)surface->pixels;
	test += offset;
	*test = color;
}

void PrintMaze(MazeMaker &maze) {
	for (int y = 0; y < maze.h; y++) {
		for (int x = 0; x < maze.w; x++) {
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
	for (int y = 0; y < maze.h; y++) {
		for (int x = 0; x < maze.w; x++) {
			MazeBlock block = maze.GetBlock(x, y);
			Uint32 color;
			switch (block.Type) {
			case BL_EMPTY: color = SDL_MapRGB(buffer->format, 0x00, 0x00, 0x00); break;
			case BL_SOLID: color = SDL_MapRGB(buffer->format, 0xCC, 0x33, 0x33); break;
			case BL_END: color = SDL_MapRGB(buffer->format, 0x00, 0xFF, 0x00); break;
			default: color = SDL_MapRGB(buffer->format, 0x00, 0x00, 0x00); break;
			}
			SDL_Rect rect{ x*blockSize, y*blockSize, blockSize, blockSize };
			SDL_FillRect(buffer, &rect, color);
		}
	}

	// draw player
	Uint32 playerColour = SDL_MapRGB(buffer->format, 0xFF, 0xFF, 0x00);
	SDL_Rect rect{ (int)((player.pos.x - 0.5f) * blockSize), (int)((player.pos.y - 0.5f) * blockSize),
					blockSize, blockSize };
	SDL_FillRect(buffer, &rect, playerColour);

	// draw player view
	Vec2f view = player.GetViewVector();
	Vec2f v0 = player.pos * blockSize;
	Vec2f v1 = (player.pos + view * 2.0f) * blockSize;
	if (v1.x < 1) v1.x = 1;
	if (v1.y < 1) v1.y = 1;
	DrawLine(v0, v1, buffer, playerColour);
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

RAYHIT_DIR RayHitDir(Vec2f offset)
{
	// Compensate for flipped y
	float angle = SDL_atan2f(offset.x, -offset.y) - M_PI / 4;
	if (angle < 0.0f) angle += 2 * M_PI; // Put in the range [0,2*PI)

	if (angle >= 0.0f && angle < M_PI / 2) return RH_RIGHT;
	else if (angle >= M_PI / 2 && angle < M_PI) return RH_TOP;
	else if (angle >= M_PI && angle < 3 * M_PI / 2) return RH_LEFT;
	else return RH_BOTTOM;
}

Uint32 SampleTexture(SDL_Surface * surface, float x, float y)
{
	// should probs fix for bitmaps with less than 32 bpp
	int sX = (surface->w - 1) * SDL_fmod(x, 1.0f);
	int sY = (surface->h - 1) * SDL_fmod(y, 1.0f);

	// bilinear filtering?
	return ((Uint32*)surface->pixels)[sY * surface->w + sX];
}
