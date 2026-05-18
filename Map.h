#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "AudioManager.h"

class Character;
class Player;
class Ghost;

class Map {
private:
    float tileSize;
    std::vector<std::vector<int>> board;
    sf::Vector2f offset;
    int dotsToCollect;
    float circlesize;
   
    sf::RectangleShape wall;
    sf::CircleShape dot;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    
    int score;
    sf::Vector2f playerPos;
    std::vector<sf::Vector2f> ghostPositions;
    AudioManager& audio;

public:
    Map(AudioManager& audioManager);

    void loadFromFile(const std::string& filename);
    void draw(sf::RenderWindow& window);

    int checkCollision(Character& c);
    bool isWall(sf::Vector2f targetPos, sf::Vector2f currentPos = { 0,0 }, bool isGhost = false);
    bool isInsideGhostHouse(sf::Vector2f pos) const;
    sf::Vector2f getGhostExitPos() const;

    void resetScore();
    void addScore(int points) { score += points; }
    void setPlayerpos(sf::Vector2f pos) { playerPos = pos; }

    int getScore() const { return score; }
    sf::Vector2f getPlayerpos() const { return playerPos; }
    std::vector<sf::Vector2f> getGhostpos() const { return ghostPositions; }
    float getTilesize() const { return tileSize; }
    sf::Vector2f getOffset() const { return offset; }
    int getDotsCount() const { return dotsToCollect; }

    float getBoardWidthPixels() const;
    float getBoardHeightPixels() const;

};