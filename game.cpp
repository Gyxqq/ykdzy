#pragma once
#include "map.hpp"
#include "log.hpp"
#include "game.hpp"
#include <cJSON.h>
#include <time.h>
#include <sys/stat.h>
#include <graphics.h>
#define SPEED 0.1
extern std::mutex global_mutex;
extern int exit_flag;
int allow_fly = 1;

int get_block_x(float x)
{
    if (x < 0)
        return (int)x - 1;
    return (int)x;
}

BOOL IsKeyPressed(int vKey) { return (GetAsyncKeyState(vKey) & 0x8000) != 0; }
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
    for (int i = 0; i < BLOCKS_PER_CHUNK_Y; i++)
    {
        if (this->world.chunks[0].blocks[i * BLOCKS_PER_CHUNK_X].type == block_type::BLOCK_AIR)
        {
            this->players[0].y = i + 16;
            glog::log("info", "Player y: " + std::to_string(i), "game");
            break;
        }
    }
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
        glog::log("info", "Loading Player: " + std::string(player->valuestring), "game loader");
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
        glog::log("info", "Player x: " + std::to_string(this->players[i].x) + " y: " + std::to_string(this->players[i].y), "game save");
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
    this->run = 0;
    this->run_state = 0;
    this->gui_open = 0;
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
    glog::log("info", "Player Saved: " + name, "player");
    return 0;
}
int player::load(std::string name)
{
    glog::log("info", "Loading Player: " + name, "player");
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
    this->run = 0;
    this->run_state = 0;
    this->gui_open = 0;
    glog::log("info", "Player Loaded: " + name, "player");
    return 0;
}
int game::update()
{

    global_mutex.lock();
    if (allow_fly) // fly mode
    {
        if (IsKeyPressed(VK_UP))
        {
            this->players[0].y += 1;
        }
        if (IsKeyPressed(VK_DOWN))
        {
            this->players[0].y -= 1;
        }
    }
    // E key
    if (IsKeyPressed(0x45))
    {

        this->players[0].gui_open = !this->players[0].gui_open;
        Sleep(200);
    }
    if (this->players[0].run == 3 && this->players[0].run_state <= 30)
    {
        this->players[0].run_state++;
        if (!player_attack_side(&this->players[0], 2))
        {
            float dist = get_distance_to_side(&this->players[0], 2);
            if (dist < 0.1)
            {
                this->players[0].y += dist;
            }
            else
            {
                this->players[0].y += 0.1;
            }
        }
    }
    else if (this->players[0].run == 3 && this->players[0].run_state > 30)
    {
        this->players[0].run = 0;
        this->players[0].run_state = 0;
    }
    if (IsKeyPressed(VK_LEFT) || IsKeyPressed('A'))
    {
        if (!player_attack_side(&this->players[0], 0))
        {
            float dist = get_distance_to_side(&this->players[0], 0);
            if (dist < SPEED)
            {
                this->players[0].x -= dist;
            }
            else
            {
                this->players[0].x -= SPEED;
            }
        }
        if (this->players[0].run == 3)
        {
        }
        else if (this->players[0].run != 1)
        {
            this->players[0].run = 1;
            this->players[0].run_state = 0;
        }
        else
        {
            this->players[0].run_state = (this->players[0].run_state + 1) % 6;
        }
    }
    if (IsKeyPressed(VK_RIGHT) || IsKeyPressed('D'))
    {
        if (!player_attack_side(&this->players[0], 1))
        {
            float dist = get_distance_to_side(&this->players[0], 1);
            if (dist < SPEED)
            {
                this->players[0].x += dist;
            }
            else
            {
                this->players[0].x += SPEED;
            }
        }

        if (this->players[0].run == 3)
        {
        }
        else if (this->players[0].run != 3)
        {
            this->players[0].run = 2;
            this->players[0].run_state = 0;
        }
        else
        {
            this->players[0].run_state = (this->players[0].run_state + 1) % 6;
        }
    }
    if (!player_on_ground(&this->players[0]))
    {
        if (this->players[0].run != 3)
        {

            float dist = get_distance_to_ground(&this->players[0]);
            if (dist < 0.2)
            {
                this->players[0].y -= dist;
            }
            else
            {
                this->players[0].y -= 0.2;
            }
        }
    }

    else if (IsKeyPressed(VK_SPACE))
    {
        this->players[0].y += 1;
        if (this->players[0].run != 3)
        {
            this->players[0].run = 3;
            this->players[0].run_state = 0;
        }
    }
    if (IsKeyPressed(VK_ESCAPE))
    {
        exit_flag = 1;
        global_mutex.unlock();
        return 0;
    }

    global_mutex.unlock();
    // load chunks
    int center_x = this->players[0].x / BLOCKS_PER_CHUNK_X;
    for (int i = 0; i < CHUNKS_PER_MAP_X; i++)
    {
        int index = center_x - CHUNKS_PER_MAP_X / 2 + i;
        int found = 0;
        for (int j = 0; j < CHUNKS_PER_MAP_X; j++)
        {
            if (this->world.chunks[j].x == index)
            {
                found = 1;
                break;
            }
        }
        if (!found)
        {
            int farthest = 0;
            for (int j = 0; j < CHUNKS_PER_MAP_X; j++)
            {
                //   找到距离玩家最远的区块
                if (abs(this->world.chunks[j].x - center_x) > abs(this->world.chunks[farthest].x - center_x))
                {
                    farthest = j;
                }
            }
            this->world.load_chunk_pos(this->savepath, farthest, center_x - CHUNKS_PER_MAP_X / 2 + i);
            glog::log("info", "Loading Chunk: " + std::to_string(center_x - CHUNKS_PER_MAP_X / 2 + i), "game");
        }
    }
    // Sleep(1);
    return 0;
}
int game::player_on_ground(player *player)
{
    int x1, x2, y1, y2;
    // 确定玩家站在哪个方块上
    x1 = get_block_x(player->x - 0.45);
    x2 = get_block_x(player->x + 0.45);
    y1 = player->y - 0.55;
    y2 = player->y - 0.55;
    if (this->world.get_block(x1, y1) == block_type::BLOCK_AIR && this->world.get_block(x2, y2) == block_type::BLOCK_AIR)
    {
        return 0;
    }
    if (this->world.get_block(x1, y1) == block_type::BLOCK_SKY && this->world.get_block(x2, y2) == block_type::BLOCK_SKY)
    {
        return 0;
    }
    if (this->world.get_block(x1, y1) == block_type::BLOCK_VOID && this->world.get_block(x2, y2) == block_type::BLOCK_VOID)
    {
        return 0;
    }

    return 1;
}
float game::get_distance_to_ground(player *player)
{
    int x1, x2, y1, y2;
    // 确定玩家站在哪个方块上
    x1 = get_block_x(player->x - 0.45);
    x2 = get_block_x(player->x + 0.45);
    y1 = player->y - 0.6 + 1;
    y2 = player->y - 0.6 + 1;
    float distance = 0;
    if ((this->world.get_block(x1, y1) == block_type::BLOCK_AIR || this->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (this->world.get_block(x1, y1) != block_type::BLOCK_VOID && this->world.get_block(x2, y2) != block_type::BLOCK_VOID))
        return 1;
    if (this->world.get_block(x1, y1) != block_type::BLOCK_AIR && this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x1, y1) != block_type::BLOCK_VOID)
    {
        distance = player->y - 0.65 - y1 + 1;
    }
    else
    {
        distance = player->y - 0.65 - y2 + 1;
    }
    return distance;
}
int game::player_attack_side(player *player, int side)
{
    int x1, x2, y1, y2;
    if (side == 0)
    { // left
        x1 = get_block_x(player->x - 0.55);
        x2 = get_block_x(player->x - 0.55);
        y1 = player->y - 0.3;
        y2 = player->y + 0.3;
    }
    else if (side == 1)
    { // right
        x1 = get_block_x(player->x + 0.55);
        x2 = get_block_x(player->x + 0.55);
        y1 = player->y - 0.3;
        y2 = player->y + 0.3;
    }
    else if (side == 2)
    { // up
        x1 = get_block_x(player->x - 0.45);
        x2 = get_block_x(player->x + 0.45);
        y1 = player->y + 0.55;
        y2 = player->y + 0.55;
    }
    else if (side == 3)
    { // down
        x1 = get_block_x(player->x - 0.45);
        x2 = get_block_x(player->x + 0.45);
        y1 = player->y - 0.55;
        y2 = player->y - 0.55;
    }
    if (this->world.get_block(x1, y1) == block_type::BLOCK_AIR && this->world.get_block(x2, y2) == block_type::BLOCK_AIR)
    {
        return 0;
    }
    if (this->world.get_block(x1, y1) == block_type::BLOCK_SKY && this->world.get_block(x2, y2) == block_type::BLOCK_SKY)
    {
        return 0;
    }
    if (this->world.get_block(x1, y1) == block_type::BLOCK_VOID && this->world.get_block(x2, y2) == block_type::BLOCK_VOID)
    {
        return 0;
    }
    return 1;
}
float game::get_distance_to_side(player *player, int side)
{
    int x1, x2, y1, y2;
    float distance = 0;
    if (side == 0)
    { // left
        x1 = get_block_x(player->x - 0.55 - 1);
        x2 = get_block_x(player->x - 0.55 - 1);
        y1 = player->y - 0.45;
        y2 = player->y + 0.45;
        if ((this->world.get_block(x1, y1) == block_type::BLOCK_AIR || this->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (this->world.get_block(x1, y1) != block_type::BLOCK_VOID && this->world.get_block(x2, y2) != block_type::BLOCK_VOID))
            return 1;
        if (this->world.get_block(x1, y1) != block_type::BLOCK_AIR && this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x1, y1) != block_type::BLOCK_VOID)
        {

            distance = player->x - 0.5 - x1 - 1;
        }
        else
        {
            distance = player->x - 0.5 - x2 - 1;
        }
    }
    else if (side == 1)
    { // right
        x1 = get_block_x(player->x + 0.55 + 1);
        x2 = get_block_x(player->x + 0.55 + 1);
        y1 = player->y - 0.45;
        y2 = player->y + 0.45;
        if ((this->world.get_block(x1, y1) == block_type::BLOCK_AIR || this->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (this->world.get_block(x1, y1) != block_type::BLOCK_VOID && this->world.get_block(x2, y2) != block_type::BLOCK_VOID))
            return 1;
        if (this->world.get_block(x1, y1) != block_type::BLOCK_AIR && this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x1, y1) != block_type::BLOCK_VOID)
        {
            distance = x1 - 1 - player->x - 0.5 + 1;
        }
        else
        {
            distance = x2 - 1 - player->x - 0.5 + 1;
        }
    }
    else if (side == 2)
    { // up
        x1 = get_block_x(player->x - 0.45);
        x2 = get_block_x(player->x + 0.45);
        y1 = player->y + 0.55 + 1;
        y2 = player->y + 0.55 + 1;
        if ((this->world.get_block(x1, y1) == block_type::BLOCK_AIR || this->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (this->world.get_block(x1, y1) != block_type::BLOCK_VOID && this->world.get_block(x2, y2) != block_type::BLOCK_VOID))
            return 1;
        if (this->world.get_block(x1, y1) != block_type::BLOCK_AIR && this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x1, y1) != block_type::BLOCK_VOID)
        {
            distance = player->y - 0.55 - y1 - 1;
        }
        else
        {
            distance = player->y - 0.55 - y2 - 1;
        }
    }
    else if (side == 3)
    { // down
        x1 = get_block_x(player->x - 0.45);
        x2 = get_block_x(player->x + 0.45);
        y1 = player->y - 0.55 - 1;
        y2 = player->y - 0.55 - 1;
        if ((this->world.get_block(x1, y1) == block_type::BLOCK_AIR || this->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (this->world.get_block(x1, y1) != block_type::BLOCK_VOID && this->world.get_block(x2, y2) != block_type::BLOCK_VOID))
            return 1;
        if (this->world.get_block(x1, y1) != block_type::BLOCK_AIR && this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x1, y1) != block_type::BLOCK_VOID)
        {
            distance = y1 - 1 - player->y - 0.55;
        }
        else
        {
            distance = y2 - 1 - player->y - 0.55;
        }
    }
    if (distance < 0)
        distance = 0;
    return distance;
}