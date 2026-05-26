#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include "utils/Math.h"
#include <optional>
#include "resources/items/Item.h"
constexpr int MAP_W = 10;
constexpr int MAP_H = 10;
constexpr int TILE_SIZE = 64;

struct Tile
{
    enum class Type
    {
        Top,
        WallTop,
        WallMiddle,
        WallBottom,
        Portal,
        Void
    };

    Type type = Type::Void;
    bool isAttacked = false;
    std::optional<Item> item;

    Tile(Type t = Type::Void)
        : type(t), isAttacked(false), item(std::nullopt) {}

    void setItem(Item newItem)
    {
        item = newItem;
    }

    bool hasItem() const
    {
        return item.has_value();
    }

    Item &getItem()
    {
        return item.value();
    }

    const Item &getItem() const
    {
        return item.value();
    }

    void removeItem()
    {
        item = std::nullopt;
    }
};

class TileMap : public sf::Drawable
{
public:
    TileMap();

    Tile get(int x, int y) const;
    Tile &get(int x, int y);

    void set(int x, int y, Tile t);
    bool inBounds(int x, int y) const;
    bool isWalkable(int x, int y) const;
    void setAttacked(const std::vector<sf::Vector2i> &targets);
    void clearAttacked();
    std::vector<sf::Vector2i> getChebyshevArea(sf::Vector2i origin, int radius);
    std::vector<sf::Vector2i> getManhattanArea(sf::Vector2i origin, int radius);
    std::vector<sf::Vector2i> getEmptyTilePositions() const;
    void setPortalAt(sf::Vector2i position);
    void regenerate();

private:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    std::array<std::array<Tile, MAP_W>, MAP_H> m_tiles;
};
