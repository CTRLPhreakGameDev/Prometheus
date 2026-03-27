#include "gameplay/bullet/bullet.hpp"
#include <vector>
#include <cmath>

void Bullet::Update(float dt, const std::vector<Rectangle> &walls) 
{
  if (!active)
    return;

  Rectangle hb = Hitbox();

  lifetime -= dt;
  if (lifetime <= 0.0f) {
    active = false;
    return;
  }

  position.x += velocity.x * dt;
  position.y += velocity.y * dt;

  hb = Hitbox();
  for (const Rectangle &w : walls) {
    if (CheckCollisionRecs(hb, w)) {
      active = false;
      return;
    }
  }
}

void Bullet::Draw() const 
{
  if (!active)
    return;

  if (sprite)
  {
	  float size = 8.0f;

	  float angle = atan2f(velocity.x, velocity.y);

	  Rectangle src = { 0, 0, (float)sprite->width, (float)sprite->height };
	  Rectangle dst = { position.x, position.y, size, size };
	  Vector2 origin = { size / 2.0f, size / 2.0f };

	  DrawTexturePro(*sprite, src, dst, origin, angle, WHITE);
  }
  else
  {
	  DrawCircleV(position, radius, RED);
  }
}
