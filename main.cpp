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
#define  ASSETS_PATH "D:\\projects\\ykdzy\\assets\\"
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    glog::log("info", "Hello, world!", "main");
    class game game;
    // game.init("D:\\projects\\ykdzy\\save1\\");
    game.load("D:\\projects\\ykdzy\\save1\\game.json");
    game.save();
    glog::log("info", "Game Loaded", "main");
    // return 0;
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