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
}
int block::save(std::vector<std::vector<char>> *data)
{
    std::vector<char> block_data;
    for (int i = 0; i < sizeof(this->type); i++)
    {
        block_data.push_back(((char *)&this->type)[i]);
    }
    for (int i = 0; i < sizeof(this->x); i++)
    {
        block_data.push_back(((char *)&this->x)[i]);
    }
    for (int i = 0; i < sizeof(this->y); i++)
    {
        block_data.push_back(((char *)&this->y)[i]);
    }
    for (int i = 0; i < this->data_size; i++)
    {
        block_data.push_back(((char *)this->data)[i]);
    }
    data->push_back(block_data);
    return 0;
}

int chunk::save(std::string name)
{
    std::fstream file;
    file.open(name, std::ios::out);
    if (!file.is_open())
    {
        glog::log("error", "Failed to open file: " + name, "chunk");
        return -1;
    }

    file.write((char *)&this->type, sizeof(this->type));
    file.write((char *)&this->x, sizeof(this->x));
    file.write((char *)&this->block_count, sizeof(this->block_count));
    std::vector<std::vector<char>> data;
    for (int i = 0; i < this->block_count; i++)
    {
        this->blocks[i].save(&data);
    }
    for (int i = 0; i < data.size(); i++)
    {
        file.write(data[i].data(), data[i].size());
    }
    file.close();
    return 0;
}
block::~block()
{
    delete[] this->data;
}
chunk::~chunk()
{
    delete[] this->blocks;
}