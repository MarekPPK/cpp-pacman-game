#include "Player.h"
#include "Map.h" 
#include <cmath>
#include <iostream>
import GameConstants;

Player::Player(float speed, sf::Vector2f pos, Map* m, sf::Texture& tex)
    : Character(speed, pos,tex), map(m), currentDir({ 1.f, 0.f }), nextDir({ 0.f, 0.f }),
    tileSize(Config::Tile::Size), offsetY(Config::Tile::OffsetY)
{
    this->setPosition(pos);
    textureRect = sf::IntRect({ 3,0 }, { 16,16 });
    sprite.setTextureRect(textureRect);
    int visualDirectionOffset = 0;
    sprite.setOrigin({ 8.f, 8.f }); 
    sprite.setScale({ 1.5f, 1.5f});

}

void Player::update() {
    prevPos = sprite.getPosition();

    // Ruch, po staniu w miejscu
    if (currentDir == sf::Vector2f(0.f, 0.f) && nextDir != sf::Vector2f(0.f, 0.f)) {
        sf::Vector2f startCheck = { position.x + nextDir.x * tileSize, position.y + nextDir.y * tileSize };
        if (!map->isWall(startCheck)) {
            currentDir = nextDir;
        }
    }

    // Ruch
    sprite.move(currentDir * speed);
    this->position = sprite.getPosition();

    // Logika skrzyżowań 
    sf::Vector2f mapOffset = map->getOffset();

    // Obecne położenie, na środku kafalka
    float gridX = std::round((position.x - mapOffset.x - tileSize / 2.f) / tileSize);
    float gridY = std::round((position.y - mapOffset.y - tileSize / 2.f) / tileSize);

    // Wyliczanie środka celu
    float centerX = gridX * tileSize + mapOffset.x + tileSize / 2.f;
    float centerY = gridY * tileSize + mapOffset.y + tileSize / 2.f;

    // odległość od celu    
    float dx = position.x - centerX;
    float dy = position.y - centerY;
    float distToCenter = dx * dx + dy * dy;

    if (distToCenter < (speed * 1.5f) * (speed * 1.5f)) {
        // Skręcanie
        if (nextDir != sf::Vector2f(0.f, 0.f) && nextDir != currentDir) {
            sf::Vector2f futurePos = { centerX + nextDir.x * tileSize, centerY + nextDir.y * tileSize };
            if (!map->isWall(futurePos)) {
                currentDir = nextDir;
                sprite.setPosition({ centerX, centerY });
            }
        }

        // Zatrzymanie
        sf::Vector2f aheadPos = { centerX + currentDir.x * tileSize, centerY + currentDir.y * tileSize };
        if (map->isWall(aheadPos)) {
            currentDir = { 0.f, 0.f };
            sprite.setPosition({ centerX, centerY });
        }

        this->position = sprite.getPosition();
    }
    updateAnimation();
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Player::getBounds() const {
    sf::Vector2f p = sprite.getPosition();
    return sf::FloatRect({ p.x - 6.f, p.y - 6.f }, { 12.f, 12.f });
}

void Player::handleInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))    nextDir = { 0.f, -1.f }; 
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))  nextDir = { 0.f, 1.f };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))  nextDir = { -1.f, 0.f };
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) nextDir = { 1.f, 0.f };

    if (nextDir == -currentDir) {
        currentDir = nextDir;
    }
}

sf::Vector2f Player::getDir() const {
    return currentDir;
}

void Player::updateAnimation() {
    

    if (currentDir != sf::Vector2f(0.f, 0.f)) {
        if (currentDir.x < 0)      visualDirectionOffset = 1 * 16; // Lewo
        else if (currentDir.x > 0) visualDirectionOffset = 0 * 16; // Prawo
        else if (currentDir.y < 0) visualDirectionOffset = 2 * 16; // Góra
        else if (currentDir.y > 0) visualDirectionOffset = 3 * 16; // Dół
    }
    int animationFrame = (animationClock.getElapsedTime().asMilliseconds() / 100) % 3;
    int tempY = visualDirectionOffset;
    textureRect.position.x = 3 + (animationFrame * 16);
    if (animationFrame == 2) tempY= 0;
    textureRect.position.y = tempY;
    sprite.setTextureRect(textureRect);

}

void Player::reset() {
    this->currentDir = { 1.f, 0.f };
    this->visualDirectionOffset = 0; 
    textureRect.position.x = 3 + (2 * 16);
    textureRect.position.y = 0;
    sprite.setTextureRect(textureRect);

}

void Player::goBack() {
    sprite.setPosition(prevPos);
    this->position = prevPos;

}

void Player::setPosition(sf::Vector2f pos) {
    Character::setPosition(pos);
    sprite.setPosition(pos);
    this->prevPos = pos;
}





