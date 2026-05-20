#include "core/input/input.hpp"
#include "raylib.h"

void Input::Update() 
{
    static constexpr int kKonamiCode[10] = { KEY_UP, KEY_UP, KEY_DOWN, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_LEFT, KEY_RIGHT, KEY_B, KEY_A };

    secret_ = false;
    int pressedKey = GetKeyPressed();
    if (pressedKey != 0)
    {
        if (pressedKey== kKonamiCode[konamiStep_])
        {
            konamiStep_++;
            if (konamiStep_ == kKonamiLen)
            {
                secret_ = true;
                konamiStep_ = 0;
            }
        }
        else
        {
            konamiStep_ = (pressedKey == kKonamiCode[0]) ? 1 : 0;
        }
    }

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
    enter_      = IsKeyPressed(KEY_ENTER);
}


