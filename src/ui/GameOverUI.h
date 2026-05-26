#ifndef GAMEOVERUI_H
#define GAMEOVERUI_H

#include <SFML/Graphics.hpp>

class GameOverUI : public sf::Drawable
{
public:
    GameOverUI(sf::Vector2f windowSize);

    bool handleEvent(const sf::Event &event, const sf::RenderWindow &window);

    bool isRestartClicked() const;

    void reset();

private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    sf::RectangleShape m_background;
    sf::Text m_titleText;
    sf::RectangleShape m_retryButton;
    sf::Text m_retryText;

    bool m_restartClicked;
};

#endif