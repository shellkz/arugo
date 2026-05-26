#pragma once

#include <vector>
#include <random>
#include <stdexcept>
#include <utility>

template <typename T>
T popRandom(std::vector<T> &list)
{
    if (list.empty())
    {
        throw std::out_of_range("Error: popRandom() empty list.");
    }
    // Initial random number generator once across app session
    // Prevent generating same random number
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // define range between [0, size - 1]
    std::uniform_int_distribution<size_t> dist(0, list.size() - 1);
    size_t randomIndex = dist(gen);
    T chosenElement = list[randomIndex];

    // swap picked wit last then pop it
    std::swap(list[randomIndex], list.back());
    list.pop_back();

    return chosenElement;
}