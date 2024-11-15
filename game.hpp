#pragma once
#include "map.hpp"
#include "log.hpp"
#include <vector>
class game
{
public:
    map world;
    std::vector<player> players;
    int init(std::string name);
    int save();
};