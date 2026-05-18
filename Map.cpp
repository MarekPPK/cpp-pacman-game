#include "Map.h"
#include "Character.h"
#include "Player.h"
#include "Ghost.h"
#include <fstream>
#include <cmath>
#include <iostream>
import GameConstants;

Map::Map(AudioManager& audioManager) :audio(audioManager),backgroundSprite(backgroundTexture) {
    tileSize = Config::Tile::Size;
    circlesize = Config::Gameplay::DotRadius;
    wall.setSize({ tileSize, tileSize });
    wall.setFillColor(sf::Color::Blue);
    dot.setRadius(3.f);
    dot.setFillColor(sf::Color::White);
    score = Config::Gameplay::ZeroPoints;
    offset = { Config::Tile::OffsetX, Config::Tile::OffsetY};
    if (!backgroundTexture.loadFromFile("assets/Images/background.png")) {
        std::cerr << "Blad ladowania tekstur!" << std::endl;
    }
    backgroundSprite.setTexture(backgroundTexture, true); 
}

void Map::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    board.clear();
    ghostPositions.clear();

    while (std::getline(file, line)) {
        std::vector<int> row;
        for (char c : line) {
            int val = c - '0';
            if (val == Config::Tile::Pacman) { 
                playerPos = {
                    (float)row.size() * tileSize + offset.x + (tileSize / 2.f),
                    (float)board.size() * tileSize + offset.y + (tileSize / 2.f)
                };
                val = Config::Tile::Empty;
            }
            if (val == Config::Tile::Ghost) { 
                sf::Vector2f gPos = {
                    (float)row.size() * tileSize + offset.x + (tileSize / 2.f),
                    (float)board.size() * tileSize + offset.y + (tileSize / 2.f)
                };
                ghostPositions.push_back(gPos);
                val = Config::Tile::Empty;
            }
            row.push_back(val);
        }
        board.push_back(row);
    }
    if (board.size() > 0 && board[0].size() > 0) {
        float logicalWidth = board[0].size() * tileSize;
        float logicalHeight = board.size() * tileSize;

        sf::Vector2u texSize = backgroundTexture.getSize();

        if (texSize.x > 0 && texSize.y > 0) {
            backgroundSprite.setScale({
                logicalWidth / static_cast<float>(texSize.x),
                logicalHeight / static_cast<float>(texSize.y)
                });
        }
        backgroundSprite.setPosition(offset);
    }
    dotsToCollect = 0;
    for (int y = 0; y < board.size(); y++) {
        for (int x = 0; x < board[0].size(); x++) {
            if (board[y][x] == Config::Tile::Dot || board[y][x] == Config::Tile::SuperFruit) {
                dotsToCollect++;
            }
        }
    }
}

void Map::draw(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
    for (int i = 0; i < (int)board.size(); i++) {
        for (int j = 0; j < (int)board[i].size(); j++) {
            sf::Vector2f drawPos = { j * tileSize + offset.x, i * tileSize + offset.y };
            if (board[i][j] == Config::Tile::Dot) { 
                dot.setPosition({
                    drawPos.x + (tileSize / 2.f) - 3.f,
                    drawPos.y + (tileSize / 2.f) - 3.f
                    });
                window.draw(dot);
            }
            else if (board[i][j] == Config::Tile::SuperFruit) { 
                sf::CircleShape fruit;
                fruit.setRadius(7.f);
                fruit.setFillColor(sf::Color::Magenta);
                fruit.setPosition({ drawPos.x + tileSize / 2.f - 7.f, drawPos.y + tileSize / 2.f - 7.f });
                window.draw(fruit);
            }
        }
    }
}
/*
Sprawdzanie kolizji postaci z różnymi elementami
*/
int Map::checkCollision(Character& c) {
    sf::Vector2f pos = c.getPosition();
    float mapWidthPixels = getBoardWidthPixels();
    float leftEdge = offset.x;
    float rightEdge = offset.x + mapWidthPixels;

    // Logika tunelu
    if (pos.x < leftEdge) {
        c.setPosition({ rightEdge - 5.0f, pos.y });
        return 0;
    }
    else if (pos.x > rightEdge) {
        c.setPosition({ leftEdge + 5.0f, pos.y });
        return 0;
    }

    int gridX = static_cast<int>(std::floor((pos.x - offset.x) / tileSize));
    int gridY = static_cast<int>(std::floor((pos.y - offset.y) / tileSize));
    sf::FloatRect charBounds = c.getBounds();
    int result = 0; 

    for (int i = gridY - 1; i <= gridY + 1; i++) {
        for (int j = gridX - 1; j <= gridX + 1; j++) {
            if (i < 0 || i >= (int)board.size() || j < 0 || j >= (int)board[i].size()) continue;

            int tileType = board[i][j];
            if (tileType == 0) continue;

            sf::FloatRect tileBounds({ j * tileSize + offset.x, i * tileSize + offset.y }, { tileSize, tileSize });

            if (tileType == Config::Tile::Wall) { 
                if (charBounds.findIntersection(tileBounds)) {
                    if (dynamic_cast<Ghost*>(&c)) {
                        c.goBack();
                    }
                    continue;
                }
            }
            else if (tileType == Config::Tile::Dot) { 
                if (dynamic_cast<Player*>(&c)) {
                    if (charBounds.findIntersection(tileBounds)) {
                        audio.play(SoundEvent::WakaWaka);
                        board[i][j] = 0;
                        score += Config::Gameplay::PointsPerDot;
                        dotsToCollect--;
                        
                    }
                }
            }
            else if (tileType == Config::Tile::Gate) { 
                if (dynamic_cast<Player*>(&c)) {
                    if (charBounds.findIntersection(tileBounds)) {
                        c.goBack();
                    }
                }
            }
            else if (tileType == Config::Tile::SuperFruit) { 
                if (dynamic_cast<Player*>(&c)) {
                    if (charBounds.findIntersection(tileBounds)) {
                        board[i][j] = 0;
                        audio.play(SoundEvent::PowerUp);
                        result = Config::Tile::SuperFruit; 
                        dotsToCollect--;
                        
                    }
                }
            }
        }
    }
    return result; 
}
/*
Funkcja sprawdza czy są jakieś przeszkody na drodze
*/
bool Map::isWall(sf::Vector2f targetPos, sf::Vector2f currentPos, bool isGhost) {
    // Przeliczamy pozycję pikselową na indeksy w tablicy 
    int x = static_cast<int>(std::floor((targetPos.x - offset.x) / tileSize));
    int y = static_cast<int>(std::floor((targetPos.y - offset.y) / tileSize));

    // Sprawdzanie granic pionwych/poziomych
    if (y < 0 || y >= (int)board.size()) {
        return true; // Poza mapą w pionie to zawsze ściana
    }
    
    // Logika tunelu
    if (x < 0 || x >= (int)board[y].size()) {
        return false;
    }

    // Pobieramy typ kafelka z tablicy
    int tileType = board[y][x];

    // Zwykła ściana
    if (tileType == Config::Tile::Wall) {
        return true;
    }

    // Logika bramki
    if (tileType == Config::Tile::Gate) {
        if (isGhost) {
            if (currentPos.y < targetPos.y) {
                // Duszki są poza domkiem
                return true;
            }
            // Duszki są jeszcze w domku
            return false;
        }
        else {
            // Dla pacmana nie można wejść do domku
            return true;
        }
    }
    // Brak przeszkód
    return false;
}

bool Map::isInsideGhostHouse(sf::Vector2f pos) const {
    int gridX = static_cast<int>((pos.x - offset.x) / tileSize);
    int gridY = static_cast<int>((pos.y - offset.y) / tileSize);
    return (gridX >= 10 && gridX <= 17 && gridY >= 12 && gridY <= 17);
}

sf::Vector2f Map::getGhostExitPos() const {
    float x = (13 * tileSize) + (tileSize / 2.f) + offset.x;
    float y = (11 * tileSize) + (tileSize / 2.f) + offset.y;
    return { x,y };
}

void Map::resetScore() {
    score = Config::Gameplay::ZeroPoints;
}

float Map::getBoardWidthPixels() const {
    if (board.empty() || board[0].empty()) {
        return 0.f;
    }
    return static_cast<float>(board[0].size()) * tileSize;
}

float Map::getBoardHeightPixels() const {
    return static_cast<float>(board.size()) * tileSize;
}

