#pragma once
#include "map.hpp"
namespace structure
{
    typedef void (*structure)(chunk *chunk);
    extern std::vector<structure::structure> structures;
    int init(std::string path);
}
