cmake_minimum_required(VERSION 3.0)
project("hyts")
set(WIN32 TRUE)

add_executable(hyts WIN32 main.cpp map.cpp log.cpp)
include_directories(${CMAKE_SOURCE_DIR}/easyx/include ${CMAKE_SOURCE_DIR}/include )
target_link_libraries(hyts ${CMAKE_SOURCE_DIR}/easyx/x64/EasyXa.lib ${CMAKE_SOURCE_DIR}/lib/libcjson.lib )
