#include "gameplay/bullet/bullet.hpp"

#include <vector>

#include "../../../cmake-build-default/_deps/raylib-src/src/raylib.h"

void Bullet::Update(float dt, const std::vector<Rectangle>& walls)
{
    if (!active)
        return;

    lifetime -= dt;
    if (lifetime <= 0.0f)
    {
        active = false;
        return;
    }

    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    hb = Hitbox();
    for (const Rectangle& w : walls)
    {
        if (CheckCollisionRecs(hb, w))
        {
            active = false;
            return;
        }
    }
}

void Bullet::Draw() const
{
    if (!active)
        return;
    DrawCircleV(position, radius, RED);
}
