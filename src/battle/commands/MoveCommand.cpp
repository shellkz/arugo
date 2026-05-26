#include "MoveCommand.h"

#include "GameWorld.h"

MoveCommand::MoveCommand(sf::Vector2i direction)
    : m_direction(direction)
{
}

ActionResult MoveCommand::execute(Entity &entity, GameWorld &gameWorld)
{
    sf::Vector2i desired = entity.getPosition() + m_direction;
    if (gameWorld.canMoveTo(entity, desired))
    {
        entity.move(m_direction);
    }

    for (auto target : gameWorld.getAttackableTarget(entity))
    {
        entity.attack(target);
        gameWorld.getMap().setAttacked(gameWorld.getMap().getChebyshevArea(entity.getPosition(), entity.getAttackRange()));
    }

    return ActionResult::Normal;
}
