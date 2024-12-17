#pragma once
#include "game.hpp"
#include "log.hpp"
#include "map.hpp"
namespace entity {
extern std::vector<class entity> entities;
extern std::vector<class arrow> arrows;
enum entity_type {
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_ZOMBIE, // 僵尸
    ENTITY_TYPE_SKELETON, // 骷髅
    ENTITY_TYPE_CREEPER, // 爬行者
    ENTITY_TYPE_ENDERMAN, // 末影人
    ENTITY_TYPE_MOB_MAX, // 怪物最大索引
    ENTITY_TYPE_PIG, // 猪
    ENTITY_TYPE_COW, // 牛
    ENTITY_TYPE_MAX
};
enum move_state {
    MOVE_STATE_STAND,
    MOVE_STATE_RUN_LEFT,
    MOVE_STATE_RUN_RIGHT,
    MOVE_STATE_JUMP,
    MOVE_STATE_MAX
};
enum side {
    SIDE_LEFT,
    SIDE_RIGHT,
    SIDE_UP,
    SIDE_DOWN,
    SIDE_MAX
};

class entity {
public:
    entity_type type;
    float x, y;
    int health;
    item drop;
    int game_tick;
    int jump_state;
    int move_;
    int move_state;
    std::vector<POINT> path;
    entity(entity_type type, float x, float y, int health, item drop)
        : type(type)
        , x(x)
        , y(y)
        , health(health)
        , drop(drop)
    {
        this->move_ = 0;
        this->game_tick = 0;
        this->jump_state = 0;
        this->move_state = 0;
    }
    entity()
    {
        this->type = ENTITY_TYPE_PLAYER;
        this->x = 0;
        this->y = 0;
        this->health = 100;
        this->drop = item();
        this->game_tick = 0;
        this->jump_state = 0;
        this->move_ = 0;
        this->move_state = 0;
    }
    int player_enemy;
    int move(game* game);
    void attack(game* game);
    int if_reach_side(game* game, int side); // 0 left 1 right 2 up 3 down
    float get_distance_to_side(game* game, int side);
    void find_path_to_player(game* game);
    void update(game* game);
    item get_drop();
    std::string get_name();
};
class arrow {
public:
    POINTFLOAT start;
    POINTFLOAT end;
    float speed;
    int damage;
    int game_tick;
    POINTFLOAT pos;
    void move(game* game);
};
void update_other(game* game);

}