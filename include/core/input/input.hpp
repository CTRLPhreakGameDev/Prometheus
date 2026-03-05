#pragma once

class Input {
public:
    void Update();

    bool Left()  const { return left_; }
    bool Right() const { return right_; }
    bool Up()    const { return up_; }
    bool Down()  const { return down_; }

private:
    bool left_ = false, right_ = false, up_ = false, down_ = false;
};
