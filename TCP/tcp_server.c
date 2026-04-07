//
// Created by 20213107 신성민 on 2026-04-01.
//
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 9090

int main() {
    WSADATA wsaData; // WSA(Windows Socket API)

    // 윈도우 소켓 초기화
    int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);// Winsock 2.2 버전 초기화
    if (iResult != 0) {
        printf("Failed WSAStartup\n");
        return 1;
    }
    printf("Successful WSAStartup\n");

    // 소켓 생성
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // IPv4, TCP 형식, TCP 프로토콜
    if (sock == INVALID_SOCKET) {
        printf("Failed creating socket\n");
        return 1;
    }
    printf("Successful creating socket\n");

    // 소켓 IP 주소와 포트 번호 할당
    struct sockaddr_in addr_s;
    addr_s.sin_family = AF_INET;
    addr_s.sin_addr.s_addr = INADDR_ANY;
    addr_s.sin_port = htons(PORT); // 네트워크 바이트 순서의 IP Port 번호로 변환

    // 소켓 연결
    {
        if (bind(sock, (struct sockaddr *) &addr_s, sizeof(addr_s)) == SOCKET_ERROR) {
            printf("Failed binding\n");
            return 1;
        }
        printf("Successful binding\n");
    }

    // 연결 대기
    {
        if (listen(sock, 5) == SOCKET_ERROR) { // backlog: 보류 중인 연결 큐의 최대 길이
            printf("Failed listening\n");
            return 1;
        }
        printf("Successful Listening\n");
    }

    // 연결 승인
    {
        struct sockaddr_in addr_c;
        socklen_t len = sizeof(addr_c);

        if ((sock = accept(sock, (struct sockaddr *) &addr_c, &len)) == SOCKET_ERROR) {
            printf("Error accepting connection\n");
            return 1;
        }
        printf("Connection accepted\n");
    }

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
        FILE *img = fopen("Gift.jpg", "wb");
        if (img == NULL) {
            printf("[!] Failed to open file for writing\n");
            free(img_data);
            return 1;
        }
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