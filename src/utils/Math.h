#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <algorithm>
#include <vector>
namespace Math
{
    inline int manhattanDistance(sf::Vector2i a, sf::Vector2i b)
    {
        sf::Vector2i delta = b - a;
        return std::abs(delta.x) + std::abs(delta.y);
    }
    inline std::vector<sf::Vector2i> getManhattanArea(sf::Vector2i origin, int radius)
    {
        std::vector<sf::Vector2i> result;

        //  #tile = 2*r*(r+1) + 1
        result.reserve(2 * radius * (radius + 1) + 1);

        for (int dy = -radius; dy <= radius; ++dy)
        {
            for (int dx = -radius; dx <= radius; ++dx)
            {
                // Manhattan distance from origin to surronding position
                if (manhattanDistance({0, 0}, {dx, dy}) <= radius)
                {
                    result.push_back({origin.x + dx, origin.y + dy});
                }
            }
        }
        return result;
    }
    inline int chebyshevDistance(const sf::Vector2i &p1, const sf::Vector2i &p2)
    {
        int dx = std::abs(p1.x - p2.x);
        int dy = std::abs(p1.y - p2.y);

        return std::max(dx, dy);
    }
    inline std::vector<sf::Vector2i> getChebyshevArea(sf::Vector2i origin, int radius)
    {
        std::vector<sf::Vector2i> area;

        if (radius <= 0)
        {
            return area;
        }

        for (int dy = -radius; dy <= radius; ++dy)
        {
            for (int dx = -radius; dx <= radius; ++dx)
            {
                // 條件 1：排除中心點本身 (dx == 0 且 dy == 0)
                if (dx == 0 && dy == 0)
                {
                    continue;
                }

                int targetX = origin.x + dx;
                int targetY = origin.y + dy;

                area.push_back(sf::Vector2i(targetX, targetY));
            }
        }

        return area;
    }
}