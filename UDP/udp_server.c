//
// Created by 20213107 신성민 on 2026-04-04.
//
#include <stdio.h>
#include <winsock2.h>

#define PORT 9091
#define CHUNK 4096

int main() {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
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
    addr_s.sin_addr.s_addr = INADDR_ANY;
    addr_s.sin_port = htons(PORT);

    // 소켓 연결
    {
        if (bind(sock, (struct sockaddr *)&addr_s, sizeof(addr_s)) == SOCKET_ERROR) {
            printf("Failed binding\n");
            return 1;
        }
    }

    {
        struct sockaddr_in addr_c;
        int addr_c_len = sizeof(addr_c);

        // 크기 수신
        u_long size = 0;
        if (recvfrom(sock, (char *)&size, sizeof(size), 0, (struct sockaddr *)&addr_c, &addr_c_len) == SOCKET_ERROR){
            printf("Failed recvfrom\n");
            return 1;
        }
        u_long i_size = ntohl(size);
        printf("File size: %lu bytes\n", i_size);

        char *img_data = (char *)malloc(i_size);
        int total_size = 0;
        int chunk_idx = 0;
        while (total_size < i_size) {
            int n = recvfrom(sock, img_data + total_size, CHUNK, 0, (struct sockaddr *)&addr_c, &addr_c_len);
            if (n == SOCKET_ERROR) {
                printf("Failed recvfrom\n");
                return 1;
            }
            total_size += n;
            chunk_idx++;
            printf("청크 %d: %d bytes 수신, 누적: %d / %lu\n", chunk_idx, n, total_size, i_size);
        }
        printf("Chunk size Finish: %d\n", chunk_idx);

        // 사진 저장
        FILE *img = fopen("Gift_udp.jpg", "wb");
        if (img == NULL) {
            printf("Failed opening file\n");
            free(img_data);
            return 1;
        }
        fwrite(img_data, total_size, 1, img);
        fclose(img);
        printf("Image Get\n");

        free(img_data);
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}