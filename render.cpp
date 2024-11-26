#pragma once
#include "render.hpp"
#include "log.hpp"
#include "assets.hpp"
#include "game.hpp"
namespace render
{
    IMAGE block_textures[block_type::BLOCK_MAX_INDEX];
    IMAGE player_textures[3][6];
    int width;
    int height;
    int init(int width, int height)
    {
        glog::log("info", "Initializing Render", "render");
        initgraph(width, height);
        render::width = width;
        render::height = height;
        BeginBatchDraw();
        for (int i = 0; i < block_type::BLOCK_MAX_INDEX; i++)
        {
            loadimage(&block_textures[i], assets::get_block_texture((block_type)i).c_str());
        }

        for (int j = 0; j < 6; j++)
        {
            loadimage(&player_textures[0][j], assets::get_player_texture("run_left", j).c_str());
        }
        for (int j = 0; j < 6; j++)
        {
            loadimage(&player_textures[1][j], assets::get_player_texture("run_right", j).c_str());
        }
        for (int j = 0; j < 6; j++)
        {
            loadimage(&player_textures[2][j], assets::get_player_texture("jump", j).c_str());
        }
        glog::log("info", "Render Initialized", "render");
        return 0;
    }
    int update_frame(game *game)
    {
        // rendplayer in center
        player player = game->players[0];
        // glog::log("info", "player x: " + std::to_string(player.x) + " y: " + std::to_string(player.y), "render");
        int x, y;
        // render blocks block大小64*64
        x = player.x - 16;
        y = player.y - 16;
        int x_block = render::width / 32 + 1;
        int y_block = render::height / 32 + 2;
        int x_start_pos = player.x - x_block / 2;
        int y_start_pos = player.y - y_block / 2;
        for (int i = 0; i < x_block; i++)
        {
            for (int j = 0; j < y_block; j++)
            {

                block_type block = game->world.get_block(x_start_pos + i, y_start_pos + j);
                putimage(i * 32,render::reverse_y(j * 32), &block_textures[block]);
            }
        }
        x = render::width / 2 - 16;
        y = render::height / 2 - 16;
        putimage(x, y, &player_textures[0][0], SRCPAINT);
        FlushBatchDraw();
        return 0;
    }
    int reverse_y(int y)
    {
        return render::height - y;
    }
    void close()
    {
        glog::log("info", "Closing Render", "render");
        closegraph();
    }
} // namespace render