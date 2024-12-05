cmake_minimum_required(VERSION 3.0)
project("testmod")
add_library(testmod SHARED testmod.cpp)
install(TARGETS testmod DESTINATION ${CMAKE_CURRENT_SOURCE_DIR})