#pragma once
#pragma warning(disable : 4996)
#include <windows.h>
#include <graphics.h> // 假设你在使用 EasyX 库
#include <stdio.h>
#include <conio.h>
#include <ctime>
#include <iostream>
#include <cJSON.h>
#include "map.hpp"
#include "log.hpp"
#include "game.hpp"
#include "assets.hpp"
#include "render.hpp"
#include "structure.hpp"
#include <mutex>
#include <thread>
std::mutex global_mutex;
int exit_flag = 0;
#define BLOCK_ASSETS_PATH "D:\\projects\\ykdzy\\assets\\blocks\\blockconfig.json"
#define PLAYER_ASSETS_PATH "D:\\projects\\ykdzy\\assets\\player\\playerconfig.json"
#define CONST_TEXTURES_PATH "D:\\projects\\ykdzy\\assets\\const\\"
#define ITEM_TEXTURES_PATH "D:\\projects\\ykdzy\\assets\\items\\itemconfig.json"
#define MODS_PATH "D:\\projects\\ykdzy\\mods\\mods.json"
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    glog::log("info", "Hello, world!", "main");
    structure::init(MODS_PATH);
    // class game game0;
    // game0.init("D:\\projects\\ykdzy\\save1\\");
    // game0.save();
    class game game;
    game.load("D:\\projects\\ykdzy\\save1\\game.json");
    game.save();
    assets::load_block_textures(BLOCK_ASSETS_PATH);
    assets::load_player_textures(PLAYER_ASSETS_PATH);
    assets::load_const_textures(CONST_TEXTURES_PATH);
    assets::load_item_textures(ITEM_TEXTURES_PATH);
    render::init(1240, 720);
    
    std::thread game_thread([&game]()
                            {
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
    std::thread game_time = std::thread([&game]()
                                        {
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
                                            } }); // 世界时间线程
    while (exit_flag == 0)
    {
        // game.update();
        global_mutex.lock();
        render::update_frame(&game);
        global_mutex.unlock();
        Sleep(1);
    }
    game_thread.join();
    game_time.join();
    game.save();
    glog::log("info", "Game Loaded", "main");
}

game &get_game()
{
    static game game;
    return game;
}