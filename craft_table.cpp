#include "craft_table.hpp"
namespace craft_table
{
    craft_table::craft_table()
    {
        this->next = NULL;
        this->is_order = 0;
        for (int i = 0; i < 9; i++)
        {
            this->items[i].type = item_type::ITEM_AIR;
            this->items[i].count = 0;
            this->items[i].stack_count = 0;
        }
        this->result.type = item_type::ITEM_AIR;
        this->result.count = 0;
        this->result.stack_count = 0;
    }
    int craft_table::init()
    {
        craft_table::craft_tables = new craft_table;
        craft_table::len = 1;
    }
    craft_table *craft_table::craft_tables;
    int craft_table::len;
    craft_table &craft_table::operator[](int index)
    {
        craft_table *table = craft_table::craft_tables;
        for (int i = 0; i < index; i++)
        {
            if (table->next == NULL)
            {
                glog::log("error", "craft_table index out of range", "craft_table");
                return *table;
            }
            table = table->next;
        }
        return *table;
    }
    int craft_table::add_crft_table(craft_table *table)
    {
        craft_table *t = craft_table::craft_tables;
        while (t->next != NULL)
        {
            t = t->next;
        }
        t->next = table;
        craft_table::len++;
    }
    int craft_table::get_len()
    {
        int len = 0;
        craft_table *t = craft_table::craft_tables;
        while (t != NULL)
        {
            t = t->next;
            len++;
        }
        craft_table::len = len;
        return len;
    }

}