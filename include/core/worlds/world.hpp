#pragma once

#include "raylib.h"
#include "core/missions/mission.hpp"
#include <vector>
#include <string>

struct WorldConfig
{
    std::string name;
    std::string bgTexture;
    float enemySpeedMult;
    int enemyHpBase;
    std::vector<Rectangle> walls;
    std::vector<MissionConfig> missions;
};