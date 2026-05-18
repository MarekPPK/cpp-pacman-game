#pragma once

#include <SFML/Graphics.hpp>
#include <SFML//System.hpp>
#include<SFML/Window.hpp>
#include <vector>
#include <memory>
#include <filesystem>

#include "Ghost.h"
#include "Map.h"
#include "Player.h"
#include "HUD.h"
#include "Button.h"
#include "AudioManager.h"
#include "MenuPacman.h"
#include "MenuGhost.h"

namespace fs = std::filesystem;

enum class GameState { Menu,EnterName, Playing, Scores,Dying, GameOver, Won};

class Game
{
private:
    sf::RenderWindow window;
    GameState currentState;
    AudioManager audio;

    sf::Texture spriteSheet;
    sf::Font menuFont;

    Map map;
    HUD hud;
    std::string playerName;
    std::vector<ScoreEntry> scores;
    
    Player pacman;
    Ghost blinky;
    Ghost pinky;
    Ghost inky;
    Ghost clyde;

    std::vector<Character*> characters;
    
    MenuPacman pacmanMenu;
    MenuPacman pacmanMenu1;
    std::vector<MenuGhost> menuGhosts;

    Button* playButton;
    Button* scoresButton;
    
    sf::Clock powerUpClock;
    sf::Clock introClock;
    sf::Clock endClock;
    sf::Clock deathClock;
    
    bool powerUpActive;
    bool waitingForIntro;
    bool gameOverflag;
    bool endFlag;
    bool isNameConfigured;

    int ghostsEatenCount;
    int lives;
    int fruitsEaten;
    
    const float Intro_duration;
    const float End_duration;
public:
    Game();
    ~Game();

    std::vector<ScoreEntry> loadFromFile(const std::string& filename);
    void saveCurrentScore();
    
    void resetFullGame();
    void resetPositions();

    void PollEvents();
    void update();
    void render();

    sf::RenderWindow& getWindow();
    
};