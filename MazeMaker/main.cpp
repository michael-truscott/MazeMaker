#include <iostream>

#include "MazeMaker.h"

void printMaze(MazeMaker &maze);

int main(int argc, char *argv[]) {
	std::cout << "blah" << std::endl;

	MazeNode n1(0, 0);
	MazeNode n2(3, 4);

	std::cout << "Distance from n1 to n2: " << n1.DistanceTo(n2) << std::endl;
	std::cout << "Distance from n2 to n1: " << n2.DistanceTo(n1) << std::endl;

	MazeMaker maze(21, 21);
	maze.GenerateMaze();
	printMaze(maze);

	return 0;
}

void printMaze(MazeMaker &maze) {
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