//
// Created by 20213107 신성민 on 2026-04-01.
//
#include <stdint.h>
#include <stdio.h>
#include <winsock2.h>

int main() {
    WSADATA wsaData;

    // Winsock 초기화
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 소켓 생성
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    // 서버 주소 설정
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9090);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 서버 연결
    connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    printf("[*] Server connect!\n");

    // 사진 수신
    {
        // 크기 수신
        u_long size = 0;
        if (recv(sock, (char *)&size, 4, 0) == SOCKET_ERROR){
            printf("[*] recv failed!\n");
            return 1;
        }

        u_long i_size = ntohl(size);
        char *img_data = (char *)malloc(i_size);
        int total_size = 0;
        while(total_size < i_size) {
            int n = recv(sock, img_data + total_size, i_size - total_size, 0);
            if (n == SOCKET_ERROR) {
                printf("[!] recv failed!\n");
                return 1;
            }
            total_size += n;
            printf("%ld\n", total_size);
        }
        FILE *img = fopen("../Gift.jpg", "wb");
        fwrite(img_data, total_size, 1, img);
        fclose(img);
        printf("[*] Image Get!\n");

        free(img_data);
    }

    // 소켓 종료
    closesocket(sock);
    WSACleanup();

    return 0;
}