#pragma once
#include <tchar.h>
#include <windows.h>
#include <string>
#define START_BACKGROUND_IMAGE "D:\\projects\\ykdzy\\assets\\const\\start_background.bmp"
#define SAVE_PATH "D:\\projects\\ykdzy\\saves\\"
int start_screen(HINSTANCE hInstance);
extern std::string save_name;
extern std::string explain;