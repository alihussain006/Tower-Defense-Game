#pragma once
#ifndef GAME_CONFIG_H
#define GAME_CONFIG_H

struct GameConfig {
    static constexpr int maxTowers = 5;
    static constexpr int towerCost = 50;
    static constexpr int startingCurrency = 100;
    static constexpr int rewardPerEnemy = 20;
};

#endif