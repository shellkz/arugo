#include "resources/items/ItemRegistry.h"

ItemRegistry::ItemRegistry()
{
}

const Item &ItemRegistry::get(int index) const
{
    return m_items[index];
}

void ItemRegistry::add(Item item)
{
    m_items.push_back(std::move(item));
}

int ItemRegistry::size() const
{
    return static_cast<int>(m_items.size());
}