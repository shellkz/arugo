#include "Item.h"

Item::Item()
    : m_dealthAttack(0.0f), m_deltaHealth(0.0f), m_texture(nullptr)
{
}

Item::Item(float dealthAttack, float deltaHealth, const sf::Texture *texture)
    : m_dealthAttack(dealthAttack), m_deltaHealth(deltaHealth), m_texture(texture)
{
}

float Item::getDealthAttack() const { return m_dealthAttack; }
float Item::getDeltaHealth() const { return m_deltaHealth; }
const sf::Texture *Item::getTexture() const { return m_texture; }

void Item::setDealthAttack(float attack) { m_dealthAttack = attack; }
void Item::setDeltaHealth(float health) { m_deltaHealth = health; }
void Item::setTexture(const sf::Texture *texture) { m_texture = texture; }