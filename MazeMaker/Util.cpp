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

void PrintMaze(Maze *maze) {
	for (int y = 0; y < maze->Height(); y++) {
		for (int x = 0; x < maze->Width(); x++) {
			char c = '0';
			switch (maze->GetBlock(x, y).Type) {
			case BLOCKTYPE::BL_EMPTY: c = ' '; break;
			case BLOCKTYPE::BL_SOLID: c = (char)219; break;
			}
			std::cout << c;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void RenderMazePreview(Maze *maze, Player &player, SDL_Surface *buffer, int blockSize)
{
	for (int y = 0; y < maze->Height(); y++) {
		for (int x = 0; x < maze->Width(); x++) {
			MazeBlock block = maze->GetBlock(x, y);
			Uint32 color;
			switch (block.Type) {
			case BLOCKTYPE::BL_EMPTY: color = SDL_MapRGB(buffer->format, 0x00, 0x00, 0x00); break;
			case BLOCKTYPE::BL_SOLID: color = SDL_MapRGB(buffer->format, 0xCC, 0xCC, 0xCC); break;
			default: color = SDL_MapRGB(buffer->format, 0x00, 0x00, 0x00); break;
			}
			SDL_Rect rect{ x*blockSize, y*blockSize, blockSize, blockSize };
			SDL_FillRect(buffer, &rect, color);
		}
	}

	SDL_Rect rect;
	// draw end block
	Uint32 endColor = SDL_MapRGB(buffer->format, 0x00, 0xFF, 0x00);
	int x, y;
	maze->GetEnd(x, y);
	rect = { x*blockSize, y*blockSize, blockSize, blockSize };
	SDL_FillRect(buffer, &rect, endColor);

	// draw reverse cards
	auto obstacles = maze->GetObstacles();
	Uint32 obColor = SDL_MapRGB(buffer->format, 0x00, 0x00, 0xFF);
	for (auto ob : obstacles) {
		if (ob->removed)
			continue;
		rect = { (int)ob->pos.x * blockSize, (int)ob->pos.y * blockSize, blockSize, blockSize };
		SDL_FillRect(buffer, &rect, obColor);
	}

	// draw player
	Uint32 playerColour = SDL_MapRGB(buffer->format, 0xFF, 0xFF, 0x00);
	rect = { (int)(std::roundf(player.pos.x - 0.5f) * blockSize), (int)(std::roundf(player.pos.y - 0.5f) * blockSize),
					blockSize, blockSize };
	SDL_FillRect(buffer, &rect, playerColour);

	// draw player view
	Vec2f view = player.GetViewVector();
	Vec2f v0 = player.pos * blockSize;
	Vec2f v1 = (player.pos + view * 1.5f) * blockSize;
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
	float angle = SDL_atan2f(offset.x, -offset.y) - ((float)M_PI / 4);
	if (angle < 0.0f) angle += (float)(2 * M_PI); // Put in the range [0,2*PI)

	if (angle >= 0.0f && angle < M_PI / 2) return RAYHIT_DIR::RH_RIGHT;
	else if (angle >= M_PI / 2 && angle < M_PI) return RAYHIT_DIR::RH_TOP;
	else if (angle >= M_PI && angle < 3 * M_PI / 2) return RAYHIT_DIR::RH_LEFT;
	else return RAYHIT_DIR::RH_BOTTOM;
}

Uint32 SampleTexture(SDL_Surface * surface, float x, float y)
{
	// should probs fix for bitmaps with less than 32 bpp
	int sX = (int)((surface->w - 1) * SDL_fmodf(x, 1.0f));
	int sY = (int)((surface->h - 1) * SDL_fmodf(y, 1.0f));

	// bilinear filtering?
	return ((Uint32*)surface->pixels)[sY * surface->w + sX];
}

// The smallest signed difference between 2 angles in radians
float AngleDiff(float angle1, float angle2)
{
	// normalize angles to [0,2*PI)
	if (angle1 < 0)
		angle1 += 2 * M_PI;
	if (angle1 >= 2 * M_PI)
		angle1 -= 2 * M_PI;
	if (angle2 < 0)
		angle2 += 2 * M_PI;
	if (angle2 >= 2 * M_PI)
		angle2 -= 2 * M_PI;
	
	// there is probably a better way of doing this
	float result = angle1 - angle2;
	if (result < 0)
		result += 2 * M_PI;
	if (result > M_PI)
		result -= 2 * M_PI;
	return result;
}
