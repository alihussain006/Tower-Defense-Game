#include "Tower.h"
#include <cmath>
#include <iostream>

Tower::Tower(float x, float y)
    : range(220), damage(10), fireRate(2.0f), timeSinceLastShot(0),
    isUpgradeable(true), upgradeLevel(1), upgradeCost(50.0f)
{
    if (!texture.loadFromFile("tower.png"))
        std::cerr << "Failed to load tower.png\n";

    sprite.setTexture(texture);
    sprite.setScale(0.3f, 0.3f); // Smaller tower
    sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
    sprite.setPosition(x, y);
    sprite.setRotation(180.f);
}

void Tower::update(float deltaTime) {
    timeSinceLastShot += deltaTime;
}

void Tower::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Tower::upgrade() {
    if (isUpgradeable && upgradeLevel < 3) {
        range += 30;
        damage += 10;
        fireRate += 1.0f;
        upgradeLevel++;
        upgradeCost += 50;
        if (upgradeLevel == 3)
            isUpgradeable = false;
    }
}

void Tower::rotateTo(sf::Vector2f target) {
    sf::Vector2f direction = target - sprite.getPosition();
    float angle = std::atan2(direction.y, direction.x) * 180 / 3.14159f;
    sprite.setRotation(angle); // or angle - 90 if cannon faces up
}

sf::Vector2f Tower::getPosition() const {
    return sprite.getPosition();
}

float Tower::getRange() const {
    return range;
}

bool Tower::canFire() const {
    return timeSinceLastShot >= (1.0f / fireRate);
}

void Tower::markFired() {
    timeSinceLastShot = 0.f;
}
