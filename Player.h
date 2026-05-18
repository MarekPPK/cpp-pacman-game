#pragma once
#include "Character.h"

class Map;

class Player : public Character
{
private:
    Map* map;
    sf::Vector2f currentDir;
    sf::Vector2f nextDir;
    sf::Clock animationClock;
    int visualDirectionOffset;
    float tileSize;
    float offsetY;

public:
    Player(float speed, sf::Vector2f pos, Map* m, sf::Texture& tex);
    void update() override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void handleInput();
    sf::Vector2f getDir() const;
    void updateAnimation();
    void reset();
    void setPosition(sf::Vector2f pos) override;
    void goBack()override;
    
};