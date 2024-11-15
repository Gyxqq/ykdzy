#pragma once
#include "map.hpp"
#include "log.hpp"
#include <vector>
class player
{
public:
    float x;
    float y;
    int health;
    int hunger;
    // int thirst;
    class item *items;
    int item_count;
    int save(std::string name);
    int load(std::string name);
};
class game
{
public:
    map world;
    std::vector<player> players;
    std::string savepath;
    int init(std::string name);
    int load(std::string name);
    int save();
};