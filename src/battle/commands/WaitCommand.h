#pragma once

#include "BattleCommand.h"

class WaitCommand : public BattleCommand
{
public:
    ActionResult execute(Entity &entity, GameWorld &gameWorld) override;
};
