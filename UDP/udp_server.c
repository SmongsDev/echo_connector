//
// Created by 20213107 신성민 on 2026-04-04.
//
#include <stdio.h>
#include <winsock2.h>

int main() {
    WSADATA wsaData;

    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("Failed WSAStartup\n");
        return 1;
    }
    printf("Successful WSAStartup\n");

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("socket creation failed\n");
        return 1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(1234);


}