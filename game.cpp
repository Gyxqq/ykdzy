#pragma once
#include "map.hpp"
#include "log.hpp"
#include "game.hpp"
#include "cJSON.h"
#include <time.h>
#include <sys/stat.h>
int game::init(std::string name)
{
    int seed = time(NULL);
    srand(seed);
    glog::log("info", "Initializing Game: " + name, "game");
    this->savepath = name;
    this->world.seed = seed;
    this->world.init(name);
    this->players.push_back(player());
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "savepath", this->savepath.c_str());
    glog::log("info", "Seed: " + std::to_string(seed), "game");
    cJSON_AddNumberToObject(root, "seed", seed);
    cJSON *players = cJSON_CreateArray();
    std::string player_savepath = "00000.player";
    // this->players[0].save(this->savepath);
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
    glog::log("info", "Loading Game: " + name, "game loader");
    // this->world.load(name);
    FILE *file = fopen(name.c_str(), "r");
    if (!file)
    {
        glog::log("error", "Failed to open file: " + name, "game loader");
        return -1;
    }
    long long size = 0;
    struct stat results;
    if (stat(name.c_str(), &results) == 0)
    {
        size = results.st_size;
        glog::log("info", "File size: " + std::to_string(size), "game loader");
    }
    else
    {
        glog::log("error", "Failed to get file size: " + name, "game loader");
        fclose(file);
        return -1;
    }
    char *data = new char[size];
    memset(data, 0, size);
    size_t read_size = fread(data, 1, size, file);

    fclose(file);
    cJSON *root = cJSON_Parse(data);
    if (root == NULL)
    {
        glog::log("error", "Failed to parse JSON Root", "game loader");
        return -1;
    }
    cJSON *savepath = cJSON_GetObjectItem(root, "savepath");
    if (savepath == NULL)
    {
        glog::log("error", "Failed to get savepath", "game loader");
        return -1;
    }
    cJSON *seed = cJSON_GetObjectItem(root, "seed");
    if (seed == NULL)
    {
        glog::log("error", "Failed to get seed", "game loader");
        return -1;
    }
    this->world.seed = seed->valueint;
    this->savepath = savepath->valuestring;
    glog::log("info", "get Savepath: " + this->savepath, "game loader");
    this->world.load(this->savepath);
    cJSON *players = cJSON_GetObjectItem(root, "players");
    if (players == NULL)
    {
        glog::log("error", "Failed to get players", "game loader");
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
    int center_x = this->players[0].x / BLOCKS_PER_CHUNK_X;
    for (int i = 0; i < CHUNKS_PER_MAP_X; i++)
    {
        this->world.load_chunk_pos_no_save(this->savepath, i, center_x - CHUNKS_PER_MAP_X / 2 + i);
        glog::log("info", "Loading Chunk: " + std::to_string(center_x - CHUNKS_PER_MAP_X / 2 + i), "game loader");
    }
    return 0;
}
int game::save()
{
    glog::log("info", "Saving Game", "game");
    this->world.save();
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "savepath", this->savepath.c_str());
    cJSON_AddNumberToObject(root, "seed", this->world.seed);
    cJSON *players = cJSON_CreateArray();
    for (int i = 0; i < this->players.size(); i++)
    {
        std::string player_savepath = this->players[i].name + ".player";
        this->players[i].save(this->savepath);
        glog::log("info", "Saving Player: " + player_savepath, "game save");
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
    // int size = this->players.size();
    // for (int i = 0; i < size; i++)
    // {
    //     glog::log("info", "Saving Player: " + this->players[i].name, "game");
    //     this->players[i].save(this->savepath);
    // }

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
    memset(this->items, 0, sizeof(class item) * MAX_ITEMS);
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