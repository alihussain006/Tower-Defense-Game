
// FastEnemy.cpp
#include "FastEnemy.h"

FastEnemy::FastEnemy(float x, float y) : Enemy(x, y) {
    shape.setFillColor(sf::Color::Magenta);
    speed = 90.f;
    health = 60;
}
