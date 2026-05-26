#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>

class ResourceManager
{
public:
    static ResourceManager &get();

    bool loadFont(const std::string &name, const std::string &path);
    const sf::Font &getFont(const std::string &name) const;

    bool loadTexture(const std::string &name, const std::string &path);
    const sf::Texture &getTexture(const std::string &name) const;

private:
    ResourceManager() = default;
    std::unordered_map<std::string, sf::Font> m_fonts;
    std::unordered_map<std::string, sf::Texture> m_textures;
};
