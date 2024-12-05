#pragma once
#include "map.hpp"
typedef int (*log_func)(char *level, char *data, char *module);
static log_func __stdcall c_log = NULL;
extern "C" __declspec(dllexport) void set_log_func(log_func func)
{
    c_log = func;
}
extern "C" __declspec(dllexport) void test_fun(chunk *chunk)
{
    int j = 0;
    for (int i = 0; i < chunk->block_count; i++)
    {
        if (chunk->blocks[i].type == block_type::BLOCK_AIR)
        {
            j++;
        }
    }
    if (c_log)
    {
        std::string log_message = "Air block count: " + std::to_string(j);
        c_log("info", (char *)log_message.c_str(), "testmod");
    }
}