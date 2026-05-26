#ifndef ENTITY_H
#define ENTITY_H
#include <SFML/Graphics.hpp>
#include <string>
#include "utils/Math.h"
#include <memory>
#include <functional>
#include "resources/items/Item.h"
class GameWorld;
class BattleCommand;
class Entity : public sf::Drawable
{
public:
    Entity(int x, int y, std::string name, float maxHealth);
    virtual ~Entity() = default;

    int getX() const { return m_x; }
    int getY() const { return m_y; }
    float getHealth() const { return m_health; }
    sf::Vector2i getPosition() const;
    float getMaxHealth() const { return m_maxHealth; }
    int getAttackRange() const { return m_attackRange; }
    float getAttack() const { return m_attack; }
    bool isActing() const { return m_isActing; }
    bool isDead() const { return int(getHealth()) <= 0; }
    std::string getName() const { return m_name; }
    sf::Color getAddColor() const { return m_addColor; }

    void move(sf::Vector2i dir);
    sf::Vector2i getNextDirection(const GameWorld &world) const;
    void deltaHealth(float delta);
    void setActing(bool value) { m_isActing = value; }
    void setHealth(float value);
    void setAttack(float value);
    void getDamaged(Entity *attacker);
    void attack(Entity *attacked);
    bool canAttack(Entity *attacked);
    void update(float deltaTime);
    void teleport(sf::Vector2i position);
    void useItem(const Item &item);

    virtual void input(const sf::Event &event);
    virtual void decideCommand(
        const GameWorld &context,
        std::function<void(Entity &, std::unique_ptr<BattleCommand>)> onCommandDecided);

private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    static sf::Color darken(sf::Color base);

    int m_x;
    int m_y;
    float m_health;
    float m_maxHealth;
    float m_attack;
    int m_attackRange;
    std::string m_name;
    bool m_isActing = false;

    float m_blinkingDuration = 0.1f;
    float m_blinkingCounter = 0.1f;
    float m_blinkingFrequency = 40.0f;
    sf::Color m_blinkingColor = sf::Color::Black;
    sf::Color m_addColor;

    std::function<void(Entity &, std::unique_ptr<BattleCommand>)> m_onCommandDecided;
};
#endif
