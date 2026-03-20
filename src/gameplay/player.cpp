#include "gameplay/player.hpp"

void Player::Update(const Input &input, float dt, const std::vector<Rectangle> &walls) 
{
  if (!active)
	  return;

  Vector2 move{0, 0};
  if (input.Left())
    move.x -= 1;
  if (input.Right())
    move.x += 1;
  if (input.Up())
    move.y -= 1;
  if (input.Down())
    move.y += 1;

  if (move.x == 0 && move.y == 0)
    return;

  move = Vector2Normalize(move);

  float dx = move.x * speed_ * dt;
  float dy = move.y * speed_ * dt;

  pos_.x += dx;
  Rectangle hb = Hitbox();

  for (const Rectangle &w : walls) {
    if (CheckCollisionRecs(hb, w)) {
      if (dx > 0)
        pos_.x = w.x - hb.width / 2;
      else
        pos_.x = (w.x + w.width) + hb.width / 2;
      hb = Hitbox();
    }
  }

  pos_.y += dy;
  hb = Hitbox();

  for (const Rectangle &w : walls) {
    if (CheckCollisionRecs(hb, w)) {
      if (dy > 0)
        pos_.y = w.y - hb.height / 2;
      else
        pos_.y = (w.y + w.height) + hb.height / 2;
    }
  }

  if (hp <= 0)
  {
	  active = false;
  }
}

void Player::Draw() const
{
  DrawRectangleV({pos_.x - 10, pos_.y - 10}, {20, 20}, BLUE);
}
