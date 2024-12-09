#pragma once
#include "map.hpp"
#include "log.hpp"
namespace craft_table
{
    class craft_table
    {
    public:
        craft_table *next;
        item items[9];
        int is_order; // 是否有序
        item result;
        int need_count; //需要的物品种数
        craft_table &operator[](int index);
        int add_crft_table(craft_table *table);
        craft_table();
        static int init();
        static craft_table *craft_tables;
        static int len;
        static int get_len();
        static int deinit();
    };
}