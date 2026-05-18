
#pragma once
#include <SFML/Audio.hpp>

#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <map>
#include <memory>
#include <stop_token>
#include <list>



enum class SoundEvent { Menu,Start, WakaWaka, GhostEat, Death, PowerUp };

class AudioManager {
private:
    std::map<SoundEvent, sf::SoundBuffer> buffers;
    sf::Music backgroundMusic;
    std::list<sf::Sound> activeSounds;
    
    // WĄTKI
    std::mutex queueMutex;
    std::condition_variable_any cv;
    std::queue<SoundEvent> eventQueue;
    std::jthread workerThread;

    void processQueue(std::stop_token stoken); 

public:
    AudioManager();
    ~AudioManager();

    void play(SoundEvent event);
    void stopAll();
    void playMenuMusic();
};