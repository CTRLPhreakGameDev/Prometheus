#include "gameplay/player.hpp"

void Player::Update(const Input &input, float dt, const std::vector<Rectangle> &walls) 
{
  if (!active)
	  return;

  if (dashTimer_ > 0.0f) dashTimer_ -= dt;
  if (dashCooldown_ > 0.0f) dashCooldown_ -= dt;

  bool dashing = dashTimer_ > 0.0f;

  if (input.Dash() && !dashing && dashCooldown_ <= 0.0f)
  {
	  Vector2 dir{0, 0};
	  if (input.Left()) dir.x -= 1;
	  if (input.Right()) dir.x += 1;
	  if (input.Up()) dir.y -= 1;
          if (input.Down()) dir.y += 1;

	  if (Vector2Length(dir) == 0.0f) dir = {1.0f, 0.0f};

	  dir = Vector2Normalize(dir);
	  dashVelocity_ = { dir.x * kDashSpeed, dir.y * kDashSpeed };
	  dashTimer_ = kDashDuration;
	  dashCooldown_ = kDashCooldown;
	  dashing = true;
  }

  float dx = 0.0f, dy = 0.0f;

  if (dashing)
  {
  	dx = dashVelocity_.x * dt;
	  dy = dashVelocity_.y * dt;
  }
  else 
  {
  	Vector2 move{0, 0};

 	if (input.Left())
    		move.x -= 1;
  	if (input.Right())
    		move.x += 1;
  	if (input.Up())
    		move.y -= 1;
  	if (input.Down())
    		move.y += 1;

  	if (move.x != 0 && move.y != 0)
		move = Vector2Normalize(move);

 	dx = move.x * speed_ * dt;
  	dy = move.y * speed_ * dt;
  }

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
  Color c = (dashTimer_ > 0.0f) ? WHITE : BLUE;
  DrawRectangleV({pos_.x - 10, pos_.y - 10}, {20, 20}, c);

  if (dashCooldown_ > 0.0f && dashTimer_ <= 0.0f)
  {
	float ratio = dashCooldown_ / kDashCooldown;
	DrawCircleLines((int)pos_.x, (int)pos_.y, 14.0f * ratio, Fade(SKYBLUE, 0.6));
  }
}
