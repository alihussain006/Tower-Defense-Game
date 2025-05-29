// Base.h
#pragma once
#include <SFML/Graphics.hpp>

class Base {
private:
    sf::Sprite sprite;
    sf::Texture texture;
    int health;
    int maxHealth;

public:
    Base(float x, float y);
    void takeDamage(int dmg);
    bool isDestroyed() const;
    int getHealth() const;
    sf::Vector2f getPosition() const;
    void draw(sf::RenderWindow& window) const;
    void reset(float x, float y);
    sf::FloatRect getBounds() const;

};
