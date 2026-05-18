#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

class Character
{
protected:
	sf::Vector2f prevPos; // Ostatnia bezpieczna pozycja
	sf::Vector2f position;// Obecna (testowana) pozycja
	sf::Vector2f startPos;
	sf::Sprite sprite;
	sf::IntRect textureRect;
	float speed;
public:
	Character(float speed, sf::Vector2f startPosition, const sf::Texture& tex) :speed(speed), prevPos(startPosition),startPos(startPosition), sprite(tex), position(startPosition) { sprite.setPosition(position); }
	virtual void update() = 0;
	virtual void draw(sf::RenderWindow& window) = 0;
	virtual sf::FloatRect getBounds() const  = 0;
	// Game wywołuje goBack(), gdy wykryje kolizję ze ścianą
	virtual void goBack() {
		this->setPosition(prevPos); 
	}
	virtual void setPosition(sf::Vector2f pos) {
		this->position = pos; 
	}
	sf::Vector2f getprevPos() const {return prevPos;}
	sf::Vector2f getPosition() const { return position; }
	virtual ~Character() = default;
	
};
