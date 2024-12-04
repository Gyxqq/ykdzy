#pragma once
#include "map.hpp"
#include "log.hpp"
#include <cJSON.h>
namespace assets
{
    enum const_texture_type
    {
        CONST_TEXTURE_HEART,
        CONST_TEXTURE_BACKPACK,
        CONST_TEXTURE_HUNGER,
        CONST_TEXTURE_WIDGETS,
        CONST_TEXTURE_MAX_INDEX
    };
    class block_texture
    {
    public:
        block_type type;
        std::string texture;
    };
    class item_texture
    {
    public:
        item_type type;
        std::string texture;
    };
    extern item_texture item_textures[ITEM_MAX_INDEX];
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
    class const_texture
    {
    public:
        const_texture_type type;
        std::string texture;
    };
    extern player_texture player_textures;
    int load_player_textures(std::string config_path);
    std::string get_player_texture(std::string type, int index);
    int load_const_textures(std::string pre_path);
    extern const_texture const_textures[CONST_TEXTURE_MAX_INDEX];
    int load_item_textures(std::string config_path);
}
