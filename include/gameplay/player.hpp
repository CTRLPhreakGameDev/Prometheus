#pragma once

#include "raylib.h"
#include "raymath.h"
#include "core/input/input.hpp"
#include <vector>

class Player
{
  public:
    explicit Player(Vector2 startPos) : pos_(startPos) {}

    void Update(const Input& input, float dt, const std::vector<Rectangle>& walls);
    void Draw() const;

    Vector2 Pos() const { return pos_; }

    Rectangle Hitbox() const
    {
	    return { pos_.x -10, pos_.y -10, 20, 20 };
    }

    int hp = 100;

  private:
    bool active = true;

    Vector2 pos_{};
    float speed_ = 200.0f;

    float dashTimer_ = 0.0f;
    float dashCooldown_ = 0.0f;
    Vector2 dashVelocity_{};

    static constexpr float kDashDuration = 0.18f;
    static constexpr float kDashSpeed = 600.0f;
    static constexpr float kDashCooldown = 0.6f;
};
