#include "MenuGhost.h"
import GameConstants;
MenuGhost::MenuGhost(float speed, int color, sf::Vector2f pos, sf::Texture& tex) : speed(speed), sprite(tex), ghostColor(color) {
    currentDir = { 1.0f,0.f };
    sprite.setPosition(pos);
    textureRect = sf::IntRect({ 3+4*16,0 }, { 16,16 });
    sprite.setTextureRect(textureRect);
    sprite.setOrigin({ 8.f,8.f });
    startPos = pos;
    sprite.setScale({ 2.f,2.f });
}

void MenuGhost::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void MenuGhost::update() {
    sprite.move(currentDir * speed);
    sf::Vector2f pos = sprite.getPosition();
    float leftBound = 300.f;
    float rightBound = 500.f;

    if (pos.x >= rightBound) {
        currentDir = { -1.f, 0.f }; 
        sprite.setPosition({ rightBound, pos.y });
    }
    else if (pos.x <= leftBound) {
        currentDir = { 1.f, 0.f };  
        sprite.setPosition({leftBound, pos.y});
    }

    int time = animationClock.getElapsedTime().asMilliseconds() / 150;
    int frameX = time % 2; 
    int frameY = (currentDir.x > 0) ? 0 : 2; 
    textureRect.position.x = 3+(frameX+ frameY)* 16;
    textureRect.position.y = (4+ghostColor) * 16;
    sprite.setTextureRect(textureRect);
}

void MenuGhost::reScale(float x, float y) {
    sprite.setScale({ x, y });
}