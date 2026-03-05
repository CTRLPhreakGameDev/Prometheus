#include "gameplay/player.hpp"
#include "raylib.h"
#include "raymath.h"

void Player::Update(const Input& input, float dt) {
    Vector2 move{0, 0};
    if (input.Left())  move.x -= 1;
    if (input.Right()) move.x += 1;
    if (input.Up())    move.y -= 1;
    if (input.Down())  move.y += 1;

    if (move.x != 0 || move.y != 0) {
        move = Vector2Normalize(move);
        pos_.x += move.x * speed_ * dt;
        pos_.y += move.y * speed_ * dt;
    }
}

void Player::Draw() const {
    DrawRectangleV({pos_.x - 10, pos_.y - 10}, {20, 20}, RED);
}
