#pragma once

const int MAZE_WIDTH = 15;
const int MAZE_HEIGHT = 15;
const int FRAMES_PER_SEC = 60;
const float TIME_STEP = 1.0f / FRAMES_PER_SEC;

bool Init();
void MainLoop();
void Update(float dt);
void Shutdown();