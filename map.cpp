#pragma once
#include "map.hpp"
#include "log.hpp"
#include <fstream>
#include <cJSON.h>
#include <sys/stat.h>
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
    this->chunks = new chunk[CHUNKS_PER_MAP_X];
    this->chunk_count = CHUNKS_PER_MAP_X;
    for (int i = 0; i < CHUNKS_PER_MAP_X; i++)
    {
        this->chunks[i].x = i;
        this->chunks[i].load(*this->config.savepath);
        glog::log("info", "Loaded Chunk: " + std::to_string(i), "map");
    }
    this->chunk_count = CHUNKS_PER_MAP_X;
    return 0;
}
void load_chunk(std::string name, int index)
{
}

void map::update()
{
}
int map::save()
{

    for (int i = 0; i < CHUNKS_PER_MAP_X; i++)
    {
        this->chunks[i].save(*this->config.savepath);
        glog::log("info", "Saved Chunk: " + std::to_string(i), "map");
    }
    return 0;
}

int chunk::save(std::string name)
{
    FILE *file;
    name.append("chunk" + std::to_string(this->x));
    name.append(".chunk");
    fopen_s(&file, name.c_str(), "wb");
    if (!file)
    {
        glog::log("error", "Failed to open file: " + name, "chunk");
        return -1;
    }
    if (this->block_count != BLOCKS_PER_CHUNK_X * BLOCKS_PER_CHUNK_Y)
    {
        glog::log("error", "Invalid block count: " + std::to_string(this->block_count), "chunk");
        return -1;
    }
    fwrite(this, sizeof(*this), 1, file);
    for (int i = 0; i < this->block_count; i++)
    {
        fwrite(&this->blocks[i], sizeof(class block), 1, file);
        if (this->blocks[i].data_size > 0)
        {
            // glog::log("info", "data size: " + std::to_string(this->blocks[i].data_size), "chunk");
            fwrite(this->blocks[i].data, sizeof(char), this->blocks[i].data_size, file);
        }
    }
    fflush(file);
    fclose(file);
    glog::log("info", "Saved Chunk: " + std::to_string(this->x) + " to " + name, "chunk");
    return 0;
}
int chunk::load(std::string name)
{
    FILE *file;
    name.append("chunk" + std::to_string(this->x));
    name.append(".chunk");
    file = fopen(name.c_str(), "rb");
    if (!file)
    {
        glog::log("error", "Failed to open file: " + name, "chunk");
        this->init(name);
        return -1;
    }
    long long size = 0;
    struct stat results;
    if (stat(name.c_str(), &results) == 0)
    {
        size = results.st_size;
        glog::log("info", "File size: " + std::to_string(size), "chunk");
    }
    else
    {
        glog::log("error", "Failed to get file size: " + name, "chunk");
        fclose(file);
        return -1;
    }
    char *data = new char[size];
    memset(data, 0, size);
    size_t read_size = fread(data, 1, size, file);
    if (read_size != size)
    {
        glog::log("error", "read size: " + std::to_string(read_size) + " != " + std::to_string(size), "chunk");
        delete[] data;
        fclose(file);
        return -1;
    }
    fclose(file);
    memcpy(this, data, sizeof(*this));
    long long offset = sizeof(*this);
    if (this->block_count != BLOCKS_PER_CHUNK_X * BLOCKS_PER_CHUNK_Y)
    {
        glog::log("error", "load Invalid block count: " + std::to_string(this->block_count), "chunk");
        fclose(file);
        delete[] data;
        return -1;
    }
    this->blocks = new class block[this->block_count];
    memset(this->blocks, 0, this->block_count * sizeof(class block));
    for (int i = 0; i < this->block_count; i++)
    {
        if (offset + sizeof(class block) > size)
        {
            glog::log("error", "offset + sizeof(class block) > size", "chunk");
            delete[] data;
            fclose(file);
            return -1;
        }
        memcpy(&this->blocks[i], (void *)((char *)data + offset), sizeof(class block));
        offset += sizeof(class block);
        if (this->blocks[i].data_size == 0)
        {
            this->blocks[i].data = NULL;
        }
        else if (this->blocks[i].data_size > 0)
        {
            // glog::log("info", "data size: " + std::to_string(this->blocks[i].data_size), "chunk");
            this->blocks[i].data = new char[this->blocks[i].data_size];
            memcpy(this->blocks[i].data, (void *)((char *)data + offset), this->blocks[i].data_size);
            offset += this->blocks[i].data_size;
        }
        else
        {
            glog::log("error", "load Invalid data size: " + std::to_string(this->blocks[i].data_size), "chunk");
        }
    }
    delete[] data;
    return 0;
}
int chunk::init(std::string name)
{
    // this->x = 0;
    this->type = chunk_type::CHUNK_DESERT;
    this->block_count = BLOCKS_PER_CHUNK_X * BLOCKS_PER_CHUNK_Y;
    this->blocks = new class block[this->block_count];
    memset(this->blocks, 0, this->block_count * sizeof(class block));
    for (int i = 0; i < this->block_count; i++)
    {
        this->blocks[i].x = i % BLOCKS_PER_CHUNK_X;
        this->blocks[i].y = i / BLOCKS_PER_CHUNK_X;
        this->blocks[i].type = block_type::BLOCK_DIRT;
        this->blocks[i].data_size = 1024;
        this->blocks[i].data = new char[this->blocks[i].data_size];
    }
    glog::log("info", "Initialized Chunk: " + std::to_string(this->x), "chunk");
    return 0;
}

block::~block()
{
}
chunk::~chunk()
{
    for (int i = 0; i < this->block_count; i++)
    {
        delete[] this->blocks[i].data;
    }
    delete[] this->blocks;
}