#pragma once
#include "map.hpp"
#include "log.hpp"
#include <cJSON.h>
namespace assets
{
    class block_texture
    {
    public:
        block_type type;
        std::string texture;
    };
    extern block_texture block_textures[block_type::BLOCK_MAX_INDEX];
    int load_block_textures(std::string config_path);

}
