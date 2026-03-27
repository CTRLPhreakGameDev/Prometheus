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

std::optional<Bullet> Enemy::Update(Vector2 playerPos, float dt, const std::vector<Rectangle>& walls)
{
	if (!active)
		return std::nullopt;

	if (flashTimer > 0.0f) flashTimer -= dt;

	Vector2 toPlayer = { playerPos.x - position.x, playerPos.y - position.y };
	float distance {VectorLengthCustom(toPlayer)};
	Vector2 dir {GetDirectionToPlayer(playerPos)};

	float stopDistance {140.0f};

	float dx = 0.0f;
	float dy = 0.0f;

	if (distance > stopDistance)
	{
		dx = dir.x * speed * dt;
		dy = dir.y * speed * dt;
	}

	if (distance < stopDistance)
	{
		dx = dir.x * speed * dt * -1;
		dy = dir.y * speed * dt * -1;
	}

	// --- X axis ---
	position.x += dx;
	Rectangle hb = Hitbox();

	for (const Rectangle& w : walls)
	{
		if (CheckCollisionRecs(hb, w))
		{
			if (dx > 0)
				position.x = w.x - hb.width / 2;
			else
				position.x = (w.x + w.width) + hb.width / 2;
			hb = Hitbox();
		}
	}

	// --- Y axis ---
	position.y += dy;
	hb = Hitbox();

	for (const Rectangle& w : walls)
	{
		if (CheckCollisionRecs(hb, w))
		{
			if (dy > 0)
				position.y = w.y - hb.height / 2;
			else
				position.y = (w.y + w.height) + hb.height / 2;
			hb = Hitbox();
		}
	}

	if (shootTimer > 0.0f)
		shootTimer -= dt;

	if (distance <= shootRange && shootTimer <= 0.0f)
	{
		shootTimer = shootCooldown;
		
		Bullet b;
		b.position = position;
		b.velocity = { dir.x * 300.0f, dir.y * 300.0f };
		return b;
	}

	if (hp <= 0)
		active = false;

	return std::nullopt;
}

void Enemy::Draw() const
{
	if (!active)
		return;
	
	if (sprite)
	{
		float size = 32.0f;

		Rectangle src = { 0, 0, (float)sprite->width, (float)sprite->height };
		Rectangle dst = { position.x, position.y, size, size };
		Vector2 origin = { size / 2.0f, size / 2.0f };
		Color tint = (flashTimer > 0.0f) ? RED : WHITE;

		DrawTexturePro(*sprite, src, dst, origin, 0.0f, tint);
	}
	else 
	{
		Color c = (flashTimer > 0.0f) ? WHITE : PURPLE;
		DrawCircleV(position, radius, c);
	}

	DrawCircleLines((int)position.x, (int)position.y, shootRange, Fade(DARKPURPLE, 0.15f));
}
