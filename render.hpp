#pragma once
#include <graphics.h>
#include "game.hpp"
#define BLOCK_TEXTURES_SIZE 32
#define ITEM_TEXTURES_SIZE 32
#define MAX_ITEMS_X 9
#define MAX_ITEMS_Y 4
#define item_begin_x 16
#define item_begin_y 168
#define inventory_begin_x 16
#define inventory_begin_y 284
namespace render
{
    extern int width;
    extern int height;
    int init(int width, int height);
    void close();
    int update_frame(game *game);
    int reverse_y(int y);
    void draw_player(player *player);
    void draw_player_info(player *player);
    void draw_inventory(player *player);
}