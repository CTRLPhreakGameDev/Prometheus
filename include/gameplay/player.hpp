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
    void Draw(float angle) const;

    void LoadSprite(const char* path, int frameCount);
    void UnloadSprite();
    
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

    Texture2D sprite_{};
    bool hasSprite_ = false;

    int frameCount_ = 1;
    int currentFrame_ = 0;
    float frameTimer_ = 0.0f;
    float frameInterval_ = 1.0f / 12.0f;
};
