#include "TileMap.h"
#include "resources/ResourceManager.h"
#include "utils/Random.h"
#include <cmath>

TileMap::TileMap()
{
    regenerate();
}
Tile &TileMap::get(int x, int y)
{
    return m_tiles[y][x];
}
Tile TileMap::get(int x, int y) const
{
    return m_tiles[y][x];
}

void TileMap::set(int x, int y, Tile t)
{
    m_tiles[y][x] = t;
}

bool TileMap::inBounds(int x, int y) const
{
    return x >= 0 && x < MAP_W && y >= 0 && y < MAP_H;
}

bool TileMap::isWalkable(int x, int y) const
{
    if (!inBounds(x, y))
        return false;
    Tile::Type t = get(x, y).type;
    return (t == Tile::Type::Top || t == Tile::Type::Portal);
}

void TileMap::setAttacked(const std::vector<sf::Vector2i> &targets)
{
    if (targets.empty())
        return;

    for (const auto &target : targets)
    {
        if (inBounds(target.x, target.y))
            m_tiles[target.y][target.x].isAttacked = true;
    }
}

void TileMap::clearAttacked()
{
    for (int y = 0; y < MAP_H; ++y)
        for (int x = 0; x < MAP_W; ++x)
            m_tiles[y][x].isAttacked = false;
}

std::vector<sf::Vector2i> TileMap::getChebyshevArea(sf::Vector2i origin, int radius)
{
    std::vector<sf::Vector2i> result = Math::getChebyshevArea(origin, radius);
    result.erase(
        std::remove_if(result.begin(), result.end(), [this](const sf::Vector2i &pos)
                       { return !this->inBounds(pos.x, pos.y); }),
        result.end());
    return result;
}

std::vector<sf::Vector2i> TileMap::getManhattanArea(sf::Vector2i origin, int radius)
{
    std::vector<sf::Vector2i> result = Math::getManhattanArea(origin, radius);
    result.erase(
        std::remove_if(result.begin(), result.end(), [this](const sf::Vector2i &pos)
                       { return !this->inBounds(pos.x, pos.y); }),
        result.end());
    return result;
}

std::vector<sf::Vector2i> TileMap::getEmptyTilePositions() const
{
    std::vector<sf::Vector2i> result;
    for (int y = 0; y < MAP_H; ++y)
        for (int x = 0; x < MAP_W; ++x)
            if (m_tiles[y][x].type == Tile::Type::Top)
                result.push_back({x, y});
    return result;
}

void TileMap::setPortalAt(sf::Vector2i position)
{
    if (inBounds(position.x, position.y))
        m_tiles[position.y][position.x].type = Tile::Type::Portal;
}

void TileMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    sf::RectangleShape brush({(float)TILE_SIZE, (float)TILE_SIZE});

    for (int y = 0; y < MAP_H; y++)
    {
        for (int x = 0; x < MAP_W; x++)
        {
            const Tile &tile = m_tiles[y][x];

            const sf::Texture *tex = nullptr;
            switch (tile.type)
            {
            case Tile::Type::Top:
                tex = &ResourceManager::get().getTexture("tiles/top");
                break;
            case Tile::Type::WallTop:
                tex = &ResourceManager::get().getTexture("tiles/wall_top");
                break;
            case Tile::Type::WallMiddle:
                tex = &ResourceManager::get().getTexture("tiles/wall_middle");
                break;
            case Tile::Type::WallBottom:
                tex = &ResourceManager::get().getTexture("tiles/wall_bottom");
                break;
            case Tile::Type::Portal:
                tex = &ResourceManager::get().getTexture("portal");
                break;
            case Tile::Type::Void:
                continue;
            }

            brush.setTexture(tex, true);
            brush.setFillColor(tile.isAttacked ? sf::Color::Red : sf::Color::White);
            brush.setPosition({(float)(x * TILE_SIZE), (float)(y * TILE_SIZE)});
            target.draw(brush, states);

            if (tile.hasItem())
            {
                const sf::Texture *itemTex = tile.getItem().getTexture();
                if (itemTex)
                {
                    brush.setTexture(itemTex, true);
                    brush.setFillColor(sf::Color::White);
                    target.draw(brush, states);
                }
            }
        }
    }
}

void TileMap::regenerate()
{
    // 1. Fill all with Void
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++)
            m_tiles[y][x] = Tile(Tile::Type::Void);

    // 2. Central shape: rows 1-5, leave bottom 4 rows for walls + margin
    const int SHAPE_TOP = 1;
    const int SHAPE_BOTTOM = MAP_H - 5; // row 5 for 10-height map
    const int SHAPE_LEFT = 1;
    const int SHAPE_RIGHT = MAP_W - 2;

    for (int y = SHAPE_TOP; y <= SHAPE_BOTTOM; y++)
        for (int x = SHAPE_LEFT; x <= SHAPE_RIGHT; x++)
            m_tiles[y][x] = Tile(Tile::Type::Top);

    // 3. Organic edge: randomly remove edge tiles, scaled by distance from shape center
    constexpr int ORGANIC_PASSES = 2;
    constexpr float REMOVE_CHANCE = 0.25f;

    const float centerX = (SHAPE_LEFT + SHAPE_RIGHT) * 0.5f;
    const float centerY = (SHAPE_TOP + SHAPE_BOTTOM) * 0.5f;
    const float maxDist = std::hypot(centerX - SHAPE_LEFT, centerY - SHAPE_TOP);

    for (int pass = 0; pass < ORGANIC_PASSES; pass++)
    {
        for (int y = SHAPE_TOP; y <= SHAPE_BOTTOM; y++)
        {
            for (int x = SHAPE_LEFT; x <= SHAPE_RIGHT; x++)
            {
                if (m_tiles[y][x].type != Tile::Type::Top)
                    continue;

                bool isEdge =
                    (y > 0 && m_tiles[y - 1][x].type == Tile::Type::Void) ||
                    (y < MAP_H - 1 && m_tiles[y + 1][x].type == Tile::Type::Void) ||
                    (x > 0 && m_tiles[y][x - 1].type == Tile::Type::Void) ||
                    (x < MAP_W - 1 && m_tiles[y][x + 1].type == Tile::Type::Void);

                if (!isEdge)
                    continue;

                float t = std::min(std::hypot(x - centerX, y - centerY) / maxDist, 1.0f);
                if (Random::randomFloat(0.f, 1.f) < REMOVE_CHANCE * t)
                    m_tiles[y][x] = Tile(Tile::Type::Void);
            }
        }
    }

    // 4. Front-edge detection: Top tile whose y+1 is not Top -> place wall column
    for (int y = SHAPE_TOP; y <= SHAPE_BOTTOM; y++)
    {
        for (int x = SHAPE_LEFT; x <= SHAPE_RIGHT; x++)
        {
            if (m_tiles[y][x].type != Tile::Type::Top)
                continue;

            if (y + 1 >= MAP_H || m_tiles[y + 1][x].type == Tile::Type::Top)
                continue;
            if (y + 1 < MAP_H && m_tiles[y + 1][x].type != Tile::Type::Top)
                m_tiles[y + 1][x] = Tile(Tile::Type::WallTop);
            if (y + 2 < MAP_H && m_tiles[y + 2][x].type != Tile::Type::Top)
                m_tiles[y + 2][x] = Tile(Tile::Type::WallMiddle);
            if (y + 3 < MAP_H && m_tiles[y + 3][x].type != Tile::Type::Top)
                m_tiles[y + 3][x] = Tile(Tile::Type::WallBottom);
            // if (y + 1 < MAP_H && m_tiles[y + 1][x].type == Tile::Type::Void)
            //     m_tiles[y + 1][x] = Tile(Tile::Type::WallTop);
            // if (y + 2 < MAP_H && m_tiles[y + 2][x].type == Tile::Type::Void)
            //     m_tiles[y + 2][x] = Tile(Tile::Type::WallMiddle);
            // if (y + 3 < MAP_H && m_tiles[y + 3][x].type == Tile::Type::Void)
            //     m_tiles[y + 3][x] = Tile(Tile::Type::WallBottom);
        }
    }
}
