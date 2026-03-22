#include "core/game.hpp"
#include <algorithm>

void Game::Reset()
{
	player_ = Player{{0.0f, 0.0f}};
	camera_.target = player_.Pos();

	playerBullets_.clear();
	enemyBullets_.clear();
	currentWeapon_ = 0;
	state_ = GameState::Playing;

	enemies_.clear();
	enemies_.push_back(Enemy{{900.0f, 500.0f}, 120.0f, 18.0f});
	enemies_.push_back(Enemy{{-400.0f, 300.0f}, 100.0f, 18.0f});
	enemies_.push_back(Enemy{{200.0f, -500.0f}, 80.0f, 18.0f});
}

void Game::Run()
{
  SetConfigFlags(FLAG_FULLSCREEN_MODE);
  InitWindow(0, 0, "game");
  SetTargetFPS(60);

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

  UnloadRenderTexture(target_);
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

  player_.Update(input_, dt, walls_);
  camFollow_.Update(camera_, player_.Pos(), dt);

  if (player_.hp <= 0)
  {
	  state_ = GameState::GameOver;
	  return;
  }

  for (Enemy& e : enemies_)
  {
  	if (auto shot = e.Update(player_.Pos(), dt, walls_))
    		enemyBullets_.push_back(*shot);
  }

  int scroll = (int)GetMouseWheelMove();
  if (scroll != 0)
  {
	  currentWeapon_ = (currentWeapon_ - scroll + (int)weapons_.size()) % (int)weapons_.size();
  }

  Vector2 mouseWorld = GetMouseWorldPos();
  Vector2 dir = Vector2Normalize(Vector2Subtract(mouseWorld, player_.Pos()));

  auto newBullets = weapons_[currentWeapon_].Update(input_.AttackHeld(), player_.Pos(), dir, dt);
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
  enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), isDeadEnemy), enemies_.end());
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

  player_.Draw();
  
  for (const Enemy& e : enemies_)
	  e.Draw();

  for (const Bullet &b : playerBullets_)
    b.Draw();

  for (const Bullet &b : enemyBullets_)
    b.Draw();

  EndMode2D();

  DrawText(weapons_[currentWeapon_].Name().c_str(), 10, 10, 20, RAYWHITE);
  DrawText(TextFormat("HP: %d", player_.hp), 10, 34, 20, RED);

  if (state_ == GameState::GameOver)
  {
	  DrawRectangle(0, 0, kRenderW, kRenderH, Fade(BLACK, 0.6f));
	  DrawText("GAME OVER", kRenderW / 2 - 90, kRenderH / 2 - 20, 40, RED);
	  DrawText("Press R to Restart", kRenderW / 2 - 90, kRenderH / 2 + 30, 20, RAYWHITE);
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
