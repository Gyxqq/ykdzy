#pragma once
#include "log.hpp"
#include "map.hpp"
#include "game.hpp"
namespace entity {
enum entity_type {
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_ZOMBIE,
    ENTITY_TYPE_SKELETON,
    ENTITY_TYPE_CREEPER,
    ENTITY_TYPE_ENDERMAN,
    ENTITY_TYPE_MAX
};
class entity {
public:
    entity_type type;
    float x, y;
    int health;
    item drop;
    int game_tick;
    int jump_state;
    int move_state;
    std::vector<POINT> path;
    entity(entity_type type, float x, float y, int health, item drop)
        : type(type)
        , x(x)
        , y(y)
        , health(health)
        , drop(drop)
    {
    }
    int player_enemy;
    void move(game* game);
    void attack(game* game);
    int if_reach_side(game* game, int side);
    float get_distance_to_side(game* game, int side);
    void find_path_to_player(game* game);
};
}