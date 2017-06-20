cmake_minimum_required(VERSION 3.6)
project(ex5)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(SOURCE_FILES ../main.cpp ServerSide/ClientInfo.cpp ServerSide/ClientInfo.h ServerSide/Group.cpp ServerSide/Group.h ServerSide/WhatsappServer.h ServerSide/WhatsappServer.cpp)
add_executable(ex5 ${SOURCE_FILES})