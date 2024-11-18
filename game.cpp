#pragma once
#include "map.hpp"
#include "log.hpp"
#include "game.hpp"
#include "cJSON.h"
#include <sys/stat.h>
int game::init(std::string name)
{
    glog::log("info", "Initializing Game: " + name, "game");
    this->world.init(name);
    this->players.push_back(player());
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "savepath", this->savepath.c_str());
    cJSON *players = cJSON_CreateArray();
    std::string player_savepath = "00000.player";
    this->players[0].save(this->savepath + player_savepath);
    cJSON_AddItemToArray(players, cJSON_CreateString(player_savepath.c_str()));
    cJSON_AddItemToObject(root, "players", players);
    char *data = cJSON_Print(root);
    FILE *file = fopen((name + "game.json").c_str(), "w");
    if (!file)
    {
        glog::log("error", "Failed to open file: " + name + "game.json", "game");
        return -1;
    }
    fwrite(data, 1, strlen(data), file);
    fclose(file);
    cJSON_Delete(root);
    delete[] data;
    this->players[0].init("00000");
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
    int i = 0;
    while (player != NULL)
    {
        this->players.push_back(class player());
        this->players[i].load(this->savepath + player->valuestring);
        i++;
        player = player->next;
    }
    cJSON_Delete(root);
    delete[] data;

    return 0;
}
int game::save()
{
    glog::log("info", "Saving Game", "game");
    this->world.save();
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "savepath", this->savepath.c_str());
    cJSON *players = cJSON_CreateArray();
    for (int i = 0; i < this->players.size(); i++)
    {
        std::string player_savepath = std::to_string(i) + ".player";
        this->players[i].save(this->savepath + player_savepath);
        cJSON_AddItemToArray(players, cJSON_CreateString(player_savepath.c_str()));
    }

    cJSON_AddItemToObject(root, "players", players);
    char *data = cJSON_Print(root);
    FILE *file = fopen((this->savepath + "game.json").c_str(), "w");
    if (!file)
    {
        glog::log("error", "Failed to open file: " + this->savepath + "game.json", "game");
        return -1;
    }
    fwrite(data, 1, strlen(data), file);
    fclose(file);
    cJSON_Delete(root);
    delete[] data;
    for (int i = 0; i < this->players.size(); i++)
    {
        this->players[i].save(this->savepath);
    }

    return 0;
}

int player::init(std::string name)
{
    glog::log("info", "Initializing Player: " + name, "player");
    this->name = name;
    this->x = 0;
    this->y = 0;
    this->health = 100;
    this->hunger = 100;
    memset(this->items, 0, sizeof(this->items) * MAX_ITEMS);
    return 0;
}

int player::save(std::string name)
{
    name.append(this->name + ".player");
    FILE *file = fopen(name.c_str(), "wb");
    if (!file)
    {
        glog::log("error", "Failed to open file: " + name, "player");
        return -1;
    }
    fwrite(this, sizeof(*this), 1, file);
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (this->items[i].data != NULL)
        {
            fwrite(this->items[i].data, this->items[i].count, this->items[i].stack_count, file);
        }
    }
    fflush(file);
    fclose(file);
    return 0;
}
int player::load(std::string name)
{
    FILE *file = fopen(name.c_str(), "rb");
    if (!file)
    {
        glog::log("error", "Failed to open file: " + name, "player");
        return -1;
    }
    fread(this, sizeof(*this), 1, file);
    for (int i = 0; i < MAX_ITEMS; i++)
    {
        if (this->items[i].data != NULL)
        {
            this->items[i].data = malloc(this->items[i].count * this->items[i].stack_count);
            fread(this->items[i].data, this->items[i].count, this->items[i].stack_count, file);
        }
    }
    fclose(file);
    return 0;
}