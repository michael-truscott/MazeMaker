#include "TestMazeMaker.h"

std::unique_ptr<Maze> TestMazeMaker::GenerateMaze(int w, int h)
{
	auto maze = std::make_unique<Maze>(w, h);
	maze->Clear();

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			if (x == 0 || y == 0 || x == w - 1 || y == h - 1)
				maze->SetBlock(x, y, BLOCKTYPE::BL_SOLID);
			else
				maze->SetBlock(x, y, BLOCKTYPE::BL_EMPTY);

		}
	}
	maze->SetPlayerStart(4, 4);
	maze->SetEnd(10, 10);

	return maze;
}
