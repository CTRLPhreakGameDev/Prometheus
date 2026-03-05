#include "core/game.hpp"

void Game::Run() {
    InitWindow(800, 450, "game");
    SetTargetFPS(60);

    camera_.offset = { 400.0f, 225.0f };
    camera_.rotation = 0.0f;
    camera_.zoom = 1.0f;

    // Start camera on player
    camera_.target = player_.Pos();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        input_.Update();
        Update(dt);

        BeginDrawing();
        ClearBackground(BLACK);
        Draw();
        EndDrawing();
    }

    CloseWindow();
}

void Game::Update(float dt) {
    player_.Update(input_, dt);
    camFollow_.Update(camera_, player_.Pos(), dt);
}

void Game::Draw() {
    BeginMode2D(camera_);
    player_.Draw();
    DrawGrid(50, 32);
    EndMode2D();

    DrawText("WASD to move", 10, 10, 20, RAYWHITE);
}
