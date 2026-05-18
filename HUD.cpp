#include "Hud.h"
#include <iostream>
#include <ranges>
import GameConstants;


HUD::HUD(sf::Texture& tex) : gameWon(fontArcade),topScores(fontArcade), menuText(fontArcade),scoreText(fontArcade), gameOver(fontArcade), endScore(fontArcade), lastScore(fontArcade), titleText(fontArcade), inputContent(fontArcade), hintText(fontArcade), highestScore(fontArcade), LifeIcon(tex),FruitIcon(tex) {
  
    if (!fontArcade.openFromFile("assets/Fonts/PressStart2P.ttf")) {
        std::cerr << "Błąd: Nie znaleziono czcionki arial.ttf dla HUD!" << std::endl;
    }
    scores.clear();
    for (int i = 0; i < 3; ++i) {
        scores.emplace_back(fontArcade);
    }
    for (auto& t : scores) {
        t.setFont(fontArcade);
        t.setCharacterSize(30);
    }
    lastScore.setFont(fontArcade);
    lastScore.setCharacterSize(30);

    scoreText.setFont(fontArcade);
    scoreText.setString("SCORE: 0");
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({ Config::Tile::OffsetX, Config::Tile::OffsetY-2*Config::Tile::Size});

    highestScore.setFont(fontArcade);
    highestScore.setString("TOP: ");
    highestScore.setCharacterSize(30);
    highestScore.setFillColor(sf::Color::White);
    highestScore.setPosition({ Config::Tile::OffsetX+16*Config::Tile::Size, Config::Tile::OffsetY - 2 * Config::Tile::Size });

    topScores.setFont(fontArcade);
    topScores.setString("---TOP SCORES---");
    topScores.setCharacterSize(50);
    topScores.setFillColor(sf::Color::Yellow);
    topScores.setPosition({ 0.f,Config::Tile::Size*4 });

    menuText.setString("PAC-MAN");
    menuText.setCharacterSize(30);
    menuText.setFillColor(sf::Color::Yellow);
    menuText.setPosition({ Config::Tile::Size*12,Config::Tile::Size * 8 });
    
    gameOver.setFont(fontArcade);
    gameOver.setString("GAME OVER");
    gameOver.setCharacterSize(40);
    gameOver.setFillColor(sf::Color::Red);
    gameOver.setPosition({ Config::Tile::Size * 12,Config::Tile::Size * 8 });

    gameWon.setFont(fontArcade);
    gameWon.setString("VICTORY");
    gameWon.setCharacterSize(60);
    gameWon.setFillColor(sf::Color::Yellow);
    gameWon.setPosition({ Config::Tile::Size * 12,Config::Tile::Size * 8 });
   
    endScore.setFont(fontArcade);
    endScore.setString("YOUR SCORE: ");
    endScore.setCharacterSize(40);
    endScore.setFillColor(sf::Color::Red);
    endScore.setPosition({ Config::Tile::Size * 4,Config::Tile::Size * 16 });

    LifeRect = sf::IntRect({ 3 + 16,16 }, { 16,16 });
    LifeIcon.setTextureRect(LifeRect);
    LifeIcon.setScale({ 2.0f,2.0f });

    FruitRect = sf::IntRect({ 3 + 2 * 16,16 * 3 }, { 16,16 });
    FruitIcon.setTextureRect(FruitRect);
    FruitIcon.setScale({ 2.0f,2.0f });

}

void HUD::update(int score, int highest, bool flag) {
    scoreText.setString("SCORE: " + std::to_string(score));
    highestScore.setString("TOP: " + std::to_string(highest));
    if (flag) {
        endScore.setString("YOUR SCORE: " + std::to_string(score));
    }
}

void HUD::draw(sf::RenderWindow& window, bool flag,int lives,int fruits) {
    if (flag) {
        window.draw(scoreText);
        window.draw(highestScore);
        float x = Config::Tile::OffsetX;
        float y = Config::Window::Width+ 2 * Config::Tile::Size;
        float spacing = 35.f;

        for (int i = 0;i < lives;i++) {
            LifeIcon.setPosition({ x + (i * spacing),y });
            window.draw(LifeIcon);
        }
        for (int i = 0;i < fruits;i++) {
            FruitIcon.setPosition({ Config::Window::Width - x - (i * spacing),y });
            window.draw(FruitIcon);
        }
    }
    else {
        endScore.setFillColor(sf::Color::Red);
        window.draw(gameOver);
        window.draw(endScore);
    }
        
}

void HUD::drawVictory(sf::RenderWindow& window) {
    endScore.setFillColor(sf::Color::Yellow);
    window.draw(endScore);
    window.draw(gameWon);
}

void HUD::drawMenu(sf::RenderWindow& window) {
    window.draw(menuText);
}

void HUD::drawScores(const std::vector<ScoreEntry>& vec, const std::string& currentPlayerName, int lastResult, sf::RenderWindow& window) {
    bool isLastInTop = false;
    // RANGES
    auto topScoress = vec | std::views::take(3);

    int i = 0;
    for (const auto& entry : topScoress) {
        if (entry.name == currentPlayerName && entry.score == lastResult) {
            scores[i].setFillColor(sf::Color::Yellow);
            isLastInTop = true;
        }
        else {
            switch (i) {
            case 0: scores[i].setFillColor(sf::Color::Red); break;
            case 1: scores[i].setFillColor(sf::Color(255, 182, 193)); break;
            case 2: scores[i].setFillColor(sf::Color::Cyan); break;
            }
        }
        scores[i].setString(std::format("{}. {}: {}", i + 1, entry.name, entry.score));
        scores[i].setPosition({ Config::Tile::Size*6, Config::Tile::Size * 12+ (i * Config::Tile::Size*2) });
        window.draw(scores[i]);
        i++;
    }

    lastScore.setString(std::format("LAST SCORE: {}", lastResult));
    lastScore.setPosition({ Config::Tile::Size * 6, Config::Tile::Size * 28 });
    lastScore.setFillColor(isLastInTop ? sf::Color::Yellow : sf::Color::White);
    window.draw(lastScore);
    window.draw(topScores);
}

void HUD::drawNameInput(sf::RenderWindow& window, const std::string& currentName) {

    titleText.setString("ENTER YOUR NAME:");
    titleText.setPosition({ Config::Tile::Size * 2, Config::Tile::Size * 8 });
    window.draw(titleText);

    inputContent.setString(currentName + "_");
    inputContent.setPosition({ Config::Tile::Size * 2, Config::Tile::Size * 12 });
    window.draw(inputContent);

    hintText.setString("PRESS ENTER TO START\n\n(3-12 LETTERS OR NUMBERS)");
    hintText.setPosition({ Config::Tile::Size * 2, Config::Tile::Size * 18});
    window.draw(hintText);
}