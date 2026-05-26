#include "GameWorld.h"
#include "resources/ResourceManager.h"
#include "battle/commands/MoveCommand.h"
#include "battle/commands/WaitCommand.h"
#include "utils/Random.h"

GameWorld::GameWorld()
    : player(3, 3, "player", 10), queue(), m_itemRegistry()
{
    m_commandFactories[Action::Wait] = []()
    {
        return std::make_unique<WaitCommand>();
    };
    m_commandFactories[Action::MoveUp] = []()
    {
        return std::make_unique<MoveCommand>(sf::Vector2i{0, -1});
    };
    m_commandFactories[Action::MoveDown] = []()
    {
        return std::make_unique<MoveCommand>(sf::Vector2i{0, 1});
    };
    m_commandFactories[Action::MoveLeft] = []()
    {
        return std::make_unique<MoveCommand>(sf::Vector2i{-1, 0});
    };
    m_commandFactories[Action::MoveRight] = []()
    {
        return std::make_unique<MoveCommand>(sf::Vector2i{1, 0});
    };

    // Load Items
    m_itemRegistry.add(Item(0.0f, 1.0f, &ResourceManager::get().getTexture("items/potion")));
    m_itemRegistry.add(Item(1.0f, 0.0f, &ResourceManager::get().getTexture("items/attack_buff")));

    gotoStartLevel();
}

void GameWorld::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(map, states);
    for (const auto *e : entities)
        target.draw(*e, states);
}

GameWorld::~GameWorld()
{
    for (Entity *enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
}

void GameWorld::input(const sf::Event &event)
{
    player.input(event);
    for (auto enemy : enemies)
        enemy->input(event);
}

// Called every frame
void GameWorld::update(float deltaTime)
{
    // Update children
    player.update(deltaTime);
    for (auto enemy : enemies)
        enemy->update(deltaTime);

    // Behavior of BattleManager in diffrent phase when updated
    switch (m_currentState)
    {
    case TurnState::DecideActor:
    {
        if (m_first_entered)
        {
            m_current_actor = queue.deq();
            printf("%s's turn.\r\n", m_current_actor->getName().c_str());
            queue.enq(m_current_actor);
            // m_first_entered = false;

            m_currentState = TurnState::DecideCommand;
            // m_first_entered = true;
        }

        break;
    }
    case TurnState::DecideCommand:
    {
        if (m_first_entered)
        {
            m_first_entered = false;
            // printf("%s is deciding command.\r\n", m_current_actor->getName().c_str());
            m_current_actor->setActing(true);
            m_current_actor->decideCommand(*this,
                                           [this](Entity &who, std::unique_ptr<BattleCommand> command)
                                           {
                                               this->onCommandDecided(who, std::move(command));
                                           });
        }
        break;
    }
    case TurnState::ExecuteCommand:
    {
        // stuck at here

        if (m_first_entered)
        {
            // printf("%s is executing command.\r\n", m_current_actor->getName().c_str());
            m_command->execute(*m_current_actor, *this);

            // m_first_entered = false;
            m_currentState = TurnState::PresentCommand;
            m_stateClock.restart();
        }
        break;
    }
    case TurnState::PresentCommand:
    {
        if (m_first_entered)
        {
            // printf("%s is presenting command.\r\n", m_current_actor->getName().c_str());
            m_first_entered = false;
        }

        if (m_stateClock.getElapsedTime().asSeconds() >= m_presentDuration)
        {
            m_current_actor->setActing(false);

            // Restart if player dead
            if (checkPlayerDied())
            {
                m_onPlayerDied();
                break;
            }
            // Pickup item if step into
            sf::Vector2i newPosition = player.getPosition();
            Tile &stepped = map.get(newPosition.x, newPosition.y);
            if (stepped.hasItem())
            {

                const Item &item = stepped.getItem();
                std::cout << "player hp before " << player.getHealth() << std::endl;
                m_current_actor->useItem(item);
                std::cout << "player hp after " << player.getHealth() << std::endl;
                stepped.removeItem();
            }

            // Goto next level if step into portal
            if (stepped.type == Tile::Type::Portal)
            {
                gotoNextLevel();
            }

            //  Player act again if kill any enemy
            bool enemyKilled = false;
            for (auto enemy : enemies)
            {
                if (enemy->isDead())
                {
                    enemyKilled = true;
                    break;
                }
            }
            if (enemyKilled)
            {
                setFlashMessage("擊殺獎勵\r\n額外行動");
                queue.erase(&player);
                queue.enqFront(&player);
            }

            clearDeadEnemies();
            updateEntitiesList();
            queue.eraseDead();

            map.clearAttacked();

            m_first_entered = true;
            m_currentState = TurnState::DecideActor;
        }
        break;
    }
    }
}
void GameWorld::onCommandDecided(Entity &who, std::unique_ptr<BattleCommand> command)
{

    m_command = std::move(command);
    m_currentState = TurnState::ExecuteCommand;
    m_first_entered = true;
    // printf("%s had decided command.\r\n", m_current_actor->getName().c_str());
}
sf::Vector2i GameWorld::toDirection(Action action) const
{
    switch (action)
    {
    case Action::MoveUp:
        return {0, -1};
    case Action::MoveDown:
        return {0, 1};
    case Action::MoveLeft:
        return {-1, 0};
    case Action::MoveRight:
        return {1, 0};
    default:
        return {0, 0};
    }
}
Entity *GameWorld::getEntityAt(int x, int y) const
{
    for (Entity *entity : entities)
    {
        if (entity != nullptr)
        {
            sf::Vector2i pos = entity->getPosition();
            if (pos.x == x && pos.y == y)
            {
                return entity;
            }
        }
    }
    return nullptr;
}

bool GameWorld::canMoveTo(const Entity &who, sf::Vector2i desiredPosition) const
{
    if (!map.isWalkable(desiredPosition.x, desiredPosition.y))
    {
        return false;
    }

    Entity *entityAtDesiredPosition = getEntityAt(desiredPosition.x, desiredPosition.y);
    if (entityAtDesiredPosition != nullptr && entityAtDesiredPosition != &who)
    {
        return false;
    }

    return true;
}

/* I should make Entity describe their attack shape with EffectArea
EffectArea
    area() -> list[vector2i]

Circle:EffectArea
    Type(Dimond(Manhetan), Block(chebyshev), Cricle(Oculedes))
    Range(raidus)
Line:EffectArea
Cross:EffectArea

Entity
    EffectArea attackArea

GameWorld
    getAttackableTargets(const Entity &attacker)
        return findEntityInside(attackArea.area())
*/
std::vector<Entity *> GameWorld::getAttackableTarget(const Entity &attacker)
{
    std::vector<Entity *> targets;
    // Player
    if (&attacker == &player)
    {
        for (Entity *enemy : enemies)
        {
            if (Math::chebyshevDistance(attacker.getPosition(), enemy->getPosition()) <= attacker.getAttackRange())
            {
                targets.push_back(enemy);
            }
        }
        return targets;
    }
    // Enemy
    if (Math::manhattanDistance(attacker.getPosition(), player.getPosition()) <= attacker.getAttackRange())
    {
        targets.push_back(&player);
    }

    return targets;
}
void GameWorld::updateEntitiesList()
{
    entities.clear();
    entities.push_back(&player);
    entities.insert(entities.end(), enemies.begin(), enemies.end());
}
bool GameWorld::spawnEnemy(sf::Vector2i position)
{
    return spawnEnemies({position}) == 1;
}
//  Spawn enemy at given position list
//  If position is:
//      1. Walkable
//      2. Not occupied by other entity
//      3. Not overlapping between given positions
//  Skip spawning if not.
int GameWorld::spawnEnemies(const std::vector<sf::Vector2i> &positions)
{
    std::vector<sf::Vector2i> spawnedPositions;
    spawnedPositions.reserve(positions.size());

    for (const sf::Vector2i &position : positions)
    {
        if (!map.isWalkable(position.x, position.y))
        {
            continue;
        }

        if (getEntityAt(position.x, position.y) != nullptr)
        {
            continue;
        }

        bool alreadySpawnedHere = std::any_of(
            spawnedPositions.begin(), spawnedPositions.end(),
            [&position](const sf::Vector2i &spawnedPosition)
            {
                return spawnedPosition == position;
            });
        if (alreadySpawnedHere)
        {
            continue;
        }
        Entity *enemy = new Entity(position.x, position.y, "enemy", 2);
        enemy->setAttack(1);
        enemies.push_back(enemy);
        spawnedPositions.push_back(position);
    }

    return static_cast<int>(spawnedPositions.size());
}
void GameWorld::clearDeadEnemies()
{
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
                       [](Entity *enemy)
                       {
                           if (enemy->isDead())
                           {
                               delete enemy;
                               return true;
                           }
                           return false;
                       }),
        enemies.end());
}
void GameWorld::destroyAllEnemies()
{
    for (Entity *enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
}
void GameWorld::gotoNextLevel()
{
    destroyAllEnemies();
    map.regenerate();

    std::vector<sf::Vector2i> emptyTiles = map.getEmptyTilePositions();
    map.setPortalAt(popRandom(emptyTiles));
    player.teleport(popRandom(emptyTiles));

    std::vector<sf::Vector2i> enemySpawnPositions;
    for (size_t i = 0; i < 2; i++)
    {
        enemySpawnPositions.push_back(popRandom(emptyTiles));
    }
    spawnEnemies(enemySpawnPositions);

    std::vector<sf::Vector2i> itemPositions;
    for (size_t i = 0; i < 2; i++)
    {
        spawnItem(popRandom(emptyTiles));
    }

    updateEntitiesList();
    // rebuild Action Queue
    queue.eraseAll();
    queue.enqAll(entities);
    queue.print();
}
void GameWorld::gotoStartLevel()
{
    destroyAllEnemies();

    map.regenerate();
    map.clearAttacked();

    std::vector<sf::Vector2i> emptyTiles = map.getEmptyTilePositions();
    map.setPortalAt(popRandom(emptyTiles));
    player.teleport(popRandom(emptyTiles));
    player.setHealth(player.getMaxHealth());
    player.setAttack(2);

    std::vector<sf::Vector2i> enemySpawnPositions;
    for (size_t i = 0; i < 2; i++)
    {
        enemySpawnPositions.push_back(popRandom(emptyTiles));
    }
    spawnEnemies(enemySpawnPositions);

    spawnItem(popRandom(emptyTiles));

    m_currentState = TurnState::DecideActor;
    m_first_entered = true;
    m_queuedAction = Action::None;
    m_stateClock.restart();

    updateEntitiesList();

    // rebuild Action Queue
    queue.eraseAll();
    queue.enqAll(entities);
    queue.print();
}
bool GameWorld::checkPlayerDied()
{
    return player.isDead();
}
void GameWorld::spawnItem(sf::Vector2i position)
{

    int index = Random::randomInt(0, m_itemRegistry.size() - 1);
    Item newItem = m_itemRegistry.get(index);

    // TODO: Modify newItem randomly, for more variation
    // newItem.setLevel(Random::randomInt(1, 5));
    // newItem.setDurability(Random::randomFloat(0.5f, 1.0f));

    map.get(position.x, position.y).setItem(std::move(newItem));
}