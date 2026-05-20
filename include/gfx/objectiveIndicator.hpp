#pragma once

#include "raylib.h"
#include "raymath.h"
#include "core/objectives/objective.hpp"

class ObjectiveIndicator
{
    public:
        void Draw(Vector2 playerScreenPos, Vector2 objectiveWorldPos, const Camera2D& camera, int renderW, int renderH) const;
};