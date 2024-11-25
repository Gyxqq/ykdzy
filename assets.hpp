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
    std::string get_block_texture(block_type type);
    class player_texture
    {
    public:
        std::string run_left[6];
        std::string run_right[6];
        std::string jump[6];
    };
    extern player_texture player_textures;
    int load_player_textures(std::string config_path);
    std::string get_player_texture(std::string type, int index);
}
