#include "gameplay/enemies/common.hpp"
#include <algorithm>
#include <cmath>

// ------------------------------------------------------
// Small vector helpers
// ------------------------------------------------------

static Vector2 VAdd(Vector2 a, Vector2 b)
{
    return { a.x + b.x, a.y + b.y };
}

static Vector2 VSub(Vector2 a, Vector2 b)
{
    return { a.x - b.x, a.y - b.y };
}

static Vector2 VScale(Vector2 v, float s)
{
    return { v.x * s, v.y * s };
}

static float VLen(Vector2 v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

static Vector2 VNorm(Vector2 v)
{
    float len = VLen(v);

    if (len <= 0.0001f)
        return { 0.0f, 0.0f };

    return { v.x / len, v.y / len };
}

static Vector2 VPerp(Vector2 v)
{
    return { -v.y, v.x };
}

// ------------------------------------------------------
// Enemy basics
// ------------------------------------------------------

Enemy::Enemy(Vector2 startPos, float moveSpeed, float size)
    : position(startPos), speed(moveSpeed), radius(size)
{
}

Vector2 Enemy::GetDirectionToPlayer(Vector2 playerPos) const
{
    return VNorm(VSub(playerPos, position));
}

void Enemy::Draw(float angle) const
{
    if (!active)
        return;

    Color tint = (flashTimer > 0.0f) ? RED : WHITE;

    if (sprite && sprite->width > 0)
    {
        float size = radius * 2.0f;

        Rectangle src = {
            0.0f,
            0.0f,
            (float)sprite->width,
            (float)sprite->height
        };

        Rectangle dst = {
            position.x,
            position.y,
            size,
            size
        };

        Vector2 origin = { size / 2.0f, size / 2.0f };

        DrawTexturePro(*sprite, src, dst, origin, angle, tint);
    }
    else
    {
        DrawCircleV(position, radius, (flashTimer > 0.0f) ? RED : MAROON);
    }
}

// ------------------------------------------------------
// Steering behaviors
// ------------------------------------------------------

Vector2 Enemy::SteerSeek(Vector2 playerPos) const
{
    return VNorm(VSub(playerPos, position));
}

Vector2 Enemy::SteerFlee(Vector2 playerPos) const
{
    return VNorm(VSub(position, playerPos));
}

Vector2 Enemy::SteerOrbit(Vector2 playerPos) const
{
    Vector2 toPlayer = VSub(playerPos, position);
    float dist = VLen(toPlayer);

    if (dist <= 0.001f)
        return { 0.0f, 0.0f };

    Vector2 dir = VNorm(toPlayer);

    // Tangent = sideways motion around the player
    Vector2 tangent = VPerp(dir);

    // Small correction to keep enemy around preferred range
    float rangeDiff = dist - ai.preferredRange;
    Vector2 radial = VScale(dir, rangeDiff * 0.01f);

    return VNorm(VAdd(tangent, radial));
}

Vector2 Enemy::SteerSeparation(const std::vector<Enemy>& allEnemies) const
{
    Vector2 force{ 0.0f, 0.0f };

    for (const Enemy& other : allEnemies)
    {
        if (&other == this)
            continue;

        if (!other.active)
            continue;

        float dx = position.x - other.position.x;
        float dy = position.y - other.position.y;

        float dist = VLen({ dx, dy });

        if (dist > 0.0f && dist < ai.separationRadius)
        {
            float strength = 1.0f - (dist / ai.separationRadius);
            Vector2 push = VScale(VNorm({ dx, dy }), strength);
            force = VAdd(force, push);
        }
    }

    return VNorm(force);
}

Vector2 Enemy::SteerWallAvoid(const std::vector<Rectangle>& walls) const
{
    Vector2 avoidance{ 0.0f, 0.0f };

    for (const Rectangle& wall : walls)
    {
        float closestX = std::clamp(position.x, wall.x, wall.x + wall.width);
        float closestY = std::clamp(position.y, wall.y, wall.y + wall.height);

        float dx = position.x - closestX;
        float dy = position.y - closestY;

        float dist = VLen({ dx, dy });

        if (dist > 0.0f && dist < ai.wallFeelerLen)
        {
            float strength = 1.0f - (dist / ai.wallFeelerLen);
            Vector2 push = VScale(VNorm({ dx, dy }), strength);
            avoidance = VAdd(avoidance, push);
        }
    }

    return VNorm(avoidance);
}

// ------------------------------------------------------
// Update
// ------------------------------------------------------

std::optional<Bullet> Enemy::Update(
    Vector2 playerPos,
    float dt,
    const std::vector<Rectangle>& walls,
    const std::vector<Enemy>& allEnemies)
{
    if (!active)
        return std::nullopt;

    if (hp <= 0)
    {
        active = false;
        return std::nullopt;
    }

    if (shootTimer > 0.0f)
        shootTimer -= dt;

    if (flashTimer > 0.0f)
    {
        flashTimer -= dt;
        if (flashTimer < 0.0f)
            flashTimer = 0.0f;
    }

    Vector2 seek = SteerSeek(playerPos);
    Vector2 orbit = SteerOrbit(playerPos);
    Vector2 separation = SteerSeparation(allEnemies);
    Vector2 wallAvoid = SteerWallAvoid(walls);

    float distToPlayer = VLen(VSub(playerPos, position));

    Vector2 flee{ 0.0f, 0.0f };
    if (distToPlayer < ai.fleeRadius)
        flee = SteerFlee(playerPos);

    Vector2 steering{ 0.0f, 0.0f };
    steering = VAdd(steering, VScale(seek, ai.seek));
    steering = VAdd(steering, VScale(orbit, ai.orbit));
    steering = VAdd(steering, VScale(separation, ai.separation));
    steering = VAdd(steering, VScale(wallAvoid, ai.wallAvoid));
    steering = VAdd(steering, VScale(flee, ai.flee));

    Vector2 moveDir = VNorm(steering);

    float dx = moveDir.x * speed * dt;
    float dy = moveDir.y * speed * dt;

    // Move on X, then resolve walls
    position.x += dx;
    Rectangle hb = Hitbox();

    for (const Rectangle& w : walls)
    {
        if (CheckCollisionRecs(hb, w))
        {
            if (dx > 0.0f)
                position.x = w.x - radius;
            else if (dx < 0.0f)
                position.x = w.x + w.width + radius;

            hb = Hitbox();
        }
    }

    // Move on Y, then resolve walls
    position.y += dy;
    hb = Hitbox();

    for (const Rectangle& w : walls)
    {
        if (CheckCollisionRecs(hb, w))
        {
            if (dy > 0.0f)
                position.y = w.y - radius;
            else if (dy < 0.0f)
                position.y = w.y + w.height + radius;

            hb = Hitbox();
        }
    }

    if (distToPlayer <= shootRange && shootTimer <= 0.0f)
    {
        shootTimer = shootCooldown;

        Vector2 bulletDir = VNorm(VSub(playerPos, position));

        Bullet bullet;
        bullet.position = position;
        bullet.velocity = VScale(bulletDir, 450.0f);
        bullet.radius = 4.0f;
        bullet.lifetime = 3.0f;
        bullet.active = true;

        return bullet;
    }

    return std::nullopt;
}

// ------------------------------------------------------
// Enemy presets
// ------------------------------------------------------

Enemy MakeBasicEnemy(Vector2 pos, float speed, int hp)
{
    Enemy e(pos, speed, 16.0f);
    e.hp = hp;
    e.shootRange = 250.0f;
    e.shootCooldown = 1.0f;

    e.ai.seek = 1.0f;
    e.ai.flee = 0.0f;
    e.ai.separation = 1.2f;
    e.ai.wallAvoid = 2.5f;
    e.ai.orbit = 2.2f;

    e.ai.preferredRange = 160.0f;
    e.ai.fleeRadius = 90.0f;
    e.ai.separationRadius = 80.0f;
    e.ai.wallFeelerLen = 50.0f;

    return e;
}

Enemy MakeFastEnemy(Vector2 pos, float speed, int hp)
{
    Enemy e(pos, speed * 1.25f, 14.0f);
    e.hp = hp;
    e.shootRange = 230.0f;
    e.shootCooldown = 0.9f;

    e.ai.seek = 1.35f;
    e.ai.flee = 0.0f;
    e.ai.separation = 1.35f;
    e.ai.wallAvoid = 2.8f;
    e.ai.orbit = 2.8f;

    e.ai.preferredRange = 140.0f;
    e.ai.fleeRadius = 70.0f;
    e.ai.separationRadius = 70.0f;
    e.ai.wallFeelerLen = 55.0f;

    return e;
}

Enemy MakeTankEnemy(Vector2 pos, float speed, int hp)
{
    Enemy e(pos, speed * 0.75f, 22.0f);
    e.hp = hp + 40;
    e.shootRange = 280.0f;
    e.shootCooldown = 1.25f;

    e.ai.seek = 1.15f;
    e.ai.flee = 0.15f;
    e.ai.separation = 1.0f;
    e.ai.wallAvoid = 3.0f;
    e.ai.orbit = 1.4f;

    e.ai.preferredRange = 185.0f;
    e.ai.fleeRadius = 110.0f;
    e.ai.separationRadius = 95.0f;
    e.ai.wallFeelerLen = 60.0f;

    return e;
}
