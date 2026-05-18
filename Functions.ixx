module;

#include <regex>
#include <string>
#include <cmath>
#include <limits>
#include <array>
#include <SFML/Graphics.hpp>
#include "Map.h"

export module Functions;
export namespace Func {
    
    /*
    Funkcja sprawdza poprawność stringa według danego wzorca
    */
    // REGEX
    bool isNameValid(const std::string& name) {
        static const std::regex pattern("^[a-zA-Z0-9]{3,12}$");
        return std::regex_match(name, pattern);
    }

    /*
    Funkcja zwraca wartość odległości do kwadratu punktów a i b
    */
    float getDistanceSq(sf::Vector2f a, sf::Vector2f b) {
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        return dx * dx + dy * dy;
    }

    /*
    Funkcja zwraca wartość odległości do kwadratu punktów a i b uwzględniając szerokość mapy
    */
    float getWrappedDistanceSq(sf::Vector2f a, sf::Vector2f b, float mapWidth) {
        float dx = std::abs(a.x - b.x);
        float dy = std::abs(a.y - b.y);
        if (dx > mapWidth / 2.f) dx = mapWidth - dx;
        return dx * dx + dy * dy;
    }

    /*
    Funkcja zwraca najlepszy kierunek by osiągnąć cel dla duszków
    */
    sf::Vector2f calculateBestDir(sf::Vector2f currentPos, sf::Vector2f targetTile, Map* map, sf::Vector2f currentDir) {
        static const std::array<sf::Vector2f, 4> directions = {
            sf::Vector2f(0, -1), sf::Vector2f(-1, 0), sf::Vector2f(0, 1), sf::Vector2f(1, 0)
        };

        sf::Vector2f bestDirection = { 0, 0 };
        float minDict = std::numeric_limits<float>::max();
        float tilesize = map->getTilesize();
        float mapWidth = map->getBoardWidthPixels();

        for (const auto& direction : directions) {
            //Blokada zawracania
            if (direction.x == -currentDir.x && direction.y == -currentDir.y && currentDir != sf::Vector2f(0, 0))
                continue;

            sf::Vector2f nextTilePos = currentPos + (direction * tilesize);

            if (!map->isWall(nextTilePos, currentPos, true)) {
                float distance = getWrappedDistanceSq(nextTilePos, targetTile, mapWidth);

                if (distance < minDict) {
                    minDict = distance;
                    bestDirection = direction;
                }
            }
        }

        // Jeśli duch nie ma gdzie iść,idzie przed siebie
        return (bestDirection != sf::Vector2f(0, 0)) ? bestDirection : currentDir;
    }
}