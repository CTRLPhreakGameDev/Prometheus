#pragma once

#include "raylib.h"
#include "core/input/input.hpp"
#include "gameplay/player.hpp"
#include "gfx/cameraFollow.hpp"

class Game {
public:
    void Run();

private:
    void Update(float dt);
    void Draw();

    Camera2D camera_{};
    CameraFollow camFollow_{};

    Input input_{};
    Player player_{ { 0.0f, 0.0f } };
};
