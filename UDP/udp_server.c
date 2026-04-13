//
// Created by 20213107 신성민 on 2026-04-04.
//
#include <stdio.h>
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

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
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
    addr_s.sin_addr.s_addr = INADDR_ANY;
    addr_s.sin_port = htons(PORT);

    // 소켓 연결
    {
        if (bind(sock, (struct sockaddr *)&addr_s, sizeof(addr_s)) == SOCKET_ERROR) {
            printf("Failed to bind\n");
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
        }

        u_long i_size = ntohl(size);
        char *img_data = (char *)malloc(i_size);
        int total_size = 0;
        while (total_size < i_size) {
            struct Udp_packet up;
            int n = recvfrom(sock, (char *)&up, sizeof(up), 0, (struct sockaddr *)&addr_c, &addr_c_len);
            if (n == SOCKET_ERROR) {
                printf("Failed recvfrom\n");
            }

            int idx = up.idx;
            total_size += n;
            printf("idx: %d\n", idx);
        }

        // 사진 저장
        FILE *img = fopen("Gift_udp.jpg", "wb");
        if (img == NULL) {
            printf("Failed to open file\n");
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