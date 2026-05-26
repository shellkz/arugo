#include "ResourceManager.h"
#include <iostream>
#include <stdexcept>

ResourceManager &ResourceManager::get()
{
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::loadFont(const std::string &name, const std::string &path)
{
    sf::Font font;
    if (!font.openFromFile(path))
    {
        std::cerr << "[-] ResourceManager: 無法載入字型 " << path << "\n";
        return false;
    }
    m_fonts[name] = std::move(font);
    std::cout << "[+] ResourceManager: 字型 [" << name << "] -> " << path << "\n";
    return true;
}

const sf::Font &ResourceManager::getFont(const std::string &name) const
{
    auto it = m_fonts.find(name);
    if (it == m_fonts.end())
        throw std::runtime_error("ResourceManager: 找不到字型 [" + name + "]");
    return it->second;
}

bool ResourceManager::loadTexture(const std::string &name, const std::string &path)
{
    sf::Texture texture;
    if (!texture.loadFromFile(path))
    {
        std::cerr << "[-] ResourceManager: 無法載入貼圖 " << path << "\n";
        return false;
    }
    m_textures[name] = std::move(texture);
    std::cout << "[+] ResourceManager: 貼圖 [" << name << "] -> " << path << "\n";
    return true;
}

const sf::Texture &ResourceManager::getTexture(const std::string &name) const
{
    auto it = m_textures.find(name);
    if (it == m_textures.end())
        throw std::runtime_error("ResourceManager: 找不到貼圖 [" + name + "]");
    return it->second;
}
