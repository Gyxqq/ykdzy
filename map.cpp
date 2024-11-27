#pragma once
#include "map.hpp"
#include "log.hpp"
#include <fstream>
#include <cJSON.h>
#include <sys/stat.h>
#include <chrono>
#include <stack>
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
            this->chunks[i].load(*this->config.savepath, this->seed);
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
    glog::log("info", "loading Chunk: " + std::to_string(load_pos), "map chunk loader");
    this->chunks[unload_pos].x = load_pos;
    this->chunks[unload_pos].load(*this->config.savepath, this->seed);
    return 0;
}
int map::load_chunk_pos_no_save(std::string name, int unload_pos, int load_pos) // 将区块按照位置加载到内存
{
    // this->chunks[unload_pos].save(*this->config.savepath);
    glog::log("info", "loading Chunk: " + std::to_string(load_pos), "map chunk loader");
    this->chunks[unload_pos].x = load_pos;
    this->chunks[unload_pos].load(*this->config.savepath, this->seed);
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
        glog::log("info", "Saved Chunk: " + std::to_string(this->chunks[i].x), "map");
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
        this->chunks[i].type = this->cauculate_chunk_type(i, this->seed);
        this->chunks[i].init(*this->config.savepath, this->seed);
        glog::log("info", "Initialized Chunk: " + std::to_string(i), "map");
    }
    return 0;
}
block_type map::get_block(int x, int y)
{
    int chunk_x = x / BLOCKS_PER_CHUNK_X;
    if (x < 0)
    {
        chunk_x--;
    }

    int in_chunk_x = x % BLOCKS_PER_CHUNK_X;
    if (chunk_x < 0)
    {
        in_chunk_x = BLOCKS_PER_CHUNK_X + in_chunk_x;
    }
    if (y < 0 || y >= BLOCKS_PER_CHUNK_Y - 1)
    {
        return block_type::BLOCK_AIR;
    }
    for (int i = 0; i < CHUNKS_PER_MAP_X; i++)
    {
        if (this->chunks[i].x == chunk_x)
        {
            return this->chunks[i].blocks[in_chunk_x + y * BLOCKS_PER_CHUNK_X].type;
        }
    }
    glog::log("error", "Chunk not found: " + std::to_string(chunk_x), "map");
    return block_type::BLOCK_AIR;
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
        fclose(file);
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
    // char *data = new char[this->block_count*2];
    // memset(data, 0, this->block_count*2);
    // int offset = 0;
    // for (int i = 0; i < this->block_count; i++)
    // {
    //     if (i%BLOCKS_PER_CHUNK_X == 0)
    //     {
    //         data[offset] = '\n';
    //         offset++;
    //     }
    //     else if (this->blocks[i].type==block_type::BLOCK_AIR)
    //     {
    //         data[offset] = ' ';
    //         offset++;
    //     }
    //     else
    //     {
    //         data[offset] = '0'+this->blocks[i].type;
    //         offset++;
    //     }

    // }
    // glog::log("info", "Chunk Data: " + std::string(data), "chunk");
    return 0;
}
int chunk::load(std::string name, int seed)
{
    FILE *file;
    name.append("chunk" + std::to_string(this->x));
    name.append(".chunk");
    file = fopen(name.c_str(), "rb");
    if (!file)
    {
        glog::log("error", "Failed to open file: " + name, "chunk");
        this->init(name, seed);
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
        this->init(name, seed);
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
        this->init(name, seed);
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
int chunk::init(std::string name, int seed)
{
    // this->x = 0;
    // this->type = chunk_type::CHUNK_DESERT;
    this->block_count = BLOCKS_PER_CHUNK_X * BLOCKS_PER_CHUNK_Y;
    this->blocks = new class block[this->block_count];
    memset(this->blocks, 0, this->block_count * sizeof(class block));
    int level = 32 + rand() % 6;
    block_type top_block = block_type::BLOCK_GRASS;
    block_type middle_block = block_type::BLOCK_DIRT;
    if (this->type == chunk_type::CHUNK_DESERT)
    {
        top_block = block_type::BLOCK_SAND;
        middle_block = block_type::BLOCK_SAND;
    }
    else if (this->type == chunk_type::CHUNK_FOREST)
    {
        top_block = block_type::BLOCK_GRASS;
        middle_block = block_type::BLOCK_DIRT;
    }
    else if (this->type == chunk_type::CHUNK_MOUNTAIN)
    {
        top_block = block_type::BLOCK_STONE;
        middle_block = block_type::BLOCK_STONE;
    }
    // else if (this->type == chunk_type::CHUNK_OCEAN)
    // {
    //     top_block = block_type::BLOCK_WATER;
    //     middle_block = block_type::BLOCK_WATER;
    // }
    // else if (this->type == chunk_type::CHUNK_LAVA)
    // {
    //     top_block = block_type::BLOCK_LAVA;
    //     middle_block = block_type::BLOCK_LAVA;
    // }
    else if (this->type == chunk_type::CHUNK_VOID)
    {
        top_block = block_type::BLOCK_VOID;
        middle_block = block_type::BLOCK_VOID;
    }
    else if (this->type == chunk_type::CHUNK_SNOW)
    {
        top_block = block_type::BLOCK_SNOW;
        middle_block = block_type::BLOCK_DIRT;
    }
    else if (this->type == chunk_type::CHUNK_GRASSLAND)
    {
        top_block = block_type::BLOCK_GRASS;
        middle_block = block_type::BLOCK_DIRT;
    }

    for (int i = 0; i < this->block_count; i++)
    {
        this->blocks[i].x = i % BLOCKS_PER_CHUNK_X;
        this->blocks[i].y = i / BLOCKS_PER_CHUNK_X;
        if (this->blocks[i].y <= level)
        {
            this->blocks[i].type = middle_block;
        }
        else
        {
            this->blocks[i].type = block_type::BLOCK_AIR;
        }
    } // 生成地形
    for (int i = 0; i < this->block_count; i++) // 生成随机山峰
    {
        if (this->blocks[i].y == level)
        {
            if (rand() % 100 < 80)
            {
                int count = 0;
                std::stack<class block> stack;
                stack.push(this->blocks[i]);
                count++;
                while (!stack.empty())
                {
                    if (count > 100)
                    {
                        break;
                    }
                    class block current = stack.top();
                    stack.pop();
                    if (rand() % 100 < 90)
                    {
                        switch (rand() % 4)
                        {
                        case 0:
                            if (current.x + 1 < BLOCKS_PER_CHUNK_X && this->blocks[current.x + 1 + current.y * BLOCKS_PER_CHUNK_X].type == block_type::BLOCK_AIR)
                            {
                                this->blocks[current.x + 1 + current.y * BLOCKS_PER_CHUNK_X].type = middle_block;
                                stack.push(this->blocks[current.x + 1 + current.y * BLOCKS_PER_CHUNK_X]);
                                count++;
                            }
                            break;
                        case 1:
                            if (current.x - 1 >= 0 && this->blocks[current.x - 1 + current.y * BLOCKS_PER_CHUNK_X].type == block_type::BLOCK_AIR)
                            {
                                this->blocks[current.x - 1 + current.y * BLOCKS_PER_CHUNK_X].type = middle_block;
                                stack.push(this->blocks[current.x - 1 + current.y * BLOCKS_PER_CHUNK_X]);
                                count++;
                            }
                            break;
                        case 2:
                            if (current.y + 1 < BLOCKS_PER_CHUNK_Y && this->blocks[current.x + (current.y + 1) * BLOCKS_PER_CHUNK_X].type == block_type::BLOCK_AIR)
                            {
                                this->blocks[current.x + (current.y + 1) * BLOCKS_PER_CHUNK_X].type = middle_block;
                                stack.push(this->blocks[current.x + (current.y + 1) * BLOCKS_PER_CHUNK_X]);
                                count++;
                            }
                            break;
                        case 3:
                            if (current.y - 1 >= 0 && this->blocks[current.x + (current.y - 1) * BLOCKS_PER_CHUNK_X].type == block_type::BLOCK_AIR)
                            {
                                this->blocks[current.x + (current.y - 1) * BLOCKS_PER_CHUNK_X].type = middle_block;
                                stack.push(this->blocks[current.x + (current.y - 1) * BLOCKS_PER_CHUNK_X]);
                                count++;
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
    for (int i = 0; i < BLOCKS_PER_CHUNK_X; i++) // 生成顶部
    {
        for (int j = BLOCKS_PER_CHUNK_Y - 1; j >= 0; j--)
        {
            if (this->blocks[i + j * BLOCKS_PER_CHUNK_X].type != block_type::BLOCK_AIR)
            {
                this->blocks[i + j * BLOCKS_PER_CHUNK_X].type = top_block;
                break;
            }
        }
    }
    // 生成随机矿物
    for (int i = 0; i < this->block_count; i++)
    {
        if (this->blocks[i].y > 25)
            continue;
        if (rand() % 100 < 10)
        {

            switch (rand() % 4)
            {
            case 0:
                if (this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type == middle_block)
                {
                    this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type = block_type::BLOCK_COAL;
                }
                break;
            case 1:
                if (this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type == middle_block)
                {
                    this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type = block_type::BLOCK_IRON;
                }
                break;
            case 2:
                if (this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type == middle_block)
                {
                    this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type = block_type::BLOCK_GOLD;
                }
                break;
            case 3:
                if (this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type == middle_block)
                {
                    this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type = block_type::BLOCK_DIAMOND;
                }
                break;
            case 4:
                if (this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type == middle_block)
                {
                    this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type = block_type::BLOCK_EMERALD;
                }
                break;
            case 5:
                if (this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type == middle_block)
                {
                    this->blocks[this->blocks[i].x + this->blocks[i].y * BLOCKS_PER_CHUNK_X].type = block_type::BLOCK_COPPER;
                }
                break;
            default:
                break;
            }
        }
    }
    this->block_count = BLOCKS_PER_CHUNK_X * BLOCKS_PER_CHUNK_Y;
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
chunk_type map::cauculate_chunk_type(int x, int seed)
{
    x = x / 4;
    int y = abs(((x + seed) * 54 / 2 - 42233) * 7 / 3);
    return (chunk_type)(y % (chunk_type::CHUNK_TYPE_END));
}