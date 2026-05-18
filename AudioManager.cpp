#include "AudioManager.h"

#include <iostream>


AudioManager::AudioManager() {
    if (!buffers[SoundEvent::Menu].loadFromFile("assets/Audio/menu.wav")) std::cout << "Brak menu.wav\n";
    if (!buffers[SoundEvent::Start].loadFromFile("assets/Audio/start.wav")) std::cout << "Brak start.wav\n";
    if (!buffers[SoundEvent::Death].loadFromFile("assets/Audio/death.wav")) std::cout << "Brak death.wav\n";
    if (!buffers[SoundEvent::GhostEat].loadFromFile("assets/Audio/death_1.wav")) std::cout << "Brak ghost.wav\n";
    if (!buffers[SoundEvent::WakaWaka].loadFromFile("assets/Audio/eat_dot.wav")) std::cout << "Brak eat_dot.wav\n";
    if (!buffers[SoundEvent::PowerUp].loadFromFile("assets/Audio/eat_fruit.wav")) std::cout << "Brak eat_fruit.wav\n";

    workerThread = std::jthread([this](std::stop_token st) {
        this->processQueue(st);
        });
}

AudioManager::~AudioManager() {
    cv.notify_all();
}

void AudioManager::play(SoundEvent event) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        eventQueue.push(event);
    }
    cv.notify_one(); 
}

void AudioManager::processQueue(std::stop_token stoken) {
    while (!stoken.stop_requested()) { 
        std::unique_lock<std::mutex> lock(queueMutex);

        cv.wait(lock, stoken, [this] { return !eventQueue.empty(); });

        if (stoken.stop_requested()) return;

        SoundEvent event = eventQueue.front();
        eventQueue.pop();
        lock.unlock(); 

        // Usuwamy z listy dźwięki, które już skończyły grać
        activeSounds.remove_if([](const sf::Sound& s) {
            return s.getStatus() == sf::SoundSource::Status::Stopped;
            });

        // Dodajemy nowy dźwięk bezpośrednio z buforem
        activeSounds.emplace_back(buffers[event]).play();
    }
}

void AudioManager::stopAll() {
    for(auto& s : activeSounds) {
        s.stop();
    }
    backgroundMusic.stop();

}

void AudioManager::playMenuMusic() {
    if (!backgroundMusic.openFromFile("assets/Audio/menu.wav")) {
        std::cout << "Błąd: Nie znaleziono menu.wav" << std::endl;
        return;
    }
    backgroundMusic.setLooping(true); 
    backgroundMusic.setVolume(20.f);  
    backgroundMusic.play();
}

