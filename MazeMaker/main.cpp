#pragma warning (disable : 4996)

#include <iostream>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <memory>
#include <chrono>

#include "main.h"
#include "MazeDemo.h"

static int screenW = 800;
static int screenH = 600;

static std::unique_ptr<MazeDemo> mazeDemo;
static SDL_Surface *screenSurface;
static SDL_Surface *drawBuffer;
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *bufferTexture;

bool Init(void *hWnd)
{
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return false;

	window = SDL_CreateWindowFrom(hWnd);
	SDL_ShowCursor(SDL_DISABLE);
	if (!window)
		return false;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (!renderer)
		return false;

	screenSurface = SDL_GetWindowSurface(window);
	if (!screenSurface)
		return false;
	screenW = screenSurface->w;
	screenH = screenSurface->h;

	int drawWidth = 720;
	int drawHeight = drawWidth * (float)screenH / screenW;

	bufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, drawWidth, drawHeight);
	drawBuffer = SDL_CreateRGBSurface(0, drawWidth, drawHeight, 32, 0,0,0,0);

	mazeDemo = std::make_unique<MazeDemo>();
	mazeDemo->Init(MAZE_WIDTH, MAZE_HEIGHT);
	return true;
}

void Update(float dt) {
	mazeDemo->Update(dt);
	// let mazedemo write to a texture
	SDL_SetRenderTarget(renderer, bufferTexture);
	mazeDemo->Render(renderer);
	//mazeDemo->Render(drawBuffer);
	// draw the texture to the screen
	SDL_SetRenderTarget(renderer, NULL);
	SDL_Rect targetRect = { 0,0, screenW, screenH };
	//SDL_Rect targetRect = { 0,0, screenW / 2, screenH / 2 };
	SDL_RenderCopy(renderer, bufferTexture, NULL, &targetRect);
	SDL_RenderPresent(renderer);
}

void Shutdown()
{
	SDL_FreeSurface(drawBuffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(bufferTexture);

	SDL_DestroyWindow(window);
	window = nullptr;
	screenSurface = nullptr; // auto-freed on window destroy
	
	SDL_Quit();
}
