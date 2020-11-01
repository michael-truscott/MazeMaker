#pragma warning (disable : 4996)

#include <iostream>
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

int main(int argc, char *argv[]) {
	if (!Init()) {
		std::cout << "init() failed: error - " << SDL_GetError() << std::endl;
		return 1;
	}
	MainLoop();
	Shutdown();

	return 0;
}

bool Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return false;

	window = SDL_CreateWindow("Maze Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN);
	if (!window)
		return false;

	screenSurface = SDL_GetWindowSurface(window);
	if (!screenSurface)
		return false;

	int drawWidth = 720;
	int drawHeight = drawWidth * (float)SCREEN_HEIGHT / SCREEN_WIDTH;

	drawBuffer = SDL_CreateRGBSurface(0, 800, 600, 32, 0,0,0,0);

	mazeDemo = std::make_unique<MazeDemo>();
	mazeDemo->Init(MAZE_WIDTH, MAZE_HEIGHT);
	return true;
}

void MainLoop()
{
	auto prevTime = std::chrono::steady_clock::now();
	auto curTime = std::chrono::steady_clock::now();
	float time = 0;
	while (!mazeDemo->IsFinished()) {
		curTime = std::chrono::steady_clock::now();
		float dt = std::chrono::duration<float>(curTime - prevTime).count();
		time += dt;
		prevTime = std::chrono::steady_clock::now();

		char title[50];
		sprintf(title, "MazeMaker: %d FPS", (int)(1.0f / dt));
		SDL_SetWindowTitle(window, title);

		while (time > TIME_STEP) {
			Update(dt);
			time -= TIME_STEP;
		}
		mazeDemo->Render(screenSurface);
		SDL_UpdateWindowSurface(window);
	}
}

void Update(float dt)
{
	mazeDemo->Update(TIME_STEP);
}

void Shutdown()
{
	SDL_FreeSurface(drawBuffer);

	SDL_DestroyWindow(window);
	window = nullptr;
	screenSurface = nullptr; // auto-freed on window destroy
	
	SDL_Quit();
}
