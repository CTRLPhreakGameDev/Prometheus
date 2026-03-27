#pragma once

#include "raylib.h"
#include "gameplay/bullet/bullet.hpp"
#include <vector>
#include <optional>

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

	const Texture2D* sprite = nullptr;
	
	Enemy() = default;
	Enemy(Vector2 startPos, float moveSpeed, float size);

	std::optional<Bullet> Update(Vector2 playerPos, float dt, const std::vector<Rectangle>& walls);
	void Draw() const;

	bool CanShoot(Vector2 playerPos) const;
	Vector2 GetDirectionToPlayer(Vector2 playerPos) const;

	Rectangle Hitbox() const
	{
		return { position.x - radius, position.y - radius, radius * 2, radius * 2 };
	}
};
