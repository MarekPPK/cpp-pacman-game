#include "Button.h"
#include<iostream>
import GameConstants;

// Implementacja konstruktora
Button::Button(std::string label, sf::Vector2f pos, const sf::Font& sharedFont)
    : text(sharedFont, label) // Inicjalizacja tekstu czcionką i etykietą
{
    shape.setSize({ Config::UI::ButtonWidth ,Config::UI::ButtonHeight });
    shape.setPosition(pos);
    shape.setFillColor(sf::Color::Blue);
    text.setCharacterSize(20);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin({
    textRect.position.x + textRect.size.x / 2.0f,
    textRect.position.y + textRect.size.y / 2.0f
        });
    text.setPosition({
    pos.x + Config::UI::ButtonWidth/ 2.0f,
    pos.y + Config::UI::ButtonHeight / 2.0f
        });
}

void Button::setText(const std::string& newText) {
    text.setString(newText);
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin({
        textRect.position.x + textRect.size.x / 2.0f,
        textRect.position.y + textRect.size.y / 2.0f
        });

    
}

void Button::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

bool Button::isClicked(sf::Vector2i mousePos) {
    // Rzutowanie mousePos na Vector2f, aby pasował do getGlobalBounds
    return shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

