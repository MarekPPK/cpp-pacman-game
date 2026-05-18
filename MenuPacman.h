#pragma once
#include<SFML/Graphics.hpp>

class MenuPacman {
private:
    sf::Sprite sprite;
    sf::IntRect textureRect;
    sf::Vector2f currentDir;
    float speed;
    sf::Clock animationClock;
    sf::Vector2f startPos;

public:
    MenuPacman(float speed, sf::Vector2f pos,sf::Texture& tex);
    void update();
    void draw(sf::RenderWindow& window);
    void reScale(float x, float y);
};

