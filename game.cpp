#pragma once
#include "map.hpp"
#include "log.hpp"
#include "game.hpp"
#include "cJSON.h"
#include <sys/stat.h>
int game::init(std::string name)
{
    glog::log("info", "Initializing Game: " + name, "game");
    this->world.load(name);
    this->players.push_back(player());
    return 0;
}
int game::load(std::string name)
{
    glog::log("info", "Loading Game: " + name, "game");
    // this->world.load(name);
    FILE *file = fopen(name.c_str(), "r");
    if (!file)
    {
        glog::log("error", "Failed to open file: " + name, "game");
        return -1;
    }
    long long size = 0;
    struct stat results;
    if (stat(name.c_str(), &results) == 0)
    {
        size = results.st_size;
        glog::log("info", "File size: " + std::to_string(size), "game");
    }
    else
    {
        glog::log("error", "Failed to get file size: " + name, "game");
        fclose(file);
        return -1;
    }
    char *data = new char[size];
    memset(data, 0, size);
    size_t read_size = fread(data, 1, size, file);
    if (read_size != size)
    {
        glog::log("error", "read size: " + std::to_string(read_size) + " != " + std::to_string(size), "game");
        delete[] data;
        fclose(file);
        return -1;
    }
    fclose(file);
    cJSON *root = cJSON_Parse(data);
    if (root == NULL)
    {
        glog::log("error", "Failed to parse JSON Root", "game");
        return -1;
    }
    cJSON *savepath = cJSON_GetObjectItem(root, "savepath");
    if (savepath == NULL)
    {
        glog::log("error", "Failed to get savepath", "game");
        return -1;
    }
    this->savepath = savepath->valuestring;
    glog::log("info", "Savepath: " + this->savepath, "game");

    cJSON *players = cJSON_GetObjectItem(root, "players");
    if (players == NULL)
    {
        glog::log("error", "Failed to get players", "game");
        return -1;
    }
    cJSON *player = players->child;
    while (player != NULL)
    {
        }

    return 0;
}
int game::save()
{
    glog::log("info", "Saving Game", "game");
    this->world.save();
    return 0;
}