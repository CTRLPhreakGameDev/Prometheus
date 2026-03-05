#include "gfx/cameraFollow.hpp"
#include <cmath>

static Vector2 LerpVec2(Vector2 a, Vector2 b, float t) {
    return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
}

void CameraFollow::Update(Camera2D& cam, Vector2 targetPos, float dt) {
    // Exponential smoothing that behaves the same at any FPS
    float t = 1.0f - expf(-smoothness_ * dt);
    cam.target = LerpVec2(cam.target, targetPos, t);
}
