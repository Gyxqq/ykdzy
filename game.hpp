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
    int run;
    int run_state;
    int gui_open;
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
    int player_on_ground(player *player);
    float get_distance_to_ground(player *player);
    int player_attack_side(player *player, int side);
    float get_distance_to_side(player *player, int side);
    int update();
    int save();
};