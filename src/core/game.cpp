#include "core/game.hpp"
#include <algorithm>
#include <cmath>

void Game::InitStars()
{
	for (Star& s : stars_)
	{
		s.position = { (float)(rand() % kRenderW), (float)(rand() % kRenderH) };
		s.speed = { (float)(rand() % -10) - (rand() % 10), (float)((rand() % -15) - (rand() % 10)) };
		s.radius = (s.speed.y > 5.5f) ? 1.0f : 1.0f;
	}
}

void Game::UpdateStars(float dt)
{
	for (Star& s : stars_)
	{
		s.position.y += s.speed.y * dt;
		s.position.x += s.speed.x * dt;

		if (s.position.y > kRenderH)
		{
			s.position.y = 0.0f;
		}
		else if (s.position.y < 0.0f)
		{
			s.position.y = kRenderH;
		}

		if (s.position.x > kRenderW)
		{
			s.position.x = 0.0f;
		}
		else if (s.position.x < 0.0f)
		{
			s.position.x =  kRenderW;
		}

	}
}

static bool MouseOverRect(Rectangle r)
{
	int sw = GetScreenWidth();
	int sh = GetScreenHeight();
	float scale = std::min((float)sw / 800, (float) sh / 450);
	float offsetX = (sw - 800 * scale) / 2.0f;
	float offsetY = (sh - 450 * scale) / 2.0f;
	Vector2 mouse = GetMousePosition();
	Vector2 rPos = { (mouse.x - offsetX) / scale, (mouse.y - offsetY) / scale };

	return CheckCollisionPointRec(rPos, r);
}

void Game::AdvanceWorld()
{
	if (texBg_.width > 0)
		UnloadTexture(texBg_);

	const WorldConfig& w = worlds_[currentWorld_];

	if (!w.bgTexture.empty())
	{
		texBg_ = LoadTexture(w.bgTexture.c_str());
		SetTextureFilter(texBg_, TEXTURE_FILTER_POINT);
		if (texBg_.width == 0)
			TraceLog(LOG_WARNING, "Failed to load bg %s", w.bgTexture.c_str());
	}
	else
	{
		texBg_ = {};
	}
	
	walls_ = w.walls;
}

void Game::UpdateMainMenu()
{
	Rectangle playRect = { 300, 180, 200, 36 };
	Rectangle optionsRect = { 300, 230, 200, 36 };
	Rectangle quitRect = { 300, 280, 200, 36 };

	if (input_.Attack())
	{
		if (MouseOverRect(playRect))
		{
			Reset();
		}
		else if (MouseOverRect(optionsRect))
		{
			state_ = GameState::Options;
		}
		else if (MouseOverRect(quitRect))
		{
			CloseWindow();
		}
	}
}

void Game::DrawMainMenu()
{
	for (const Star& s : stars_)
	{
		unsigned char alpha = (s.radius > 1.0f) ? 255 : 180;
		DrawCircleV(s.position, s.radius, {255, 255, 255, alpha});
	}

	if (texFredrick_.width > 0)
	{
		float scale = (float)kRenderW / texFredrick_.width;
		float dstH = texFredrick_.height * scale;
		float dstY = kRenderH - dstH;

		Rectangle src = { 0, 0, (float)texFredrick_.width, (float)texFredrick_.height };
		Rectangle dst = { 0, dstY, (float)kRenderW, dstH };

		DrawTexturePro(texFredrick_, src, dst, { 0, 0 }, 0.0f, WHITE);
	}

	const char* title = "OPERATION PROMETHEUS";
	int titleW = MeasureText(title, 55);
	DrawText(title, kRenderW / 2 - titleW / 2, 60, 55, RAYWHITE);

	Rectangle playRect = { 300, 180, 200, 36 };
	Rectangle optionsRect = { 300, 230, 200, 36 };
	Rectangle quitRect = { 300, 280, 200, 36 };

	auto drawButton = [](Rectangle r, const char* label)
	{
		Color bg = MouseOverRect(r) ? Fade(RAYWHITE, 0.25f) : Fade(RAYWHITE, 0.08f);
		DrawRectangleRec(r, bg);
		DrawRectangleLinesEx(r, 1, RAYWHITE);

		int tw = MeasureText(label, 20);
		DrawText(label, (int)(r.x + r.width / 2 - tw / 2), (int)(r.y + 8), 20, RAYWHITE);
	};

	drawButton(playRect, "PLAY");
	drawButton(optionsRect, "OPTIONS");
	drawButton(quitRect, "QUIT");
}

void Game::DrawOptions()
{
	for (const Star& s : stars_)
		DrawCircleV(s.position, s.radius, {255, 255, 255, 180});

	const char* title = "CONTROLS";
	int tw = MeasureText(title, 40);
	DrawText(title, kRenderW / 2 - tw / 2, 60, 40, RAYWHITE);

	const char* lines[] = {
		"WASD / Arrow Keys  -  Move",
		"Left Mouse Button  -  Shoot",
		"Left Alt           -  Dash",
		"Scroll Wheel       -  Switch Weapons",
		"Tab                -  Show Score",
		"R                  -  Restart (game over)",
	};

	int y = 140;
	for (const char* line : lines)
	{
		int lw = MeasureText(line, 18);
		DrawText(line, kRenderW / 2 - lw / 2, y, 18, LIGHTGRAY);
		y += 32;
	}

	Rectangle backRect = { 300, 370, 200, 36 };
	auto drawButton = [](Rectangle r, const char* label)
    	{
        	Color bg = MouseOverRect(r) ? Fade(RAYWHITE, 0.25f) : Fade(RAYWHITE, 0.08f);
        	DrawRectangleRec(r, bg);
        	DrawRectangleLinesEx(r, 1, RAYWHITE);

        	int tw = MeasureText(label, 20);
        	DrawText(label, (int)(r.x + r.width / 2 - tw / 2), (int)(r.y + 8), 20, RAYWHITE);
    	};

	drawButton(backRect, "BACK");

	if (input_.Attack() && MouseOverRect(backRect))
		state_ = GameState::MainMenu;
}

void Game::DrawHud()
{
	float barWidth = 200.0f;
	float barHeight = 12.0f;
	float x = 110 - barWidth / 2.0f;
	float y = kRenderH - 24.0f;

	DrawRectangle((int)x, (int)y, (int)barWidth, (int)barHeight, DARKGRAY);

	float ratio = (float)player_.hp / (float)player_.maxHp;
	float filledWidth = barWidth * ratio;

	DrawRectangle((int)x, (int)y, (int)filledWidth, (int)barHeight, RED);

	DrawRectangleLinesEx({x, y, barWidth, barHeight}, 1, RAYWHITE);
}

void Game::DrawBetweenWorlds()
{
	DrawRectangle(0, 0, kRenderW, kRenderH, Fade(BLACK, 0.75f));

	const WorldConfig& finishedWorld = worlds_[currentWorld_ - 1];

	const char* title = TextFormat("%s - Complete!", finishedWorld.name.c_str());
	int titleW = MeasureText(title, 36);
	DrawText(title, kRenderW / 2 - titleW / 2, 100, 36, GREEN);

	const char* wScore = TextFormat("World Score: %d", worldScore_);
	int wScoreW = MeasureText(wScore, 22);
	DrawText(wScore, kRenderW / 2 - wScoreW / 2, 175, 22, YELLOW);

	const char* tScore = TextFormat("Total Score: %d", score_);
	int tScoreW = MeasureText(tScore, 22);
	DrawText(tScore, kRenderW / 2 - tScoreW / 2, 210, 22, RAYWHITE);

	if (currentWorld_ < (int)worlds_.size())
	{
		const char* next = TextFormat("Up Next: %s", worlds_[currentWorld_].name.c_str());
		int nextW = MeasureText(next, 20);
		DrawText(next, kRenderW / 2 - nextW / 2, 275, 20, LIGHTGRAY);
	}
	else
	{
		const char* fin = "We Reached the planet!";
		int finW = MeasureText(fin, 20);
		DrawText(fin, kRenderW / 2 - finW / 2, 275, 20, GOLD);
	}

	const char* prompt = "Press ENTER to Continue";
	int promptW = MeasureText(prompt, 18);
	DrawText(prompt, kRenderW / 2 - promptW / 2, 300, 18, Fade(RAYWHITE, 0.75f));
}

void Game::SpawnWave()
{
	enemies_.clear();

	const WorldConfig& world = worlds_[currentWorld_];

	int count = std::min(2 + (wave_ - 1), 10);
	float speed = world.enemySpeedMult * std::min(80.0f + wave_ * 15.0f, 220.0f);
	int hp = world.enemyHpBase + wave_ * 10;

	std::vector<Vector2> spawnPoints = {
		{ 900.0f,  500.0f},
        {-400.0f,  300.0f},
        { 200.0f, -500.0f},
        { 700.0f, -300.0f},
        {-600.0f, -200.0f},
        { 400.0f,  600.0f},
        {-200.0f,  600.0f},
        { 800.0f,    0.0f},
        {   0.0f, -700.0f},
        {-700.0f,  400.0f},
	};

	for (int i = 0; i < count; i++)
	{
		Vector2 pos = spawnPoints[i % spawnPoints.size()];

		Enemy e;

		switch (currentWorld_)
		{
			case 0:
				e = MakeBasicEnemy(pos, speed, hp);
				break;
			
			case 1:
				e = (i % 2 == 0) ? MakeBasicEnemy(pos, speed, hp) : MakeFastEnemy(pos, speed, hp);
				break;

			case 2:
				if (i % 3 == 0) 
					e = MakeTankEnemy(pos, speed, hp);
				else if (i % 3 == 1)
					e = MakeFastEnemy(pos, speed, hp);
				else
					e = MakeBasicEnemy(pos, speed, hp);
				break;

			default:
				e = (i % 2 == 0) ? MakeTankEnemy(pos, speed, hp) : MakeFastEnemy(pos, speed, hp);
				break;
		}

		e.sprite = &texEnemy_;
		enemies_.push_back(e);
	}
}

void Game::Reset()
{
	player_ = Player{{0.0f, 0.0f}};
	player_.LoadSprite("assets/sprites/player.png", 3);
	camera_.target = player_.Pos();

	playerBullets_.clear();
	enemyBullets_.clear();
	currentWeapon_ = 0;
	wave_ = 1;
	score_ = 0;
	worldScore_ = 0;
	currentWorld_ = 0;
	state_ = GameState::Playing;

	AdvanceWorld();
	SpawnWave();
}

void Game::Run()
{
	SetConfigFlags(FLAG_FULLSCREEN_MODE);
	InitWindow(0, 0, "game");
	SetTargetFPS(60);
	SetExitKey(0);

	InitAudioDevice();

	sfxShoot_ = LoadSound("assets/sfx/shoot.wav");
	sfxHit_ = LoadSound("assets/sfx/hit.wav");
	sfxDeath_ = LoadSound("assets/sfx/death.wav");

	SetSoundVolume(sfxShoot_, 0.4f);
	SetSoundVolume(sfxHit_, 0.6f);
	SetSoundVolume(sfxDeath_, 0.8f);

	texEnemy_ = LoadTexture("assets/sprites/walter.png");
	texBullet_ = LoadTexture("assets/sprites/bullet.png");
	texFredrick_ = LoadTexture("assets/sprites/fredrick.png");
	texHair_ = LoadTexture("assets/sprites/crosshair.png");

	if (texEnemy_.width == 0) TraceLog(LOG_WARNING, "Failed to load enemy sprite");
	if (texBullet_.width == 0) TraceLog(LOG_WARNING, "Failed to load bullet sprite");
	if (texFredrick_.width == 0) TraceLog(LOG_WARNING, "Failed to load Fredrick");
	if (texHair_.width == 0) TraceLog(LOG_WARNING, "Failed to load the hair");

	SetTextureFilter(texEnemy_, TEXTURE_FILTER_POINT);
	SetTextureFilter(texBullet_, TEXTURE_FILTER_POINT);
	SetTextureFilter(texFredrick_, TEXTURE_FILTER_POINT);
	SetTextureFilter(texHair_, TEXTURE_FILTER_POINT);

	target_ = LoadRenderTexture(kRenderW, kRenderH);
	SetTextureFilter(target_.texture, TEXTURE_FILTER_POINT);

	camera_.offset = { kRenderW / 2.0f, kRenderH / 2.0f };
	camera_.rotation = 0.0f;
	camera_.zoom = 1.0f;

	weapons_ = {
		Weapon({"Pistol", 400.0f, 0.3f, 4.0f, 3.0f, 1, 0.0f, 10}),
		Weapon({"Shotgun", 350.0f, 1.2f, 4.5f, 2.0f, 6, 30.0f, 8}),
		Weapon({"Super MEGA Gun", 500.0f, 0.08f, 3.0f, 2.0f, 1, 0.0f, 5}),
		Weapon({"Deletifier", 1000.0f, 0.0f, 5.0f, 10.0f, 1, 0.0f, 1000}),
	};

	worlds_ = {
		WorldConfig{
			/*name*/"SEC 1",
			/*bgTexture*/"assets/sprites/lvl1bg.png",
			/*maxWaves*/3,
			/*enemySpeedMult*/1.0f,
			/*enemyHpBase*/80,
			/*walls*/{ {100, 200, 300, 30}, {500, 120, 40, 220} }
		},
		WorldConfig{
			/*name*/"SEC 2",
			/*bgTexture*/"assets/sprites/lvl2bg.png",
			/*maxWaves*/6,
			/*enemySpeedMult*/1.25f,
			/*enemyHpBase*/100,
			/*walls*/{ {-300, 100, 40, 300}, {200, -200, 250, 30}, {400, 300, 30, 200} }
		},
		WorldConfig{
			/*name*/"SEC 3",
			/*bgTexture*/"assets/sprites/lvl3bg.png",
			/*maxWaves*/10,
			/*enemySpeedMult*/1.5f,
			/*enemyHpBase*/130,
			/*walls*/{ {0, 300, 500, 20}, {-400, -100, 30, 400}, {300, -300, 200, 30} }
		},
		WorldConfig{
			/*name*/"SEC 4",
			/*bgTexture*/"assets/sprites/lvl4bg.png",
			/*maxWaves*/15,
			/*enemySpeedMult*/1.75f,
			/*enemyHpBase*/170,
			/*walls*/{ {-200, 200, 400, 25}, {200, -200, 25, 400}, {-300, -300, 300, 25} }
		},
	};

	InitStars();

	while (!WindowShouldClose() && !shouldQuit_)
	{
		float dt = GetFrameTime();

		input_.Update();
		UpdateStars(dt);
		Update(dt);

		BeginDrawing();
		ClearBackground(BLACK);
		Draw();
		EndDrawing();
	}

	player_.UnloadSprite();

	UnloadTexture(texEnemy_);
	UnloadTexture(texBullet_);
	UnloadTexture(texFredrick_);
	UnloadTexture(texHair_);
	if (texBg_.width > 0)
		UnloadTexture(texBg_);

	UnloadSound(sfxShoot_);
	UnloadSound(sfxHit_);
	UnloadSound(sfxDeath_);

	UnloadRenderTexture(target_);

	CloseAudioDevice();
	CloseWindow();
}

void Game::Update(float dt)
{
	if (state_ == GameState::MainMenu || state_ == GameState::Options)
	{
		UpdateMainMenu();
		return;
	}

	if (state_ == GameState::GameOver)
	{
		if (input_.Restart())
			Reset();

		return;
	}

	if (state_ == GameState::BetweenWorlds)
	{
		if (input_.Enter())
		{
			if (currentWorld_ < (int)worlds_.size())
			{
				wave_ = 1;
				worldScore_ = 0;
				player_.hp += 25;
				AdvanceWorld();
				SpawnWave();
				state_ = GameState::Playing;
			}
			else
			{
				state_ = GameState::MainMenu;
			}
		}
		
		return;
	}

	if (input_.Pause())
	{
		if (state_ == GameState::Paused)
		{
			state_ = stateBeforePause_;
		}
		else
		{
			stateBeforePause_ = state_;
			state_ = GameState::Paused;
		}

		return;
	}

	if (state_ == GameState::Paused && input_.Quit())
	{
		
		shouldQuit_ = true;
		
		return;
	}

	if (state_ == GameState::Paused)
		return;

	if (state_ == GameState::BetweenWaves)
	{
		waveTimer_ -= dt;

		if (waveTimer_ <= 0.0f)
		{
			wave_++;
			
			if (wave_ > worlds_[currentWorld_].maxWaves)
			{
				currentWorld_++;
				state_ = GameState::BetweenWorlds;
			}
			else
			{
				SpawnWave();
				state_ = GameState::Playing;
			}
		}

		player_.Update(input_, dt, walls_);
		camFollow_.Update(camera_, player_.Pos(), dt);

		return;
	}

	player_.Update(input_, dt, walls_);
	camFollow_.Update(camera_, player_.Pos(), dt);

	if (player_.hp <= 0)
	{
		PlaySound(sfxDeath_);
		state_ = GameState::GameOver;
		return;
	}

	for (Enemy& e : enemies_)
	{
		if (auto shot = e.Update(player_.Pos(), dt, walls_))
		{
			shot->sprite = &texBullet_;
			enemyBullets_.push_back(*shot);
		}
	}

	int scroll = (int)GetMouseWheelMove();
	if (scroll != 0)
	{
		currentWeapon_ = (currentWeapon_ - scroll + (int)weapons_.size()) % (int)weapons_.size();
	}

	Vector2 mouseWorld = GetMouseWorldPos();
	Vector2 dir = Vector2Normalize(Vector2Subtract(mouseWorld, player_.Pos()));

	playerAngle_ = atan2f(dir.y, dir.x) * RAD2DEG + 90.0f;

	auto newBullets = weapons_[currentWeapon_].Update(input_.AttackHeld(), player_.Pos(), dir, dt);

	if (!newBullets.empty())
		PlaySound(sfxShoot_);

	for (Bullet& b : newBullets)
		b.sprite =  &texBullet_;

	playerBullets_.insert(playerBullets_.end(), newBullets.begin(), newBullets.end());

	for (Bullet &b : playerBullets_)
		b.Update(dt, walls_);

	for (Bullet &b : enemyBullets_)
		b.Update(dt, walls_);

	Rectangle playerHb = player_.Hitbox();

	for (Bullet &b : enemyBullets_)
	{
		if (b.active && CheckCollisionRecs(b.Hitbox(), playerHb))
		{
			b.active = false;
			PlaySound(sfxHit_);
			player_.hp -= 10;
		}
	}

	for (Enemy& e : enemies_)
	{
		if (e.active) 
		{
			Rectangle enemyHb = e.Hitbox();
			for (Bullet &b : playerBullets_)
				if (b.active && CheckCollisionRecs(b.Hitbox(), enemyHb)) 
				{
					b.active = false;
					e.hp -= weapons_[currentWeapon_].Damage();
					e.flashTimer = 0.15f;
				}
		}
	}

	auto isDead = [](const Bullet& b) { return !b.active; };

	playerBullets_.erase(std::remove_if(playerBullets_.begin(), playerBullets_.end(), isDead), playerBullets_.end());
	enemyBullets_.erase(std::remove_if(enemyBullets_.begin(), enemyBullets_.end(), isDead), enemyBullets_.end());

	auto isDeadEnemy =[](const Enemy& e) { return !e.active; };

	int kills = (int)std::count_if(enemies_.begin(), enemies_.end(), isDeadEnemy);
	score_ += kills * 100;
	worldScore_ += kills * 100;

	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), isDeadEnemy), enemies_.end());

	if (enemies_.empty())
	{
		state_ = GameState::BetweenWaves;
		waveTimer_ = kWaveDelay;

		playerBullets_.clear();
		enemyBullets_.clear();
	}
}

Vector2 Game::GetMouseWorldPos() const
{
	int sw = GetScreenWidth();
	int sh = GetScreenHeight();

	float scale = std::min((float)sw / kRenderW, (float)sh / kRenderH);
	float offsetX = (sw - kRenderW * scale) / 2.0f;
	float offsetY = (sh - kRenderH * scale) / 2.0f;

	Vector2 mouse = GetMousePosition();
	Vector2 renderPos = { (mouse.x - offsetX) / scale, (mouse.y - offsetY) / scale };

	return GetScreenToWorld2D(renderPos, camera_);
}

void Game::DrawPauseMenu()
{
	DrawRectangle(0, 0, kRenderW, kRenderH, Fade(BLACK, 0.55f));
	
	const char* title = "PAUSED";
	int titleW = MeasureText(title, 40);
	DrawText(title, kRenderW / 2.0f - titleW / 2.0f, kRenderH / 2 - 60, 40, RAYWHITE);

	const char* resume = "Press 'ESC' to Resume";
	int resumeW = MeasureText(resume, 20);
	DrawText(resume, kRenderW / 2 - resumeW / 2, kRenderH / 2, 20, LIGHTGRAY);

	const char* quit = "Press 'Q' to Quit";
	int quitW = MeasureText(quit, 20);
	DrawText(quit, kRenderW / 2 - quitW / 2, kRenderH / 2 + 30, 20, LIGHTGRAY);
}

void Game::Draw()
{
	BeginTextureMode(target_);
	ClearBackground({ 5, 5, 15, 255 });

	if (texBg_.width > 0)
		DrawTexture(texBg_, 0, 0, WHITE);

	if (state_ == GameState::MainMenu)
	{
		DrawMainMenu();
		ShowCursor();
	}
	else if (state_ == GameState::Options)
	{
		DrawOptions();
		ShowCursor();
	}
	else if (state_ == GameState::BetweenWorlds)
	{
		for (const Star& s : stars_)
		{
			unsigned char alpha = (s.radius > 1.0f) ? 255 : 180;
			DrawCircleV(s.position, s.radius, {255, 255, 255, alpha});
		}
		DrawBetweenWorlds();
		ShowCursor();
	}
	else 
	{
		for (const Star& s : stars_)
		{
			unsigned char alpha = (s.radius > 1.0f) ? 255 : 180;
			DrawCircleV(s.position, s.radius, {255, 255, 255, alpha});
		}
		
		BeginMode2D(camera_);

		HideCursor();

		Vector2 mouseWorld = GetMouseWorldPos();
		float size = 32.0f;
		Rectangle src = { 0, 0, (float)texHair_.width, (float)texHair_.height };
		Rectangle dst = { mouseWorld.x, mouseWorld.y, size, size };
		Vector2 origin = { size / 2.0f, size / 2.0f };
		DrawTexturePro(texHair_, src, dst, origin, 0.0f, WHITE);

		for (const Rectangle &r : walls_) 
		{
			DrawRectangleLinesEx(r, 1, DARKGRAY);
		}

		player_.Draw(playerAngle_);
	
		for (const Enemy& e : enemies_)
		{
			Vector2 dir = Vector2Normalize(Vector2Subtract(e.position, player_.Pos()));

			walterAngle_ = atan2f(dir.y, dir.x) * RAD2DEG + 270.0f;

			e.Draw(walterAngle_);
		}
		for (const Bullet &b : playerBullets_)
			b.Draw();

		for (const Bullet &b : enemyBullets_)
			b.Draw();

		EndMode2D();

		const char* waveMsg = TextFormat("Wave: %d", wave_);
		int waveW = MeasureText(waveMsg, 30);

		DrawText(weapons_[currentWeapon_].Name().c_str(), 10, kRenderH - 50.0f, 20, RAYWHITE);
		DrawHud();
		if (input_.GameInfo())
		{
			DrawText(TextFormat("Score: %d", score_), 10, 20, 20, YELLOW);
		}
		DrawText(waveMsg, kRenderW / 2 - waveW / 2, 10, 30, RAYWHITE);

		if (state_ == GameState::BetweenWaves)
		{
			const char* msg = TextFormat("Wave %d Complete!", wave_);
			int textW  = MeasureText(msg, 35);

			DrawText(msg, kRenderW / 2 - textW / 2, kRenderH / 2 - 40, 30, GREEN);

			const char* next = TextFormat("Next Wave in %.1f...", waveTimer_);
			int nextW = MeasureText(next, 20);
			DrawText(next, kRenderW / 2 - nextW / 2, kRenderH / 2, 20, RAYWHITE);
		}

		if (state_ == GameState::GameOver)
		{
			DrawRectangle(0, 0, kRenderW, kRenderH, Fade(BLACK, 0.6f));
			DrawText("GAME OVER", kRenderW / 2 - 90, kRenderH / 2 - 20, 40, RED);
		
			const char* scoreMsg = TextFormat("Final Score: %d", score_);
			int scoreW = MeasureText(scoreMsg, 20);
			DrawText(scoreMsg, kRenderW /2 -scoreW / 2, kRenderH / 2 + 20, 20, YELLOW);

			DrawText("Press R to Restart", kRenderW / 2 - 90, kRenderH / 2 + 50, 20, RAYWHITE);
		}

		if (state_ == GameState::Paused)
		{
			DrawPauseMenu();
		}
	}

	EndTextureMode();

	int sw = GetScreenWidth();
	int sh = GetScreenHeight();

	float scale = std::min((float)sw / kRenderW, (float)sh / kRenderH);

	float dstW = kRenderW * scale;
	float dstH = kRenderH * scale;
	float offsetX = (sw - dstW) / 2.0f;
	float offsetY = (sh - dstH) / 2.0f;

	Rectangle src = { 0.0f, 0.0f, (float)kRenderW, -(float)kRenderH };
	Rectangle dst = { offsetX, offsetY, dstW, dstH };

	DrawTexturePro(target_.texture, src, dst, {0.0f, 0.0f}, 0.0f, WHITE);
}
