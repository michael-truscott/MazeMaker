#pragma once

#include "MazeMaker.h"
#include "Player.h"
#include "Vec2f.h"
#include <SDL.h>

void SetPixel(SDL_Surface *surface, int x, int y, Uint32 color);
void PrintMaze(MazeMaker &maze);
void RenderMazePreview(MazeMaker &maze, Player &player, SDL_Surface *buffer, int blockSize);
void DrawLine(Vec2f v0, Vec2f v1, SDL_Surface *image, Uint32 color);