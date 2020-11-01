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
	m_state(ST_START),
	m_stateChangeAfter(0.0f),
	m_rockToDelete(nullptr)
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

	m_mazeSolver = std::make_unique<RealTimeMazeSolver>(m_maze.get(), m_player.get());

	m_state = ST_START;
	m_stateChangeAfter = WALL_GROW_TIME;
	m_flipView = false;
}

bool MazeDemo::CollidedWithMap(Vec2f v) {
	int x = (int)v.x;
	int y = (int)v.y;
	if (x < 0 || x >= m_maze->Width() || y < 0 || y >= m_maze->Height())
		return true;

	return m_maze->GetBlock(x, y).Type == BL_SOLID;
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
	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_QUIT:
			m_isFinished = true;
			break;
		case SDL_KEYDOWN:
			if (ev.key.keysym.sym == SDLK_ESCAPE) {
				m_isFinished = true;
			}
			break;
		}
	}

	switch (m_state) {
	case ST_START:
	case ST_WALLGROW:
		// make walls grow then start running
		m_stateChangeAfter -= dt;
		m_wallScaleFactor = (WALL_GROW_TIME - m_stateChangeAfter) * DEFAULT_WALLSCALE;
		if (m_stateChangeAfter <= 0.0f) {
			m_wallScaleFactor = DEFAULT_WALLSCALE;
			m_state = ST_RUNNING;
		}
		break;
	case ST_RUNNING:
	{
		if (m_rockToDelete) {
			m_maze->RemoveObstacle(m_rockToDelete);
			m_rockToDelete = nullptr;
		}
		// run frame
		m_mazeSolver->Update(dt);
		// did we hit a rock or something to make us flip?
		Sprite *rock = HitRock(dt);
		if (rock) {
			m_rockToDelete = rock;
			m_stateChangeAfter = WALL_GROW_TIME;
			m_state = ST_WALLSHRINK;
			break;
		}
		// are we within range of the exit? stop moving, schedule a restart
		if (HitExit(dt)) {
			m_stateChangeAfter = TIME_TIL_RESTART;
			m_state = ST_FINISHED;
			break;
		}
		break;
	}
	case ST_WALLSHRINK:
		// TODO: test
		m_stateChangeAfter -= dt;
		m_wallScaleFactor = (m_stateChangeAfter) * DEFAULT_WALLSCALE;
		if (m_stateChangeAfter <= 0.0f) {
			m_flipView = !m_flipView;
			m_wallScaleFactor = 0.01f;
			m_stateChangeAfter = WALL_GROW_TIME;
			m_state = ST_WALLGROW;
		}
		break;
	case ST_FINISHED:
		// restart once the time runs out
		m_stateChangeAfter -= dt;
		if (m_stateChangeAfter <= 0.0f)
			Restart();
		break;
	}
}

void MazeDemo::Render(SDL_Surface *buffer)
{
	// should probs do this earlier
	if (m_depthBuffer == nullptr)
		m_depthBuffer = new float[buffer->w * buffer->h];

	SDL_FillRect(buffer, nullptr, SDL_MapRGB(buffer->format, 0, 0, 0));
	for (int i = 0; i < buffer->w * buffer->h; i++)
		m_depthBuffer[i] = std::numeric_limits<float>::max();

	// cache these because accessing smart pointers wrecks the FPS in debug mode for some reason
	int mazeW = m_maze->Width(), mazeH = m_maze->Height();

	for (int x = 0; x < buffer->w; x++) {
		// cast a ray for each column of the screen buffer
		// leftmost ray will be (player angle + fov/2)
		// rightmost ray will be (player angle - fov/2)
		float columnRatio = (float)x / buffer->w;
		float rayAngle = (m_player->angle + m_fov / 2) - columnRatio * m_fov;

		bool hitWall = false;
		float distToWall = 0;
		float tx = 0, ty = 0;

		Vec2f eye = Vec2f{ SDL_cosf(rayAngle), -SDL_sinf(rayAngle) };
		bool odd = false;

		// get distance to wall for this column's ray
		while (!hitWall && distToWall < MAX_RAYDEPTH) {
			distToWall += 0.01f; // todo: speed up by only testing on guaranteed x/y intercepts (i.e. wolf3d)

			int testX = (int)(m_player->pos.x + eye.x * distToWall);
			int testY = (int)(m_player->pos.y + eye.y * distToWall);

			// bounds check
			if (testX < 0 || testY < 0 || testX >= mazeW || testY >= mazeH) {
				hitWall = true;
				distToWall = MAX_RAYDEPTH;
				break;
			}
			MazeBlock block = m_maze->GetBlock(testX, testY);
			if (block.Type == BL_SOLID) { // we've got a hit
				hitWall = true;

				// Get texture sample x coord
				Vec2f blockMid(testX + 0.5f, testY + 0.5f);
				Vec2f testPoint = m_player->pos + (eye * distToWall);
				Vec2f offset = testPoint - blockMid;
				switch (RayHitDir(offset)) {
				case RH_TOP:
					tx = testPoint.x - testX;
					break;
				case RH_BOTTOM:
					tx = 1.0f - (testPoint.x - testX);
					break;
				case RH_LEFT:
					tx = testPoint.y - testY;
					break;
				case RH_RIGHT:
					tx = 1.0f - (testPoint.y - testY);
					break;
				}

				// correct for fisheye at larger fovs
				float theta = std::abs(rayAngle - m_player->angle);
				if (m_fisheyeCorrection) {
					float correction = SDL_cosf(theta);
					distToWall *= correction;
				}
			}
		}

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

	// draw sprites
	auto sprites = m_maze->GetSprites();
	for (int i = 0; i < sprites.size(); i++) {
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
		float sAspectRatio = sprite->bitmap->h / sprite->bitmap->w;
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
				int dX = (int)sMiddle - (sWidth / 2) + x;
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

	// minimap
	int blockSize = 8;
	if (m_showMiniMap)
		RenderMazePreview(m_maze.get(), *m_player, buffer, blockSize);
}
