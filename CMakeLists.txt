cmake_minimum_required(VERSION 3.0)
project("hyts")

add_executable(hyts main.cpp)
include_directories(${CMAKE_SOURCE_DIR}/easyx/include)
target_link_libraries(hyts ${CMAKE_SOURCE_DIR}/easyx/x64/EasyXa.lib)
