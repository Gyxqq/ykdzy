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

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    glog::log("info","class block size: "+std::to_string(sizeof(class block)),"main");
    glog::log("info","class chunk size: "+std::to_string(sizeof(class chunk)),"main");
    glog::log("info", "Hello, world!", "main");
    class map m;
    m.load("D:\\projects\\ykdzy\\map.json");
    m.save();
    // for(int i=0;i<64;i++){
    //     m.load("D:\\projects\\ykdzy\\map.json");
    //     m.save();
    // }
    
    return 0;
}
