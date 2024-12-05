#pragma once
#include "render.hpp"
#include "log.hpp"
#include "assets.hpp"
#include "game.hpp"
#include <queue>

namespace render
{
    void put_transparentimage(int x, int y, IMAGE *img)
    {
        IMAGE img1;
        DWORD *d1;
        img1 = *img;
        d1 = GetImageBuffer(&img1);
        float h, s, l;
        for (int i = 0; i < img1.getheight() * img1.getwidth(); i++)
        {
            RGBtoHSL(BGR(d1[i]), &h, &s, &l);
            if (l < 0.03)
            {
                d1[i] = BGR(WHITE);
            }
            if (d1[i] != BGR(WHITE))
            {
                d1[i] = 0;
            }
        }
        putimage(x, y, &img1, SRCAND);
        putimage(x, y, img, SRCPAINT);
    }
    int get_block_x(float x)
    {
        if (x < 0)
            return (int)x - 1;
        return (int)x;
    }
    IMAGE block_textures[block_type::BLOCK_MAX_INDEX];
    IMAGE player_textures[3][6];
    IMAGE const_textures[assets::const_texture_type::CONST_TEXTURE_MAX_INDEX];
    IMAGE item_textures[item_type::ITEM_MAX_INDEX];
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
        setbkmode(TRANSPARENT);
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
        for (int i = 0; i < assets::const_texture_type::CONST_TEXTURE_MAX_INDEX; i++)
        {
            loadimage(&const_textures[i], assets::const_textures[i].texture.c_str());
        }
        for (int i = 0; i < item_type::ITEM_MAX_INDEX; i++)
        {
            loadimage(&item_textures[i], assets::item_textures[i].texture.c_str());
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
        int x_block = render::width / BLOCK_TEXTURES_SIZE + 4;
        int y_block = render::height / BLOCK_TEXTURES_SIZE + 4;
        int x_start_pos = get_block_x(player.x - x_block / 2.0);
        int y_start_pos = player.y - y_block / 2.0;
        for (int i = 0; i < x_block; i++)
        {
            for (int j = 0; j < y_block; j++)
            {

                block_type block = game->world.get_block(x_start_pos + i, y_start_pos + j);
                if (block < block_type::BLOCK_AIR || block >= block_type::BLOCK_MAX_INDEX)
                    glog::log("error", "Block out of range: " + std::to_string(block), "render");
                // 计算方块在屏幕上的位置
                int pos_x = render::width / 2 - (player.x - 0.5 - x_start_pos * 1.0 - i * 1.0) * BLOCK_TEXTURES_SIZE;
                int pos_y = render::height / 2 - (player.y - 0.5 - y_start_pos * 1.0 - j * 1.0) * BLOCK_TEXTURES_SIZE;
                // if (x_start_pos + i < 0)
                //     pos_x -= 32;
                putimage(pos_x, reverse_y(pos_y), &block_textures[block], SRCPAINT);
            }
        }
        x = render::width / 2;
        y = render::height / 2;
        // putimage(x, y, &player_textures[0][0], SRCPAINT); // player
        put_transparentimage(x, reverse_y(y), &player_textures[0][0]);
        draw_player_info(&(game->players[0]));
        if (game->players[0].gui_open)
        {
            draw_inventory(&(game->players[0]));
        }
        draw_widgets(&(game->players[0]));
        draw_item_on_mouse(game);   
        {
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
            if (game->show_debug)
            {
                outtextxy(0, 0, ("FPS:" + std::to_string(sum / fps_queue.size())).c_str());
                char pos[100];
                sprintf(pos, "X:%.2f Y:%.2f", player.x, player.y);
                outtextxy(0, 20, pos);
                sprintf(pos, "mouse X:%d Y:%d", game->mouse_pos.x, game->mouse_pos.y);
                outtextxy(0, 40, pos);
                sprintf(pos, "hungry:%d health:%d", player.hunger, player.health);
                outtextxy(0, 60, pos);
            }
        }
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
    void draw_player_info(player *player)
    {
        int heart_count = player->health / 10;
        for (int i = 0; i < heart_count; i++)
        {

            // putimage(render::width - (16 + i * 16), 5, &const_textures[assets::const_texture_type::CONST_TEXTURE_HEART], SRCPAINT);
            put_transparentimage(render::width - (16 + i * 16), 5, &const_textures[assets::const_texture_type::CONST_TEXTURE_HEART]);
        }
        int hunger = player->hunger / 10;
        for (int i = 0; i < hunger; i++)
        {
            put_transparentimage(render::width - (16 + i * 16), 25, &const_textures[assets::const_texture_type::CONST_TEXTURE_HUNGER]);
        }
    }
    void draw_inventory(player *player)
    {
        settextstyle(14, 0, _T("宋体"));
        put_transparentimage(render::width / 2 - 160, render::height / 2 - 169, &const_textures[assets::const_texture_type::CONST_TEXTURE_BACKPACK]);
        int pos_x = render::width / 2 - 160 + item_begin_x;
        int pos_y = render::height / 2 - 169 + item_begin_y;
        for (int i = 0; i < 27; i++)
        {
            int x = i % 9;
            int y = i / 9;
            if (player->items[i].type == item_type::ITEM_AIR)
                continue;
            put_transparentimage(pos_x + x * 36, pos_y + y * 36, &item_textures[player->items[i].type]);
            outtextxy(pos_x + x * 36 + 16, pos_y + y * 36 + 20, std::to_string(player->items[i].count).c_str());
        }
        pos_x = render::width / 2 - 160 + inventory_begin_x;
        pos_y = render::height / 2 - 169 + inventory_begin_y;
        for (int i = 0; i < 9; i++)
        {
            if (player->items[i + 27].type == item_type::ITEM_AIR)
                continue;
            put_transparentimage(pos_x + i * 36, pos_y, &item_textures[player->items[i + 27].type]);
            outtextxy(pos_x + i * 36 + 16, pos_y + 20, std::to_string(player->items[i + 27].count).c_str());
        }
        settextstyle(20, 0, _T("宋体"));
    }
    void draw_widgets(player *player)
    {
        int x = render::width / 2 - 180;
        int y = render::height - 48;
        put_transparentimage(x, y, &const_textures[assets::const_texture_type::CONST_TEXTURE_WIDGETS]);
        settextstyle(14, 0, _T("宋体"));
        for (int i = 0; i < 9; i++)
        {
            if (player->items[i + 27].type == item_type::ITEM_AIR)
                continue;
            put_transparentimage(x + 6 + i * 40, y + 6, &item_textures[player->items[i + 27].type]);
            outtextxy(x + 6 + i * 40 + 16, y + 6 + 20, std::to_string(player->items[i + 27].count).c_str());
        }
        settextstyle(20, 0, _T("宋体"));
        setlinecolor(WHITE);
        rectangle(x + 6 + player->chossing_item * 40, y + 6, x + 6 + player->chossing_item * 40 + 32, y + 6 + 32);
    }
    void draw_item_on_mouse(game* game)
    {
        if (game->item_on_mouse.type == item_type::ITEM_AIR)
            return;
        int x = game->mouse_pos.x - 16;
        int y = game->mouse_pos.y - 16;
        put_transparentimage(x, y, &item_textures[game->item_on_mouse.type]);
        settextstyle(14, 0, _T("宋体"));
        outtextxy(x + 16, y + 20, std::to_string(game->item_on_mouse.count).c_str());
        settextstyle(20, 0, _T("宋体"));

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