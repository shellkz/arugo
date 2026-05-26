#include "FlashMessage.h"
#include "resources/ResourceManager.h"

FlashMessage::FlashMessage(sf::Vector2f screenSize)
    : m_text(ResourceManager::get().getFont("default")), m_screenSize(screenSize)
{
    m_text.setCharacterSize(48);
    m_text.setOutlineThickness(3.f);
}

void FlashMessage::show(const std::string &msg)
{
    m_text.setString(sf::String::fromUtf8(msg.begin(), msg.end()));

    // 置中
    sf::FloatRect bounds = m_text.getLocalBounds();
    m_text.setOrigin({bounds.position.x + bounds.size.x / 2.f,
                      bounds.position.y + bounds.size.y / 2.f});
    m_text.setPosition({m_screenSize.x / 2.f, m_screenSize.y / 2.f});

    // 重播
    m_alpha = 0.f;
    m_holdTimer = 0.f;
    m_phase = Phase::FadeIn;
}

void FlashMessage::update(float dt)
{
    switch (m_phase)
    {
    case Phase::FadeIn:
        m_alpha += dt * 255.f / FADE_DURATION;
        if (m_alpha >= 255.f)
        {
            m_alpha = 255.f;
            m_phase = Phase::Hold;
        }
        break;

    case Phase::Hold:
        m_holdTimer += dt;
        if (m_holdTimer >= HOLD_DURATION)
            m_phase = Phase::FadeOut;
        break;

    case Phase::FadeOut:
        m_alpha -= dt * 255.f / FADE_DURATION;
        if (m_alpha <= 0.f)
        {
            m_alpha = 0.f;
            m_phase = Phase::Idle;
        }
        break;

    case Phase::Idle:
        break;
    }

    auto a = static_cast<uint8_t>(m_alpha);

    m_text.setFillColor(sf::Color(255, 233, 160, a));
    m_text.setOutlineColor(sf::Color(26, 16, 0, a));
}

void FlashMessage::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (m_phase == Phase::Idle)
        return;
    target.draw(m_text, states);
}