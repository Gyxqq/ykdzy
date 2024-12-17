#pragma once
#include "assets.hpp"

namespace assets {
block_texture block_textures[block_type::BLOCK_MAX_INDEX];
player_texture player_textures;
const_texture const_textures[const_texture_type::CONST_TEXTURE_MAX_INDEX];
item_texture item_textures[item_type::ITEM_MAX_INDEX];
entity_texture entity_textures[entity::entity_type::ENTITY_TYPE_MAX];
//entity_texture entity_textures[entity::entity_type::ENTITY_TYPE_MAX];
int assets::load_block_textures(std::string config_path)
{
    glog::log("info", "Loading Block Textures: " + config_path, "assets");
    FILE* file;
    file = fopen(config_path.c_str(), "rb");
    if (!file) {
        glog::log("error", "Failed to open file: " + config_path, "assets");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = new char[size];
    memset(data, 0, size);
    size_t read_size = fread(data, 1, size, file);
    if (read_size != size) {
        glog::log("error", "read size: " + std::to_string(read_size) + " != " + std::to_string(size), "assets");
        delete[] data;
        fclose(file);
        return -1;
    }
    fclose(file);
    cJSON* root = cJSON_Parse(data);
    if (root == NULL) {
        glog::log("error", "Failed to parse json: " + std::string(cJSON_GetErrorPtr()), "assets");
        delete[] data;
        return -1;
    }

    cJSON* path = cJSON_GetObjectItem(root, "path");
    std::string texture_path(path->valuestring);
    glog::log("info", "Texture Path: " + texture_path, "assets");
    cJSON* textures = cJSON_GetObjectItem(root, "blocks");
    cJSON* default_texture = cJSON_GetObjectItem(root, "default_texture");
    for (int i = 0; i < block_type::BLOCK_MAX_INDEX; i++) {
        block_textures[i].type = (block_type)i;
        block_textures[i].texture = texture_path + default_texture->valuestring;
    }
    for (int i = 0; i < cJSON_GetArraySize(textures); i++) {
        cJSON* item = cJSON_GetArrayItem(textures, i);
        int type = cJSON_GetObjectItem(item, "id")->valueint;
        block_textures[type].type = (block_type)type;
        block_textures[type].texture = texture_path + cJSON_GetObjectItem(item, "texture")->valuestring;
        std::string name = cJSON_GetObjectItem(item, "name")->valuestring;
        glog::log("info", "Loaded Block Texture: " + name + " path: " + block_textures[type].texture, "assets");
    }
    cJSON_Delete(root);
    return 0;
}
std::string assets::get_block_texture(block_type type)
{
    return block_textures[type].texture;
}
int assets::load_player_textures(std::string config_path)
{
    glog::log("info", "Loading Player Textures: " + config_path, "assets");
    FILE* file;
    file = fopen(config_path.c_str(), "rb");
    if (!file) {
        glog::log("error", "Failed to open file: " + config_path, "assets");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = new char[size];
    memset(data, 0, size);
    size_t read_size = fread(data, 1, size, file);
    if (read_size != size) {
        glog::log("error", "read size: " + std::to_string(read_size) + " != " + std::to_string(size), "assets");
        delete[] data;
        fclose(file);
        return -1;
    }
    fclose(file);
    cJSON* root = cJSON_Parse(data);
    if (root == NULL) {
        glog::log("error", "Failed to parse json: " + std::string(cJSON_GetErrorPtr()), "assets");
        delete[] data;
        return -1;
    }
    std::string texture_path = cJSON_GetObjectItem(root, "path")->valuestring;
    cJSON* path = cJSON_GetObjectItem(root, "run_left");
    for (int i = 0; i < 6; i++) {
        player_textures.run_left[i] = texture_path + cJSON_GetArrayItem(path, i)->valuestring;
    }
    path = cJSON_GetObjectItem(root, "run_right");
    for (int i = 0; i < 6; i++) {
        player_textures.run_right[i] = texture_path + cJSON_GetArrayItem(path, i)->valuestring;
    }
    path = cJSON_GetObjectItem(root, "jump");
    for (int i = 0; i < 6; i++) {
        player_textures.jump[i] = texture_path + cJSON_GetArrayItem(path, i)->valuestring;
    }
    player_textures.stand = texture_path + cJSON_GetObjectItem(root, "stand")->valuestring;
    cJSON_Delete(root);
    glog::log("info", "Loaded Player Textures", "assets");
    return 0;
}
std::string assets::get_player_texture(std::string type, int index)
{
    if (type == "run_left") {
        return player_textures.run_left[index];
    } else if (type == "run_right") {
        return player_textures.run_right[index];
    } else if (type == "jump") {
        return player_textures.jump[index];
    } else if (type == "stand") {
        return player_textures.stand;
    }
    return "";
}
int assets::load_const_textures(std::string pre_path)
{
    glog::log("info", "Loading Const Textures: " + pre_path, "assets");
    const_textures[const_texture_type::CONST_TEXTURE_HEART].type = const_texture_type::CONST_TEXTURE_HEART;
    const_textures[const_texture_type::CONST_TEXTURE_HEART].texture = pre_path + "heart.png";
    const_textures[const_texture_type::CONST_TEXTURE_BACKPACK].type = const_texture_type::CONST_TEXTURE_BACKPACK;
    const_textures[const_texture_type::CONST_TEXTURE_BACKPACK].texture = pre_path + "inventory.png";
    const_textures[const_texture_type::CONST_TEXTURE_HUNGER].type = const_texture_type::CONST_TEXTURE_HUNGER;
    const_textures[const_texture_type::CONST_TEXTURE_HUNGER].texture = pre_path + "hunger.png";
    const_textures[const_texture_type::CONST_TEXTURE_WIDGETS].type = const_texture_type::CONST_TEXTURE_WIDGETS;
    const_textures[const_texture_type::CONST_TEXTURE_WIDGETS].texture = pre_path + "widgets.png";
    const_textures[const_texture_type::CONST_TEXTURE_START_BACKGROUND].type = const_texture_type::CONST_TEXTURE_START_BACKGROUND;
    const_textures[const_texture_type::CONST_TEXTURE_START_BACKGROUND].texture = pre_path + "start_background.png";
    const_textures[const_texture_type::CONST_TEXTURE_ARROW].type = const_texture_type::CONST_TEXTURE_ARROW;
    const_textures[const_texture_type::CONST_TEXTURE_ARROW].texture = pre_path + "arrow.png";
    return 0;
}
int assets::load_item_textures(std::string config_path)
{
    glog::log("info", "Loading Item Textures: " + config_path, "assets");
    FILE* file;
    file = fopen(config_path.c_str(), "rb");
    if (!file) {
        glog::log("error", "Failed to open file: " + config_path, "assets");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = new char[size];
    memset(data, 0, size);
    size_t read_size = fread(data, 1, size, file);
    if (read_size != size) {
        glog::log("error", "read size: " + std::to_string(read_size) + " != " + std::to_string(size), "assets");
        delete[] data;
        fclose(file);
        return -1;
    }
    fclose(file);
    cJSON* root = cJSON_Parse(data);
    if (root == NULL) {
        glog::log("error", "Failed to parse json: " + std::string(cJSON_GetErrorPtr()), "assets");
        delete[] data;
        return -1;
    }

    cJSON* path = cJSON_GetObjectItem(root, "path");
    std::string texture_path(path->valuestring);
    glog::log("info", "Texture Path: " + texture_path, "assets");
    cJSON* textures = cJSON_GetObjectItem(root, "items");
    cJSON* default_texture = cJSON_GetObjectItem(root, "default_texture");
    for (int i = 0; i < item_type::ITEM_MAX_INDEX; i++) {
        item_textures[i].type = (item_type)i;
        item_textures[i].texture = texture_path + default_texture->valuestring;
    }
    for (int i = 0; i < cJSON_GetArraySize(textures); i++) {
        cJSON* item = cJSON_GetArrayItem(textures, i);
        int type = cJSON_GetObjectItem(item, "id")->valueint;
        item_textures[type].type = (item_type)type;
        item_textures[type].texture = texture_path + cJSON_GetObjectItem(item, "texture")->valuestring;
        std::string name = cJSON_GetObjectItem(item, "name")->valuestring;
        glog ::log("info", "Loaded Item Texture: " + name + " path: " + item_textures[type].texture, "assets");
    }
}
int assets::load_entity_textures(std::string config_path)
{
    glog::log("info", "Loading Item Textures: " + config_path, "assets");
    FILE* file;
    file = fopen(config_path.c_str(), "rb");
    if (!file) {
        glog::log("error", "Failed to open file: " + config_path, "assets");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = new char[size];
    memset(data, 0, size);
    size_t read_size = fread(data, 1, size, file);
    if (read_size != size) {
        glog::log("error", "read size: " + std::to_string(read_size) + " != " + std::to_string(size), "assets");
        delete[] data;
        fclose(file);
        return -1;
    }
    fclose(file);
    cJSON* root = cJSON_Parse(data);
    if (root == NULL) {
        glog::log("error", "Failed to parse json: " + std::string(cJSON_GetErrorPtr()), "assets");
        delete[] data;
        return -1;
    }
    std::string run_left[6] = {
        "run_left_0.png",
        "run_left_1.png",
        "run_left_2.png",
        "run_left_3.png",
        "run_left_4.png",
        "run_left_5.png"
    };
    std::string run_right[6] = {
        "run_right_0.png",
        "run_right_1.png",
        "run_right_2.png",
        "run_right_3.png",
        "run_right_4.png",
        "run_right_5.png"
    };
    std::string jump[6] = {
        "jump_0.png",
        "jump_1.png",
        "jump_2.png",
        "jump_3.png",
        "jump_4.png",
        "jump_5.png"
    };
    cJSON* path = cJSON_GetObjectItem(root, "path");
    std::string texture_path(path->valuestring);
    glog::log("info", "Texture Path: " + texture_path, "assets");
    cJSON* textures = cJSON_GetObjectItem(root, "entities");
    auto default_sub = std::string(cJSON_GetObjectItem(root, "default")->valuestring);
    for (int i = 0; i < entity::entity_type::ENTITY_TYPE_MAX; i++) {
        for (int j = 0; j < 6; j++) {
            entity_textures[i].run_left[j] = texture_path + default_sub + run_left[j];
            entity_textures[i].run_right[j] = texture_path + default_sub + run_right[j];
            entity_textures[i].jump[j] = texture_path + default_sub + jump[j];
        }
        entity_textures[i].stand = texture_path + default_sub + "stand.png";
    }
    for (int i = 0; i < cJSON_GetArraySize(textures); i++) {
        cJSON* item = cJSON_GetArrayItem(textures, i);
        int type = cJSON_GetObjectItem(item, "id")->valueint;
        auto subpath = cJSON_GetObjectItem(item, "path")->valuestring;
        for (int j = 0; j < 6; j++) {
            entity_textures[type].run_left[j] = texture_path + subpath + run_left[j];
            entity_textures[type].run_right[j] = texture_path + subpath + run_right[j];
            entity_textures[type].jump[j] = texture_path + subpath + jump[j];
        }
        entity_textures[type].stand = texture_path + subpath + "stand.png";
        glog::log("info", "Loaded Entity Texture: " + std::to_string(type), "assets");
    }
}

} // namespace assets