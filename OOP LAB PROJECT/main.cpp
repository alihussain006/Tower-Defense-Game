#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <map>
#include "SniperTower.h"
#include "Bullet.h"
#include "Base.h"

enum GameState { MENU, PLAYING, GAME_OVER };
GameState gameState = MENU;

// Game objects and variables
std::vector<Enemy> enemies;
std::vector<Tower*> towers;
std::map<Tower*, int> towerHealth;
std::vector<Bullet> bullets;
std::vector<Bullet> enemyBullets;
std::map<Enemy*, float> enemyFireTimers;

Base base(900.f, 380.f);

// Sound assets
sf::Texture backgroundTexture;
sf::Sprite backgroundSprite;
sf::SoundBuffer fireBuffer, deathBuffer, enemyFireBuffer;
sf::SoundBuffer towerDestroyedBuffer, baseDestroyedBuffer;
sf::Sound towerDestroyedSound, baseDestroyedSound;
std::vector<sf::Sound> fireSounds(5);
sf::Sound deathSound, enemyFireSound;
sf::Music backgroundMusic;

// Game stats
int highScore = 0;
int playerScore = 0;
int playerHealth = 10;
int playerCurrency = 100;
const int towerCost = 25;

// Wave management
int waveNumber = 1;
int enemiesPerWave = 5;
int enemiesSpawnedThisWave = 0;
float enemySpawnTimer = 0;
float spawnInterval = 1.0f;
float waveCooldown = 3.0f;
float waveTimer = 0;

// UI Elements
sf::Font font;
sf::Text scoreText;
sf::Text highScoreText;
sf::Text currencyText;
sf::Text gameOverText;
sf::Text tryAgainText;

// Menu UI Elements
sf::Text titleText;
sf::Text startButtonText;
sf::Text exitButtonText;
sf::RectangleShape startButton;
sf::RectangleShape exitButton;

void loadHighScore(int& highScore) {
    std::ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    else {
        highScore = 0;
    }
}

void saveHighScore(int score) {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << score;
        file.close();
    }
}

void playFireSound() {
    for (auto& sound : fireSounds) {
        if (sound.getStatus() != sf::Sound::Playing) {
            sound.play();
            break;
        }
    }
}

void drawHealthBar(sf::RenderWindow& window, sf::Vector2f position, float width, float height, int current, int max, sf::Color color) {
    float ratio = static_cast<float>(current) / static_cast<float>(max);
    sf::RectangleShape bgBar(sf::Vector2f(width, height));
    bgBar.setFillColor(sf::Color(50, 50, 50));
    bgBar.setPosition(position);
    sf::RectangleShape fgBar(sf::Vector2f(width * ratio, height));
    fgBar.setFillColor(color);
    fgBar.setPosition(position);
    window.draw(bgBar);
    window.draw(fgBar);
}

void resetGame() {
    enemies.clear();
    bullets.clear();
    enemyBullets.clear();
    for (auto* tower : towers) delete tower;
    towers.clear();
    towerHealth.clear();
    enemyFireTimers.clear();
    enemySpawnTimer = 0;
    waveNumber = 1;
    enemiesPerWave = 5;
    enemiesSpawnedThisWave = 0;
    waveTimer = 0;
    playerHealth = 10;
    playerScore = 0;
    playerCurrency = 100;
    base.reset(900.f, 380.f);
    gameState = PLAYING;
}

void initMenu() {
    titleText.setFont(font);
    titleText.setString("Tower Defense: Strategic Warfare");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition(150, 150);
    titleText.setStyle(sf::Text::Bold);

    startButton.setSize(sf::Vector2f(300, 60));
    startButton.setFillColor(sf::Color(70, 70, 70));
    startButton.setPosition(362, 300);
    startButton.setOutlineThickness(2);
    startButton.setOutlineColor(sf::Color::White);

    startButtonText.setFont(font);
    startButtonText.setString("Start New Game");
    startButtonText.setCharacterSize(30);
    startButtonText.setFillColor(sf::Color::White);
    startButtonText.setPosition(400, 310);

    exitButton.setSize(sf::Vector2f(300, 60));
    exitButton.setFillColor(sf::Color(70, 70, 70));
    exitButton.setPosition(362, 400);
    exitButton.setOutlineThickness(2);
    exitButton.setOutlineColor(sf::Color::White);

    exitButtonText.setFont(font);
    exitButtonText.setString("Exit Game");
    exitButtonText.setCharacterSize(30);
    exitButtonText.setFillColor(sf::Color::White);
    exitButtonText.setPosition(450, 410);
}

void handleMenuInput(sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    // Start button hover effect
    if (startButton.getGlobalBounds().contains(mousePos)) {
        startButton.setFillColor(sf::Color(100, 100, 100));
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            resetGame();
        }
    }
    else {
        startButton.setFillColor(sf::Color(70, 70, 70));
    }

    // Exit button hover effect
    if (exitButton.getGlobalBounds().contains(mousePos)) {
        exitButton.setFillColor(sf::Color(100, 100, 100));
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            window.close();
        }
    }
    else {
        exitButton.setFillColor(sf::Color(70, 70, 70));
    }
}

void drawMenu(sf::RenderWindow& window) {
    window.draw(titleText);
    window.draw(startButton);
    window.draw(startButtonText);
    window.draw(exitButton);
    window.draw(exitButtonText);
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    sf::RenderWindow window(sf::VideoMode(1024, 768), "Tower Defense: Strategic Warfare");
    window.setFramerateLimit(60);

    // Load music
    if (!backgroundMusic.openFromFile("background.mp3")) {
        std::cerr << "Failed to load background music!" << std::endl;
    }
    else {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(50.f);
        backgroundMusic.play();
    }

    // Load font
    if (!font.loadFromFile("C:/Windows/Fonts/Arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    // Load sounds
    fireBuffer.loadFromFile("fire.wav");
    enemyFireBuffer.loadFromFile("enemy_fire.wav");
    deathBuffer.loadFromFile("destroy.wav");
    towerDestroyedBuffer.loadFromFile("destroy.wav");
    baseDestroyedBuffer.loadFromFile("destroy.wav");

    for (auto& sound : fireSounds) sound.setBuffer(fireBuffer);
    deathSound.setBuffer(deathBuffer);
    enemyFireSound.setBuffer(enemyFireBuffer);
    towerDestroyedSound.setBuffer(towerDestroyedBuffer);
    baseDestroyedSound.setBuffer(baseDestroyedBuffer);

    // Load background
    if (!backgroundTexture.loadFromFile("background.png")) {
        std::cerr << "Failed to load background.png\n";
    }
    else {
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            window.getSize().x / static_cast<float>(backgroundTexture.getSize().x),
            window.getSize().y / static_cast<float>(backgroundTexture.getSize().y)
        );
    }

    // Load high score
    loadHighScore(highScore);

    // Initialize game UI
    scoreText.setFont(font);
    scoreText.setCharacterSize(18);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    scoreText.setStyle(sf::Text::Bold);

    highScoreText.setFont(font);
    highScoreText.setCharacterSize(18);
    highScoreText.setFillColor(sf::Color::White);
    highScoreText.setPosition(10, 40);
    highScoreText.setStyle(sf::Text::Bold);

    currencyText.setFont(font);
    currencyText.setCharacterSize(18);
    currencyText.setFillColor(sf::Color::Yellow);
    currencyText.setPosition(10, 70);
    currencyText.setStyle(sf::Text::Bold);

    gameOverText.setFont(font);
    gameOverText.setCharacterSize(36);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setString("Game Over!");
    gameOverText.setPosition(260, 200);
    gameOverText.setStyle(sf::Text::Bold);

    tryAgainText.setFont(font);
    tryAgainText.setCharacterSize(24);
    tryAgainText.setFillColor(sf::Color::White);
    tryAgainText.setString("Click to Try Again");
    tryAgainText.setPosition(270, 300);
    tryAgainText.setStyle(sf::Text::Bold);

    // Initialize menu
    initMenu();

    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameState == MENU) {
                handleMenuInput(window);
            }
            else if (gameState == PLAYING && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (event.mouseButton.button == sf::Mouse::Left && pos.x >= window.getSize().x / 2.f) {
                    if (playerCurrency >= towerCost) {
                        Tower* t = new SniperTower(pos.x, pos.y);
                        towers.push_back(t);
                        towerHealth[t] = 100;
                        playerCurrency -= towerCost;
                    }
                }
            }
            else if (gameState == GAME_OVER && event.type == sf::Event::MouseButtonPressed) {
                gameState = MENU;
            }
        }

        window.clear();
        window.draw(backgroundSprite);

        if (gameState == MENU) {
            drawMenu(window);
        }
        else if (gameState == PLAYING) {
            // Wave management
            waveTimer += deltaTime;
            if (enemiesSpawnedThisWave < enemiesPerWave) {
                enemySpawnTimer += deltaTime;
                if (enemySpawnTimer >= spawnInterval) {
                    float yPos = 100 + rand() % 400;
                    enemies.push_back(Enemy(0, yPos));
                    enemiesSpawnedThisWave++;
                    enemySpawnTimer = 0;
                }
            }
            else if (enemies.empty() && waveTimer >= waveCooldown) {
                waveNumber++;
                enemiesPerWave += 2;
                enemiesSpawnedThisWave = 0;
                waveTimer = 0;
            }

            // Enemy updates
            for (auto& enemy : enemies) {
                if (enemy.getPosition().x < 650)
                    enemy.move(deltaTime);
                enemyFireTimers[&enemy] += deltaTime;
                if (enemyFireTimers[&enemy] >= 1.5f) {
                    Tower* target = nullptr;
                    float closestDist = 200.f;
                    for (auto* tower : towers) {
                        float dist = std::hypot(tower->getPosition().x - enemy.getPosition().x,
                            tower->getPosition().y - enemy.getPosition().y);
                        if (dist < closestDist) {
                            closestDist = dist;
                            target = tower;
                        }
                    }
                    if (target) {
                        enemyBullets.emplace_back(enemy.getPosition(), target->getPosition());
                        enemyFireSound.play();
                        enemyFireTimers[&enemy] = 0.f;
                    }
                    else if (enemy.getPosition().x >= 550) {
                        enemyBullets.emplace_back(enemy.getPosition(), base.getPosition());
                        base.takeDamage(10);
                        enemyFireSound.play();
                        enemyFireTimers[&enemy] = 0.f;
                    }
                }
            }

            // Tower updates
            for (auto* tower : towers) {
                tower->update(deltaTime);
                sf::Vector2f nearestTargetPos;
                float minDistance = tower->getRange();
                bool foundTarget = false;
                for (auto& enemy : enemies) {
                    if (!enemy.isDead()) {
                        float dist = std::hypot(enemy.getPosition().x - tower->getPosition().x,
                            enemy.getPosition().y - tower->getPosition().y);
                        if (dist <= minDistance) {
                            nearestTargetPos = enemy.getPosition();
                            minDistance = dist;
                            foundTarget = true;
                        }
                    }
                }
                if (foundTarget) {
                    tower->rotateTo(nearestTargetPos);
                    if (tower->canFire()) {
                        bullets.emplace_back(tower->getPosition(), nearestTargetPos);
                        tower->markFired();
                        playFireSound();
                    }
                }
            }

            // Bullet updates
            for (auto& bullet : bullets) bullet.update(deltaTime);
            for (auto& bullet : enemyBullets) bullet.update(deltaTime);

            // Collision detection
            for (auto& bullet : bullets) {
                for (auto& enemy : enemies)
                    if (!enemy.isDead() && bullet.isActive && bullet.checkCollision(enemy.getPosition(), 15.f)) {
                        enemy.takeDamage(bullet.damage);
                        bullet.isActive = false;
                    }
            }

            for (auto& bullet : enemyBullets) {
                for (auto* tower : towers) {
                    if (bullet.isActive && bullet.checkCollision(tower->getPosition(), 20.f)) {
                        towerHealth[tower] -= 10;
                        bullet.isActive = false;
                    }
                }
            }

            // Cleanup
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet& b) { return !b.isActive; }), bullets.end());
            enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(), [](Bullet& b) { return !b.isActive; }), enemyBullets.end());

            towers.erase(std::remove_if(towers.begin(), towers.end(), [](Tower* t) {
                if (towerHealth[t] <= 0) {
                    towerDestroyedSound.play();
                    delete t;
                    towerHealth.erase(t);
                    return true;
                }
                return false;
                }), towers.end());

            enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [&](Enemy& e) {
                if (e.isDead()) {
                    deathSound.play();
                    playerScore += 10;
                    playerCurrency += 5;
                    return true;
                }
                if (e.getPosition().x > 800) {
                    playerHealth--;
                    return true;
                }
                return false;
                }), enemies.end());

            // Game over check
            if (playerScore > highScore) highScore = playerScore;
            if (playerHealth <= 0 || base.isDestroyed()) {
                if (base.isDestroyed()) {
                    baseDestroyedSound.play();
                    sf::sleep(sf::seconds(0.5f));
                }
                saveHighScore(highScore);
                gameState = GAME_OVER;
            }

            // Draw game objects
            for (auto& bullet : bullets) bullet.draw(window);
            for (auto& bullet : enemyBullets) bullet.draw(window);
            for (auto& enemy : enemies) {
                enemy.draw(window);
                drawHealthBar(window, enemy.getPosition() + sf::Vector2f(-15, -25), 30.f, 4.f, enemy.getHealth(), 100, sf::Color::Red);
            }
            for (auto* tower : towers) {
                tower->draw(window);
                drawHealthBar(window, tower->getPosition() + sf::Vector2f(-20, -30), 40.f, 5.f, towerHealth[tower], 100, sf::Color::Green);
            }

            base.draw(window);
            drawHealthBar(window, base.getPosition() + sf::Vector2f(-30, -70), 60.f, 6.f, base.getHealth(), 200, sf::Color::Blue);

            // Update and draw UI text
            scoreText.setString("Score: " + std::to_string(playerScore));
            highScoreText.setString("High Score: " + std::to_string(highScore));
            currencyText.setString("Currency: " + std::to_string(playerCurrency));

            window.draw(scoreText);
            window.draw(highScoreText);
            window.draw(currencyText);
        }
        else if (gameState == GAME_OVER) {
            window.draw(gameOverText);
            window.draw(tryAgainText);
        }

        window.display();
    }

    // Cleanup
    for (auto* tower : towers) delete tower;
    return 0;
}