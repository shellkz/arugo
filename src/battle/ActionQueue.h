#pragma once
#include <deque>
#include "vector"
#include <cstdio>
class Entity;

class ActionQueue
{
public:
    Entity *deq();
    void enq(Entity *entity);
    void enqAll(const std::vector<Entity *> &entities);
    void enqFront(Entity *entity);

    void erase(Entity *entity);
    void eraseDead();
    void eraseAll();

    size_t size() const { return m_queue.size(); }
    bool empty() const { return m_queue.empty(); }
    void print() const;

private:
    std::deque<Entity *> m_queue;
};