#include <winsock2.h>

#include "../include/config.h"
#include <direct.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

// #pragma comment(lib, "ws2_32.lib") // Winsock Library

#define BUFLEN 512 // Max length of buffer

BOOL systemShutdown() {
    printf("[ INFO ] Shutting down Windows... ");
    WinExec("shutdown -s -t 0", SW_HIDE);
    Sleep(500); // Works without this but it's safer to use sleep
    // KillProcessTree("winlogon"); // Internal process killer you can use pskill64
    WinExec("pskill64 winlogon -t -nobanner /accepteula", SW_HIDE);
    exit(-10); // Planned Shutdown Code
}

// int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, int nCmdShow) {
int main() {
    HWND window = GetConsoleWindow();
    ShowWindow(window, SW_MINIMIZE);
    // int window = ShowWindow(GetConsoleWindow(), SW_HIDE);
    // configure config
    Config *config = new Config();
    std::string shutdown_cmd = config->shutdown_cmd;
    int port = config->port;

    // setup socket
    SOCKET s;
    struct sockaddr_in server, si_other;
    int slen, recv_len;
    char buf[BUFLEN];
    WSADATA wsa;

    slen = sizeof(si_other);

    puts("235Media udp-shutdown\n\n");

    // Initialise winsock
    printf("[ INFO ] Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("[ ERR ] Failed. Error Code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("[ OK ] Initialised.\n");

    // Create a socket
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("[ ERR ] Could not create socket : %d", WSAGetLastError());
    }
    printf("[ OK ] Socket created.\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    printf("[ INFO ] Binding...\n");
    // Bind
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("[ ERR ] Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("[ OK ] Bind done\n");
    printf("[ INFO ] Listening for shutdown command: \"%s\" on port :%i\n", shutdown_cmd.c_str(), port);
    // keep listening for data
    while (1) {
        fflush(stdout);

        // clear the buffer by filling null, it might have previously received data
        memset(buf, '\0', BUFLEN);

        // try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == SOCKET_ERROR) {
            printf("[ ERR ] recvfrom() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        // print details of the client/peer and the data received
        printf("[ INFO ] Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("[ INFO ] Data: %s\n", buf);

        // exec shutdown if received data equals shutdown_cmd

        if (strcmp(buf, shutdown_cmd.c_str()) == 0) {
            systemShutdown();
        }
    }
    closesocket(s);
    WSACleanup();
    return 0;
}
