#pragma once
#include "craft_table.hpp"
namespace craft_table {
craft_table::craft_table()
{
    memset(this, 0, sizeof(craft_table));
}

std::vector<craft_table> craft_table::craft_tables;
int craft_table::len;

int craft_table::add_craft_table(craft_table* table)
{
    craft_table::craft_tables.push_back(*table);
    return 0;
}
int craft_table::get_len()
{
    craft_table::len = craft_table::craft_tables.size();
    return craft_table::len;
}
int craft_table::init()
{

    craft_table* table;

    {
        table = new craft_table();
        memset(table, 0, sizeof(craft_table));
        table->items[0].type = item_type::ITEM_STICK;
        table->items[0].count = 1;
        table->items[0].stack_count = 64;
        table->items[1].type = item_type::ITEM_COAL;
        table->items[1].count = 1;
        table->items[1].stack_count = 64;
        table->result.type = item_type::ITEM_TORCH;
        table->result.count = 4;
        table->result.stack_count = 64;
        table->need_count = 2;
        table->is_order = 0;
        table->add_craft_table(table); // 火把
    }
    {
        table = new craft_table();
        memset(table, 0, sizeof(craft_table));
        table->items[0].type = item_type::ITEM_LOG;
        table->items[0].count = 1;
        table->items[0].stack_count = 64;
        table->result.type = item_type::ITEM_WOOD;
        table->result.count = 4;
        table->result.stack_count = 64;
        table->need_count = 1;
        table->is_order = 0;
        table->add_craft_table(table); // 木头合成木板
    }
    {
        memset(table, 0, sizeof(craft_table));
        table = new craft_table();
        table->items[0].type = item_type::ITEM_WOOD;
        table->items[0].count = 1;
        table->items[0].stack_count = 64;
        table->items[3].type = item_type::ITEM_WOOD;
        table->items[3].count = 1;
        table->items[3].stack_count = 64;
        table->result.type = item_type::ITEM_STICK;
        table->result.count = 4;
        table->result.stack_count = 64;
        table->need_count = 2;
        table->is_order = 1;
        table->add_craft_table(table); // 木板合成木棍
    }
    glog::log("info", "start Log", "craft log");
    int len = craft_table::craft_table::get_len();
    glog::log("info", "craft_table len: " + std::to_string(len), "craft log");
    for (int i = 0; i < len; i++) {
        glog::log("info", "craft_table[" + std::to_string(i) + "]: ", "craft log");
        glog::log("info", "items: ", "craft log");
        int j = craft_table::craft_table::craft_tables[i].need_count;
        glog::log("info", "need_count: " + std::to_string(j), "craft log");
        for (int k = 0; k < 9; k++) {
            glog::log("info",
                "type: " + std::to_string(craft_table::craft_table::craft_tables[i].items[k].type) + " count: " + std::to_string(craft_table::craft_table::craft_tables[i].items[k].count),
                "craft log");
        }
        glog::log("info",
            "result: type: " + std::to_string(craft_table::craft_table::craft_tables[i].result.type) + " count: " + std::to_string(craft_table::craft_table::craft_tables[i].result.count),
            "craft log");
    }

    return 0;
}
int craft_table::deinit()
{

    return 0;
}
}