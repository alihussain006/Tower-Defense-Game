#pragma once
#include <SFML/Graphics.hpp>

class Bullet {
public:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f velocity;
    float speed = 400.f;
    float damage = 10.f;
    bool isActive = true;

    Bullet(sf::Vector2f startPos, sf::Vector2f targetPos);
    void update(float deltaTime);
    bool checkCollision(sf::Vector2f enemyPos, float radius);
    void draw(sf::RenderWindow& window);
};
