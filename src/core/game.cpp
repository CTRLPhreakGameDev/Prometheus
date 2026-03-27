#include "core/game.hpp"
#include <algorithm>
#include <cmath>

void Game::SpawnWave()
{
	enemies_.clear();

	int count = std::min(2 + (wave_ - 1), 10);
	float speed = std::min(80.0f + wave_ * 15.0f, 220.0f);

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
		Enemy e{spawnPoints[i % spawnPoints.size()], speed, 16.0f};
		e.sprite =  &texEnemy_;
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
	state_ = GameState::Playing;
	wave_ = 1;
	score_ = 0;

	SpawnWave();
}

void Game::Run()
{
	SetConfigFlags(FLAG_FULLSCREEN_MODE);
	InitWindow(0, 0, "game");
	SetTargetFPS(60);

	InitAudioDevice();

	sfxShoot_ = LoadSound("assets/sfx/shoot.wav");
	sfxHit_ = LoadSound("assets/sfx/hit.wav");
	sfxDeath_ = LoadSound("assets/sfx/death.wav");

	SetSoundVolume(sfxShoot_, 0.4f);
	SetSoundVolume(sfxHit_, 0.6f);
	SetSoundVolume(sfxDeath_, 0.8f);

	texEnemy_ = LoadTexture("assets/sprites/enemy.png");
	texBullet_ = LoadTexture("assets/sprites/bullet.png");

	if (texEnemy_.width == 0) TraceLog(LOG_WARNING, "Failed to load enemy sprite");
	if (texBullet_.width == 0) TraceLog(LOG_WARNING, "Failed to load bullet sprite");

	SetTextureFilter(texEnemy_, TEXTURE_FILTER_POINT);
	SetTextureFilter(texBullet_, TEXTURE_FILTER_POINT);

	target_ = LoadRenderTexture(kRenderW, kRenderH);
	SetTextureFilter(target_.texture, TEXTURE_FILTER_POINT);

	camera_.offset = { kRenderW / 2.0f, kRenderH / 2.0f };
	camera_.rotation = 0.0f;
	camera_.zoom = 1.0f;

	weapons_ = {
		Weapon({"Pistol", 400.0f, 0.3f, 4.0f, 3.0f, 1, 0.0f, 10}),
		Weapon({"Shotgun", 350.0f, 1.2f, 4.5f, 2.0f, 6, 30.0f, 8}),
		Weapon({"Super MEGA Gun", 500.0f, 0.08f, 3.0f, 2.0f, 1, 0.0f, 5}),
	};

	Reset();

	while (!WindowShouldClose())
	{
		float dt = GetFrameTime();

		input_.Update();
		Update(dt);

		BeginDrawing();
		ClearBackground(BLACK);
		Draw();
		EndDrawing();
	}

	player_.UnloadSprite();

	UnloadTexture(texEnemy_);
	UnloadTexture(texBullet_);

	UnloadSound(sfxShoot_);
	UnloadSound(sfxHit_);
	UnloadSound(sfxDeath_);

	UnloadRenderTexture(target_);

	CloseAudioDevice();
	CloseWindow();
}

void Game::Update(float dt)
{
  if (state_ == GameState::GameOver)
  {
	  if (IsKeyPressed(KEY_R))
		  Reset();

	  return;
  }

  if (state_ == GameState::BetweenWaves)
  {
	  waveTimer_ -= dt;

	  if (waveTimer_ <= 0.0f)
	  {
		  wave_++;
		  SpawnWave();
		  state_ = GameState::Playing;
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

void Game::Draw()
{
  BeginTextureMode(target_);
  ClearBackground(BLACK);

  BeginMode2D(camera_);

  for (const Rectangle &r : walls_) 
  {
    DrawRectangleLinesEx(r, 1, DARKGRAY);
  }

  player_.Draw(playerAngle_);
  
  for (const Enemy& e : enemies_)
	  e.Draw();

  for (const Bullet &b : playerBullets_)
    b.Draw();

  for (const Bullet &b : enemyBullets_)
    b.Draw();

  EndMode2D();

  const char* waveMsg = TextFormat("Wave: %d", wave_);
  int waveW = MeasureText(waveMsg, 30);

  DrawText(weapons_[currentWeapon_].Name().c_str(), 10, 10, 20, RAYWHITE);
  DrawText(TextFormat("HP: %d", player_.hp), 10, 34, 20, RED);
  DrawText(TextFormat("Score: %d", score_), 10, 58, 20, YELLOW);
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
