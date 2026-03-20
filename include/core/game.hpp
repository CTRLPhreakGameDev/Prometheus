#pragma once

#include "core/input/input.hpp"
#include "gameplay/bullet/bullet.hpp"
#include "gameplay/weapon/weapon.hpp"
#include "gameplay/enemies/common.hpp"
#include "gameplay/player.hpp"
#include "gfx/cameraFollow.hpp"
#include "raylib.h"
#include <vector>

class Game {
public:
  void Run();

private:
  void Update(float dt);
  void Draw();

  Vector2 GetMouseWorldPos() const;

  RenderTexture2D target_;
  static constexpr int kRenderW = 800;
  static constexpr int kRenderH = 450;

  Camera2D camera_{};
  CameraFollow camFollow_{};

  Input input_{};
  Player player_{{0.0f, 0.0f}};
  Enemy enemy{{900.0f, 500.0f}, 120.0f, 18.0f};

  std::vector<Rectangle> walls_ = {{100, 200, 300, 30}, {500, 120, 40, 220}};

  std::vector<Weapon> weapons_;
  int currentWeapon_ = 0;

  std::vector<Bullet> playerBullets_;
  std::vector<Bullet> enemyBullets_;
};
