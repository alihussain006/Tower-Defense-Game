#pragma once
#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>

class Enemy {
private:
    static sf::Texture texture;
    sf::Sprite sprite;
    float speed;
    int health;
    bool dead;

public:
    Enemy(float x, float y);
    void move(float dt);
    void takeDamage(int dmg);
    bool isDead() const;
    sf::Vector2f getPosition() const;
    void draw(sf::RenderWindow& window);
    int getHealth() const;
};

#endif