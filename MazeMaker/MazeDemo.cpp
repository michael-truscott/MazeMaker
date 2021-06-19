#include "MazeDemo.h"

#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>
#include "Util.h"
#include "DfsMazeMaker.h"
#include "TestMazeMaker.h"
#include "RealTimeMazeSolver.h"

MazeDemo::MazeDemo() :
	m_mazeMaker(nullptr),
	m_bricks(nullptr),
	m_mazeSolver(nullptr),
	m_isFinished(false),
	m_fov(DEFAULT_FOV),
	m_fisheyeCorrection(true),
	m_showMiniMap(true),
	m_wallScaleFactor(DEFAULT_WALLSCALE),
	m_spriteScaleFactor(DEFAULT_SPRITESCALE),
	m_flipView(false),
	m_depthBuffer(nullptr),
	m_mazeW(0),
	m_mazeH(0),
	m_state(DemoState::ST_START),
	m_stateChangeAfter(0.0f),
	m_rockToDelete(nullptr),
	m_inputState()
{
	m_bricks = SDL_LoadBMP("D:/ChampMaze/data/bricks.bmp");
}

MazeDemo::~MazeDemo()
{
	SDL_FreeSurface(m_bricks);
	delete[] m_depthBuffer;
}

void MazeDemo::Init(int w, int h)
{
	m_mazeW = w;
	m_mazeH = h;
	
	m_mazeMaker = std::make_unique<DfsMazeMaker>();

	Restart();
}

void MazeDemo::Restart()
{
	m_maze = m_mazeMaker->GenerateMaze(m_mazeW, m_mazeH);

	int playerX, playerY;
	m_maze->GetPlayerStart(playerX, playerY);

	m_player = std::make_unique<Player>();
	// offset to the middle of the block
	m_player->pos.x = (float)playerX + 0.5f;
	m_player->pos.y = (float)playerY + 0.5f;
	m_player->angle = 0.0f;

	// Rotate the player until they're not facing a wall
	for (int i = 0; i < 3; i++) {
		Vec2f viewPos = m_player->pos + m_player->GetViewVector();
		MazeBlock block = m_maze->GetBlock((int)viewPos.x, (int)viewPos.y);
		if (block.Type == BLOCKTYPE::BL_EMPTY)
			break;
		m_player->angle += (float)(M_PI / 2);
	}

	m_mazeSolver = std::make_unique<RealTimeMazeSolver>(m_maze.get(), m_player.get());

	m_state = DemoState::ST_START;
	m_stateChangeAfter = WALL_GROW_TIME;
	m_flipView = false;
}

bool MazeDemo::CollidedWithMap(Vec2f v) {
	int x = (int)v.x;
	int y = (int)v.y;
	if (x < 0 || x >= m_maze->Width() || y < 0 || y >= m_maze->Height())
		return true;

	return m_maze->GetBlock(x, y).Type == BLOCKTYPE::BL_SOLID;
}

Sprite* MazeDemo::HitRock(float dt)
{
	auto obstacles = m_maze->GetObstacles();
	for (auto ob : obstacles) {
		Vec2f dist = m_player->pos - ob->pos;
		if (dist.Length() < 0.8f) {
			return ob;
		}
	}
	
	return nullptr;
}

bool MazeDemo::HitExit(float dt)
{
	float dist = dt * MOVE_SPEED;
	Vec2f view = m_player->GetViewVector();

	Vec2f nextPos = m_player->pos + (view * dist);
	int endX, endY;
	m_maze->GetEnd(endX, endY);
	return ((int)nextPos.x == endX && (int)nextPos.y == endY);
}

void MazeDemo::Update(float dt)
{
	switch (m_state) {
	case DemoState::ST_START:
	case DemoState::ST_WALLGROW:
		// make walls grow then start running
		m_stateChangeAfter -= dt;
		m_wallScaleFactor = (WALL_GROW_TIME - m_stateChangeAfter) * DEFAULT_WALLSCALE;
		if (m_stateChangeAfter <= 0.0f) {
			m_wallScaleFactor = DEFAULT_WALLSCALE;
			m_state = DemoState::ST_RUNNING;
		}
		break;
	case DemoState::ST_RUNNING:
	{
		if (m_rockToDelete) {
			m_maze->RemoveObstacle(m_rockToDelete);
			m_rockToDelete = nullptr;
		}
		// run frame
		m_mazeSolver->Update(dt);
		//MovePlayer(dt);


		// did we hit a rock or something to make us flip?
		Sprite *rock = HitRock(dt);
		if (rock) {
			m_rockToDelete = rock;
			m_stateChangeAfter = WALL_GROW_TIME;
			m_state = DemoState::ST_WALLSHRINK;
			break;
		}
		// are we within range of the exit? stop moving, schedule a restart
		if (HitExit(dt)) {
			m_stateChangeAfter = TIME_TIL_RESTART;
			m_state = DemoState::ST_FINISHED;
			break;
		}
		break;
	}
	case DemoState::ST_WALLSHRINK:
		// TODO: test
		m_stateChangeAfter -= dt;
		m_wallScaleFactor = (m_stateChangeAfter) * DEFAULT_WALLSCALE;
		if (m_stateChangeAfter <= 0.0f) {
			m_flipView = !m_flipView;
			m_wallScaleFactor = 0.01f;
			m_stateChangeAfter = WALL_GROW_TIME;
			m_state = DemoState::ST_WALLGROW;
		}
		break;
	case DemoState::ST_FINISHED:
		// restart once the time runs out
		m_stateChangeAfter -= dt;
		if (m_stateChangeAfter <= 0.0f)
			Restart();
		break;
	}
}

void MazeDemo::MovePlayer(float dt) {
	if (m_inputState.rotateL)
		m_player->angle += ROTATE_SPEED * dt;
	if (m_inputState.rotateR)
		m_player->angle -= ROTATE_SPEED * dt;

	Vec2f playerMoveDist{ 0,0 };
	if (m_inputState.forward)
		playerMoveDist = m_player->GetViewVector() * (MOVE_SPEED * dt);
	if (m_inputState.back)
		playerMoveDist = m_player->GetViewVector() * (-MOVE_SPEED * dt);
	if (m_inputState.strafeL)
		playerMoveDist = m_player->GetStrafeVector() * (-MOVE_SPEED * dt);
	if (m_inputState.strafeR)
		playerMoveDist = m_player->GetStrafeVector() * (MOVE_SPEED * dt);


	// TODO: AABB collision?
	Vec2f newPos = m_player->pos + playerMoveDist;
	if (!CollidedWithMap(newPos)) {
		m_player->pos = newPos;
		return;
	}
	newPos = Vec2f(m_player->pos.x + playerMoveDist.x, m_player->pos.y); // slide along y axis
	if (!CollidedWithMap(newPos)) {
		m_player->pos = newPos;
		return;
	}
	newPos = Vec2f(m_player->pos.x, m_player->pos.y + playerMoveDist.y); // slide along x axis
	if (!CollidedWithMap(newPos)) {
		m_player->pos = newPos;
		return;
	}
}

void MazeDemo::Render(SDL_Surface *buffer)
{
	// should probs do this earlier
	if (m_depthBuffer == nullptr)
		m_depthBuffer = new float[buffer->w * buffer->h];

	SDL_FillRect(buffer, nullptr, SDL_MapRGB(buffer->format, 0, 0, 0));
	std::fill_n(m_depthBuffer, buffer->w * buffer->h, std::numeric_limits<float>::max());

	RenderMaze(buffer);
	RenderSprites(buffer);

	// minimap
	int blockSize = 8;
	if (m_showMiniMap)
		RenderMazePreview(m_maze.get(), *m_player, buffer, blockSize);
}

void MazeDemo::TraceRay(const Vec2f pos, const float rayAngle, float &distToWall, float &tx, bool &xSide) {
	// more efficient algorithm based on https://lodev.org/cgtutor/raycasting.html
	bool hitWall = false;
	distToWall = 0;
	tx = 0;

	int mazeX = (int)pos.x;
	int mazeY = (int)pos.y;

	Vec2f eye = Vec2f{ SDL_cosf(rayAngle), -SDL_sinf(rayAngle) };
	// the distance between each x/y grid line intercept
	float deltaX = (eye.y == 0) ? 0 : ((eye.x == 0) ? 1 : std::abs(1 / eye.x));
	float deltaY = (eye.x == 0) ? 0 : ((eye.y == 0) ? 1 : std::abs(1 / eye.y));

	int stepX, stepY;
	float xDist, yDist;
	if (eye.x > 0) { // facing right
		xDist = (1 - (pos.x - mazeX)) * deltaX;
		stepX = 1;
	}
	else { // left
		xDist = (pos.x - mazeX) * deltaX;
		stepX = -1;
	}
	if (eye.y > 0) { // facing down (south)
		yDist = (1 - (pos.y - mazeY)) * deltaY;
		stepY = 1;
	}
	else { // up (north)
		yDist = (pos.y - mazeY) * deltaY;
		stepY = -1;
	}

	Vec2f testX = (eye * xDist) + pos;
	Vec2f testY = (eye * yDist) + pos;
	while (!hitWall) {
		if (xDist < yDist) {
			xDist += deltaX;
			mazeX += stepX;
			xSide = true;
		}
		else {
			yDist += deltaY;
			mazeY += stepY;
			xSide = false;
		}

		if (m_maze->GetBlock(mazeX, mazeY).Type == BLOCKTYPE::BL_SOLID) {
			hitWall = true;
			// get distance between ray hit and the camera plane
			if (xSide)
				distToWall = (mazeX - pos.x + (1 - stepX) / 2) / eye.x;
			else
				distToWall = (mazeY - pos.y + (1 - stepY) / 2) / eye.y;

			// Get texture sample x coord
			Vec2f blockMid(mazeX + 0.5f, mazeY + 0.5f);
			Vec2f testPoint = m_player->pos + (eye * distToWall);
			Vec2f offset = testPoint - blockMid;
			switch (RayHitDir(offset)) {
			case RAYHIT_DIR::RH_TOP:
				tx = testPoint.x - mazeX;
				break;
			case RAYHIT_DIR::RH_BOTTOM:
				tx = 1.0f - (testPoint.x - mazeX);
				break;
			case RAYHIT_DIR::RH_LEFT:
				tx = testPoint.y - mazeY;
				break;
			case RAYHIT_DIR::RH_RIGHT:
				tx = 1.0f - (testPoint.y - mazeY);
				break;
			}

			if (m_fisheyeCorrection) {
				float theta = std::abs(rayAngle - m_player->angle);
				float correction = SDL_cosf(theta);
				distToWall *= correction;
			}
		}
	}
}

void MazeDemo::RenderMaze(SDL_Surface *buffer) {
	for (int x = 0; x < buffer->w; x++) {
		// cast a ray for each column of the screen buffer
		// leftmost ray will be (player angle + fov/2)
		// rightmost ray will be (player angle - fov/2)
		float columnRatio = (float)x / buffer->w;
		float rayAngle = (m_player->angle + m_fov / 2) - columnRatio * m_fov;

		float distToWall, tx, ty;
		bool xSide;
		TraceRay(m_player->pos, rayAngle, distToWall, tx, xSide);

		int wallTop = (int)((float)(buffer->h) / 2 - (m_wallScaleFactor * buffer->h) / ((float)distToWall));
		int wallBottom = buffer->h - wallTop;

		// draw the column
		Uint32 color;
		for (int y = 0; y < buffer->h; y++) {
			if (y <= wallTop) { // ceiling
				color = SDL_MapRGB(buffer->format, 0xFF, 0xAF, 0x41);
			}
			else if (y > wallTop && y <= wallBottom) { // wall
				// how far is y between wallTop and wallBottom
				ty = (y - wallTop) / (float)(wallBottom - wallTop);
				color = SampleTexture(m_bricks, tx, ty);

			}
			else { // floor
				color = SDL_MapRGB(buffer->format, 0x95, 0xA5, 0xA6);
			}
			if (m_flipView)
				SetPixel(buffer, buffer->w - 1 - x, buffer->h - 1 - y, color);
			else
				SetPixel(buffer, x, y, color);
			m_depthBuffer[y*buffer->w + x] = distToWall;
		}
	}
}

void MazeDemo::RenderSprites(SDL_Surface *buffer) {
	// draw sprites
	auto sprites = m_maze->GetSprites();
	for (size_t i = 0; i < sprites.size(); i++) {
		auto sprite = sprites[i];
		if (sprite->removed)
			continue;
		Vec2f playerView = m_player->GetViewVector();
		Vec2f dist = sprite->pos - m_player->pos;
		float distToSprite = dist.Length();
		if (distToSprite < 0.5f)
			continue;
		dist.Normalize();
		float playerAngle = SDL_atan2f(playerView.x, -playerView.y); // internal angle might be twisted too far
		float sAngle = SDL_atan2f(dist.x, -dist.y);
		float angleDiff = AngleDiff(playerAngle, sAngle);
		if (std::abs(angleDiff) > (m_fov / 2))
			continue;

		// fisheye compensation necessary?
		if (m_fisheyeCorrection)
			distToSprite *= SDL_cosf(std::abs(angleDiff));
		float minViewableAngle = playerAngle - m_fov / 2;
		float xFactor = -AngleDiff(playerAngle - m_fov / 2, sAngle) / m_fov; // 0 - 1.0 representing the object's X coord within the bounds of the FOV
		// Not really sure why this works just kinda tweaked it til it did

		float sHeight = 2 * (m_spriteScaleFactor * buffer->h) / (float)distToSprite;
		float sCenterY = (buffer->h / 2) + sprite->offsetY * m_spriteScaleFactor / distToSprite;
		float sTop = sCenterY - sHeight / 2;
		float sBottom = sCenterY + sHeight / 2;
		float sAspectRatio = (float)sprite->bitmap->h / sprite->bitmap->w;
		float sWidth = sHeight / sAspectRatio;
		float sMiddle = xFactor * buffer->w;

		Uint32 transparent = SDL_MapRGB(sprite->bitmap->format, 0xFF, 0x00, 0xFF);
		// scaled blit would be good but need to draw per-pixel to utilise depth buffer
		for (int x = 0; x < sWidth; x++) {
			for (int y = 0; y < sHeight; y++) {
				float sX = x / sWidth;
				float sY = y / sHeight;
				Uint32 pixel = SampleTexture(sprite->bitmap, sX, sY);
				if (pixel == transparent)
					continue;
				int dX = (int)sMiddle - (int)(sWidth / 2) + x;
				int dY = (int)sTop + y;
				if (dX < 0 || dX >= buffer->w || dY < 0 || dY >= buffer->h)
					continue;
				if (distToSprite < m_depthBuffer[dY*buffer->w + dX]) {
					if (m_flipView)
						SetPixel(buffer, buffer->w - 1 - dX, buffer->h - 1 - dY, pixel);
					else
						SetPixel(buffer, dX, dY, pixel);
					m_depthBuffer[dY*buffer->w + dX] = distToSprite;
				}
			}
		}
	}
}
