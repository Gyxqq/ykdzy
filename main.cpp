#pragma once
#pragma warning(disable : 4996)
#include "EasyXPng.hpp"
#include "assets.hpp"
#include "craft_table.hpp"
#include "game.hpp"
#include "log.hpp"
#include "map.hpp"
#include "render.hpp"
#include "startwindow.hpp"
#include "structure.hpp"
#include <cJSON.h>
#include <conio.h>
#include <ctime>
#include <graphics.h> // 假设你在使用 EasyX 库
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <thread>
#include <windows.h>
std::mutex global_mutex;
int exit_flag = 0;
#define BLOCK_ASSETS_PATH ".\\assets\\blocks\\blockconfig.json"
#define PLAYER_ASSETS_PATH ".\\assets\\player\\playerconfig.json"
#define CONST_TEXTURES_PATH ".\\assets\\const\\"
#define ITEM_TEXTURES_PATH ".\\assets\\items\\itemconfig.json"
#define ENTITY_TEXTURES_PATH ".\\assets\\entity\\entity_config.json"
#define MODS_PATH ".\\mods\\mods.json"
#define SAVE_PATH ".\\saves\\"
#define MUSIC_PATH ".\\assets\\music\\"
#define MUSIC_COUNT 1
#pragma comment(lib, "winmm.lib")
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int tag = start_screen(hInstance);
    glog::log("info", "Hello, world!", "main");
    craft_table::craft_table::init();
    structure::init(MODS_PATH);
    class game game0;
    // game0.init("D:\\projects\\ykdzy\\save1\\");
    // game0.save();

    class game game;
    if (tag == 1) {
        game0.init(save_name);
        game0.save();
        game.load(save_name + "game.json");
        game.save();
    } else if (tag == 2) {
        game.load(save_name + "game.json");
        game.save();
    } else {
        return 0;
    }
    /*game.load("D:\\projects\\ykdzy\\save1\\game.json");
    game.save();*/
    assets::load_block_textures(BLOCK_ASSETS_PATH);
    assets::load_player_textures(PLAYER_ASSETS_PATH);
    assets::load_const_textures(CONST_TEXTURES_PATH);
    assets::load_entity_textures(ENTITY_TEXTURES_PATH);
    assets::load_item_textures(ITEM_TEXTURES_PATH);
    render::init(1240, 720);

    std::thread game_thread([&game]() {
                                while (exit_flag == 0)
                                {
                                    auto start = std::chrono::system_clock::now();
                                    game.update();
                                    auto end = std::chrono::system_clock::now();
                                    std::chrono::duration<double> elapsed_seconds = end - start;
                                    if (elapsed_seconds.count() < 0.01)
                                    {
                                        Sleep((0.01 - elapsed_seconds.count()) * 1000);
                                    }
                                } });

    std::thread game_time = std::thread([&game]() {
                                            while (exit_flag == 0)
                                            {
                                                auto start = std::chrono::system_clock::now();
                                                global_mutex.lock();
                                                game.world_time++;
                                                global_mutex.unlock();
                                                auto end = std::chrono::system_clock::now();
                                                if (std::chrono::duration<double>(end - start).count() < 1)
                                                {
                                                    Sleep((1 - std::chrono::duration<double>(end - start).count()) * 1000);
                                                }
                                            } }); // 世界时间

    std::thread music = std::thread([]() {
        int music_index = 0;

        while (exit_flag == 0) {
            mciSendString("close all", NULL, 0, NULL);
            char music_cmd[256];
            sprintf(music_cmd, "open %s type mpegvideo alias music", (MUSIC_PATH + std::to_string(music_index) + ".mp3").c_str());
            mciSendString(music_cmd, NULL, 0, NULL);
            mciSendString("play music", NULL, 0, NULL);
            music_index++;
            music_index %= MUSIC_COUNT;
            MCI_STATUS_PARMS status;
            status.dwItem = MCI_STATUS_MODE;
            while (exit_flag == 0) {
                mciSendString(_T("status myAudio mode"), (LPSTR)&status.dwItem, sizeof(status.dwItem), NULL);

                // 如果音频已经停止，退出循环
                if (status.dwItem == MCI_MODE_STOP) {
                    break;
                }

                Sleep(1000); // 等待 100 毫秒检查状态
            }
        }
    });

    while (exit_flag == 0) {
        // game.update();
        global_mutex.lock();
        render::update_frame(&game);
        global_mutex.unlock();
        Sleep(1);
    }
    game_thread.join();
    game_time.join();
    music.join();
    game.save();
    craft_table::craft_table::deinit();
    //glog::log("info", "Game Loaded", "main");
}

game& get_game()
{
    static game game;
    return game;
}
