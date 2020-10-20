#pragma warning (disable : 4996)

#include <iostream>
#include <SDL.h>
#include <memory>
#include <chrono>

#include "MazeDemo.h"
#include "MazeMaker.h"

const int MAZE_WIDTH = 21;
const int MAZE_HEIGHT = 21;
const int FRAMES_PER_SEC = 60;
const float TIME_STEP = 1.0f / FRAMES_PER_SEC;

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

static std::unique_ptr<MazeDemo> mazeDemo;
static SDL_Surface *screenSurface;
static SDL_Window *window;

int main(int argc, char *argv[]);
bool Init();
void MainLoop();
void Shutdown();

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

	mazeDemo = std::make_unique<MazeDemo>();
	mazeDemo->Init(MAZE_WIDTH, MAZE_HEIGHT, true);
	return true;
}

void MainLoop()
{
	auto prevTime = std::chrono::steady_clock::now();
	auto curTime = std::chrono::steady_clock::now();
	while (!mazeDemo->IsFinished()) {
		curTime = std::chrono::steady_clock::now();
		float dt = std::chrono::duration<float>(curTime - prevTime).count();
		if (dt < TIME_STEP)
			continue;
		prevTime = std::chrono::steady_clock::now();
		char title[50];
		sprintf(title, "MazeMaker: %d FPS", (int)(1.0f / dt));
		SDL_SetWindowTitle(window, title);
		mazeDemo->Update(dt);
		mazeDemo->Render(screenSurface);
		SDL_UpdateWindowSurface(window);
	}
}

void Shutdown()
{
	SDL_DestroyWindow(window);
	window = nullptr;
	screenSurface = nullptr; // auto-freed on window destroy
	
	SDL_Quit();
}
