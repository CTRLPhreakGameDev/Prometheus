#pragma once

class Input {
public:
    void Update();

    bool Left()   const { return left_; }
    bool Right()  const { return right_; }
    bool Up()     const { return up_; }
    bool Down()   const { return down_; }
    bool Attack() const { return attack_; }
    bool AttackHeld() const { return attackHeld_; }

private:
    bool left_ = false, right_ = false, up_ = false, down_ = false;

    bool attack_ = false, attackHeld_ = false;
};
