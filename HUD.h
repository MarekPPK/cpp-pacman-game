#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct ScoreEntry {
    std::string name;
    int score;
};

class HUD {
private:
    sf::Font fontArcade;

    

    sf::Text scoreText;
    sf::Text highestScore;
    sf::Text gameWon;
    sf::Text gameOver;
    sf::Text endScore;
    
    sf::Text titleText;
    sf::Text inputContent;
    sf::Text hintText;
    sf::Text menuText;
    
    sf::Text lastScore;
    sf::Text topScores;
    std::vector<sf::Text> scores;
    
    sf::Sprite LifeIcon;
    sf::IntRect LifeRect;
    sf::Sprite FruitIcon;
    sf::IntRect FruitRect;


public:
    HUD(sf::Texture& tex);
    void update(int score,int highest, bool flag);
    void draw(sf::RenderWindow& window, bool flag, int lives,int fruits);
    void drawScores(const std::vector<ScoreEntry>& vec, const std::string& currentPlayerName, int lastScore, sf::RenderWindow& window);
    void drawMenu(sf::RenderWindow& window);
    void drawNameInput(sf::RenderWindow& window, const std::string& currentName);
    sf::Text& getInputContent() { return inputContent; }
    void drawVictory(sf::RenderWindow& window);
};