#include "GameOverUI.h"
#include "resources/ResourceManager.h"

GameOverUI::GameOverUI(sf::Vector2f windowSize)
    : m_restartClicked(false),
      m_retryText(ResourceManager::get().getFont("default")),
      m_titleText(ResourceManager::get().getFont("default"))
{
    // 1. 背景容器 (半透明黑底)
    m_background.setSize(windowSize);
    m_background.setFillColor(sf::Color(0, 0, 0, 150));

    // 2. Game Over 標題文字
    m_titleText.setString("GAME OVER");
    m_titleText.setCharacterSize(50);
    m_titleText.setFillColor(sf::Color::Red);
    m_titleText.setStyle(sf::Text::Bold);

    // 計算標題置中
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();

    m_titleText.setOrigin(
        {titleBounds.position.x + titleBounds.size.x / 2.0f,
         titleBounds.position.y + titleBounds.size.y / 2.0f});
    m_titleText.setPosition({windowSize.x / 2.0f, windowSize.y / 2.0f - 100.0f});

    // 3. Retry 按鈕外框
    m_retryButton.setSize(sf::Vector2f(200.0f, 60.0f));
    m_retryButton.setFillColor(sf::Color(100, 100, 100));
    m_retryButton.setOutlineThickness(2.0f);
    m_retryButton.setOutlineColor(sf::Color::White);

    // 計算按鈕置中
    sf::FloatRect btnBounds = m_retryButton.getLocalBounds();
    m_retryButton.setOrigin({btnBounds.size.x / 2.0f, btnBounds.size.y / 2.0f});
    m_retryButton.setPosition({windowSize.x / 2.0f, windowSize.y / 2.0f + 50.0f});

    // 4. Retry 按鈕文字
    m_retryText.setString("RETRY");
    m_retryText.setCharacterSize(30);
    m_retryText.setFillColor(sf::Color::White);

    // 將按鈕文字置中到按鈕的正中央
    sf::FloatRect retryTxtBounds = m_retryText.getLocalBounds();
    m_retryText.setOrigin(
        {retryTxtBounds.position.x + retryTxtBounds.size.x / 2.0f,
         retryTxtBounds.position.y + retryTxtBounds.size.y / 2.0f});

    m_retryText.setPosition(m_retryButton.getPosition());
}

bool GameOverUI::handleEvent(const sf::Event &event, const sf::RenderWindow &window)
{
    //  if (const auto *key = event->getIf<sf::Event::MouseButtonPressed>())
    //     {

    //         if (key->code == sf::Keyboard::Key::Escape)
    if (const auto *mouseBtn = event.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mouseBtn->button == sf::Mouse::Button::Left)
        {
            // 將視窗的滑鼠像素座標轉換為世界的座標空間，防止視窗縮放導致判定錯位
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(mouseBtn->position.x, mouseBtn->position.y));

            // 檢查滑鼠點擊點是否在按鈕的 GlobalBounds 矩形內
            if (m_retryButton.getGlobalBounds().contains(mousePos))
            {
                m_restartClicked = true;
                return true; // 成功攔截事件
            }
        }

        return false;
    }
    return false;
}
bool GameOverUI::isRestartClicked() const
{
    return m_restartClicked;
}

void GameOverUI::reset()
{
    m_restartClicked = false;
}

void GameOverUI::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    // 利用 target.draw 依序繪製圖層 (後畫的會蓋在先畫的上面)
    target.draw(m_background, states);
    target.draw(m_titleText, states);
    target.draw(m_retryButton, states);
    target.draw(m_retryText, states);
}
