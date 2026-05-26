#pragma once

#include <SFML/Window/Event.hpp>

enum class Action
{
    None,
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Wait,
    Quit
};

class InputHandler
{
public:
    Action translate(const sf::Event &event) const;
};
