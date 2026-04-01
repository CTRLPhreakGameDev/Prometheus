#include "core/input/input.hpp"
#include "raylib.h"

void Input::Update() {
    left_       = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    right_      = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
    up_         = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    down_       = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    attack_     = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    attackHeld_ = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    dash_       = IsKeyPressed(KEY_LEFT_ALT);
    gameInfo_   = IsKeyDown(KEY_TAB);
    restart_    = IsKeyPressed(KEY_R);
    pause_      = IsKeyPressed(KEY_ESCAPE);
    quit_       = IsKeyPressed(KEY_Q);
}
