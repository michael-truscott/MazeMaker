#pragma once

#include "Player.h"
#include "Vec2f.h"
#include "Maze.h"
#include <SDL.h>

enum RAYHIT_DIR {
	RH_RIGHT,
	RH_TOP,
	RH_LEFT,
	RH_BOTTOM,
};

void SetPixel(SDL_Surface *surface, int x, int y, Uint32 color);
void PrintMaze(Maze *maze);
void RenderMazePreview(Maze *maze, Player &player, SDL_Surface *buffer, int blockSize);
void DrawLine(Vec2f v0, Vec2f v1, SDL_Surface *image, Uint32 color);
RAYHIT_DIR RayHitDir(Vec2f offset);
Uint32 SampleTexture(SDL_Surface *surface, float x, float y);
float AngleDiff(float angle1, float angle2);