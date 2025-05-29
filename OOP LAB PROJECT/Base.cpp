// Base.cpp
#include "Base.h"
#include <iostream>

Base::Base(float x, float y) {
    if (!texture.loadFromFile("base.png")) {
        std::cerr << "Failed to load base.png" << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
    sprite.setScale(0.13f, 0.13f); // enlarge the image (adjust as needed)
    sprite.setPosition(x - 0.f, y); // shift 0 pixels left (tweak as needed)


    maxHealth = 200;
    health = maxHealth;
}

void Base::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

void Base::takeDamage(int dmg) {
    health -= dmg;
    if (health < 0) health = 0;
}

bool Base::isDestroyed() const {
    return health <= 0;
}

int Base::getHealth() const {
    return health;
}

sf::Vector2f Base::getPosition() const {
    return sprite.getPosition();
}

sf::FloatRect Base::getBounds() const {
    return sprite.getGlobalBounds();
}

void Base::reset(float x, float y) {
    sprite.setPosition(x, y);
    health = maxHealth;
}
