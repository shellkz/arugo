#include "InputHandler.h"

Action InputHandler::translate(const sf::Event &event) const
{
    if (event.is<sf::Event::Closed>())
        return Action::Quit;

    if (const auto *key = event.getIf<sf::Event::KeyPressed>())
    {
        switch (key->code)
        {
        case sf::Keyboard::Key::W:
            return Action::MoveUp;
        case sf::Keyboard::Key::S:
            return Action::MoveDown;
        case sf::Keyboard::Key::A:
            return Action::MoveLeft;
        case sf::Keyboard::Key::D:
            return Action::MoveRight;
        case sf::Keyboard::Key::Z:
            return Action::Wait;
        default:
            return Action::None;
        }
    }
    return Action::None;
}
