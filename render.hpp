#pragma once
#include <graphics.h>
#include "game.hpp"
namespace render
{
    int init(int width, int height);
    void close();
    int update_frame(game *game);
    int reverse_y(int y);
    void draw_player(player *player);
    void draw_player_info(player *player);
    void draw_inventory(player *player);
}