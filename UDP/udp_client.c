//
// Created by sssm0 on 2026-04-08.
//
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define PORT 9992
#define CHUNK 500

struct Udp_packet {
    int idx;
    int data_len;
    char data[CHUNK];
};

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed WSAStartup\n");
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == INVALID_SOCKET) {
        printf("Failed to create socket\n");
        return 1;
    }

    struct sockaddr_in addr_s;
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = inet_addr("192.168.45.128");
    addr_s.sin_port = htons(PORT);

    // 사진 송신
    {
        FILE *file = fopen("pic.jpg", "rb");
        if (file == NULL) {
            printf("Failed to open file\n");
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
            printf("Failed to send size\n");
            return 1;
        }

        u_long offset = 0;
        int chunk_idx = 0;
        while (offset < size) {
            struct Udp_packet up;
            up.idx = chunk_idx;
            up.data_len = (size - offset) < CHUNK ? (size - offset) : CHUNK;
            memcpy(up.data, buffer + offset, up.data_len);
            if (sendto(sock, (char *)&up, sizeof(up), 0, (struct sockaddr *)&addr_s, sizeof(addr_s)) == SOCKET_ERROR) {
                printf("Failed to send img: %d\n", chunk_idx);
                return 1;
            }
            offset += up.data_len;
            chunk_idx++;
            Sleep(1000);
        }
        free(buffer);
        printf("Image send\n");
    }

    // 소켓 종료
    closesocket(sock);
    WSACleanup();
    return 0;
}