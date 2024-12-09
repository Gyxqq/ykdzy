#pragma once
#include "craft_table.hpp"
namespace craft_table {
craft_table::craft_table()
{
    this->next = NULL;
    this->is_order = 0;
    for (int i = 0; i < 9; i++) {
        this->items[i].type = item_type::ITEM_AIR;
        this->items[i].count = 0;
        this->items[i].stack_count = 0;
    }
    this->result.type = item_type::ITEM_AIR;
    this->result.count = 0;
    this->result.stack_count = 0;
    this->need_count = 0;
}

craft_table* craft_table::craft_tables;
int craft_table::len;
craft_table& craft_table::operator[](int index)
{
    craft_table* table = craft_table::craft_tables;
    for (int i = 0; i < index; i++) {
        if (table->next == NULL) {
            glog::log("error", "craft_table index out of range", "craft_table");
            return *table;
        }
        table = table->next;
    }
    return *table;
}
int craft_table::add_crft_table(craft_table* table)
{
    craft_table* t = craft_table::craft_tables;
    while (t->next != NULL) {
        t = t->next;
    }
    t->next = table;
    craft_table::len++;
    return 0;
}
int craft_table::get_len()
{
    int len = 0;
    craft_table* t = craft_table::craft_tables;
    while (t != NULL) {
        t = t->next;
        len++;
    }
    len -= 1;
    craft_table::len = len;
    return len;
}
int craft_table::init()
{
    craft_table::craft_tables = new craft_table;
    craft_table::len = 1;
    craft_table* table;

    {
        table = new craft_table;
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
        table->add_crft_table(table); // 火把
    }
    {
        table = new craft_table;
        table->items[0].type = item_type::ITEM_LOG;
        table->items[0].count = 1;
        table->items[0].stack_count = 64;
        table->result.type = item_type::ITEM_WOOD;
        table->result.count = 4;
        table->result.stack_count = 64;
        table->need_count = 1;
        table->is_order = 0;
        table->add_crft_table(table); // 木头合成木板
    }
    {
        table = new craft_table;
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
        table->add_crft_table(table); // 木板合成木棍
    }
    return 0;
}
int craft_table::deinit()
{
    craft_table* t = craft_table::craft_tables;
    craft_table* temp;
    while (t != NULL) {
        temp = t;
        t = t->next;
        delete temp;
    }
    return 0;
}
}