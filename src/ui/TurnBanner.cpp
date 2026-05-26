#include "ui/TurnBanner.h"
#include "resources/ResourceManager.h"

TurnBanner::TurnBanner()
    : m_text(ResourceManager::get().getFont("default"))
{
    m_text.setCharacterSize(48);
}

void TurnBanner::setState(TurnState state, std::string_view actorName)
{
    bool isPlayerTurn = (actorName == "player" && state == TurnState::DecideCommand);

    if (isPlayerTurn)
    {
        // 如果當前狀態已經是 DecideCommand 且紀錄是 player，代表是在「同一個回合內思考」的影格延續
        if (m_state == TurnState::DecideCommand && m_currentActor == "player")
        {
            return; // 真正的同回合延續，直接 return
        }

        // 能走到這裡，代表是「全新的一回合」輪到玩家了（從敵人切回來，或是從 Execute 切回來的全新回合）
        m_state = state;
        m_currentActor = actorName;

        // 重新觸發 FadeIn 動畫
        m_alpha = 0.f;
        m_phase = Phase::FadeIn;
        m_holdTimer = 0.f;

        m_text.setString("Player's Turn");

        // SFML 3.0 置中
        sf::FloatRect textRect = m_text.getLocalBounds();
        m_text.setOrigin({textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f});
        m_text.setPosition({400.f, 300.f});
    }
    else
    {
        // 當玩家按下移動（切換到 ExecuteCommand）或是輪到敵人動
        m_state = state;
        m_currentActor = actorName; // 正確更新為當前的 actor（例如 "enemy" 或空），這樣下次重回玩家回合才能成功觸發！

        // 【關鍵】這裡我們「不」去改動 m_phase。
        // 讓大字報維持原本的狀態（不論是 Hold 還是 FadeOut），讓它在 update 裡面自己優雅地走完淡出。
    }
}

void TurnBanner::update(float deltaTime)
{
    switch (m_phase)
    {
    case Phase::FadeIn:
        m_alpha += deltaTime * 255.f / FADE_DURATION;
        if (m_alpha >= 255.f)
        {
            m_alpha = 255.f;
            m_phase = Phase::Hold;
        }
        break;

    case Phase::Hold:
        m_holdTimer += deltaTime;
        if (m_holdTimer >= HOLD_DURATION)
        {
            m_phase = Phase::FadeOut;
        }
        break;

    case Phase::FadeOut:
        m_alpha -= deltaTime * 255.f / FADE_DURATION;
        if (m_alpha <= 0.f)
        {
            m_alpha = 0.f;
            m_phase = Phase::Hidden;
        }
        break;

    case Phase::Hidden:
        break;
    }

    sf::Color textColor = sf::Color::White;
    textColor.a = static_cast<uint8_t>(m_alpha);
    m_text.setFillColor(textColor);
}

void TurnBanner::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (m_phase == Phase::Hidden)
        return;

    target.draw(m_text, states);
}