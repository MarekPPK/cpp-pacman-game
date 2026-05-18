#include "Ghost.h"
#include "Map.h"
#include "Player.h"
#include <cmath>
import GameConstants;
import Functions;

Ghost::Ghost(float speed, sf::Vector2f Pos, GhostType t, Player* p, Map* m, sf::Texture& tex, Ghost* bRef)
    : Character(speed, Pos,tex), type(t), pacman(p), map(m), blinkyRef(bRef),
    tileSize(Config::Tile::Size), offsetY(Config::Tile::OffsetY), frightened(false) 
{
    visualDirectionOffset = 0;
    int row = 0;
    switch (type) {
    case GhostType::Blinky: row = 0; break; 
    case GhostType::Pinky:  row = 1; break; 
    case GhostType::Inky:   row = 2; break; 
    case GhostType::Clyde:  row = 3; break; 
    }
    int startY = 64;
    textureRect = sf::IntRect({ 4,startY+(row*16)}, {16, 16});
    sprite.setTextureRect(textureRect);
    sprite.setOrigin({ 8.f, 8.f}); 
    sprite.setScale({ 1.5f, 1.5f});
    sf::Vector2f startPos;
    startPos.x = std::floor(Pos.x / tileSize) * tileSize + tileSize / 2.f;
    startPos.y = std::floor((Pos.y - offsetY) / tileSize) * tileSize + tileSize / 2.f + offsetY;
    this->setPosition(startPos);
    currentDir = { 0.f, 0.f };
    decideDirection();
    readyToLeave = false;
}

void Ghost::setFrightened(bool value, bool isEndingPhase) {
    frightened = value;
    isFruitEnding = isEndingPhase;
}

bool Ghost::isFrightened() const { return frightened; }

sf::Vector2f Ghost::getCornerTarget() {
    float mapWidth = map->getBoardWidthPixels();
    float mapHeight = map->getBoardHeightPixels(); 
    sf::Vector2f offset = map->getOffset();

    switch (type) {
    case GhostType::Blinky: return { mapWidth+offset.x, 0.f };      
    case GhostType::Pinky:  return { offset.x, offset.y };           
    case GhostType::Inky:   return { mapWidth+offset.x, mapHeight+offset.y };
    case GhostType::Clyde:  return { offset.x, mapHeight+offset.y };    
    default: return { offset.x, offset.y };
    }
}

/*
Ustala kafelek w który mają celować duchy i zapewnia że wychodzą one z domku
*/
void Ghost::decideDirection() {
    sf::Vector2f targetTile;
    sf::Vector2f currentPos = sprite.getPosition();
    bool isInHouse = map->isInsideGhostHouse(currentPos);

    // Zapewnienie że duch wyjdzie z domku
    if (isInHouse) {
        if (readyToLeave) {
           targetTile = map->getGhostExitPos();
       }
        else {
            float houseCenterY = map->getOffset().y + (14 * map->getTilesize());
            float offset = std::sin(animationClock.getElapsedTime().asSeconds() * 5.f) * 20.f;
            targetTile = { currentPos.x, houseCenterY + offset };
        }
        
    }
    else if (frightened) {
        targetTile = getCornerTarget();
    }
    else {
        switch (type) {
        // Celem jest pacman
        case GhostType::Blinky:
            targetTile = pacman->getPosition();
            break;
        // Celem jest punkt oddalony o 4 kafelki od pacmana, w kierunku w którym jest skierowany pacman
        case GhostType::Pinky:
            targetTile = pacman->getPosition() + (pacman->getDir() * (tileSize * 4.f));
            break;
        case GhostType::Inky: {
            if (blinkyRef) {
                //Pivot to kafelek oddalony o 2 kafelki od pacmana
                sf::Vector2f pivot = pacman->getPosition() + (pacman->getDir() * (tileSize * 2.f));
                //Celem jest dwukrotny wektor od blinkiego
                targetTile = pivot + (pivot - blinkyRef->getPosition());
            }
            else targetTile = pacman->getPosition();
            break;
        }
        case GhostType::Clyde:
            if (Func::getDistanceSq(currentPos, pacman->getPosition()) > std::pow(tileSize * 8.f, 2))
                // Jeśli jest wystarczająco daleko celem jest pacman
                targetTile = pacman->getPosition();
            else {
                // Jeśli się zbliży za bardzo to ucieka do swojego rogu
                targetTile = { 0.f, 800.f };
            }
            break;
        }
    }
    // Wybieranie najlepszego kierunku na podstawie celu
    currentDir = Func::calculateBestDir(currentPos, targetTile, map, currentDir);
}

void Ghost::update() {
    if (currentDir == sf::Vector2f(0.f, 0.f)) decideDirection();

    prevPos = sprite.getPosition();
    // Ruch
    sprite.move(currentDir * speed);
    this->position = sprite.getPosition();

    sf::Vector2f mapOffset = map->getOffset();
    float mapWidth = map->getBoardWidthPixels();

    if (position.x >= mapOffset.x - tileSize && position.x <= mapOffset.x + mapWidth + tileSize) {
        // Środek kafelka docelowego
        float centerX = std::floor((position.x - mapOffset.x) / tileSize) * tileSize + tileSize / 2.f + mapOffset.x;
        float centerY = std::floor((position.y - offsetY) / tileSize) * tileSize + tileSize / 2.f + offsetY;

        float distToCenter = std::sqrt(Func::getDistanceSq(position, { centerX, centerY }));

        // Logika skrzyżowania
        if (distToCenter < speed) {
            this->setPosition({ centerX, centerY });
            decideDirection();
            sprite.move(currentDir * speed);
            this->position = sprite.getPosition();
        }
    }
    updateAnimation();
}

void Ghost::updateAnimation() {
    int yPos = 64;
    if (!frightened) {
        yPos += (static_cast<int>(type) * 16);
    }

    int legFrame = (animationClock.getElapsedTime().asMilliseconds() / 200) % 2;
    int xPos = 3;

    if (frightened) {
        int baseFrame = 8; 

        if (isFruitEnding) {
            int flash = (animationClock.getElapsedTime().asMilliseconds() / 250) % 2;
            if (flash == 1) {
                baseFrame = 10; 
            }
        }
        xPos += (baseFrame + legFrame) * 16;
    }
    else {
        if (currentDir.x < 0)      visualDirectionOffset = 2 * 16;
        else if (currentDir.x > 0) visualDirectionOffset = 0 * 16;
        else if (currentDir.y < 0) visualDirectionOffset = 4 * 16;
        else if (currentDir.y > 0) visualDirectionOffset = 6 * 16;

        xPos += visualDirectionOffset + (legFrame * 16);
    }

    textureRect.position = { xPos, yPos };
    sprite.setTextureRect(textureRect);
}

void Ghost::reset() {
    this->currentDir = { 0.f, 0.f };
    this->frightened = false;
    int row = 0;
    switch (type) {
    case GhostType::Blinky: row = 0; break;
    case GhostType::Pinky:  row = 1; break;
    case GhostType::Inky:   row = 2; break;
    case GhostType::Clyde:  row = 3; break;
    }

    int startY = 64;
    textureRect.position.x = 3; 
    textureRect.position.y = startY + (row * 16);
    sprite.setTextureRect(textureRect);
}

void Ghost::goBack() {
    Character::goBack();
    decideDirection();
}

void Ghost::draw(sf::RenderWindow& window) { window.draw(sprite); }

sf::FloatRect Ghost::getBounds() const {
    sf::Vector2f p = sprite.getPosition();
    return sf::FloatRect({ p.x - 4.f, p.y - 4.f }, { 8.f, 8.f });
}

void Ghost::setPosition(sf::Vector2f pos) {
    this->position = pos;
    sprite.setPosition(pos);
    this->prevPos = pos;
}


