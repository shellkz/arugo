#pragma once
#include <vector>
#include "resources/items/Item.h"
class ItemRegistry
{
public:
    ItemRegistry();

    const Item &get(int index) const;
    void add(Item item);
    int size() const;

private:
    std::vector<Item> m_items;
};