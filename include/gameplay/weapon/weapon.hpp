#pragma once

#include "gameplay/bullet/bullet.hpp"
#include "raylib.h"
#include <string>
#include <vector>

struct WeaponConfig
{
	std::string name;
	float bulletSpeed = 400.0f;
	float fireRate = 0.15f;
	float bulletRadius = 4.0f;
	float bulletLifetime = 3.0f;
	int bulletsPerShot = 1;
	float spreadDegrees = 0.0f;
	int damage = 10;
};

class Weapon
{
	public:
		explicit Weapon(WeaponConfig config) : config_(config) {}

		std::vector<Bullet> Update(bool fireHeld, Vector2 origin, Vector2 aimDir, float dt);

		const std::string& Name() const { return config_.name; }

		int Damage() const { return config_.damage; }

	private:
		WeaponConfig config_;
		float cooldownTimer_ = 0.0f;
};
