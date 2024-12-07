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
        int is_order;
        item result;
        int need_count;
        craft_table &operator[](int index);
        int add_crft_table(craft_table *table);
        craft_table();
        static int init();
        static craft_table *craft_tables;
        static int len;
        int get_len();
    };
}