#pragma once

#include "raylib.h"

enum class ObjType 
{ 
    Defend, 
    Destroy, 
    Reach
};

struct ObjectivePoint 
{
    Vector2 position{};
    float radius = 40.0f;
    int hp  = -1;
    int maxHp = -1;
    ObjType type = ObjType::Defend;
    const Texture2D* sprite = nullptr;
    bool active = true;

    Rectangle Hitbox() const
    {
        return { position.x - radius, position.y - radius, radius * 2, radius * 2 };
    }
};