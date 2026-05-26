#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class FlashMessage : public sf::Drawable
{
public:
    FlashMessage(sf::Vector2f screenSize);

    void show(const std::string &msg);
    void update(float dt);

private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    enum class Phase
    {
        Idle,
        FadeIn,
        Hold,
        FadeOut
    };

    sf::Text m_text;
    sf::Vector2f m_screenSize;
    Phase m_phase = Phase::Idle;
    float m_alpha = 0.f;
    float m_holdTimer = 0.f;

    static constexpr float FADE_DURATION = 0.05f;
    static constexpr float HOLD_DURATION = 0.5f;
};