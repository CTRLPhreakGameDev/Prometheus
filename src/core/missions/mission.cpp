#include "core/missions/mission.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

void MissionState::Update(const std::vector<Enemy>& enemies, bool playerDead, float dt)
{
    if (complete || failed || !config)
        return;
        
        switch (config->type)
        {
            case MissionType::Exterminate:
            {
            if (!enemies.empty())
                hasHadEnemies = true;
                
            bool allDead = std::none_of(enemies.begin(), enemies.end(), [](const Enemy& e){ return e.active; });
            if (hasHadEnemies && allDead)
            {
                complete = true;
                TraceLog(LOG_DEBUG, "all enemies dead");
            }
            break;
        }

        case MissionType::KillBoss:
        {
            if (config->bossIndex >= 0 &&
                config->bossIndex < (int)enemies.size())
            {
                if (!enemies[config->bossIndex].active)
                    complete = true;
            }
            break;
        }

        case MissionType::Defend:
        {
            if (playerDead)
            {
                failed = true;
                break;
            }
            defendTimer += dt;
            if (defendTimer >= config->defendDuration)
                complete = true;
            break;
        }
    }
}

std::string MissionState::HudText(const std::vector<Enemy>& enemies) const
{
    if (!config) return "";

    switch (config->type)
    {
        case MissionType::Exterminate:
        {
            int alive = (int)std::count_if(enemies.begin(), enemies.end(), [](const Enemy& e){ return e.active; });
            return "Enemies left: " + std::to_string(alive);
        }

        case MissionType::KillBoss:
            return "Kill the Boss!";

        case MissionType::Defend:
        {
            float remaining = config->defendDuration - defendTimer;
            std::ostringstream ss;
            ss << "Survive: " << std::fixed << std::setprecision(1) << remaining << "s";
            return ss.str();
        }
    }
    return "";
}