## mod 文件夹架构
### modname.json
```json
{
    "path": "testmod\\testmod.dll",
    "name": [
        "test_fun",
        "test_fun2"
    ]
}
```
name为mod中的函数名，path为mod编译出的动态链接库的路径
### modname.cpp
模组主文件
其中有数个function，每个function对应一个modname.json中的name
function的参数为指向当前chunk的指针，chunk的结构如下
```cpp
class chunk
{
public:
    int x;
    chunk_type type;
    block *blocks;
    int block_count;
    ~chunk();
};
```
block结构如下
```cpp
class block
{
public:
    block_type type;
    int x;
    int y;
    void *data;
    int data_size;
    ~block();
};
```
block_type和chunk_type为枚举类型，分别表示方块类型和区块类型，定义可以在map.h中找到
data为指向数据的指针，data_size为数据大小
blocks[i+j*BLOCKS_PER_CHUNK_X]表示y为j，x为i的方块
坐标轴方向为x右为正，y上为正
mod 的主要功能是对chunk进行操作，增加、删除、修改block来生成特定的自定义的结构
请保证不要出现内存错误，否则会导致游戏崩溃
chunk的大小也可以在map.h中找到
### build
#### 编译环境
> vs2017以上+cmake

使用cmake配置项目
```cmake
cmake_minimum_required(VERSION 3.0)
project("testmod")
set(WIN32 TRUE)
add_library(testmod SHARED testmod.cpp)
```
其中testmod.cpp为modname.cpp的一个示例
运行build.bat即可编译出动态链接库
dll可以在build/Debug中找到
比如说testmod.dll
### 打包
将dll按照modname.json中的路径放入对应文件夹或者根目录
注意一个mod只能有一个dll
然后将mod文件夹打包成zip文件



