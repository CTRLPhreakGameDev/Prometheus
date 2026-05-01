#pragma once

#include "gameplay/enemies/common.hpp"
#include "raylib.h"
#include <vector>
#include <string>
#include <functional>

enum class MissionType
{
    Exterminate,
    KillBoss,
    Defend,
};

struct MissionConfig
{
    std::string title;
    std::string description;
    MissionType type;

    std::vector<Enemy> enemies;

    float defendDuration = 0.0f;

    int bossIndex = -1;
};

struct MissionState
{
    const MissionConfig* config = nullptr;

    float defendTimer = 0.0f;
    bool complete = false;
    bool failed = false;
    bool hasHadEnemies = false;

    void Start(const MissionConfig& cfg)
    {
        config = &cfg;
        defendTimer = 0.0f;
        complete = false;
        failed = false;
        hasHadEnemies = false;
    }

    void Update(const std::vector<Enemy>& enemies, bool playerDead, float dt);

    std::string HudText(const std::vector<Enemy>& enemies) const;
};