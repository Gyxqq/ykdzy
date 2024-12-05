#pragma once
#include "map.hpp"
namespace structure
{
    typedef void (*structure)(chunk *chunk);
    typedef int (*log_func)(char *level, char *data, char *module);
    extern std::vector<structure::structure> structures;
    int init(std::string path);
}
