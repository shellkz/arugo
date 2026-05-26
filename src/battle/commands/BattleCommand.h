#pragma once

enum class ActionResult;
class Entity;
class GameWorld;

class BattleCommand
{
public:
    virtual ~BattleCommand();

    virtual ActionResult execute(Entity &entity, GameWorld &gameWorld) = 0;
};
