#include "MazeDemo.h"

#include <iostream>
#include <cmath>
#include "Util.h"

MazeDemo::MazeDemo() :
	m_mazeMaker(),
	m_isFinished(false),
	m_fov((float)M_PI / 3.0f),
	m_fisheyeCorrection(true)
{
}

void MazeDemo::Init(int w, int h, bool testMap)
{
	m_mazeMaker = std::make_unique<MazeMaker>(w, h);

	if (testMap) {
		// test map: just a big empty room with a wall around the perimeter
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				BLOCKTYPE block = BL_EMPTY;
				if (x == 0 || y == 0 || x == w - 1 || y == h - 1)
					block = BL_SOLID;
				m_mazeMaker->SetBlock(x, y, block);
			}
		}
		m_mazeMaker->SetBlock(8, 8, BL_PLAYERSTART);
		m_mazeMaker->SetBlock(5, 5, BL_END);
	}
	else {
		m_mazeMaker->GenerateMaze();
	}

	int playerX, playerY;
	m_mazeMaker->GetPlayerStart(playerX, playerY);

	m_player = std::make_unique<Player>();
	// offset to the middle of the block
	m_player->x = (float)playerX + 0.5f;
	m_player->y = (float)playerY + 0.5f;
	m_player->angle = 0.0f;
}

void MazeDemo::Update(float dt)
{
	//std::cout << "dt: " << dt << std::endl;
	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		if (ev.type == SDL_QUIT) {
			m_isFinished = true;
			return;
		}

		if (ev.type == SDL_KEYDOWN) {
			switch (ev.key.keysym.sym) {
			case SDLK_ESCAPE:
				SDL_Event quitEv;
				quitEv.type = SDL_QUIT;
				SDL_PushEvent(&quitEv);
				break;

			case SDLK_w: // walk
			{
				Vec2f view = m_player->GetViewVector();
				m_player->x += view.x * MOVE_SPEED * dt;
				m_player->y += view.y * MOVE_SPEED * dt;
				break;
			}
			case SDLK_s:
			{
				Vec2f view = m_player->GetViewVector();
				m_player->x -= view.x * MOVE_SPEED * dt;
				m_player->y -= view.y * MOVE_SPEED * dt;
				break;
			}
			case SDLK_q: // strafe
			{
				Vec2f strafe = m_player->GetStrafeVector();
				m_player->x -= strafe.x * MOVE_SPEED * dt;
				m_player->y -= strafe.y * MOVE_SPEED * dt;
				break;
			}
			case SDLK_e:
			{
				Vec2f strafe = m_player->GetStrafeVector();
				m_player->x += strafe.x * MOVE_SPEED * dt;
				m_player->y += strafe.y * MOVE_SPEED * dt;
				break;
			}
			case SDLK_a: // rotate
				m_player->angle += ROTATE_SPEED * dt;
				std::cout << "player angle: " << m_player->angle << std::endl;
				break;
			case SDLK_d:
				m_player->angle -= ROTATE_SPEED * dt;
				std::cout << "player angle: " << m_player->angle << std::endl;
				break;

			case SDLK_f:
				m_fisheyeCorrection = !m_fisheyeCorrection;
				break;
			}
			
		}
	}
}

void MazeDemo::Render(SDL_Surface *buffer)
{
	SDL_FillRect(buffer, nullptr, SDL_MapRGB(buffer->format, 0, 0, 0));

	for (int x = 0; x < buffer->w; x++) {
		// cast a ray for each column of the screen buffer
		// leftmost ray will be (player angle + fov/2)
		// rightmost ray will be (player angle - fov/2)
		float columnRatio = (float)x / buffer->w;
		float rayAngle = (m_player->angle + m_fov / 2) - columnRatio * m_fov;

		bool hitWall = false;
		float distToWall = 0;

		Vec2f eye = Vec2f{ SDL_cosf(rayAngle), -SDL_sinf(rayAngle) };
		bool odd = false;

		
		// get distance to wall
		while (!hitWall && distToWall < MAX_RAYDEPTH) {
			distToWall += 0.1f;

			int testX = (int)(m_player->x + eye.x * distToWall);
			int testY = (int)(m_player->y + eye.y * distToWall);

			// bounds check
			if (testX < 0 || testY < 0 || testX >= m_mazeMaker->Width() || testY >= m_mazeMaker->Height()) {
				hitWall = true;
				distToWall = MAX_RAYDEPTH;
				break;
			}
			MazeBlock block = m_mazeMaker->GetBlock(testX, testY);
			switch (block.Type) {
				case BL_SOLID: // we've got a hit
					// correct for fisheye at larger fovs
					float theta = std::abs(rayAngle - m_player->angle);
					if (m_fisheyeCorrection) {
						float correction = SDL_cosf(theta);
						distToWall *= correction;
					}

					hitWall = true;
					odd = (((testX + testY) % 2) == 1);
					break;
			}
		}

		int ceilingEnd = (int)((float)(buffer->h) / 2 - buffer->h / (float)distToWall);
		int floorStart = buffer->h - ceilingEnd;

		// draw the column
		Uint32 color;
		for (int y = 0; y < buffer->h; y++) {
			if (y < ceilingEnd) { // ceiling
				float scale = 1.0f - ((float)y / (buffer->h / 2));
				color = SDL_MapRGB(buffer->format, scale * 0xFF, scale * 0xAF, scale * 0x41);
			}
			else if (y > ceilingEnd && y <= floorStart) { // wall
				color = odd ? SDL_MapRGB(buffer->format, 0xFF, 0xDD, 0xDD) :
					SDL_MapRGB(buffer->format, 0xCC, 0xCC, 0xEE);
			}
			else { // floor
				float scale = 1.0f;
				color = SDL_MapRGB(buffer->format, scale * 0x95, scale * 0xA5, scale * 0xA6);
			}
			SetPixel(buffer, x, y, color);
		}
	}

	int blockSize = 8;
	RenderMazePreview(*m_mazeMaker, *m_player, buffer, blockSize);
}
