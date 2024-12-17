#pragma once
#include "map.hpp"
#include "log.hpp"
#include "craft_table.hpp"
//#include "entity.hpp"
#include <vector>
#include <windows.h>
#define MAX_ITEMS 36
#define MAX_ENTITY_NUM 100
#define MAX_NO_MOB_NUM 30
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
    int run;// 0 stand 1 left 2 right 3 jump
    int run_state; 
    int gui_open;
    int chossing_item;
    int last_attack_tick;
    // int thirst;
    std::string name;
    class item items[MAX_ITEMS];
    int save(std::string name);
    int load(std::string name);
    int init(std::string name);
    int get_destroy_speed();
};
class game
{
public:
    map world;
    std::vector<player> players;
    std::string savepath;
    block_attcking attacking_block;
    int show_debug;
    int world_time;
    
    unsigned long long int game_tick;
    POINT mouse_pos;
    item item_on_mouse;
    item craft_table[5];
    craft_table::craft_table *now_craft_table;
    // int chossing_item;
    int init(std::string name);
    int load(std::string name);
    int player_on_ground(player *player);
    float get_distance_to_ground(player *player);
    int player_attack_side(player *player, int side);
    float get_distance_to_side(player *player, int side);
    item get_block_drop(block *block);
    block get_block_by_item(item item);
    int spawn_entity();
    int get_food(item* item);
    void check_num();
    void check_craft();
    int update();
    int save();
    POINTFLOAT screen_to_world(POINT point);
    POINT world_to_screen(POINTFLOAT point);

    //~game();
};
item get_item_by_type(item_type type);