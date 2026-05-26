#include <SFML/Graphics.hpp>

#include "GameWorld.h"
#include "InputHandler.h"
#include "resources/ResourceManager.h"
#include "ui/GameOverUI.h"
#include "ui/TurnBanner.h"
#include "ui/FlashMessage.h"
enum class GameState
{
    Playing,
    GameOver
};
void adaptScreenSize(sf::RenderWindow &window, sf::Vector2u targetSize, sf::Vector2u newSize)
{
    float windowWidth = static_cast<float>(newSize.x);
    float windowHeight = static_cast<float>(newSize.y);

    float scaleX = windowWidth / targetSize.x;
    float scaleY = windowHeight / targetSize.y;
    float scale = std::min(scaleX, scaleY);

    float viewWidth = targetSize.x * scale;
    float viewHeight = targetSize.y * scale;

    sf::FloatRect viewport({0.f, 0.f}, {1.f, 1.f});
    viewport.size.x = viewWidth / windowWidth;
    viewport.size.y = viewHeight / windowHeight;

    viewport.position.x = (1.f - viewport.size.x) / 2.f;
    viewport.position.y = (1.f - viewport.size.y) / 2.f;

    sf::View view = window.getView();
    view.setSize(sf::Vector2f(targetSize.x, targetSize.y));
    view.setCenter(sf::Vector2f(targetSize.x, targetSize.y) / 2.f);
    view.setViewport(viewport);

    window.setView(view);
}
int main()
{

    // Window Init
    sf::RenderWindow window(
        sf::VideoMode({800u, 600u}),
        "Arugo");
    window.setFramerateLimit(24);

    // Global variables
    GameState gameState = GameState::Playing;

    // Assets
    if (!ResourceManager::get().loadFont("default", "assets/fonts/NotoSerifTC-Medium.ttf"))
        return -1;
    ResourceManager::get().loadTexture("tiles/top",         "assets/tiles/top.png");
    ResourceManager::get().loadTexture("tiles/wall_top",    "assets/tiles/wall_top.png");
    ResourceManager::get().loadTexture("tiles/wall_middle", "assets/tiles/wall_middle.png");
    ResourceManager::get().loadTexture("tiles/wall_bottom", "assets/tiles/wall_bottom.png");
    ResourceManager::get().loadTexture("portal",            "assets/tiles/portal.png");
    ResourceManager::get().loadTexture("hero",              "assets/actors/hero.png");
    ResourceManager::get().loadTexture("goblin",            "assets/actors/goblin.png");
    ResourceManager::get().loadTexture("items/potion",      "assets/items/potion.png");
    ResourceManager::get().loadTexture("items/attack_buff", "assets/items/attack_buff.png");

    // Custom component Init
    InputHandler inputHandler;
    GameWorld world;
    world.onPlayerDied(
        [&gameState]()
        {
            gameState = GameState::GameOver;
        });
    GameOverUI gameoverUI(sf::Vector2f(window.getSize()));
    TurnBanner turnBanner;
    FlashMessage flashMessage(sf::Vector2f(window.getSize()));
    // Helper component
    sf::Clock deltaClock;

    // Game Loop
    while (window.isOpen())
    {
        // Input event
        while (const std::optional event = window.pollEvent())
        {
            // Build-in close app action
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (const auto *resized = event->getIf<sf::Event::Resized>())
            {
                adaptScreenSize(window, {800, 600}, resized->size);
            }
            if (const auto *key = event->getIf<sf::Event::KeyPressed>())
            {

                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
            }

            // UI action
            bool consumed = gameoverUI.handleEvent(*event, window);

            if (consumed)
            {
                if (gameoverUI.isRestartClicked())
                {
                    gameState = GameState::Playing;
                    world.gotoStartLevel();
                    gameoverUI.reset();
                }
                continue;
            }

            if (gameState == GameState::GameOver)
            {
                continue;
            }
            world.input(*event);
        }

        // Loop Callback 'update()' before render each frame
        float deltaTime = deltaClock.restart().asSeconds();
        world.update(deltaTime);

        // Trigger only one time each time message is changed
        if (world.hasFlashMessage())
        {
            flashMessage.show(world.getFlashMessage());
            world.clearFlashMessage();
        }
        flashMessage.update(deltaTime);
        // Not really helpful, maybe use as "Level Indicator" Like: "【Level 1】"
        // turnBanner.setState(world.getCurrentBattlePhase(), world.getCurrentActor().getName());
        // turnBanner.update(deltaTime);

        // Rendering
        window.clear();

        window.draw(world);
        if (gameState == GameState::GameOver)
        {
            window.draw(gameoverUI);
        }
        window.draw(turnBanner);
        window.draw(flashMessage);
        window.display();
    }
}
