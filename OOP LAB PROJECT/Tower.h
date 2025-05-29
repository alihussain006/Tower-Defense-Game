#pragma once
#include <SFML/Graphics.hpp>
#include "Enemy.h"

class Tower {
protected:
    sf::Texture texture;
    sf::Sprite sprite;
    float range;
    float damage;
    float fireRate;
    float timeSinceLastShot;
    int upgradeLevel;

public:
    float upgradeCost;
    bool isUpgradeable;

    Tower(float x, float y);

    virtual void attack(Enemy& enemy) = 0;
    virtual void update(float deltaTime);
    virtual void draw(sf::RenderWindow& window);

    void upgrade();
    void rotateTo(sf::Vector2f target);

    sf::Vector2f getPosition() const;
    float getRange() const;
    bool canFire() const;
    void markFired();
};
