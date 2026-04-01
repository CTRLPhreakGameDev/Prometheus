#pragma once

#include "core/input/input.hpp"
#include "gameplay/bullet/bullet.hpp"
#include "gameplay/weapon/weapon.hpp"
#include "gameplay/enemies/common.hpp"
#include "gameplay/player.hpp"
#include "gfx/cameraFollow.hpp"
#include "raylib.h"
#include <vector>
#include <array>

enum class GameState
{
	MainMenu,
	Options,
	Playing,
	BetweenWaves,
	GameOver,
	Paused,
};

struct Star
{
	Vector2 position;
	Vector2 speed;
	float radius;
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
		void DrawMainMenu();
		void DrawOptions();
		void UpdateMainMenu();
		void InitStars();
		void UpdateStars(float dt);
		void DrawPauseMenu();

		GameState stateBeforePause_ = GameState::Playing;

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

		GameState state_ = GameState::MainMenu;

		int wave_ = 1;
		float waveTimer_ = 0.0f;
		int score_ = 0;

		static constexpr float kWaveDelay = 3.0f;

		Sound sfxShoot_;
		Sound sfxHit_;
		Sound sfxDeath_;

		Texture2D texEnemy_{};
		Texture2D texBullet_{};
		Texture2D texFredrick_{};
		Texture2D texHair_{};
		Texture2D texBg_{};
		float playerAngle_ = 0.0f;
		float walterAngle_ = 0.0f;

		std::array<Star, 150> stars_;

		bool shouldQuit_ = false;
};
