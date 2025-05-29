#include "Bullet.h"
#include <cmath>
#include <iostream>

Bullet::Bullet(sf::Vector2f startPos, sf::Vector2f targetPos) {
    if (!texture.loadFromFile("bullet.png"))
        std::cerr << "Failed to load bullet.png\n";

    sprite.setTexture(texture);
    sprite.setScale(0.5f, 0.5f); // Scale down if needed

    // 🧭 Set origin to center of image
    sf::Vector2u size = texture.getSize();
    sprite.setOrigin(size.x / 2.0f, size.y / 2.0f);

    sprite.setPosition(startPos);

    // Direction + Velocity
    sf::Vector2f direction = targetPos - startPos;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    velocity = (length != 0) ? direction / length * speed : sf::Vector2f(0.f, 0.f);

    // 🔄 Rotate bullet so it faces the target
    float angle = std::atan2(velocity.y, velocity.x) * 180 / 3.14159f;
    sprite.setRotation(angle);
}



void Bullet::update(float deltaTime) {
    sprite.move(velocity * deltaTime);
}

bool Bullet::checkCollision(sf::Vector2f enemyPos, float radius) {
    sf::Vector2f diff = enemyPos - sprite.getPosition();
    return std::sqrt(diff.x * diff.x + diff.y * diff.y) < radius;
}

void Bullet::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
