#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Button {
private:
    sf::RectangleShape shape;
    sf::Text text;
public:
    Button(std::string label, sf::Vector2f pos, const sf::Font& sharedFont);

    void draw(sf::RenderWindow& window);
    bool isClicked(sf::Vector2i mousePos);
    void setLabel(std::string s) { this->text.setString(s); }
    void setText(const std::string& newText);
};