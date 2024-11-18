#pragma once
#include "map.hpp"
#include "log.hpp"
#include <fstream>
#include <cJSON.h>
#include <sys/stat.h>
#include <chrono>
int map::load(std::string name)
{
    glog::log("info", "Loading Map: " + name, "map");
    this->config.savepath = new std::string();
    *(this->config.savepath) = name;
    this->chunks = new chunk[CHUNKS_PER_MAP_X];
    this->chunk_count = CHUNKS_PER_MAP_X;
    memset(this->chunks, 0, CHUNKS_PER_MAP_X * sizeof(chunk));
    return 0;
}
int map::load_chunk(std::string name, int unload_index, int load_index) // 将区块按照索引加载到内存
{
    for (int i = 0; i < CHUNKS_PER_MAP_X; i++)
    {
        if (this->chunks[i].x == load_index)
        {
            glog::log("info", "Chunk: " + std::to_string(load_index) + " already loaded", "map");
            return CHUNK_ALREDY_LOADED;
        }
    }
    for (int i = 0; i < CHUNKS_PER_MAP_X; i++)
    {
        if (this->chunks[i].x == unload_index)
        {
            this->chunks[i].save(*this->config.savepath);
            glog::log("info", "Saved Chunk: " + std::to_string(unload_index), "map");
            this->chunks[i].x = load_index;
            this->chunks[i].load(*this->config.savepath);
            glog::log("info", "Loaded Chunk: " + std::to_string(load_index), "map");
            return 0;
        }
    }
    glog::log("error", "chunk not found: " + std::to_string(unload_index), "map");
    return CHUNK_NOT_LOADED;
}

int map::load_chunk_pos(std::string name, int unload_pos, int load_pos) // 将区块按照位置加载到内存
{
    this->chunks[unload_pos].save(*this->config.savepath);
    glog::log("info", "Saved Chunk: " + std::to_string(unload_pos), "map");
    this->chunks[unload_pos].x = load_pos;
    this->chunks[unload_pos].load(*this->config.savepath);
    return 0;
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
int map::init(std::string name)
{
    glog::log("info", "Initializing Map: " + name, "map");
    this->config.savepath = new std::string();
    this->config.name = new std::string();
    *this->config.savepath = name;
    *this->config.name = name;
    this->chunks = new chunk[CHUNKS_PER_MAP_X];
    this->chunk_count = CHUNKS_PER_MAP_X;
    for (int i = 0; i < CHUNKS_PER_MAP_X; i++)
    {
        this->chunks[i].x = i;
        this->chunks[i].init(*this->config.savepath);
        glog::log("info", "Initialized Chunk: " + std::to_string(i), "map");
    }
    return 0;
}

int chunk::save(std::string name)
{
    auto start = std::chrono::high_resolution_clock::now();
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
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    glog::log("info", "Save Time: " + std::to_string(elapsed.count()), "chunk");
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
        this->init(name);
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
        // fclose(file);
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
            // fclose(file);
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
        if (this->blocks[i].data != NULL)
            delete[] this->blocks[i].data;
    }
    if (this->blocks != NULL)
        delete[] this->blocks;
}
map::~map()
{
    if (this->chunks != NULL)
        delete[] this->chunks;
    delete this->config.savepath;
    delete this->config.name;
}