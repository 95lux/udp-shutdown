#include "network.h"

SOCKET InitializeSocket(int port, std::string shutdown_cmd) {
    WSADATA wsaData;
    SOCKET s;
    struct sockaddr_in server;

    // Initialize winsock
    printf("[ INFO ] Initialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
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
    if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("[ ERR ] Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("[ OK ] Bind done\n");
    printf("[ INFO ] Listening for shutdown command: \"%s\" on port :%i\n", shutdown_cmd.c_str(), port);

    return s;
}

void CloseSocket(SOCKET s) {
    // Close the socket
    closesocket(s);

    // Cleanup winsock
    WSACleanup();
}