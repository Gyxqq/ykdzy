#pragma once
#include "render.hpp"
#include "log.hpp"
#include "assets.hpp"
#include "game.hpp"
#include <queue>
namespace render
{

    IMAGE block_textures[block_type::BLOCK_MAX_INDEX];
    IMAGE player_textures[3][6];
    int width;
    int height;
    std::deque<int> fps_queue;
    int init(int width, int height)
    {
        glog::log("info", "Initializing Render", "render");
        initgraph(width, height);
        render::width = width;
        render::height = height;
        settextstyle(20, 0, _T("宋体"));
        settextcolor(WHITE);
        BeginBatchDraw();
        HWND hwnd = GetHWnd();
        HMENU hmenu = GetSystemMenu(hwnd, FALSE);
        if (hmenu != NULL)
        {
            DeleteMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
        }
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
        // start time
        cleardevice();
        auto start = std::chrono::high_resolution_clock::now();
        player player = game->players[0];
        // glog::log("info", "player x: " + std::to_string(player.x) + " y: " + std::to_string(player.y), "render");
        int x, y;
        // render blocks block大小32*32
        x = player.x;
        y = player.y;
        int x_block = render::width / 32 + 4;
        int y_block = render::height / 32 + 4;
        int x_start_pos = player.x - x_block / 2;
        int y_start_pos = player.y - y_block / 2;
        for (int i = 0; i < x_block; i++)
        {
            for (int j = 0; j < y_block; j++)
            {

                block_type block = game->world.get_block(x_start_pos + i, y_start_pos + j);
                if (block < block_type::BLOCK_AIR || block >= block_type::BLOCK_MAX_INDEX)
                    glog::log("error", "Block out of range: " + std::to_string(block), "render");
                // 计算方块在屏幕上的位置
                int pos_x = render::width / 2 - (player.x - x_start_pos * 1.0) * 32 + i * 32-16;
                int pos_y = render::height / 2 - (player.y - y_start_pos * 1.0) * 32 + j * 32+16;

                putimage(pos_x, reverse_y(pos_y), &block_textures[block], SRCPAINT);
            }
        }
        x = render::width / 2;
        y = render::height / 2;
        putimage(x, y, &player_textures[0][0], SRCPAINT);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        if (elapsed.count() < 0.01)
            Sleep((0.01 - elapsed.count()) * 1000);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        if (fps_queue.size() > 200)
            fps_queue.pop_front();
        fps_queue.push_back(1 / elapsed.count());
        // 计算fps
        int sum = 0;
        for (int i = 0; i < fps_queue.size(); i++)
        {
            sum += fps_queue[i];
        }
        outtextxy(0, 0, ("FPS:" + std::to_string(sum / fps_queue.size())).c_str());
        FlushBatchDraw();
        // end time
        return 0;
    }
    void draw_player(player *player)
    {
        int x = player->x;
        int y = player->y;
        int i, j;
        i = player->run - 1;
        j = player->run_state;
        putimage(x, y, &player_textures[i % 3][j % 6], SRCPAINT);
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