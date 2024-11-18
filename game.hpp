#pragma once
#include "map.hpp"
#include "log.hpp"
#include <vector>
#define MAX_ITEMS 20
class player
{
public:
    float x;
    float y;
    int health;
    int hunger;
    // int thirst;
    std::string name;
    class item items[MAX_ITEMS];
    int save(std::string name);
    int load(std::string name);
    int init(std::string name);
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