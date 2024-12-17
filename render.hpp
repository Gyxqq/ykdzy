#pragma once
#include <graphics.h>
#include "game.hpp"
#include <vector>
#define BLOCK_TEXTURES_SIZE 32
#define ITEM_TEXTURES_SIZE 32
#define MAX_ITEMS_X 9
#define MAX_ITEMS_Y 4
#define item_begin_x 16
#define item_begin_y 168
#define inventory_begin_x 16
#define inventory_begin_y 284
#define CRAFT_TABLE_BEGIN_X 195
#define CRAFT_TABLE_BEGIN_Y 35
#define CRAFT_OUT_X 307
#define CRAFT_OUT_Y 55
namespace render
{
    extern int width;
    extern int height;
    extern IMAGE *light; // 光照
    int init(int width, int height);
    void close();
    int update_frame(game *game);
    int reverse_y(int y);
    void draw_player(player *player);
    void draw_player_info(player *player);
    void draw_inventory(player *player);
    void draw_widgets(player *player);
    void draw_item_on_mouse(game *game);
    void draw_craft_table(game* game);
    void draw_entities(game* game);
    void draw_arrows(game* game);
    class light
    {
    public:
        POINT render_pos;
        float world_x;
        float world_y;
        int radius; //光照半径
        int intensity; // 0-255
    };
    extern std::vector<class light> lights_list;
    POINT get_render_pos(float x, float y,player *player);
    void draw_light();
    void get_light(block *block,player *player);
}