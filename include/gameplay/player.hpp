#pragma once
#include "raylib.h"
#include "core/input/input.hpp"

class Player {
public:
    explicit Player(Vector2 startPos) : pos_(startPos) {}

    void Update(const Input& input, float dt);
    void Draw() const;

    Vector2 Pos() const { return pos_; }

    Rectangle Hitbox() const
    {
	    return {pos_.x -10, pos_.y -10, 20, 20 };
    }

private:
    Vector2 pos_{};
    float speed_ = 200.0f;
};
