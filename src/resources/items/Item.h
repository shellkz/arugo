#pragma once
#include <SFML/Graphics.hpp>

class Item
{
private:
    float m_dealthAttack;
    float m_deltaHealth;
    const sf::Texture *m_texture;

public:
    Item();
    Item(float dealthAttack, float deltaHealth, const sf::Texture *texture);

    float getDealthAttack() const;
    float getDeltaHealth() const;
    const sf::Texture *getTexture() const;

    void setDealthAttack(float attack);
    void setDeltaHealth(float health);
    void setTexture(const sf::Texture *texture);
};