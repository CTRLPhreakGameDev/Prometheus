#include "gameplay/enemies/common.hpp"
#include <cmath>

static float VectorLengthCustom(Vector2 v)
{
	return std::sqrt(v.x * v.x + v.y * v.y);
}

static Vector2 NormalizeSafe(Vector2 v)
{
	float len {VectorLengthCustom(v)};
	if (len <= 0.0001f)
		return {0.f, 0.0f};
	return { v.x / len, v.y / len };
}

Enemy::Enemy(Vector2 startPos, float moveSpeed, float size)
{
	position = {startPos};
	speed = {moveSpeed};
	radius = {size};
}

Vector2 Enemy::GetDirectionToPlayer(Vector2 playerPos) const
{
	Vector2 toPlayer = { playerPos.x - position.x, playerPos.y - position.y };

	return NormalizeSafe(toPlayer);
}

void Enemy::Update(Vector2 playerPos, float dt, const std::vector<Rectangle>& walls)
{
	if (!active)
		return;

	Vector2 toPlayer = { playerPos.x - position.x, playerPos.y - position.y };
	Rectangle hb {Hitbox()};

	float distance {VectorLengthCustom(toPlayer)};
	Vector2 dir {GetDirectionToPlayer(playerPos)};

	float stopDistance {24.0f};
	if (distance > stopDistance)
	{
		position.x += dir.x * speed * dt;
		position.y += dir.y * speed * dt;
	}
	
	hb = Hitbox();

	for (const Rectangle& w : walls)
	{
		if (CheckCollisionRecs(hb, w))
		{
			if (position.x > 0)
			{
				position.x = w.x - hb.width / 2;
			}
			else 
			{
				position.x = (w.x + w.width) + hb.width / 2;
			}

			hb = Hitbox();
		}
	}

	hb = Hitbox();

	for (const Rectangle& w : walls)
	{
		if (CheckCollisionRecs(hb, w))
		{
			if (position.y > 0)
			{
				position.y = w.y - hb.height / 2;
			}
			else 
			{
				position.y = (w.y + w.height) + hb.height / 2;
			}
		}
	}

	if (shootTimer > 0.0f)
		shootTimer -= dt;

	if (distance <= shootRange && shootTimer <= 0.0f)
	{
		shootTimer = shootCooldown;
		// shoot
	}
}

void Enemy::Draw() const
{
	if (!active)
		return;

	DrawCircleV(position, radius, RED);
	DrawCircleLines((int)position.x, (int)position.y, shootRange, Fade(DARKPURPLE, 0.15f));
}
