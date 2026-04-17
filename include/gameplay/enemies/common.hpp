#pragma once

#include "raylib.h"
#include "gameplay/bullet/bullet.hpp"
#include <vector>
#include <optional>

struct AIWeights
{
	float seek = 1.0f;
	float flee = 0.0f;
	float separation = 1.2f;
	float wallAvoid = 2.5f;
	float orbit = 2.5f;

	float preferredRange = 160.0f;
	float fleeRadius = 100.0f;
	float separationRadius = 80.0f;
	float wallFeelerLen = 50.0f;
};

struct Enemy
{
	Vector2 position{};
	float speed {100.0f};
	float radius {16.0f};

	float shootRange {250.0f};
	float shootCooldown {1.0f};
	float shootTimer {0.0f};

	bool active {true};
	int hp {80};

	float flashTimer { 0.0f };

	AIWeights ai{};

	const Texture2D* sprite = nullptr;
	
	Enemy() = default;
	Enemy(Vector2 startPos, float moveSpeed, float size);

	std::optional<Bullet> Update(Vector2 playerPos, float dt, const std::vector<Rectangle>& walls, const std::vector<Enemy>& allEemies);
	void Draw(float angle) const;

	Vector2 GetDirectionToPlayer(Vector2 playerPos) const;

	Rectangle Hitbox() const
	{
		return { position.x - radius, position.y - radius, radius * 2, radius * 2 };
	}

private:
	Vector2 SteerSeek(Vector2 playerPos) const;
	Vector2 SteerFlee(Vector2 playerPos) const;
	Vector2 SteerOrbit(Vector2 playerPos) const;
	Vector2 SteerSeparation(const std::vector<Enemy>& allEnemies) const;
	Vector2 SteerWallAvoid(const std::vector<Rectangle>& walls) const;
};

Enemy MakeBasicEnemy(Vector2 pos, float speed, int hp);
Enemy MakeFastEnemy(Vector2 pos, float speed, int hp);
Enemy MakeTankEnemy(Vector2 pos, float speed, int hp);
