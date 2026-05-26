#include "ActionQueue.h"
#include "Entity.h"
#include <algorithm>

Entity *ActionQueue::deq()
{
    if (m_queue.empty())
    {
        return nullptr;
    }
    Entity *first = m_queue.front();
    m_queue.pop_front();
    return first;
}

void ActionQueue::enq(Entity *entity)
{
    if (entity != nullptr)
    {
        m_queue.push_back(entity);
    }
}
void ActionQueue::enqFront(Entity *entity)
{
    if (entity != nullptr)
    {
        m_queue.push_front(entity);
    }
}
void ActionQueue::erase(Entity *entity)
{
    if (entity == nullptr)
        return;

    auto it = std::remove(m_queue.begin(), m_queue.end(), entity);

    m_queue.erase(it, m_queue.end());
}
void ActionQueue::eraseDead()
{

    auto it = std::remove_if(m_queue.begin(), m_queue.end(), [](const Entity *entity)
                             { return entity != nullptr && entity->isDead(); });

    m_queue.erase(it, m_queue.end());
}

void ActionQueue::eraseAll()
{
    m_queue.clear();
}

void ActionQueue::enqAll(const std::vector<Entity *> &entities)
{

    for (Entity *entity : entities)
    {
        enq(entity);
    }
}
void ActionQueue::print() const
{
    for (Entity *e : m_queue)
    {
        printf("%s\r\n", e->getName().c_str());
    }
}