#include "MenuPacman.h"
import GameConstants;
MenuPacman::MenuPacman(float speed, sf::Vector2f pos, sf::Texture& tex) : speed(speed),sprite(tex) {
    currentDir = { 1.0f,0.f };
    sprite.setPosition(pos);
    textureRect = sf::IntRect({ 3,0 }, { 16,16 });
    sprite.setTextureRect(textureRect);
    sprite.setOrigin({ 8.f,8.f });
    startPos = pos;
}

void MenuPacman::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void MenuPacman::update() {
    sprite.move(currentDir * speed);
    sf::Vector2f position = sprite.getPosition();
    const float left = startPos.x;
    const float top = startPos.y;
    const float right = Config::Window::Width - startPos.x;  
    const float bottom = Config::Window::Height - startPos.y;

    if (position.x > right && position.y >= top) { 
        currentDir = { 0.f,1.f };
        sprite.setPosition({ right,top}); 
    }
    else if (position.x >= right&& position.y > bottom) {
        currentDir = { -1.f,0.f };
        sprite.setPosition({ right,bottom});
    }
    else if (position.x < left&& position.y >= bottom) {
        currentDir = { 0.f,-1.f };
        sprite.setPosition({ left,bottom});
    }
    else if (position.x <= left && position.y < top) {
        currentDir = { 1.f,0.f };
        sprite.setPosition({ left ,top});
    }
   

    int animationFrameY = (animationClock.getElapsedTime().asMilliseconds() / 100) % 4;
    int animationFrameX = (animationClock.getElapsedTime().asMilliseconds() / 100) % 2;
    textureRect.position.x = 3 + animationFrameX * 16;
    textureRect.position.y = animationFrameY * 16;
    sprite.setTextureRect(textureRect);
}

void MenuPacman::reScale(float x, float y) {
    sprite.setScale({ x, y });
}