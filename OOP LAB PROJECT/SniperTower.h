#pragma once
#include "Tower.h"

class SniperTower : public Tower {
public:
    SniperTower(float x, float y);
    void attack(Enemy& enemy) override;
};
