#pragma once

#include "raylib.h"
#include <vector>
#include <string>

struct WorldConfig
{
    std::string name;
    std::string bgTexture;
    int maxWaves;
    int enemySpeedMult;
    int enemyHpBase;
    std::vector<Rectangle> walls;
};