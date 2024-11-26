#pragma  once
#include <graphics.h>
#include "game.hpp"
namespace render
{
    int init(int width, int height);
    void close();
    int update_frame( game *game);
     int reverse_y(int y);
}