#pragma once
#include "map.hpp"
#include "log.hpp"
#include <vector>
#include <windows.h>
#define MAX_ITEMS 36
class block_attcking
{
public:
    block *block;
    int attacking_state;
};
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
    int chossing_item;
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
    block_attcking attacking_block;
    int show_debug;
    POINT mouse_pos;
    item item_on_mouse;
    // int chossing_item;
    int init(std::string name);
    int load(std::string name);
    int player_on_ground(player *player);
    float get_distance_to_ground(player *player);
    int player_attack_side(player *player, int side);
    float get_distance_to_side(player *player, int side);
    item get_block_drop(block *block);
    block get_block_by_item(item item);
    void check_num();
    int update();
    int save();
};