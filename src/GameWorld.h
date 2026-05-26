#pragma once

#include "utils/Collection.h"
#include "Entity.h"
#include "InputHandler.h"
#include "TileMap.h"
#include <SFML/Graphics.hpp>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include "battle/ActionQueue.h"
#include "resources/items/ItemRegistry.h"
#include <iostream>
class BattleCommand;

enum class TurnState
{
    DecideActor,
    DecideCommand,
    ExecuteCommand,
    PresentCommand,
};

enum class ActionResult
{
    Normal,
    EnemyKilled,
    GotoNextLevel,
};
class GameWorld : public sf::Drawable
{
public:
    GameWorld();
    ~GameWorld();

    void onPlayerDied(std::function<void()> callback) { m_onPlayerDied = callback; }
    TileMap &getMap() { return map; }

    const Entity &getPlayer() const { return player; }
    const std::vector<Entity *> &getEntities() const { return entities; }
    const TurnState getCurrentBattlePhase() const { return m_currentState; }
    const Entity &getCurrentActor() const { return *m_current_actor; }
    const std::string &getFlashMessage() const { return m_flashMessage; }
    bool hasFlashMessage() const { return !m_flashMessage.empty(); }

    Entity *getEntityAt(int x, int y) const;
    bool canMoveTo(int x, int y) const;
    bool canMoveTo(const Entity &who, sf::Vector2i desiredPosition) const;
    std::vector<Entity *> getAttackableTarget(const Entity &attacker);
    void clearFlashMessage() { m_flashMessage = ""; }
    void setFlashMessage(std::string value) { m_flashMessage = value; }
    void processAction(Action action);
    void update(float deltaTime);
    bool spawnEnemy(sf::Vector2i position);
    int spawnEnemies(const std::vector<sf::Vector2i> &positions);
    void destroyAllEnemies();
    void spawnItem(sf::Vector2i position);
    void gotoNextLevel();
    void gotoStartLevel();

    virtual void input(const sf::Event &event);

private:
    std::function<void()> m_onPlayerDied;

    TileMap map;
    ItemRegistry m_itemRegistry;
    ActionQueue queue;
    Entity player;
    std::vector<Entity *> enemies;
    std::vector<Entity *> entities;

    TurnState m_currentState = TurnState::DecideActor;
    Action m_queuedAction = Action::None;
    sf::Clock m_stateClock;
    const float m_presentDuration = 0.2f;
    std::unordered_map<Action, std::function<std::unique_ptr<BattleCommand>()>> m_commandFactories;
    std::unique_ptr<BattleCommand> m_command;
    TurnState m_afterPlayerPresent;
    bool m_first_entered = true;
    Entity *m_current_actor;
    std::string m_flashMessage;

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    sf::Vector2i toDirection(Action action) const;
    void updateEntitiesList();
    void clearDeadEnemies();
    ActionResult playerTakeAction(Action action);
    void enemyTakeAction();
    bool checkPlayerDied();

    void onCommandDecided(Entity &who, std::unique_ptr<BattleCommand> command);
};
