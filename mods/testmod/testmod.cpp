#pragma once
#include "map.hpp"

void test_fun(chunk *chunk)
{
    int j = 0;
    for (int i = 0; i < chunk->block_count; i++)
    {
        if (chunk->blocks[i].type == block_type::BLOCK_AIR)
        {
            j++;
        }
    }
}