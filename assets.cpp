#pragma once
#include "assets.hpp"
namespace assets
{
    block_texture block_textures[block_type::BLOCK_MAX_INDEX];
    player_texture player_textures;
    int assets::load_block_textures(std::string config_path)
    {
        glog::log("info", "Loading Block Textures: " + config_path, "assets");
        FILE *file;
        file = fopen(config_path.c_str(), "rb");
        if (!file)
        {
            glog::log("error", "Failed to open file: " + config_path, "assets");
            return -1;
        }

        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *data = new char[size];
        memset(data, 0, size);
        size_t read_size = fread(data, 1, size, file);
        if (read_size != size)
        {
            glog::log("error", "read size: " + std::to_string(read_size) + " != " + std::to_string(size), "assets");
            delete[] data;
            fclose(file);
            return -1;
        }
        fclose(file);
        cJSON *root = cJSON_Parse(data);
        if (root == NULL)
        {
            glog::log("error", "Failed to parse json: " + std::string(cJSON_GetErrorPtr()), "assets");
            delete[] data;
            return -1;
        }

        cJSON *path = cJSON_GetObjectItem(root, "path");
        std::string texture_path(path->valuestring);
        glog::log("info", "Texture Path: " + texture_path, "assets");
        cJSON *textures = cJSON_GetObjectItem(root, "blocks");
        cJSON *default_texture = cJSON_GetObjectItem(root, "default_texture");
        for (int i = 0; i < block_type::BLOCK_MAX_INDEX; i++)
        {
            block_textures[i].type = (block_type)i;
            block_textures[i].texture = texture_path + default_texture->valuestring;
        }
        for (int i = 0; i < cJSON_GetArraySize(textures); i++)
        {
            cJSON *item = cJSON_GetArrayItem(textures, i);
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
        FILE *file;
        file = fopen(config_path.c_str(), "rb");
        if (!file)
        {
            glog::log("error", "Failed to open file: " + config_path, "assets");
            return -1;
        }

        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);
        char *data = new char[size];
        memset(data, 0, size);
        size_t read_size = fread(data, 1, size, file);
        if (read_size != size)
        {
            glog::log("error", "read size: " + std::to_string(read_size) + " != " + std::to_string(size), "assets");
            delete[] data;
            fclose(file);
            return -1;
        }
        fclose(file);
        cJSON *root = cJSON_Parse(data);
        if (root == NULL)
        {
            glog::log("error", "Failed to parse json: " + std::string(cJSON_GetErrorPtr()), "assets");
            delete[] data;
            return -1;
        }
        std::string texture_path = cJSON_GetObjectItem(root, "path")->valuestring;
        cJSON *path = cJSON_GetObjectItem(root, "run_left");
        for (int i = 0; i < 6; i++)
        {
            player_textures.run_left[i] = texture_path + cJSON_GetArrayItem(path, i)->valuestring;
        }
        path = cJSON_GetObjectItem(root, "run_right");
        for (int i = 0; i < 6; i++)
        {
            player_textures.run_right[i] = texture_path + cJSON_GetArrayItem(path, i)->valuestring;
        }
        path = cJSON_GetObjectItem(root, "jump");
        for (int i = 0; i < 6; i++)
        {
            player_textures.jump[i] = texture_path + cJSON_GetArrayItem(path, i)->valuestring;
        }
        cJSON_Delete(root);
        glog::log("info", "Loaded Player Textures", "assets");
        return 0;
    }
    std::string assets::get_player_texture(std::string type, int index)
    {
        if (type == "run_left")
        {
            return player_textures.run_left[index];
        }
        else if (type == "run_right")
        {
            return player_textures.run_right[index];
        }
        else if (type == "jump")
        {
            return player_textures.jump[index];
        }
        return "";
    }

} // namespace assets