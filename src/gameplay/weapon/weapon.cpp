#include "gameplay/weapon/weapon.hpp"
#include <cmath>

static Vector2 RotateDir(Vector2 dir, float degrees)
{
	float rad = degrees * DEG2RAD;
	float c = cosf(rad);
	float s = sinf(rad);
	return { dir.x * c - dir.y * s, dir.x * s + dir.y * c };
}

std::vector<Bullet> Weapon::Update(bool fireHeld, Vector2 origin, Vector2 aimDir, float dt)
{
	std::vector<Bullet> fired;

	if (cooldownTimer_ > 0.0f)
		cooldownTimer_ -= dt;

	if (fireHeld && cooldownTimer_ <= 0.0f)
	{
		cooldownTimer_ = config_.fireRate;

		for (int i = 0; i < config_.bulletsPerShot; i++)
		{
			float offset = 0.0f;
			if (config_.bulletsPerShot > 1)
			{
				float step =  config_.spreadDegrees / (config_.bulletsPerShot - 1);
				offset = -config_.spreadDegrees / 2.0f + i * step;
			}

			Vector2 dir = RotateDir(aimDir, offset);

			Bullet b;
			b.position = origin;
			b.velocity = { dir.x * config_.bulletSpeed, dir.y * config_.bulletSpeed };
			b.radius = config_.bulletRadius;
			b.lifetime = config_.bulletLifetime;
			fired.push_back(b);
		}
	}

	return fired;
}
