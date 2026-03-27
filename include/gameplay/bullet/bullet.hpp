#pragma once

#include "raylib.h"
#include <vector>


struct Bullet {
    Vector2 position{};
    Vector2 velocity{};
    float radius = 4.0f;
    float lifetime = 3.0f;
    bool active = true;

    const Texture2D* sprite = nullptr;

    void Update(float dt, const std::vector<Rectangle>& walls);
    void Draw() const;

    Rectangle Hitbox() const
    {
        return { position.x - radius, position.y - radius, radius * 2, radius * 2 };
    }
};
