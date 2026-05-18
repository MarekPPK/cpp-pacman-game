#pragma once
#include<SFML/Graphics.hpp>

class MenuGhost {
private:
    sf::Sprite sprite;
    sf::IntRect textureRect;
    sf::Vector2f currentDir;
    float speed;
    sf::Clock animationClock;
    sf::Vector2f startPos;
    int ghostColor;

public:
    MenuGhost(float speed, int color, sf::Vector2f pos, sf::Texture& tex);
    void update();
    void draw(sf::RenderWindow& window);
    void reScale(float x, float y);
};
