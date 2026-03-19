#include "core/game.hpp"
#include <algorithm>

void Game::Run() {
  InitWindow(800, 450, "game");
  SetTargetFPS(60);

  camera_.offset = {400.0f, 225.0f};
  camera_.rotation = 0.0f;
  camera_.zoom = 1.0f;

  // Start camera on player
  camera_.target = player_.Pos();

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    input_.Update();
    Update(dt);

    BeginDrawing();
    ClearBackground(BLACK);
    Draw();
    EndDrawing();
  }

  CloseWindow();
}

void Game::Update(float dt) {
  player_.Update(input_, dt, walls_);
  camFollow_.Update(camera_, player_.Pos(), dt);
  enemy.Update(player_.Pos(), dt, walls_);

  if (IsKeyPressed(KEY_SPACE))
    SpawnPlayerBullet();

  for (Bullet &b : playerBullets_)
    b.Update(dt, walls_);

  for (Bullet &b : enemyBullets_)
    b.Update(dt, walls_);

  auto isDead = [](const Bullet &b) { return !b.active; };

  playerBullets_.erase(
      std::remove_if(playerBullets_.begin(), playerBullets_.end(), isDead),
      playerBullets_.end());
  enemyBullets_.erase(
      std::remove_if(enemyBullets_.begin(), enemyBullets_.end(), isDead),
      enemyBullets_.end());
}

void Game::Draw() {
  BeginMode2D(camera_);

  for (const Rectangle &r : walls_) {
    DrawRectangleLinesEx(r, 1, DARKGRAY);
  }

  player_.Draw();
  enemy.Draw();

  for (const Bullet &b : playerBullets_)
    b.Draw();

  for (const Bullet &b : enemyBullets_)
    b.Draw();

  EndMode2D();

  DrawText("WASD to move", 10, 10, 20, RAYWHITE);
}

void Game::SpawnPlayerBullet() {
  Vector2 dir = enemy.GetDirectionToPlayer(
      player_.Pos()); // placeholder — replace with mouse aim later
  // flip it so it goes FROM player TOWARD enemy
  dir = {-dir.x, -dir.y};

  Bullet b;
  b.position = player_.Pos();
  b.velocity = {dir.x * 400.0f, dir.y * 400.0f}; // 400 = bullet speed
  playerBullets_.push_back(b);
}
