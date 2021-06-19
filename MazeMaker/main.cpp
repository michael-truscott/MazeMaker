#pragma warning (disable : 4996)

#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <memory>
#include <chrono>

#include "main.h"
#include "MazeDemo.h"

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

static std::unique_ptr<MazeDemo> mazeDemo;
static SDL_Surface *screenSurface;
static SDL_Surface *drawBuffer;
static SDL_Window *window;

bool Init(void *hWnd)
{
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return false;

	window = SDL_CreateWindowFrom(hWnd);
	SDL_ShowCursor(SDL_DISABLE);
	if (!window)
		return false;

	screenSurface = SDL_GetWindowSurface(window);
	if (!screenSurface)
		return false;
	SCREEN_WIDTH = screenSurface->w;
	SCREEN_HEIGHT = screenSurface->h;

	int drawWidth = 720;
	int drawHeight = (int)(drawWidth * (float)SCREEN_HEIGHT / SCREEN_WIDTH);

	drawBuffer = SDL_CreateRGBSurface(0, 800, 600, 32, 0,0,0,0);

	mazeDemo = std::make_unique<MazeDemo>();
	mazeDemo->Init(MAZE_WIDTH, MAZE_HEIGHT);
	return true;
}

void Update(float dt) {
	mazeDemo->Update(dt);
	mazeDemo->Render(drawBuffer);

	SDL_BlitScaled(drawBuffer, nullptr, screenSurface, nullptr);
	SDL_UpdateWindowSurface(window);
}

void Shutdown()
{
	SDL_FreeSurface(drawBuffer);

	SDL_DestroyWindow(window);
	window = nullptr;
	screenSurface = nullptr; // auto-freed on window destroy
	
	SDL_Quit();
}
