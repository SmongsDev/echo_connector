//
// Created by sssm0 on 2026-04-08.
//
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 9091
#define CHUNK 4096

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed WSAStartup\n");
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        printf("Failed creating socket\n");
        return 1;
    }

    struct sockaddr_in addr_s;
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_s.sin_port = htons(PORT);

    // 사진 송신
    {
        FILE *file = fopen("pic.jpg", "rb");
        if (file == NULL) {
            printf("Failed opening file\n");
            return 1;
        }

        fseek(file, 0, SEEK_END);
        const u_long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *buffer = (char *)malloc(size);
        fread(buffer, size, 1, file);
        fclose(file);

        // 크기 송신
        u_long n_size = htonl(size);
        if (sendto(sock, (char *)&n_size, sizeof(n_size), 0, (struct sockaddr *)&addr_s, sizeof(addr_s)) == SOCKET_ERROR) {
            printf("Failed sending size\n");
            return 1;
        }

        u_long offset = 0;
        int chunk_idx = 0;
        while (offset < size) {
            int send_size = (size - offset) < CHUNK ? (size - offset) : CHUNK;
            if (sendto(sock, buffer + offset, send_size, 0, (struct sockaddr *)&addr_s, sizeof(addr_s)) == SOCKET_ERROR) {
                printf("Failed sending img: %d\n", chunk_idx);
                return 1;
            }
            offset += send_size;
            chunk_idx++;
        }
        free(buffer);
    }

    // 소켓 종료
    closesocket(sock);
    WSACleanup();
    return 0;
}