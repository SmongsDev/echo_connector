cmake_minimum_required(VERSION 3.28)
project(network C)

set(CMAKE_C_STANDARD 99)

set(CMAKE_SYSTEM_PROCESSOR x86_64)

add_executable(t_server TCP/tcp_server.c)
add_executable(t_client TCP/tcp_client.c)
add_executable(u_server UDP/udp_server.c)
#add_executable(u_client UDP/udp_client.c)

configure_file(${CMAKE_SOURCE_DIR}/picture.jpg ${CMAKE_BINARY_DIR}/picture.jpg COPYONLY)

target_link_libraries(t_server ws2_32)
target_link_libraries(t_client ws2_32)
target_link_libraries(u_server ws2_32)
#target_link_libraries(u_client ws2_32)
