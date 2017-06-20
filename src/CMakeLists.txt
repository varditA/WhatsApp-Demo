cmake_minimum_required(VERSION 3.6)
project(ex5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(SOURCE_FILES main.cpp ClientInfo.cpp ClientInfo.h Group.cpp Group.h WhatsappServer.h WhatsappServer.cpp)
add_executable(ex5 ${SOURCE_FILES})