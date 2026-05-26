#ifndef TURN_UI_H
#define TURN_UI_H

#include <SFML/Graphics.hpp>
#include <string>
#include <string_view>

#include "GameWorld.h"

class TurnBanner : public sf::Drawable
{
public:
    TurnBanner();
    void setState(TurnState state, std::string_view actorName);
    void update(float deltaTime);

private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
    std::string getBannerText(TurnState state, std::string_view actor) const;

    enum class Phase
    {
        FadeIn,
        Hold,
        FadeOut,
        Hidden
    };

    TurnState m_state = TurnState::DecideActor;
    std::string m_currentActor = "";
    Phase m_phase = Phase::Hidden;

    float m_alpha = 0.f;
    float m_holdTimer = 0.f;

    sf::Text m_text;

    static constexpr float FADE_DURATION = 0.05f;
    static constexpr float HOLD_DURATION = 0.3f;
};

#endif