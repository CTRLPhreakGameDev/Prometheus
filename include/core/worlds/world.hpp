#pragma once

#include "raylib.h"
#include <vector>
#include <string>

struct WorldConfig
{
    std::string name;
    std::string bgTexture;
    int maxWaves;
    float enemySpeedMult;
    int enemyHpBase;
    std::vector<Rectangle> walls;
};