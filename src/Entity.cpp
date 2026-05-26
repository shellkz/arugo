#include "Entity.h"
#include "GameWorld.h"
#include "resources/ResourceManager.h"
#include "TileMap.h"
#include "battle/commands/MoveCommand.h"
#include "battle/commands/WaitCommand.h"

Entity::Entity(int x, int y, std::string name, float maxHealth)
    : m_x(x), m_y(y), m_attackRange(1), m_health(maxHealth), m_maxHealth(maxHealth), m_name(name), m_addColor(sf::Color(0, 0, 0, 0))
{
}

void Entity::move(sf::Vector2i dir)
{
    m_x += dir.x;
    m_y += dir.y;
}

sf::Vector2i Entity::getPosition() const
{
    return sf::Vector2i{m_x, m_y};
}

sf::Vector2i Entity::getNextDirection(const GameWorld &world) const
{
    sf::Vector2i playerPos = world.getPlayer().getPosition();
    sf::Vector2i myPos = getPosition();

    sf::Vector2i dir(0, 0);

    // 用 else if 把 X 和 Y 綁在一起，確保同時只有一個軸向會觸發
    if (myPos.x < playerPos.x)
    {
        dir.x = 1;
    }
    else if (myPos.x > playerPos.x)
    {
        dir.x = -1;
    }
    else if (myPos.y < playerPos.y)
    { // 只有當 X 軸完全相等時，才會走到這裡
        dir.y = 1;
    }
    else if (myPos.y > playerPos.y)
    {
        dir.y = -1;
    }

    return dir;
}
bool Entity::canAttack(Entity *attacked)
{

    return Math::chebyshevDistance(this->getPosition(), attacked->getPosition()) <= this->getAttackRange();
}
void Entity::attack(Entity *attacked)
{
    attacked->getDamaged(this);
}

void Entity::getDamaged(Entity *attacker)
{
    deltaHealth(-attacker->getAttack());
    m_blinkingCounter = 0.0;
}

void Entity::deltaHealth(float delta)
{
    m_health += delta;
    m_health = std::clamp(m_health, 0.0f, m_maxHealth);
}

void Entity::setHealth(float value)
{
    m_health = value;
    m_health = std::clamp(m_health, 0.0f, m_maxHealth);
}

void Entity::update(float deltaTime)
{
    if (m_blinkingCounter < m_blinkingDuration)
    {
        m_blinkingCounter += deltaTime;
        float t = m_blinkingCounter * m_blinkingFrequency;
        float value = (std::sin(t) + 1.0f) / 2.0f;
        m_addColor = m_blinkingColor;
        m_addColor.a = static_cast<uint8_t>(255 * value);
    }
    else
    {
        m_addColor = sf::Color(255, 255, 255, 255);
    }
}

void Entity::teleport(sf::Vector2i position)
{
    m_x = position.x;
    m_y = position.y;
}

void Entity::setAttack(float value)
{
    m_attack = value;
    m_attack = std::clamp(m_attack, 0.0f, 255.0f);
}

void Entity::decideCommand(
    const GameWorld &context,
    std::function<void(Entity &, std::unique_ptr<BattleCommand>)> onCommandDecided)
{
    if (getName() == "player")
    {
        printf("%s figuring out command.\r\n", getName().c_str());
        m_onCommandDecided = onCommandDecided;
    }
    else
    {
        printf("%s figuring out command.\r\n", getName().c_str());
        auto command = std::make_unique<MoveCommand>(getNextDirection(context));
        onCommandDecided(*this, std::move(command));
    }
}

void Entity::input(const sf::Event &event)
{
    if (getName() == "player")
    {
        if (!m_onCommandDecided)
            return;

        // event to action
        Action action = Action::None;

        if (const auto *key = event.getIf<sf::Event::KeyPressed>())
        {
            switch (key->code)
            {
            case sf::Keyboard::Key::W:
                printf("TEST");
                action = Action::MoveUp;
                break;
            case sf::Keyboard::Key::S:
                action = Action::MoveDown;
                break;
            case sf::Keyboard::Key::A:
                action = Action::MoveLeft;
                break;
            case sf::Keyboard::Key::D:
                action = Action::MoveRight;
                break;
            case sf::Keyboard::Key::Z:
                action = Action::Wait;
                break;
            default:
                action = Action::None;
                break;
            }
        }

        // Action to command
        std::unique_ptr<BattleCommand> command = nullptr;
        sf::Vector2i direction{0, 0};
        switch (action)
        {
        case Action::MoveUp:
            command = std::make_unique<MoveCommand>(sf::Vector2i(0, -1));
            break;
        case Action::MoveDown:
            command = std::make_unique<MoveCommand>(sf::Vector2i(0, 1));
            break;
        case Action::MoveLeft:
            command = std::make_unique<MoveCommand>(sf::Vector2i(-1, 0));
            break;
        case Action::MoveRight:
            command = std::make_unique<MoveCommand>(sf::Vector2i(1, 0));
            break;

        case Action::Wait:
            command = std::make_unique<WaitCommand>();
            break;
        default:
            break;
        }

        // Report inputed command
        if (command != nullptr)
        {
            m_onCommandDecided(*this, std::move(command));
            m_onCommandDecided = nullptr;
        }
    }
    else
    {
        // 敵人 AI 不需要理會玩家的鍵盤事件，直接留空
    }
}

void Entity::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    bool isPlayer = (m_name == "player");

    sf::RectangleShape sprite({(float)TILE_SIZE, (float)TILE_SIZE});
    sprite.setTexture(&ResourceManager::get().getTexture(isPlayer ? "hero" : "goblin"), true);
    sprite.setFillColor(m_addColor);
    sprite.setPosition({(float)(m_x * TILE_SIZE), (float)(m_y * TILE_SIZE)});
    target.draw(sprite, states);

    sf::Text hpText(ResourceManager::get().getFont("default"));
    hpText.setString(std::to_string(static_cast<int>(m_health)));
    hpText.setCharacterSize(24);

    sf::Color fillColor    = isPlayer ? sf::Color(40, 120, 230) : sf::Color(230, 40, 40);
    sf::Color outlineColor = isPlayer ? sf::Color(8, 10, 18)    : sf::Color(18, 8, 8);
    if (!m_isActing)
    {
        fillColor    = darken(fillColor);
        outlineColor = darken(outlineColor);
    }
    hpText.setFillColor(fillColor);
    hpText.setOutlineColor(outlineColor);
    hpText.setOutlineThickness(3.f);

    sf::FloatRect bounds = hpText.getLocalBounds();
    hpText.setOrigin({bounds.position.x + bounds.size.x / 2.f,
                      bounds.position.y + bounds.size.y / 2.f});
    hpText.setStyle(sf::Text::Style::Bold);
    hpText.setPosition({m_x * TILE_SIZE + TILE_SIZE / 2.f, m_y * TILE_SIZE - 14.f});

    if (m_isActing)
    {
        sf::Text bgText = hpText;
        bgText.setFillColor(sf::Color::Transparent);
        bgText.setOutlineColor(isPlayer ? sf::Color(207, 220, 157) : sf::Color(255, 130, 100));
        bgText.setOutlineThickness(5.0f);
        bgText.setStyle(sf::Text::Style::Bold);
        target.draw(bgText, states);
    }
    target.draw(hpText, states);
}

sf::Color Entity::darken(sf::Color base)
{
    return sf::Color(
        base.r * 165 / 255,
        base.g * 165 / 255,
        base.b * 165 / 255,
        base.a);
}

void Entity::useItem(const Item &item)
{
    deltaHealth(item.getDeltaHealth());
    m_attack += item.getDealthAttack();
}