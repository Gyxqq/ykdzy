#pragma once
#include "map.hpp"
#include "log.hpp"
#include <fstream>
#include <cJSON.h>
int map::load(std::string name)
{
    std::fstream file;
    file.open(name, std::ios::in);
    if (!file.is_open())
    {
        glog::log("error", "Failed to open file: " + name, "map");
        return -1;
    }
    long long size = file.tellg();
    file.seekg(0, std::ios::end);
    size = file.tellg() - size;
    file.seekg(0, std::ios::beg);
    char *data = new char[size];
    file.read(data, size);
    file.close();
    cJSON *root = cJSON_Parse(data);
    if (root == NULL)
    {
        glog::log("error", "Failed to parse JSON Root", "map");
        return -1;
    }
    cJSON *map_name = cJSON_GetObjectItem(root, "name");
    if (map_name == NULL)
    {
        glog::log("error", "Failed to get map name", "map");
        return -1;
    }
    glog::log("info", "Map Name: " + std::string(map_name->valuestring), "map");

    return 0;
}

void map::update()
{
    // Update map
    // ...
    // Update chunks
    // ...
    // Update players
    // ...
}