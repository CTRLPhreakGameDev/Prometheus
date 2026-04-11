#pragma once

class Input {
public:
    void Update();

    bool Left()       const { return left_; }
    bool Right()      const { return right_; }
    bool Up()         const { return up_; }
    bool Down()       const { return down_; }
    bool Attack()     const { return attack_; }
    bool AttackHeld() const { return attackHeld_; }
    bool Dash()       const { return dash_; }
    bool GameInfo()   const { return gameInfo_; }
    bool Restart()    const { return restart_; }
    bool Pause()      const { return pause_; }
    bool Quit()       const { return quit_; }
    bool Enter()      const { return enter_; }

private:
    bool left_ = false, right_ = false, up_ = false, down_ = false;

    bool attack_ = false, attackHeld_ = false;

    bool dash_ = false;

    bool gameInfo_ = false;

    bool restart_ = false;

    bool pause_ = false;
    
    bool quit_ = false;

    bool enter_ = false;
};
