#include "gameplay/player.hpp"
#include <cmath>

void Player::LoadSprite(const char* path, int frameCount)
{
	sprite_ = LoadTexture(path);
	hasSprite_ = true;
	frameCount_ = frameCount;
}

void Player::UnloadSprite()
{
	if (hasSprite_)
		UnloadTexture(sprite_);
}

void Player::Update(const Input &input, float dt, const std::vector<Rectangle> &walls) 
{
  if (!active)
	  return;

  frameTimer_ += dt;
  if (frameTimer_ >= frameInterval_)
  {
	  frameTimer_ -= frameInterval_;

	  currentFrame_ = (currentFrame_ + 1) % frameCount_;
  }

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

void Player::Draw(float angle) const
{
  if (hasSprite_)
  {
	  float size = 32.0f;
	  float frameWidth = (float)sprite_.width / frameCount_;

	  Rectangle src = {
		currentFrame_ + frameWidth,
		0,
		frameWidth,
		(float)sprite_.height
	  };

	  Rectangle dst = { pos_.x, pos_.y, size, size };
	  Vector2 origin = { size / 2.0f, size / 2.0f };

  	  Color tint = (dashTimer_ > 0.0f) ? Fade(WHITE, 0.6f) : WHITE;

	  DrawTexturePro(sprite_, src, dst, origin, angle, tint);
  }
  else 
  {
	  Color c = (dashTimer_ > 0.0f) ? WHITE : BLUE;
  	  DrawRectangleV({pos_.x - 10, pos_.y - 10}, {20, 20}, c);
  }

  if (dashCooldown_ > 0.0f && dashTimer_ <= 0.0f)
  {
	float ratio = dashCooldown_ / kDashCooldown;
	DrawCircleLines((int)pos_.x, (int)pos_.y, 14.0f * ratio, Fade(SKYBLUE, 0.6));
  }
}
