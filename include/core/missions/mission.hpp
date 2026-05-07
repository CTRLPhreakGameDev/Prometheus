#pragma once

#include "gameplay/enemies/common.hpp"
#include "core/objectives/objective.hpp"
#include "raylib.h"
#include <vector>
#include <string>
#include <functional>
#include <optional>

enum class MissionType
{
    Exterminate,
    KillBoss,
    Defend,
    DefendPoint,
    DestroyTarget,
    Reach,
};

struct MissionConfig
{
    std::string title;
    std::string description;
    MissionType type;

    std::vector<Enemy> enemies;

    float defendDuration = 0.0f;

    int bossIndex = -1;

    std::optional<ObjectivePoint> objective;
};

struct MissionState
{
    const MissionConfig* config = nullptr;

    float defendTimer = 0.0f;
    bool complete = false;
    bool failed = false;
    bool hasHadEnemies = false;
    std::optional<ObjectivePoint> objective;

    void Start(const MissionConfig& cfg)
    {
        config = &cfg;
        defendTimer = 0.0f;
        complete = false;
        failed = false;
        hasHadEnemies = false;
        objective = cfg.objective;
    }

    void Update(const std::vector<Enemy>& enemies, bool playerDead, float dt, Vector2 plaerPos = {0,0});

    std::string HudText(const std::vector<Enemy>& enemies) const;
};