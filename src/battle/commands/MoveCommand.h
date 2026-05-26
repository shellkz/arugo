#pragma once

#include "BattleCommand.h"

#include <SFML/System/Vector2.hpp>

class MoveCommand : public BattleCommand
{
public:
    explicit MoveCommand(sf::Vector2i direction);

    ActionResult execute(Entity &entity, GameWorld &gameWorld) override;

private:
    sf::Vector2i m_direction;
};
