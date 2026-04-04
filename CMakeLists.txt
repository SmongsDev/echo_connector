cmake_minimum_required(VERSION 3.28)
project(network C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_SYSTEM_PROCESSOR x86_64)

add_executable(server TCP/server.c)
add_executable(client TCP/client.c)

#configure_file(${CMAKE_SOURCE_DIR}/picture.jpg ${CMAKE_BINARY_DIR}/picture.jpg COPYONLY)

target_link_libraries(server ws2_32)
target_link_libraries(client ws2_32)
