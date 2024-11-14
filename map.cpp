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
    this->config.name = new std::string(map_name->valuestring);
    cJSON *map_savepath = cJSON_GetObjectItem(root, "savepath");
    if (map_savepath == NULL)
    {
        glog::log("error", "Failed to get map savepath", "map");
        return -1;
    }
    glog::log("info", "Map Savepath: " + std::string(map_savepath->valuestring), "map");
    this->config.savepath = new std::string(map_savepath->valuestring);
    cJSON_Delete(root);
    delete[] data;
    // load chunks
    this->chunks = new class chunk[CHUNKS_PER_MAP_X];
    memset(this->chunks, 0, CHUNKS_PER_MAP_X * sizeof(class chunk));
    for (int i = 0; i < CHUNKS_PER_MAP_X; i++)
    {
        this->chunks[i].x = i;
        this->chunks[i].load(*this->config.savepath);
        glog::log("info", "Loaded Chunk: " + std::to_string(i), "map");
    }
    return 0;
}

void map::update()
{
}
int map::save()
{

    // save chunks
    for (int i = 0; i < CHUNKS_PER_MAP_X; i++)
    {
        this->chunks[i].save(*this->config.savepath);
        glog::log("info", "Saved Chunk: " + std::to_string(i), "map");
    }
    return 0;
}
int block::save(std::vector<class data> *data)
{
    int data_size = sizeof(*this);
    data_size += this->data_size;
    class data d;
    d.size = data_size;
    d.data = new char[data_size];
    memcpy(d.data, this, sizeof(*this));
    memcpy((void *)((char *)d.data + sizeof(*this)), this->data, this->data_size);
    data->push_back(d);
    return 0;
}

int chunk::save(std::string name)
{
    std::fstream file;
    name.append("chunk" + std::to_string(this->x));
    name.append(".chunk");
    file.open(name, std::ios::out);
    if (!file.is_open())
    {
        glog::log("error", "Failed to open file: " + name, "chunk");
        return -1;
    }
    file.write((char *)this, sizeof(*this));
    std::vector<class data> data;
    for (int i = 0; i < this->block_count; i++)
    {
        this->blocks[i].save(&data);
    }
    for (int i = 0; i < data.size(); i++)
    {
        file.write((char *)data[i].data, data[i].size);
        delete[] data[i].data;
    }
    file.close();
    return 0;
}
int chunk::load(std::string name)
{
    std::fstream file;
    name.append("chunk" + std::to_string(this->x));
    name.append(".chunk");
    file.open(name, std::ios::in);
    if (!file.is_open())
    {
        glog::log("error", "Failed to open file: " + name, "chunk");
        this->init(name);
        return -1;
    }
    long long size = file.tellg();
    file.seekg(0, std::ios::end);
    size = file.tellg() - size;
    file.seekg(0, std::ios::beg);
    char *data = new char[size];
    file.read(data, size);
    file.close();
    memcpy(this, data, sizeof(*this));
    int offset = sizeof(*this);
    for (int i = 0; i < this->block_count; i++)
    {
        this->blocks = new block[this->block_count];
        memcpy(&this->blocks[i], (void *)((char *)data + offset), sizeof(this->blocks[i]));
        offset += sizeof(this->blocks[i]);
        this->blocks[i].data = new char[this->blocks[i].data_size];
        memcpy(this->blocks[i].data, (void *)((char *)data + offset), this->blocks[i].data_size);
        offset += this->blocks[i].data_size;
    }
    delete[] data;
    return 0;
}
int chunk::init(std::string name)
{
    this->x = 0;
    this->type = chunk_type::CHUNK_DESERT;
    this->block_count = BLOCKS_PER_CHUNK_X * BLOCKS_PER_CHUNK_Y;
    this->blocks = new class block[this->block_count];
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