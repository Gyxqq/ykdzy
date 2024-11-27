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
#include <mutex>
#include <thread>
std::mutex global_mutex;
int exit_flag = 0;
#define BLOCK_ASSETS_PATH "D:\\projects\\ykdzy\\assets\\blocks\\blockconfig.json"
#define PLAYER_ASSETS_PATH "D:\\projects\\ykdzy\\assets\\player\\playerconfig.json"
#define CONST_TEXTURES_PATH "D:\\projects\\ykdzy\\assets\\const\\"
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    glog::log("info", "Hello, world!", "main");
    // class game game0;
    // game0.init("D:\\projects\\ykdzy\\save1\\");
    // game0.save();
    class game game;
    game.load("D:\\projects\\ykdzy\\save1\\game.json");
    game.save();
    assets::load_block_textures(BLOCK_ASSETS_PATH);
    assets::load_player_textures(PLAYER_ASSETS_PATH);
    assets::load_const_textures(CONST_TEXTURES_PATH);
    render::init(1240, 720);
    std::thread game_thread([&game]()
                            {
                                while (exit_flag == 0)
                                {
                                    game.update();
                                    Sleep(1);
                                } });

    while (exit_flag == 0)
    {
        // game.update();
        global_mutex.lock();
        render::update_frame(&game);
        global_mutex.unlock();
        Sleep(1);
    }
    game_thread.join();
    game.save();
    glog::log("info", "Game Loaded", "main");
}

