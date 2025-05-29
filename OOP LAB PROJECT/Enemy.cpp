#include "Enemy.h"
#include <iostream>

sf::Texture Enemy::texture;

Enemy::Enemy(float x, float y) {
    if (texture.getSize().x == 0) {
        if (!texture.loadFromFile("enemy.png")) {
            std::cerr << "Failed to load enemy.png\\n";
        }
    }

    sprite.setTexture(texture);
    sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
    sprite.setScale(0.3f, 0.3f);
    sprite.setPosition(x, y);
    health = 100;
    dead = false;
    speed = 50.f;
}

void Enemy::move(float dt) {
    sprite.move(speed * dt, 0);
}

void Enemy::takeDamage(int dmg) {
    health -= dmg;
    if (health <= 0) dead = true;
}

bool Enemy::isDead() const {
    return dead;
}

sf::Vector2f Enemy::getPosition() const {
    return sprite.getPosition();
}

void Enemy::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

int Enemy::getHealth() const {
    return health;
}