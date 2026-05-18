#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Character.h"

class Map;
class Player;

enum class GhostType { Blinky, Pinky, Inky, Clyde };

class Ghost : public Character {
private:
    GhostType type;
    Player* pacman;
    Map* map;
    Ghost* blinkyRef;

    sf::Vector2f currentDir;
    float tileSize;
    float offsetY;
    bool readyToLeave;

    bool frightened;
    bool isFruitEnding;

    sf::Clock animationClock;
    int visualDirectionOffset;

    sf::Vector2f getCornerTarget();


public:
    Ghost(float speed, sf::Vector2f Pos, GhostType t, Player* p, Map* m, sf::Texture& tex, Ghost* bRef = nullptr);

    void setFrightened(bool value, bool isEndingPhase = false);
    bool isFrightened() const;
    void decideDirection();
    void setReadyToLeave(bool r) { readyToLeave = r; }

    void update() override;
    void reset();
    void draw(sf::RenderWindow& window) override;
    void goBack() override;
    sf::FloatRect getBounds() const override;
    void setPosition(sf::Vector2f pos) override;

    void updateAnimation();

};