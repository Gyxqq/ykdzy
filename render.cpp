#pragma once
#include "render.hpp"
#include "log.hpp"
#include "assets.hpp"
namespace render
{
    IMAGE block_textures[block_type::BLOCK_MAX_INDEX];
    IMAGE player_textures[3][6];
    int init(int width, int height)
    {
        glog::log("info", "Initializing Render", "render");
        initgraph(width, height);
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
    void close()
    {
        glog::log("info", "Closing Render", "render");
        closegraph();
    }
} // namespace render