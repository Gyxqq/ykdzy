#pragma once
#include "log.hpp"
#include "map.hpp"
#include "game.hpp"
namespace entity {
extern std::vector<class entity> entities;
enum entity_type {
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_ZOMBIE, // ��ʬ
    ENTITY_TYPE_SKELETON, // ����
    ENTITY_TYPE_CREEPER, // ������
    ENTITY_TYPE_ENDERMAN, // ĩӰ��
    ENTITY_TYPE_MOB_MAX, // �����������
    ENTITY_TYPE_PIG, // ��
    ENTITY_TYPE_COW, // ţ   
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
        this->game_tick = 0;
        this->jump_state = 0;
        this->move_state = 0;
    }
    entity() {
        this->type = ENTITY_TYPE_PLAYER;
        this->x = 0;
        this->y = 0;
        this->health = 100;
        this->drop = item();
        this->game_tick = 0;
        this->jump_state = 0;
        this->move_state = 0;
    }
    int player_enemy;
    void move(game* game);
    void attack(game* game);
    int if_reach_side(game* game, int side);
    float get_distance_to_side(game* game, int side);
    void find_path_to_player(game* game);
    item get_drop();
};

}