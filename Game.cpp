#include "Game.h"
#include <iostream>
#include <cmath>
#include <fstream>
#include <regex>
import GameConstants;
import Functions;



Game::Game() :
    window(sf::VideoMode({ Config::Window::Width, Config::Window::Height}), "Pac-Man"),
    currentState(GameState::Menu),
    map(audio),
    pacman(Config::Gameplay::PacmanSpeed, { 0,0 }, & map, spriteSheet),
    pacmanMenu(0.5f,{50.f,50.f},spriteSheet),
    pacmanMenu1(0.5f, { 90.f,110.f }, spriteSheet),
    blinky(Config::Gameplay::GhostSpeed, { 0,0 }, GhostType::Blinky, & pacman, & map, spriteSheet),
    pinky(Config::Gameplay::GhostSpeed, { 0,0 }, GhostType::Pinky, & pacman, & map, spriteSheet),
    inky(Config::Gameplay::GhostSpeed, { 0,0 }, GhostType::Inky, & pacman, & map, spriteSheet, & blinky),
    clyde(Config::Gameplay::GhostSpeed, { 0,0 }, GhostType::Clyde, & pacman, & map, spriteSheet),
    powerUpActive(false),
    waitingForIntro(false),
    Intro_duration(Config::Timer::IntroDuration),
    ghostsEatenCount(Config::Gameplay::EatenAtStart),
    lives(Config::Gameplay::InitialLives),
    gameOverflag(true),
    End_duration(Config::Timer::EndDuration),
    endFlag(false),
    isNameConfigured(false),
    hud(spriteSheet),
    fruitsEaten(0)
{
    if (!spriteSheet.loadFromFile("assets/Images/Sprites_only.png")) {
        std::cerr << "Blad ladowania tekstur!" << std::endl;
    }
    map.loadFromFile("assets/Data/map.txt");
    
    auto gPos = map.getGhostpos();
    pacman.setPosition(map.getPlayerpos());
    if (gPos.size() >= 4) {
        blinky.setPosition(gPos[0]);
        pinky.setPosition(gPos[1]);
        inky.setPosition(gPos[2]);
        clyde.setPosition(gPos[3]);
    }

    characters.push_back(&pacman);
    characters.push_back(&blinky);
    characters.push_back(&pinky);
    characters.push_back(&inky);
    characters.push_back(&clyde);
    
    for (int i = 0;i < 4;i++) {
        menuGhosts.emplace_back(0.2f, i, sf::Vector2f{ 300.f + (i * 50.f),250.f + (i * 50.f) }, spriteSheet);
    }
    
    if (menuFont.openFromFile("assets/Fonts/PressStart2P.ttf")) {
        playButton = new Button("PLAY", { 280,500}, menuFont);
        scoresButton = new Button("SCORES", { 280,600 }, menuFont);
    }
    audio.playMenuMusic();
    pacmanMenu.reScale(5.f, 5.f);
    pacmanMenu1.reScale(5.f, 5.f);
}

Game::~Game() {
    delete playButton;
    delete scoresButton;
}

// REGEX, RANGES
std::vector<ScoreEntry> Game::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::vector<ScoreEntry> allScores;

    std::regex pattern(R"(^([^:]+):\s*(\d+)$)");
    std::smatch matches;

    while (std::getline(file, line)) {
        if (std::regex_search(line, matches, pattern)) {
            allScores.push_back({ matches[1].str(), std::stoi(matches[2].str()) });
        }
    }

    std::ranges::sort(allScores, [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
        });

    return allScores;
}

// FILESYSTEM
void Game::saveCurrentScore() {
    fs::path saveDir = "saves";
    if (!fs::exists(saveDir)) {
        fs::create_directory(saveDir);
    }

    fs::path saveFile = saveDir / "scores.txt"; 

    std::ofstream file(saveFile, std::ios::app);
    if (file.is_open()) {
        file << playerName << ": " << map.getScore() << "\n";
    }
}
void Game::resetFullGame() {
    map.loadFromFile("assets/Data/map.txt");
    map.resetScore();
    endFlag = false;
    gameOverflag = true;
    lives = Config::Gameplay::InitialLives;
    ghostsEatenCount = Config::Gameplay::EatenAtStart;
    fruitsEaten = 0;
    powerUpActive = false;
    waitingForIntro = true;
    blinky.setReadyToLeave(false);
    pinky.setReadyToLeave(false);
    inky.setReadyToLeave(false);
    clyde.setReadyToLeave(false);

    pacman.setPosition(map.getPlayerpos());
    auto gPos = map.getGhostpos();
    if (gPos.size() >= 4) {
        blinky.setPosition(gPos[0]);
        pinky.setPosition(gPos[1]);
        inky.setPosition(gPos[2]);
        clyde.setPosition(gPos[3]);
    }

    for (auto* g : { &blinky, &pinky, &inky, &clyde }) {
        g->setFrightened(false);
    }

    int topScore = scores.empty() ? 0 : scores[0].score;
    hud.update(Config::Gameplay::ZeroPoints, topScore, endFlag);
    audio.stopAll();
    audio.play(SoundEvent::Start);
    introClock.restart();
    
    spriteSheet.setSmooth(false);
}

void Game::resetPositions() {
    pacman.reset();
    blinky.reset();
    pinky.reset();
    inky.reset();
    clyde.reset();
}

void Game::PollEvents() {
    while (std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            audio.stopAll();
            window.close();
        }
        if (currentState == GameState::Menu) {
            if (const auto* mouseButtonEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonEvent->button == sf::Mouse::Button::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    // FILESYSTEM
                    if (playButton->isClicked(mousePos)) { 
                        fs::path savePath = fs::current_path() / "saves" / "scores.txt";
                        scores = loadFromFile(savePath.string());
                        int topScore = scores.empty() ? 0 : scores[0].score;
                        hud.update(Config::Gameplay::ZeroPoints, topScore, endFlag);
                        if (!isNameConfigured)currentState = GameState::EnterName;
                        else {
                            currentState=GameState::Playing; resetFullGame();
                        }
                    }
                    if (scoresButton->isClicked(mousePos)) {
                        // FILESYSTEM
                        fs::path savePath = fs::current_path() / "saves" / "scores.txt";
                        if (fs::exists(savePath)) {
                            scores = loadFromFile(savePath.string()); 
                            currentState = GameState::Scores;
                        }
                        else {
                            std::cerr << "Nie znaleziono pliku wyników w: " << savePath << std::endl;
                        }
                    }
                }
            }
        }
        if (currentState == GameState::EnterName) {
            if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                char c = static_cast<char>(textEntered->unicode);
                uint32_t unicode = textEntered->unicode;
                
                bool isBackspace = (textEntered->unicode == 8);
                if (isBackspace) { 
                    if (!playerName.empty()) playerName.pop_back();
                }
                else if (unicode >= 32 && unicode <= 126 && playerName.length() < 12) {
                    char c = static_cast<char>(unicode);
                    if (c >= 'a' && c <= 'z') {
                        c -= 32;
                    }

                    playerName += c;
                    hud.getInputContent().setFillColor(sf::Color::White); 
                }
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Enter) {
                    if (Func::isNameValid(playerName)) {
                        hud.getInputContent().setFillColor(sf::Color::White);
                        currentState = GameState::Playing;
                        resetFullGame();
                        isNameConfigured = true;
                        playButton->setText("PLAY AGAIN");
                    }
                    else {
                        hud.getInputContent().setFillColor(sf::Color::Red);
                    }
                }
            }
        }
        if (currentState == GameState::Scores || currentState==GameState::Playing) {
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {

                    if (currentState == GameState::Playing) {
                        audio.stopAll(); audio.playMenuMusic();
                    }
                    currentState = GameState::Menu;
                }
            }
        }
    }
}

void Game::update() {
    if (currentState == GameState::Menu) {
        pacmanMenu.update();
        pacmanMenu1.update();
        for (auto& elem : menuGhosts) {
            elem.update();
        }
    }
    if (currentState == GameState::GameOver) {
        if (endClock.getElapsedTime().asSeconds() > End_duration) {
            currentState = GameState::Menu; 
            audio.playMenuMusic();          
        }
        return; 
    }
    if (currentState == GameState::Won) {
        if (endClock.getElapsedTime().asSeconds() > End_duration) {
            currentState = GameState::Menu;
            audio.playMenuMusic();
        }
        return; 
    }
    
    if (currentState == GameState::Dying) {
        if (deathClock.getElapsedTime().asSeconds() > 2.5f) {
            pacman.setPosition(map.getPlayerpos());
            auto gPos = map.getGhostpos();
            std::vector<Ghost*> allGhosts = { &blinky, &pinky, &inky, &clyde };
            for (int i = 0; i < 4; i++) {
                allGhosts[i]->setPosition(gPos[i]);
                allGhosts[i]->setFrightened(false);
                allGhosts[i]->setReadyToLeave(false);
            }
            ghostsEatenCount = Config::Gameplay::EatenAtStart;
            powerUpActive = false;

            currentState = GameState::Playing;
            audio.play(SoundEvent::Start);
            introClock.restart();
            waitingForIntro = true;
        }
        return;
    }
    if (currentState != GameState::Playing) {
        return;
    }
    if (map.getDotsCount() == 0) {
        currentState = GameState::Won;
        endFlag = true;
        int topScore = scores.empty() ? 0 : scores[0].score;
        hud.update(map.getScore(), topScore, endFlag);
        endClock.restart(); 
        saveCurrentScore(); 
        return;
    }

    if (waitingForIntro) {
        if (introClock.getElapsedTime().asSeconds() >= Intro_duration) {
            waitingForIntro = false;
            introClock.restart(); // RESTARTUJEMY zegar, by elapsed liczyło od zera po intro
        }
        return;
    }

    if (currentState == GameState::Playing && !waitingForIntro) {
        float elapsed = introClock.getElapsedTime().asSeconds() - Intro_duration;
        float cooldown = (lives < Config::Gameplay::InitialLives) ? 1.5f : 3.0f;
        blinky.setReadyToLeave(true);
        if (elapsed > cooldown)pinky.setReadyToLeave(true);
        if (elapsed > cooldown*2)inky.setReadyToLeave(true);
        if (elapsed > cooldown*3)clyde.setReadyToLeave(true);
    }

    pacman.handleInput();

    // Ruch i kolizje z mapą
    for (auto c : characters) {
        c->update();
        int result = map.checkCollision(*c);

        if (result == Config::Tile::SuperFruit&& dynamic_cast<Player*>(c)) {
            powerUpActive = true;
            fruitsEaten++;
            powerUpClock.restart();
            ghostsEatenCount = Config::Gameplay::EatenAtStart;
            map.addScore(Config::Gameplay::PointsPerDot);
            for (auto* g : { &blinky, &pinky, &inky, &clyde }) g->setFrightened(true);
        }
    }

    // Licznik power-upa
    if (powerUpActive) {
        float elapsed = powerUpClock.getElapsedTime().asSeconds();
        std::vector<Ghost*> ghosts = { &blinky, &pinky, &inky, &clyde };

        if (elapsed > 10.f) {
            // Koniec
            powerUpActive = false;
            for (auto* g : ghosts) g->setFrightened(false, false);
        }
        else if (elapsed > 7.f) {
            // Ostatnie 3 sekundy
            for (auto* g : ghosts) {
                if (g->isFrightened()) g->setFrightened(true, true);
            }
        }
    }

    // Kolizje między postaciami
    std::vector<Ghost*> ghosts = { &blinky, &pinky, &inky, &clyde };
    sf::FloatRect pacmanBounds = pacman.getBounds();

    for (Ghost* g : ghosts) {
        if (pacmanBounds.findIntersection(g->getBounds())) {
            if (g->isFrightened()) {
                int reward = Config::Gameplay::PointsPerGhost * static_cast<int>(std::pow(2, ghostsEatenCount));
                map.addScore(reward);
                ghostsEatenCount++;
                audio.play(SoundEvent::GhostEat);
                g->setPosition(map.getGhostpos()[0]);
                g->setFrightened(false,false);
            }
            else {
                lives--;
                audio.play(SoundEvent::Death);
                resetPositions();
                if (lives <= 0) {
                    endFlag = true;
                    currentState = GameState::GameOver;
                    endClock.restart();
                    saveCurrentScore();
                    
                }
                else {
                    
                    currentState = GameState::Dying;
                    deathClock.restart();
                }
                break;
            }
        }
    }
    int topScore = scores.empty() ? 0 : scores[0].score;
    hud.update(map.getScore(), topScore,endFlag);
    
}

void Game::render() {
    window.clear(sf::Color::Black);

    switch (currentState) {
    case GameState::Menu:
        playButton->draw(window);
        scoresButton->draw(window);
        pacmanMenu.draw(window);
        pacmanMenu1.draw(window);
        for (auto& ghost : menuGhosts) ghost.draw(window);
        hud.drawMenu(window);
        break;

    case GameState::Playing:
    case GameState::Dying:
        map.draw(window);
        for (auto* c : characters) c->draw(window);
        hud.draw(window, gameOverflag, lives, fruitsEaten);
        break;

    case GameState::Won:
        map.draw(window);
        hud.drawVictory(window);
        break;

    case GameState::EnterName:
        hud.drawNameInput(window, playerName);
        break;

    case GameState::GameOver:
        map.draw(window);
        gameOverflag = false; 
        hud.draw(window, gameOverflag, lives, fruitsEaten);
        break;

    case GameState::Scores:
        hud.drawScores(scores, playerName, map.getScore(), window);
        break;
    }

    window.display();
}

sf::RenderWindow& Game::getWindow() {
    return window;
}

