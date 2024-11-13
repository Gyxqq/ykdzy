enum block_type
{
    BLOCK_DIRT,              // 泥土
    BLOCK_GRASS,             // 草地
    BLOCK_STONE,             // 石头
    BLOCK_WOOD,              // 木头
    BLOCK_LEAVES,            // 树叶
    BLOCK_WATER,             // 水
    BLOCK_SAND,              // 沙子
    BLOCK_CACTUS,            // 仙人掌
    BLOCK_SNOW,              // 雪
    BLOCK_ICE,               // 冰
    BLOCK_LAVA,              // 岩浆
    BLOCK_COAL,              // 煤炭
    BLOCK_IRON,              // 铁
    BLOCK_GOLD,              // 金
    BLOCK_DIAMOND,           // 钻石
    BLOCK_EMERALD,           // 绿宝石
    BLOCK_COPPER,            // 铜
    BLOCK_RAINBOW,           // 彩虹
    BLOCK_CLOUD,             // 云
    BLOCK_SKY,               // 天空
    BLOCK_VOID,              // 虚空
    BLOCK_BEDROCK,           // 基岩
    BLOCK_CHEST,             // 箱子
    BLOCK_FURNACE,           // 熔炉
    BLOCK_CRAFTING_TABLE,    // 工作台
    BLOCK_ANVIL,             // 铁砧
    BLOCK_ENCHANTMENT_TABLE, // 附魔台
    BLOCK_BEACON,            // 信标
    BLOCK_TORCH,             // 火把
    BLOCK_LANTERN,           // 灯笼
    BLOCK_BED,               // 床
    BLOCK_CAKE,              // 蛋糕
    BLOCK_FLOWER,            // 花
    BLOCK_PLANT,             // 植物
    BLOCK_SAPLING,           // 树苗
    BLOCK_LOG,               // 木头
    BLOCK_MUSHROOM,          // 蘑菇
    BLOCK_GLASS,             // 玻璃
};
enum item_type
{
    ITEM_STICK,          // 木棍
    ITEM_WOOD,           // 木头
    ITEM_COAL,           // 煤炭
    ITEM_RAW_IRON,       // 铁矿
    ITEM_INGOT_IRON,     // 铁锭
    ITEM_RAW_GOLD,       // 金矿
    ITEM_INGOT_GOLD,     // 金锭
    ITEM_DIAMOND,        // 钻石
    ITEM_EMERALD,        // 绿宝石
    ITEM_COPPER,         // 铜
    ITEM_CACTUS,         // 仙人掌
    ITEM_CHEST,          // 箱子
    ITEM_FURNACE,        // 熔炉
    ITEM_CRAFTING_TABLE, // 工作台
    ITEM_GLASS,          // 玻璃
};
enum chunk_type
{
    CHUNK_GRASSLAND, // 草地
    CHUNK_DESERT,    // 沙漠
    CHUNK_SNOW,      // 雪地
    CHUNK_FOREST,    // 森林
    CHUNK_MOUNTAIN,  // 山地
    CHUNK_OCEAN,     // 海洋
    CHUNK_LAVA,      // 火山
    CHUNK_VOID,      // 虚空
};
class item
{
public:
    item_type type;
    int stack_count;
    int count;
    void *data;
};
class block
{
public:
    block_type type;
    int x;
    int y;
    void *data;
};

class chunk
{
public:
    int x;
    int y;
    chunk_type type;
    block *blocks;
};