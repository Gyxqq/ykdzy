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
#define BLOCK_ASSETS_PATH "D:\\projects\\ykdzy\\assets\\blocks\\blockconfig.json"
#define PLAYER_ASSETS_PATH "D:\\projects\\ykdzy\\assets\\player\\playerconfig.json"
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    glog::log("info", "Hello, world!", "main");
    class game game;
    // game.init("D:\\projects\\ykdzy\\save1\\");
    game.load("D:\\projects\\ykdzy\\save1\\game.json");
    assets::load_block_textures(BLOCK_ASSETS_PATH);
    assets::load_player_textures(PLAYER_ASSETS_PATH);
    render::init(1240, 720);
    game.save();
    glog::log("info", "Game Loaded", "main");
}

// int main()
// {
//     glog::log("info", "Hello, world!", "main");
//     class game game;
//     game.init("D:\\projects\\ykdzy\\save1\\");

//     // game.load("D:\\projects\\ykdzy\\save1\\game.json");
//     game.save();
//     glog::log("info", "Game Loaded", "main");
//     return 0;
// }