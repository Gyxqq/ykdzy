#pragma once
#include "game.hpp"

#include <Windows.h>
#include <cJSON.h>
#include <graphics.h>
#include <sys/stat.h>
#include <time.h>

#include <algorithm>
#include <mutex>
#include <regex>
#include <thread>

#include "log.hpp"
#include "map.hpp"
#include "render.hpp"
#define SPEED speed_all
float speed_all = 0.1;
extern std::mutex global_mutex;
extern int exit_flag;
int allow_fly = 1;

int get_block_x(float x)
{
    if (x < 0)
        return (int)x - 1;
    return (int)x;
}

BOOL IsKeyPressed(int vKey)
{
    if (GetForegroundWindow() != GetHWnd())
        return 0;
    return (GetAsyncKeyState(vKey) & 0x8000) != 0;
}
int game::init(std::string name)
{
    int seed = time(NULL);
    srand(seed);
    glog::log("info", "Initializing Game: " + name, "game");
    this->savepath = name;
    this->world.seed = seed;
    this->world.init(name);
    this->players.push_back(player());
    this->attacking_block.block = NULL;
    this->world_time = 0;
    memset(this->craft_table, 0, sizeof(this->craft_table));
    this->now_craft_table = NULL;
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "savepath", this->savepath.c_str());
    glog::log("info", "Seed: " + std::to_string(seed), "game");
    cJSON_AddNumberToObject(root, "seed", seed);
    cJSON_AddNumberToObject(root, "time", 0);
    cJSON* players = cJSON_CreateArray();
    std::string player_savepath = "00000.player";
    // this->players[0].save(this->savepath);
    cJSON_AddItemToArray(players, cJSON_CreateString(player_savepath.c_str()));
    cJSON_AddItemToObject(root, "players", players);
    char* data = cJSON_Print(root);
    FILE* file = fopen((name + "game.json").c_str(), "w");
    if (!file) {
        glog::log("error", "Failed to open file: " + name + "game.json", "game");
        return -1;
    }
    fwrite(data, 1, strlen(data), file);
    fclose(file);
    cJSON_Delete(root);
    delete[] data;
    this->players[0].init("00000");
    for (int i = 0; i < BLOCKS_PER_CHUNK_Y; i++) {
        if (this->world.chunks[0].blocks[i * BLOCKS_PER_CHUNK_X].type == block_type::BLOCK_AIR) {
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
    FILE* file = fopen(name.c_str(), "r");
    if (!file) {
        glog::log("error", "Failed to open file: " + name, "game loader");
        return -1;
    }
    long long size = 0;
    struct stat results;
    if (stat(name.c_str(), &results) == 0) {
        size = results.st_size;
        glog::log("info", "File size: " + std::to_string(size), "game loader");
    } else {
        glog::log("error", "Failed to get file size: " + name, "game loader");
        fclose(file);
        return -1;
    }
    char* data = new char[size];
    memset(data, 0, size);
    size_t read_size = fread(data, 1, size, file);

    fclose(file);
    cJSON* root = cJSON_Parse(data);
    if (root == NULL) {
        glog::log("error", "Failed to parse JSON Root", "game loader");
        return -1;
    }
    cJSON* savepath = cJSON_GetObjectItem(root, "savepath");
    if (savepath == NULL) {
        glog::log("error", "Failed to get savepath", "game loader");
        return -1;
    }
    cJSON* seed = cJSON_GetObjectItem(root, "seed");
    if (seed == NULL) {
        glog::log("error", "Failed to get seed", "game loader");
        return -1;
    }
    cJSON* time = cJSON_GetObjectItem(root, "time");
    if (time == NULL) {
        glog::log("error", "Failed to get time", "game loader");
        return -1;
    }
    this->world.seed = seed->valueint;
    glog::log("info", "get Seed: " + std::to_string(this->world.seed),
        "game loader");
    this->world_time = time->valueint;
    glog::log("info", "get Time: " + std::to_string(this->world_time),
        "game loader");
    this->savepath = savepath->valuestring;
    glog::log("info", "get Savepath: " + this->savepath, "game loader");
    this->world.load(this->savepath);
    cJSON* players = cJSON_GetObjectItem(root, "players");
    if (players == NULL) {
        glog::log("error", "Failed to get players", "game loader");
        return -1;
    }
    cJSON* player = players->child;
    int i = 0;
    while (player != NULL) {
        glog::log("info", "Loading Player: " + std::string(player->valuestring),
            "game loader");
        this->players.push_back(class player());
        this->players[i].load(this->savepath + player->valuestring);
        i++;
        player = player->next;
    }
    cJSON_Delete(root);
    delete[] data;
    int center_x = this->players[0].x / BLOCKS_PER_CHUNK_X;
    for (int i = 0; i < CHUNKS_PER_MAP_X; i++) {
        this->world.load_chunk_pos_no_save(this->savepath, i,
            center_x - CHUNKS_PER_MAP_X / 2 + i);
        glog::log(
            "info",
            "Loading Chunk: " + std::to_string(center_x - CHUNKS_PER_MAP_X / 2 + i),
            "game loader");
    }
    memset(&this->attacking_block, 0, sizeof(this->attacking_block));
    this->show_debug = 0;
    memset(&this->mouse_pos, 0, sizeof(this->mouse_pos));
    memset(&this->item_on_mouse, 0, sizeof(this->item_on_mouse));
    memset(this->craft_table, 0, sizeof(this->craft_table));
    this->now_craft_table = NULL;
    // this->chossing_item = 0;
    return 0;
}
int game::save()
{
    glog::log("info", "Saving Game", "game");
    this->world.save();
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "savepath", this->savepath.c_str());
    cJSON_AddNumberToObject(root, "seed", this->world.seed);
    cJSON_AddNumberToObject(root, "time", this->world_time);
    cJSON* players = cJSON_CreateArray();
    for (int i = 0; i < this->players.size(); i++) {
        std::string player_savepath = this->players[i].name + ".player";
        this->players[i].save(this->savepath);
        glog::log("info", "Saving Player: " + player_savepath, "game save");
        glog::log("info",
            "Player x: " + std::to_string(this->players[i].x) + " y: " + std::to_string(this->players[i].y),
            "game save");
        cJSON_AddItemToArray(players, cJSON_CreateString(player_savepath.c_str()));
    }

    cJSON_AddItemToObject(root, "players", players);
    char* data = cJSON_Print(root);
    FILE* file = fopen((this->savepath + "game.json").c_str(), "w");
    if (!file) {
        glog::log("error", "Failed to open file: " + this->savepath + "game.json",
            "game");
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
    FILE* file = fopen(name.c_str(), "wb");
    if (!file) {
        glog::log("error", "Failed to open file: " + name, "player");
        return -1;
    }
    fwrite(this, sizeof(*this), 1, file);
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (this->items[i].data != NULL) {
            fwrite(this->items[i].data, this->items[i].count,
                this->items[i].stack_count, file);
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
    FILE* file = fopen(name.c_str(), "rb");
    if (!file) {
        glog::log("error", "Failed to open file: " + name, "player");
        return -1;
    }
    fread(this, sizeof(*this), 1, file);
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (this->items[i].data != NULL) {
            this->items[i].data = malloc(this->items[i].count * this->items[i].stack_count);
            fread(this->items[i].data, this->items[i].count,
                this->items[i].stack_count, file);
        }
    }
    fclose(file);
    this->run = 0;
    this->run_state = 0;
    this->gui_open = 0;
    this->chossing_item = 0;
    glog::log("info", "Player Loaded: " + name, "player");
    return 0;
}
int game::update()
{
    global_mutex.lock();
    {
        if (rand() % 10000 == 0) {
            if (this->players[0].hunger > 0) {
                this->players[0].hunger--;
            }
        }
    }
    for (int i = 0; i < MAX_ITEMS; i++) {
        if (this->players[0].items[i].type != item_type::ITEM_AIR && this->players[0].items[i].count == 0) {
            this->players[0].items[i].type = item_type::ITEM_AIR;
            this->players[0].items[i].count = 0;
            this->players[0].items[i].stack_count = 0;
            this->players[0].items[i].data = NULL;
        }
    }
    for (int i = 0; i < 4; i++) {
        if (this->craft_table[i].count == 0) {
            this->craft_table[i].type = item_type::ITEM_AIR;
            this->craft_table[i].count = 0;
            this->craft_table[i].stack_count = 0;
            this->craft_table[i].data = NULL;
        }
    }
    if (this->item_on_mouse.count == 0) {
        this->item_on_mouse.type = item_type::ITEM_AIR;
        this->item_on_mouse.count = 0;
        this->item_on_mouse.stack_count = 0;
        this->item_on_mouse.data = NULL;
    }
    if (allow_fly) // fly mode
    {
        if (IsKeyPressed(VK_UP)) {
            this->players[0].y += 1;
        }
        if (IsKeyPressed(VK_DOWN)) {
            this->players[0].y -= 1;
        }
        if (IsKeyPressed(VK_LEFT)) {
            this->players[0].x -= 1;
        }
        if (IsKeyPressed(VK_RIGHT)) {
            this->players[0].x += 1;
        }
    }

    if (GetCursorPos(&this->mouse_pos)) {
        ScreenToClient(GetHWnd(), &this->mouse_pos);
    } // 获取鼠标位置
    check_num();
    check_craft();
    if (IsKeyPressed(VK_F1)) {
        std::thread debug([&]() {
            char input[2048];
            int get = InputBox(input, 2048, "Debug", "Input Command:", 0, 0, 0, 0);
            if (get) {
                glog::log("info", "Debug Command: " + std::string(input), "game");
                std::regex reg("tp\\s+(-?\\d+)\\s+(\\d+)");
                std::smatch match;
                std::string input_str(input);
                if (std::regex_match(input_str, match, reg)) {
                    global_mutex.lock();
                    this->players[0].x = std::stoi(match[1].str());
                    this->players[0].y = std::stoi(match[2].str());
                    if (this->players[0].y < 0) {
                        this->players[0].y = 0;
                    }
                    glog::log("info",
                        "Teleport to: " + std::to_string(this->players[0].x) + " " + std::to_string(this->players[0].y),
                        "game");
                    global_mutex.unlock();
                    return;
                }
                reg = std::regex("st\\s+(\\d+)\\s+(\\d+)");
                if (std::regex_match(input_str, match, reg)) {
                    global_mutex.lock();
                    int hour = std::stoi(match[1].str());
                    int minute = std::stoi(match[2].str());
                    if (hour < 0 || hour > 23) {
                        glog::log("error", "Invalid Hour", "game");

                    } else if (minute < 0 || minute > 59) {
                        glog::log("error", "Invalid Hour", "game");

                    } else {
                        this->world_time -= this->world_time % (60 * 24);
                        this->world_time += hour * 60 + minute;
                    }
                    glog::log("info", "set Time: " + std::to_string(this->world_time),
                        "game");
                    global_mutex.unlock();
                    return;
                }
                reg.assign("fly");
                if (std::regex_match(input_str, match, reg)) {
                    global_mutex.lock();
                    allow_fly = !allow_fly;
                    glog::log("info", "Fly Mode: " + std::to_string(allow_fly), "game");
                    global_mutex.unlock();
                    return;
                }
                reg.assign("give\\s+(\\d+)\\s+(\\d+)");
                if (std::regex_match(input_str, match, reg)) {
                    global_mutex.lock();
                    int type = std::stoi(match[1].str());
                    int count = std::stoi(match[2].str());
                    if (type < 0 || type >= item_type::ITEM_MAX_INDEX) {
                        glog::log("error", "Invalid Item Type", "game");
                    } else if (count < 0) {
                        glog::log("error", "Invalid Item Count", "game");
                    } else {
                        item item = get_item_by_type((item_type)type);
                        item.stack_count >= count ? item.count = count
                                                  : item.count = item.stack_count;
                        if (item.type == item_type::ITEM_AIR) {
                            glog::log("error", "Invalid Item Type", "game");
                        } else {
                            for (int i = 0; i < MAX_ITEMS; i++) {
                                if (this->players[0].items[i].type == type) {
                                    if (this->players[0].items[i].stack_count - this->players[0].items[i].count >= 0) {
                                        if (this->players[0].items[i].stack_count - this->players[0].items[i].count >= count) {
                                            this->players[0].items[i].count += count;
                                            count = 0;
                                            break;
                                        } else {
                                            count -= this->players[0].items[i].stack_count - this->players[0].items[i].count;
                                            this->players[0].items[i].count = this->players[0].items[i].stack_count;
                                        }
                                    }

                                } else if (this->players[0].items[i].type == item_type::ITEM_AIR) {
                                    this->players[0].items[i] = item;
                                    count = 0;
                                    break;
                                }
                                if (count == 0) {
                                    break;
                                }
                            }
                        }
                    }
                    glog::log("info",
                        "Give Item: " + std::to_string(type) + " " + std::to_string(count),
                        "game");
                    global_mutex.unlock();
                    return;
                }
                reg.assign("log");
                if (std::regex_match(input_str, match, reg)) {
                    glog::log("info", "start Log", "game log");
                    int len = craft_table::craft_table::get_len();
                    glog::log("info", "craft_table len: " + std::to_string(len), "game log");
                    for (int i = 0; i < len; i++) {
                        glog::log("info", "craft_table[" + std::to_string(i) + "]: ", "game log");
                        glog::log("info", "items: ", "game log");
                        int j = craft_table::craft_table::craft_tables[i].need_count;
                        glog::log("info", "need_count: " + std::to_string(j), "game log");
                        for (int k = 0; k < 9; k++) {
                            glog::log("info",
                                "type: " + std::to_string(craft_table::craft_table::craft_tables[i].items[k].type) + " count: " + std::to_string(craft_table::craft_table::craft_tables[i].items[k].count),
                                "game log");
                        }
                        glog::log("info",
                            "result: type: " + std::to_string(craft_table::craft_table::craft_tables[i].result.type) + " count: " + std::to_string(craft_table::craft_table::craft_tables[i].result.count),
                            "game log");
                    }

                    return;
                }

                else {
                    glog::log("error", "Invalid Command", "game");
                }
            }
        });

        Sleep(200);
        debug.detach();
    }

    if (IsKeyPressed(VK_LBUTTON) && !this->players[0].gui_open) {
        if (this->mouse_pos.x >= 0 && this->mouse_pos.x <= render::width && this->mouse_pos.y >= 0 && this->mouse_pos.y <= render::height) {
            int x = render::width / 2 + BLOCK_TEXTURES_SIZE / 2;
            int y = render::height / 2 + BLOCK_TEXTURES_SIZE / 2;
            float offset_x = (this->mouse_pos.x - x) * 1.0 / BLOCK_TEXTURES_SIZE;
            float offset_y = (this->mouse_pos.y - y) * 1.0 / BLOCK_TEXTURES_SIZE;
            if (offset_x * offset_x + offset_y * offset_y <= 16) {
                int block_x = get_block_x(this->players[0].x + offset_x);
                int block_y = this->players[0].y - offset_y;
                // glog::log("info", "block x: " + std::to_string(block_x) + " y: " +
                // std::to_string(block_y), "game");
                if (block_y < BLOCKS_PER_CHUNK_Y && block_y >= 0) {
                    int type = this->world.get_block(block_x, block_y);
                    if (type != block_type::BLOCK_AIR && type != block_type::BLOCK_VOID) {
                        if (this->attacking_block.block == NULL) {
                            this->attacking_block.block = this->world.get_block_ptr(block_x, block_y);
                            this->attacking_block.attacking_state = 0;
                        } else {
                            if (this->attacking_block.block == this->world.get_block_ptr(block_x, block_y)) {
                                this->attacking_block.attacking_state++;
                                // glog::log("info", "attacking block x: " +
                                // std::to_string(block_x) + " y: " + std::to_string(block_y) +
                                // " state: " +
                                // std::to_string(this->attacking_block.attacking_state),
                                // "game");
                                if (this->attacking_block.attacking_state >= 10) {
                                    item drop = this->get_block_drop(this->attacking_block.block);
                                    if (drop.type != item_type::ITEM_AIR) {
                                        for (int i = 0; i < MAX_ITEMS; i++) {
                                            if (this->players[0].items[i].type == drop.type) {
                                                this->players[0].items[i].stack_count = drop.stack_count;
                                                if (this->players[0].items[i].stack_count - this->players[0].items[i].count >= 0) {
                                                    if (this->players[0].items[i].stack_count - this->players[0].items[i].count >= drop.count) {
                                                        this->players[0].items[i].count += drop.count;
                                                        drop.count = 0;
                                                        break;
                                                    } else {
                                                        drop.count -= this->players[0].items[i].stack_count - this->players[0].items[i].count;
                                                        this->players[0].items[i].count = this->players[0].items[i].stack_count;
                                                    }
                                                }
                                            }
                                        }
                                        if (drop.count > 0) {
                                            for (int i = 0; i < MAX_ITEMS; i++) {
                                                if (this->players[0].items[i].type == item_type::ITEM_AIR) {
                                                    this->players[0].items[i] = drop;
                                                    drop.count = 0;
                                                    break;
                                                }
                                            }
                                        }
                                        if (drop.count > 0) {
                                            if (drop.data != NULL) {
                                                delete drop.data;
                                                drop.data = NULL;
                                            }
                                        }
                                    }
                                    if (this->attacking_block.block->data != NULL) {
                                        delete this->attacking_block.block->data;
                                        this->attacking_block.block->data = NULL;
                                        this->attacking_block.block->data_size = 0;
                                    }
                                    this->attacking_block.block->type = block_type::BLOCK_AIR;
                                    this->attacking_block.block = NULL;
                                    glog::log("info",
                                        "destroy block x: " + std::to_string(block_x) + " y: " + std::to_string(block_y),
                                        "game");
                                    glog::log("info",
                                        "drop item type: " + std::to_string(drop.type) + " count: " + std::to_string(drop.count),
                                        "game");
                                }
                            } else {
                                this->attacking_block.block = this->world.get_block_ptr(block_x, block_y);
                                this->attacking_block.attacking_state = 0;
                            }
                        }
                    }
                }
            }
        }
    } else if (!IsKeyPressed(VK_LBUTTON) && !this->players[0].gui_open) {
        this->attacking_block.block = NULL;
        this->attacking_block.attacking_state = 0;
    } else if (IsKeyPressed(VK_LBUTTON) && this->players[0].gui_open) {
        int pos_x = render::width / 2 - 160 + item_begin_x;
        int pos_y = render::height / 2 - 169 + item_begin_y;

        if ((this->mouse_pos.x >= pos_x && this->mouse_pos.x <= pos_x + 9 * 36 && this->mouse_pos.y >= pos_y && this->mouse_pos.y <= pos_y + 4 * 36) || this->mouse_pos.x >= render::width / 2 - 160 + inventory_begin_x && this->mouse_pos.x <= render::width / 2 - 160 + inventory_begin_x + 9 * 36 && this->mouse_pos.y >= render::height / 2 - 169 + inventory_begin_y && this->mouse_pos.y <= render::height / 2 - 169 + inventory_begin_y + 36) {
            int x, y, index;
            index = -1;
            if (this->mouse_pos.x >= pos_x && this->mouse_pos.x <= pos_x + 9 * 36 && this->mouse_pos.y >= pos_y && this->mouse_pos.y <= pos_y + 4 * 36) {
                global_mutex.unlock();
                Sleep(200);
                global_mutex.lock();
                glog::log("info", "get item", "game");
                x = (this->mouse_pos.x - pos_x) / 36;
                y = (this->mouse_pos.y - pos_y) / 36;
                index = y * 9 + x;
            } else if (this->mouse_pos.x >= render::width / 2 - 160 + inventory_begin_x && this->mouse_pos.x <= render::width / 2 - 160 + inventory_begin_x + 9 * 36 && this->mouse_pos.y >= render::height / 2 - 169 + inventory_begin_y && this->mouse_pos.y <= render::height / 2 - 169 + inventory_begin_y + 36) {
                global_mutex.unlock();
                Sleep(200);
                global_mutex.lock();
                glog::log("info", "get item", "game");
                x = (this->mouse_pos.x - render::width / 2 - 160 + inventory_begin_x) / 36;
                index = x + 27;
            }
            // _ASSERT(index >= 0 && index < MAX_ITEMS);
            if (index >= 0 && index < MAX_ITEMS) {
                if (this->players[0].items[index].type != item_type::ITEM_AIR && this->item_on_mouse.type == item_type::ITEM_AIR) {
                    this->item_on_mouse = this->players[0].items[index];
                    this->players[0].items[index].type = item_type::ITEM_AIR;
                    this->players[0].items[index].count = 0;
                    this->players[0].items[index].stack_count = 0;
                } else if (this->players[0].items[index].type == item_type::ITEM_AIR && this->item_on_mouse.type != item_type::ITEM_AIR) {
                    this->players[0].items[index] = this->item_on_mouse;
                    this->item_on_mouse.type = item_type::ITEM_AIR;
                    this->item_on_mouse.count = 0;

                    this->item_on_mouse.stack_count = 0;
                } else if (this->players[0].items[index].type == item_type::ITEM_AIR && this->item_on_mouse.type == item_type::ITEM_AIR) {
                    ;
                } else if (this->players[0].items[index].type == this->item_on_mouse.type) {
                    if (this->players[0].items[index].count + this->item_on_mouse.count <= this->players[0].items[index].stack_count) {
                        this->players[0].items[index].count += this->item_on_mouse.count;
                        this->item_on_mouse.count = 0;
                    } else {
                        this->item_on_mouse.count = this->players[0].items[index].count + this->item_on_mouse.count - this->players[0].items[index].stack_count;
                        this->players[0].items[index].count = this->players[0].items[index].stack_count;
                    }
                }
            }
        } else if ((this->mouse_pos.x >= 195 + render::width / 2 - 160 && this->mouse_pos.y >= 35 + render::height / 2 - 169) && (this->mouse_pos.x <= 264 + render::width / 2 - 160 && this->mouse_pos.y <= 104 + render::height / 2 - 169)) {
            pos_x = render::width / 2 - 160 + CRAFT_TABLE_BEGIN_X;
            pos_y = render::height / 2 - 169 + CRAFT_TABLE_BEGIN_Y;
            global_mutex.unlock();
            Sleep(200);
            global_mutex.lock();
            if (this->mouse_pos.x >= pos_x && this->mouse_pos.x <= pos_x + 2 * 36 && this->mouse_pos.y >= pos_y && this->mouse_pos.y <= pos_y + 2 * 36) {
                int index = (this->mouse_pos.x - pos_x) / 36 + (this->mouse_pos.y - pos_y) / 36 * 2;

                if (this->item_on_mouse.type != item_type::ITEM_AIR) {
                    if (this->craft_table[index].type == item_type::ITEM_AIR) {
                        this->craft_table[index] = this->item_on_mouse;
                        this->item_on_mouse.type = item_type::ITEM_AIR;
                        this->item_on_mouse.count = 0;
                        this->item_on_mouse.stack_count = 0;
                        this->item_on_mouse.data = NULL;
                    } else if (this->craft_table[index].type == this->item_on_mouse.type) {

                        if (this->craft_table[index].stack_count - this->craft_table[index].count >= this->item_on_mouse.count) {
                            this->craft_table[index].count += this->item_on_mouse.count;
                            this->item_on_mouse.type = item_type::ITEM_AIR;
                            this->item_on_mouse.count = 0;
                            this->item_on_mouse.stack_count = 0;
                            this->item_on_mouse.data = NULL;
                        } else {
                            this->item_on_mouse.count -= this->craft_table[index].stack_count - this->craft_table[index].count;
                            this->craft_table[index].count = this->craft_table[index].stack_count;
                        }
                    }
                } else {
                    if (this->craft_table[index].type != item_type::ITEM_AIR) {
                        this->item_on_mouse = this->craft_table[index];
                        this->craft_table[index].type = item_type::ITEM_AIR;
                        this->craft_table[index].count = 0;
                        this->craft_table[index].stack_count = 0;
                        this->craft_table[index].data = NULL;
                    }
                }
            }
        } else if (this->mouse_pos.x >= render::width / 2 - 160 + CRAFT_OUT_X && this->mouse_pos.y >= CRAFT_OUT_Y + render::height / 2 - 169 && this->mouse_pos.x <= 36 + CRAFT_OUT_X + render::width / 2 - 160 && this->mouse_pos.y <= 36 + CRAFT_OUT_Y + render::height / 2 - 169) {
            global_mutex.unlock();
            Sleep(200);
            global_mutex.lock();
            if (this->now_craft_table != NULL) {
                item res = this->now_craft_table->result;
                if (this->item_on_mouse.type == item_type::ITEM_AIR) {
                    if (res.type != item_type::ITEM_AIR) {
                        if (res.count > 0) {
                            int get_count = 0;
                            int flag = 1;
                            int get_index[4] = { 0 };
                            std::vector<item> tmp;
                            for (int i = 0; i < 4; i++) {
                                tmp.push_back(this->craft_table[i]);
                            }
                            for (int i = 0; i < 9; i++) {
                                if (this->now_craft_table->items[i].type != item_type::ITEM_AIR) {
                                    if (get_count >= this->now_craft_table->need_count) {
                                        break;
                                    }
                                    for (int j = 0; j < 4; j++) {
                                        if (get_index[j] == 1)
                                            continue;
                                        if (this->craft_table[j].type == this->now_craft_table->items[i].type) {
                                            if (this->craft_table[j].count >= this->now_craft_table->items[i].count) {
                                                get_index[j] = 1;
                                                get_count++;
                                                tmp[j].count -= this->now_craft_table->items[i].count;
                                                break;
                                            }
                                        }
                                        if (j == 3) {
                                            flag = 0;
                                            break;
                                        }
                                    }
                                    if (flag == 0) {
                                        break;
                                    }
                                }
                            }
                            if (flag == 1) {
                                for (int i = 0; i < 4; i++) {
                                    this->craft_table[i] = tmp[i];
                                }
                                this->item_on_mouse = res;
                            }
                        }
                    }
                } else if (res.type == this->item_on_mouse.type) {
                    if (this->item_on_mouse.stack_count - this->item_on_mouse.count >= res.count) {
                        if (res.type != item_type::ITEM_AIR) {
                            if (res.count > 0) {
                                int get_count = 0;
                                int flag = 1;
                                int get_index[4] = { 0 };
                                std::vector<item> tmp;
                                for (int i = 0; i < 4; i++) {
                                    tmp.push_back(this->craft_table[i]);
                                }
                                for (int i = 0; i < 9; i++) {
                                    if (this->now_craft_table->items[i].type != item_type::ITEM_AIR) {
                                        if (get_count >= this->now_craft_table->need_count) {
                                            break;
                                        }
                                        for (int j = 0; j < 4; j++) {
                                            if (get_index[j] == 1)
                                                continue;
                                            if (this->craft_table[j].type == this->now_craft_table->items[i].type) {
                                                if (this->craft_table[j].count >= this->now_craft_table->items[i].count) {
                                                    get_index[j] = 1;
                                                    get_count++;
                                                    tmp[j].count -= this->now_craft_table->items[i].count;
                                                    break;
                                                }
                                            }
                                            if (j == 3) {
                                                flag = 0;
                                                break;
                                            }
                                        }
                                        if (flag == 0) {
                                            break;
                                        }
                                    }
                                }
                                if (flag == 1) {
                                    for (int i = 0; i < 4; i++) {
                                        this->craft_table[i] = tmp[i];
                                    }
                                    this->item_on_mouse.count += res.count;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
            if (IsKeyPressed('Q') && !this->players[0].gui_open)

            {
                if (this->players[0].items[this->players[0].chossing_item + 27].type != item_type::ITEM_AIR) {
                    if (this->players[0].items[this->players[0].chossing_item + 27].count > 0) {
                        this->players[0].items[this->players[0].chossing_item + 27].count--;
                        if (this->players[0].items[this->players[0].chossing_item + 27].count == 0) {
                            this->players[0].items[this->players[0].chossing_item + 27].type = item_type::ITEM_AIR;
                            this->players[0].items[this->players[0].chossing_item + 27].count = 0;
                            this->players[0]
                                .items[this->players[0].chossing_item + 27]
                                .stack_count
                                = 0;
                        }
                    }
                }
                global_mutex.unlock();
                Sleep(200);
                global_mutex.lock();
            }
            if (IsKeyPressed('Q') && this->players[0].gui_open) {
                int pos_x = render::width / 2 - 160 + item_begin_x;
                int pos_y = render::height / 2 - 169 + item_begin_y;
                int x, y, index;
                index = -1;
                if (this->mouse_pos.x >= pos_x && this->mouse_pos.x <= pos_x + 9 * 36 && this->mouse_pos.y >= pos_y && this->mouse_pos.y <= pos_y + 4 * 36) {
                    global_mutex.unlock();
                    Sleep(200);
                    global_mutex.lock();
                    glog::log("info", "get item", "game");
                    x = (this->mouse_pos.x - pos_x) / 36;
                    y = (this->mouse_pos.y - pos_y) / 36;
                    index = y * 9 + x;
                } else if (this->mouse_pos.x >= render::width / 2 - 160 + inventory_begin_x && this->mouse_pos.x <= render::width / 2 - 160 + inventory_begin_x + 9 * 36 && this->mouse_pos.y >= render::height / 2 - 169 + inventory_begin_y && this->mouse_pos.y <= render::height / 2 - 169 + inventory_begin_y + 36) {
                    global_mutex.unlock();
                    Sleep(200);
                    global_mutex.lock();
                    glog::log("info", "get item", "game");
                    x = (this->mouse_pos.x - render::width / 2 - 160 + inventory_begin_x) / 36;
                    index = x + 27;
                }
                // _ASSERT(index >= 0 && index < MAX_ITEMS);
                if (index >= 0 && index < MAX_ITEMS) {
                    if (this->players[0].items[index].type != item_type::ITEM_AIR) {
                        if (this->players[0].items[index].count > 0) {
                            this->players[0].items[index].count--;
                            if (this->players[0].items[index].count == 0) {
                                this->players[0].items[index].type = item_type::ITEM_AIR;
                                this->players[0].items[index].count = 0;
                                this->players[0].items[index].stack_count = 0;
                            }
                        }
                    }
                }
            }
            if (IsKeyPressed(VK_RBUTTON) && !this->players[0].gui_open) {
                if (this->mouse_pos.x >= 0 && this->mouse_pos.x <= render::width && this->mouse_pos.y >= 0 && this->mouse_pos.y <= render::height) {
                    int x = render::width / 2 + BLOCK_TEXTURES_SIZE / 2;
                    int y = render::height / 2 + BLOCK_TEXTURES_SIZE / 2;
                    float offset_x = (this->mouse_pos.x - x) * 1.0 / BLOCK_TEXTURES_SIZE;
                    float offset_y = (this->mouse_pos.y - y) * 1.0 / BLOCK_TEXTURES_SIZE;
                    if (offset_x * offset_x + offset_y * offset_y <= 16 && offset_x * offset_x + offset_y * offset_y >= 1) {
                        int block_x = get_block_x(this->players[0].x + offset_x);
                        int block_y = this->players[0].y - offset_y;
                        if (block_y < BLOCKS_PER_CHUNK_Y && block_y >= 0) {
                            int type = this->world.get_block(block_x, block_y);
                            if (type == block_type::BLOCK_AIR) {
                                if (this->players[0]
                                        .items[this->players[0].chossing_item + 27]
                                        .type
                                    != item_type::ITEM_AIR) {
                                    block block = this->get_block_by_item(
                                        this->players[0].items[this->players[0].chossing_item + 27]);
                                    if (block.type != block_type::BLOCK_AIR) {
                                        block.x = block_x;
                                        block.y = block_y;
                                        *this->world.get_block_ptr(block_x, block_y) = block;
                                        glog::log("info",
                                            "place block x: " + std::to_string(block_x) + " y: " + std::to_string(block_y),
                                            "game");
                                        this->players[0]
                                            .items[this->players[0].chossing_item + 27]
                                            .count--;
                                        if (this->players[0]
                                                .items[this->players[0].chossing_item + 27]
                                                .count
                                            == 0) {
                                            this->players[0]
                                                .items[this->players[0].chossing_item + 27]
                                                .type
                                                = item_type::ITEM_AIR;
                                            this->players[0]
                                                .items[this->players[0].chossing_item + 27]
                                                .count
                                                = 0;
                                            this->players[0]
                                                .items[this->players[0].chossing_item + 27]
                                                .stack_count
                                                = 0;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (IsKeyPressed('E')) {
                this->players[0].gui_open = !this->players[0].gui_open;
                global_mutex.unlock();
                Sleep(200);
                global_mutex.lock();
            }
            if (IsKeyPressed('R')) {
                std::sort(this->players[0].items, this->players[0].items + 27,
                    [](item a, item b) -> bool {
                        return a.type > b.type || (a.type == b.type && a.count > b.count);
                    });
                global_mutex.unlock();
                Sleep(200);
                global_mutex.lock();
            }
            if (IsKeyPressed(VK_F3)) {
                this->show_debug = !this->show_debug;
                global_mutex.unlock();
                Sleep(200);
                global_mutex.lock();
            }
            if (this->players[0].run == 3 && this->players[0].run_state <= 30) {
                this->players[0].run_state++;
                if (!player_attack_side(&this->players[0], 2)) {
                    float dist = get_distance_to_side(&this->players[0], 2);
                    if (dist < 0.1) {
                        this->players[0].y += dist;
                    } else {
                        this->players[0].y += 0.1;
                    }
                }
            } else if (this->players[0].run == 3 && this->players[0].run_state > 30) {
                this->players[0].run = 0;
                this->players[0].run_state = 0;
            }
            if (IsKeyPressed(VK_LEFT) || IsKeyPressed('A')) {
                if (!player_attack_side(&this->players[0], 0)) {
                    float dist = get_distance_to_side(&this->players[0], 0);
                    if (dist < SPEED) {
                        this->players[0].x -= dist;
                    } else {
                        this->players[0].x -= SPEED;
                    }
                }
                if (this->players[0].run == 3) {
                } else if (this->players[0].run != 1) {
                    this->players[0].run = 1;
                    this->players[0].run_state = 0;
                } else {
                    this->players[0].run_state = (this->players[0].run_state + 1) % 6;
                }
            }
            if (IsKeyPressed(VK_RIGHT) || IsKeyPressed('D')) {
                if (!player_attack_side(&this->players[0], 1)) {
                    float dist = get_distance_to_side(&this->players[0], 1);
                    if (dist < SPEED) {
                        this->players[0].x += dist;
                    } else {
                        this->players[0].x += SPEED;
                    }
                }

                if (this->players[0].run == 3) {
                } else if (this->players[0].run != 3) {
                    this->players[0].run = 2;
                    this->players[0].run_state = 0;
                } else {
                    this->players[0].run_state = (this->players[0].run_state + 1) % 6;
                }
            }
            if (!player_on_ground(&this->players[0])) {
                if (this->players[0].run != 3) {
                    float dist = get_distance_to_ground(&this->players[0]);
                    if (dist < 0.2) {
                        this->players[0].y -= dist;
                    } else {
                        this->players[0].y -= 0.2;
                    }
                }
            }

            else if (IsKeyPressed(VK_SPACE)) {
                // this->players[0].y += 1;
                if (this->players[0].run != 3) {
                    this->players[0].run = 3;
                    this->players[0].run_state = 0;
                }
            }
            if (IsKeyPressed(VK_ESCAPE)) {
                exit_flag = 1;
                global_mutex.unlock();
                return 0;
            }

            // load chunks
            int center_x = this->players[0].x / BLOCKS_PER_CHUNK_X;
            for (int i = 0; i < CHUNKS_PER_MAP_X; i++) {
                int index = center_x - CHUNKS_PER_MAP_X / 2 + i;
                int found = 0;
                for (int j = 0; j < CHUNKS_PER_MAP_X; j++) {
                    if (this->world.chunks[j].x == index) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    int farthest = 0;
                    for (int j = 0; j < CHUNKS_PER_MAP_X; j++) {
                        //   找到距离玩家最远的区块
                        if (abs(this->world.chunks[j].x - center_x) > abs(this->world.chunks[farthest].x - center_x)) {
                            farthest = j;
                        }
                    }
                    this->world.load_chunk_pos(this->savepath, farthest,
                        center_x - CHUNKS_PER_MAP_X / 2 + i);
                    glog::log("info",
                        "Loading Chunk: " + std::to_string(center_x - CHUNKS_PER_MAP_X / 2 + i),
                        "game");
                }
            }
            global_mutex.unlock();
            // Sleep(1);
            return 0;
        }
        int game::player_on_ground(player * player)
        {
            int x1, x2, y1, y2;
            // 确定玩家站在哪个方块上
            x1 = get_block_x(player->x - 0.45);
            x2 = get_block_x(player->x + 0.45);
            y1 = player->y - 0.55;
            y2 = player->y - 0.55;
            if (this->world.get_block(x1, y1) == block_type::BLOCK_AIR && this->world.get_block(x2, y2) == block_type::BLOCK_AIR) {
                return 0;
            }
            if (this->world.get_block(x1, y1) == block_type::BLOCK_SKY && this->world.get_block(x2, y2) == block_type::BLOCK_SKY) {
                return 0;
            }
            if (this->world.get_block(x1, y1) == block_type::BLOCK_VOID && this->world.get_block(x2, y2) == block_type::BLOCK_VOID) {
                return 0;
            }

            return 1;
        }
        float game::get_distance_to_ground(player * player)
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
            if (this->world.get_block(x1, y1) != block_type::BLOCK_AIR && this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x1, y1) != block_type::BLOCK_VOID) {
                distance = player->y - 0.65 - y1 + 1;
            } else {
                distance = player->y - 0.65 - y2 + 1;
            }
            return distance;
        }
        int game::player_attack_side(player * player, int side)
        {
            int x1, x2, y1, y2;
            if (side == 0) { // left
                x1 = get_block_x(player->x - 0.55);
                x2 = get_block_x(player->x - 0.55);
                y1 = player->y - 0.3;
                y2 = player->y + 0.3;
            } else if (side == 1) { // right
                x1 = get_block_x(player->x + 0.55);
                x2 = get_block_x(player->x + 0.55);
                y1 = player->y - 0.3;
                y2 = player->y + 0.3;
            } else if (side == 2) { // up
                x1 = get_block_x(player->x - 0.45);
                x2 = get_block_x(player->x + 0.45);
                y1 = player->y + 0.55;
                y2 = player->y + 0.55;
            } else if (side == 3) { // down
                x1 = get_block_x(player->x - 0.45);
                x2 = get_block_x(player->x + 0.45);
                y1 = player->y - 0.55;
                y2 = player->y - 0.55;
            }
            if (this->world.get_block(x1, y1) == block_type::BLOCK_AIR && this->world.get_block(x2, y2) == block_type::BLOCK_AIR) {
                return 0;
            }
            if (this->world.get_block(x1, y1) == block_type::BLOCK_SKY && this->world.get_block(x2, y2) == block_type::BLOCK_SKY) {
                return 0;
            }
            if (this->world.get_block(x1, y1) == block_type::BLOCK_VOID && this->world.get_block(x2, y2) == block_type::BLOCK_VOID) {
                return 0;
            }
            return 1;
        }
        float game::get_distance_to_side(player * player, int side)
        {
            int x1, x2, y1, y2;
            float distance = 0;
            if (side == 0) { // left
                x1 = get_block_x(player->x - 0.55 - 1);
                x2 = get_block_x(player->x - 0.55 - 1);
                y1 = player->y - 0.45;
                y2 = player->y + 0.45;
                if ((this->world.get_block(x1, y1) == block_type::BLOCK_AIR || this->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (this->world.get_block(x1, y1) != block_type::BLOCK_VOID && this->world.get_block(x2, y2) != block_type::BLOCK_VOID))
                    return 1;
                if (this->world.get_block(x1, y1) != block_type::BLOCK_AIR && this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x1, y1) != block_type::BLOCK_VOID) {
                    distance = player->x - 0.5 - x1 - 1;
                } else {
                    distance = player->x - 0.5 - x2 - 1;
                }
            } else if (side == 1) { // right
                x1 = get_block_x(player->x + 0.55 + 1);
                x2 = get_block_x(player->x + 0.55 + 1);
                y1 = player->y - 0.45;
                y2 = player->y + 0.45;
                if ((this->world.get_block(x1, y1) == block_type::BLOCK_AIR || this->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (this->world.get_block(x1, y1) != block_type::BLOCK_VOID && this->world.get_block(x2, y2) != block_type::BLOCK_VOID))
                    return 1;
                if (this->world.get_block(x1, y1) != block_type::BLOCK_AIR && this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x1, y1) != block_type::BLOCK_VOID) {
                    distance = x1 - 1 - player->x - 0.5 + 1;
                } else {
                    distance = x2 - 1 - player->x - 0.5 + 1;
                }
            } else if (side == 2) { // up
                x1 = get_block_x(player->x - 0.45);
                x2 = get_block_x(player->x + 0.45);
                y1 = player->y + 0.55 + 1;
                y2 = player->y + 0.55 + 1;
                if ((this->world.get_block(x1, y1) == block_type::BLOCK_AIR || this->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (this->world.get_block(x1, y1) != block_type::BLOCK_VOID && this->world.get_block(x2, y2) != block_type::BLOCK_VOID))
                    return 1;
                if (this->world.get_block(x1, y1) != block_type::BLOCK_AIR && this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x1, y1) != block_type::BLOCK_VOID) {
                    distance = player->y - 0.55 - y1 + 1;
                } else {
                    distance = player->y - 0.55 - y2 + 1;
                }
            } else if (side == 3) { // down
                x1 = get_block_x(player->x - 0.45);
                x2 = get_block_x(player->x + 0.45);
                y1 = player->y - 0.55 - 1;
                y2 = player->y - 0.55 - 1;
                if ((this->world.get_block(x1, y1) == block_type::BLOCK_AIR || this->world.get_block(x2, y2) == block_type::BLOCK_AIR) && (this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x2, y2) != block_type::BLOCK_SKY) && (this->world.get_block(x1, y1) != block_type::BLOCK_VOID && this->world.get_block(x2, y2) != block_type::BLOCK_VOID))
                    return 1;
                if (this->world.get_block(x1, y1) != block_type::BLOCK_AIR && this->world.get_block(x1, y1) != block_type::BLOCK_SKY && this->world.get_block(x1, y1) != block_type::BLOCK_VOID) {
                    distance = y1 - 1 - player->y - 0.55;
                } else {
                    distance = y2 - 1 - player->y - 0.55;
                }
            }
            if (distance < 0)
                distance = 0;
            return distance;
        }
        item game::get_block_drop(block * block)
        {
            item item;
            item.type = item_type::ITEM_AIR;
            item.stack_count = 0;
            item.count = 0;
            item.data = NULL;
            switch (block->type) {
            case block_type::BLOCK_DIRT:
                item.type = item_type::ITEM_DIRT;
                item.stack_count = 64;
                item.count = 1;
                break;
            case block_type::BLOCK_GRASS:
                item.type = item_type::ITEM_DIRT;
                item.stack_count = 64;
                item.count = 1;
                break;
            case block_type::BLOCK_STONE:
                item.type = item_type::ITEM_COBBLESTONE;
                item.stack_count = 64;
                item.count = 1;
                break;
            case block_type::BLOCK_LOG:
                item.type = item_type::ITEM_LOG;
                item.stack_count = 64;
                item.count = 1;
                break;
            case block_type::BLOCK_SAND:
                item.type = item_type::ITEM_SAND;
                item.stack_count = 64;
                item.count = 1;
                break;
            case block_type::BLOCK_SNOW:
                item.type = item_type::ITEM_SNOW;
                item.stack_count = 64;
                item.count = 1;
                break;
            case block_type::BLOCK_IRON:
                item.type = item_type::ITEM_RAW_IRON;
                item.stack_count = 64;
                item.count = 1;
                break;
            case block_type::BLOCK_GOLD:
                item.type = item_type::ITEM_RAW_GOLD;
                item.stack_count = 64;
                item.count = 1;
                break;
            case block_type::BLOCK_COAL:
                item.type = item_type::ITEM_COAL;
                item.stack_count = 64;
                item.count = 1;
                break;
            case block_type::BLOCK_DIAMOND:
                item.type = item_type::ITEM_DIAMOND;
                item.stack_count = 64;
                item.count = 1;
                break;
            case block_type::BLOCK_WOOD:
                item.type = item_type::ITEM_LOG;
                item.stack_count = 64;
                item.count = 1;
                break;
            case block_type::BLOCK_LEAVES:
                if (rand() % 100 < 20) {
                    item.type = item_type::ITEM_APPLE;
                    item.stack_count = 64;
                    item.count = 2;
                } else if (rand() % 100 < 20) {
                    item.type = item_type::ITEM_LEAVES;
                    item.stack_count = 64;
                    item.count = 1;
                } else if (rand() % 100 < 20) {
                    item.type = item_type::ITEM_SAPLING;
                    item.stack_count = 0;
                    item.count = 1;
                } else if (rand() % 100 < 20) {
                    item.type = item_type::ITEM_STICK;
                    item.stack_count = 64;
                    item.count = 3;
                }

                break;
            case block_type::BLOCK_TORCH:
                item.type = item_type::ITEM_TORCH;
                item.stack_count = 64;
                item.count = 1;
                break;
            }
            return item;
        }
        void game::check_num()
        {
            if (IsKeyPressed(0x31)) {
                glog::log("info", "chossing item 0", "game");
                this->players[0].chossing_item = 0;
                Sleep(100);
            }
            if (IsKeyPressed(0x32)) {
                this->players[0].chossing_item = 1;
                Sleep(100);
            }
            if (IsKeyPressed(0x33)) {
                this->players[0].chossing_item = 2;
                Sleep(100);
            }
            if (IsKeyPressed(0x34)) {
                this->players[0].chossing_item = 3;
                Sleep(100);
            }
            if (IsKeyPressed(0x35)) {
                this->players[0].chossing_item = 4;
                Sleep(100);
            }
            if (IsKeyPressed(0x36)) {
                this->players[0].chossing_item = 5;
                Sleep(100);
            }
            if (IsKeyPressed(0x37)) {
                this->players[0].chossing_item = 6;
                Sleep(100);
            }
            if (IsKeyPressed(0x38)) {
                this->players[0].chossing_item = 7;
                Sleep(100);
            }
            if (IsKeyPressed(0x39)) {
                this->players[0].chossing_item = 8;
                Sleep(100);
            }

            // Sleep(100);
        }
        block game::get_block_by_item(item item)
        {
            block block;
            block.type = block_type::BLOCK_AIR;
            block.data = NULL;
            block.data_size = 0;
            switch (item.type) {
            case item_type::ITEM_DIRT:
                block.type = block_type::BLOCK_DIRT;
                break;
            case item_type::ITEM_COBBLESTONE:
                block.type = block_type::BLOCK_STONE;
                break;
            case item_type::ITEM_LOG:
                block.type = block_type::BLOCK_LOG;
                break;
            case item_type::ITEM_SAND:
                block.type = block_type::BLOCK_SAND;
                break;
            case item_type::ITEM_TORCH:
                block.type = block_type::BLOCK_TORCH;
                break;
            }
            return block;
        }
        item get_item_by_type(item_type type)
        {
            item item;
            switch (type) {
            default:
                item.type = type;
                item.stack_count = 64;
                item.count = 1;
                item.data = NULL;

                break;
            }
            return item;
        }
        void game::check_craft()
        {
            int len = craft_table::craft_table::get_len();
            int found = 0;
            for (int i = 0; i < len; i++) {
                int craft_table_flag = 1;
                if (craft_table::craft_table::craft_tables[i].result.type == item_type::ITEM_AIR) {
                    craft_table_flag = 0;
                    continue;
                }
                /*  craft_table::craft_table tmp = craft_table::craft_table::craft_tables[i];
                  int c = 0;*/
                if (craft_table::craft_table::craft_tables[i].is_order == true) {
                    for (int j = 0; j < 9; j++) {
                        int j_1 = 0;
                        switch (j) {
                        case 0:
                            j_1 = 0;
                            break;
                        case 1:
                            j_1 = 1;
                            break;
                        case 3:
                            j_1 = 2;
                            break;
                        case 4:
                            j_1 = 3;
                            break;
                        default:
                            j_1 = -1;
                            break;
                        }
                        if (j_1 == -1 && craft_table::craft_table::craft_tables[i].result.type != item_type::ITEM_AIR) {
                            craft_table_flag = 0;
                            break;
                        }
                        if (j_1 == -1) {
                            continue;
                        }
                        if (this->craft_table[j_1].type != craft_table::craft_table::craft_tables[i].items[j].type) {
                            craft_table_flag = 0;
                            break;
                        }
                        // craft_table_flag = 1;
                    }
                } else {

                    std::vector<item> temp_items;
                    for (int j = 0; j < 4; j++) {
                        if (this->craft_table[j].type != item_type::ITEM_AIR) {
                            temp_items.push_back(this->craft_table[j]);
                        }
                    }
                    int get_item = 0;
                    for (int j = 0; j < 9; j++) {
                        if (temp_items.size() == 0) {
                            if (get_item == craft_table::craft_table::craft_tables[i].need_count) {
                                break;
                            } else {
                                craft_table_flag = 0;
                                break;
                            }
                        }
                        if (craft_table::craft_table::craft_tables[i].items[j].type == item_type::ITEM_AIR) {
                            continue;
                        }

                        int flag = 0;
                        for (int k = 0; k < temp_items.size(); k++) {

                            if (temp_items[k].type == craft_table::craft_table::craft_tables[i].items[j].type) {
                                if (temp_items[k].count >= craft_table::craft_table::craft_tables[i].items[j].count) {
                                    flag = 1;
                                    get_item++;
                                    temp_items.erase(temp_items.begin() + k);
                                    break;
                                }
                            }
                        }
                        if (flag == 0) {
                            craft_table_flag = 0;
                            break;
                        }
                        if (j == 8 && get_item < craft_table::craft_table::craft_tables[i].need_count) {
                            craft_table_flag = 0;
                            break;
                        }
                        // craft_table_flag = 1;
                    }
                }
                if (craft_table_flag == 1) {
                    this->now_craft_table = &craft_table::craft_table::craft_tables[i];
                    this->craft_table[4] = craft_table::craft_table::craft_tables[i].result;
                    found = 1;
                    break;
                }
            }
            if (found == 0) {
                this->now_craft_table = NULL;
                this->craft_table[4].type = item_type::ITEM_AIR;
                this->craft_table[4].count = 0;
                this->craft_table[4].stack_count = 0;
                this->craft_table[4].data = NULL;
            }
        }