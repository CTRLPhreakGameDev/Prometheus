#pragma once

#include "core/input/input.hpp"
#include "gameplay/bullet/bullet.hpp"
#include "gameplay/weapon/weapon.hpp"
#include "gameplay/enemies/common.hpp"
#include "gameplay/player.hpp"
#include "gfx/cameraFollow.hpp"
#include "raylib.h"
#include <vector>

enum class GameState
{
	Playing,
	BetweenWaves,
	GameOver,
};

class Game {
	public:
  		void Run();

	private:
  		void Update(float dt);
  		void Draw();
		void Reset();
		void SpawnWave();
		void DrawHud();

  		Vector2 GetMouseWorldPos() const;

  		RenderTexture2D target_;
  		static constexpr int kRenderW = 800;
  		static constexpr int kRenderH = 450;

  		Camera2D camera_{};
  		CameraFollow camFollow_{};

  		Input input_{};
  		Player player_{{0.0f, 0.0f}};
		std::vector<Enemy> enemies_;


 		std::vector<Rectangle> walls_ = {{100, 200, 300, 30}, {500, 120, 40, 220}};

  		std::vector<Weapon> weapons_;
  		int currentWeapon_ = 0;

  		std::vector<Bullet> playerBullets_;
  		std::vector<Bullet> enemyBullets_;

		GameState state_ = GameState::Playing;

		int wave_ = 1;
		float waveTimer_ = 0.0f;
		int score_ = 0;

		static constexpr float kWaveDelay = 3.0f;

		Sound sfxShoot_;
		Sound sfxHit_;
		Sound sfxDeath_;

		Texture2D texEnemy_{};
		Texture2D texBullet_{};
		float playerAngle_ = 0.0f;
};
