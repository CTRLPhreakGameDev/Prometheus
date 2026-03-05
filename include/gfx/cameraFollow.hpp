#pragma once
#include "raylib.h"

class CameraFollow {
public:
    // smoothness: bigger = snappier (e.g. 8–15)
    explicit CameraFollow(float smoothness = 10.0f) : smoothness_(smoothness) {}

    void Update(Camera2D& cam, Vector2 targetPos, float dt);

private:
    float smoothness_;
};
