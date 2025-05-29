#include "SniperTower.h"

SniperTower::SniperTower(float x, float y) : Tower(x, y) {}

void SniperTower::attack(Enemy& enemy) {
    enemy.takeDamage(damage);
}
