//
// Created by 20213107 신성민 on 2026-04-01.
//
#include <stdint.h>
#include <stdio.h>
#include <winsock2.h>

#define PORT 9090

int main() {
    WSADATA wsaData;

    // Winsock 초기화
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // 소켓 생성
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    // 서버 주소 설정
    struct sockaddr_in addr_s;
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr_s.sin_port = htons(PORT);

    // 서버 연결
    if (connect(sock, (struct sockaddr *)&addr_s, sizeof(addr_s)) == SOCKET_ERROR) {
        printf("Failed connect\n");
        return 1;
    }
    printf("[*] Server connect!\n");

    // 사진 송신
    {
        FILE *file = fopen("picture.jpg", "rb");
        if (file == NULL) {
            printf("Error opening file\n");
            return 1;
        }

        // 파일 크기
        fseek(file, 0, SEEK_END);
        const u_long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *buffer = (char *) malloc(size + 1);
        fread(buffer, size, 1, file);
        fclose(file);

        // 크기 송신
        u_long n_size = htonl(size);
        if (send(sock, (char *)&n_size, 4, 0) == SOCKET_ERROR) {
            printf("Failed sending image size\n");
            return 1;
        }

        if (send(sock, buffer, size, 0) == SOCKET_ERROR) {
            printf("Failed sending\n");
            return 1;
        }
        printf("Successful sending\n");

        free(buffer);
    }


    // 소켓 종료
    closesocket(sock);
    WSACleanup();

    return 0;
}