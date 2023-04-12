#include "../include/config.h"
#include <direct.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <winsock2.h>

// #pragma comment(lib, "ws2_32.lib") // Winsock Library

#define BUFLEN 512 // Max length of buffer

BOOL systemShutdown() {
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Get a token for this process.

    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return (FALSE);

    // Get the LUID for the shutdown privilege.

    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
                         &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1; // one privilege to set
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // Get the shutdown privilege for this process.

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
                          (PTOKEN_PRIVILEGES)NULL, 0);

    if (GetLastError() != ERROR_SUCCESS)
        return FALSE;

    // Shut down the system and force all applications to close.

    if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE,
                       SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
                           SHTDN_REASON_MINOR_UPGRADE |
                           SHTDN_REASON_FLAG_PLANNED))
        return FALSE;

    // shutdown was successful
    return TRUE;
}

int main() {

    // configure config
    Config config;

    // setup socket
    SOCKET s;
    struct sockaddr_in server, si_other;
    int slen, recv_len;
    char buf[BUFLEN];
    WSADATA wsa;

    slen = sizeof(si_other);

    // Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");

    // Create a socket
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
    }
    printf("Socket created.\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(config.port);

    // Bind
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("Bind done");

    // keep listening for data
    while (1) {
        printf("Waiting for data...");
        fflush(stdout);

        // clear the buffer by filling null, it might have previously received data
        memset(buf, '\0', BUFLEN);

        // try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == SOCKET_ERROR) {
            printf("recvfrom() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }

        // print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n", buf);
        if (strcmp(buf, config.shutdown_cmd) == 0) {
            systemShutdown();
        }
        // now reply the client with the same data
        if (sendto(s, buf, recv_len, 0, (struct sockaddr *)&si_other, slen) == SOCKET_ERROR) {
            printf("sendto() failed with error code : %d", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    }
    closesocket(s);
    WSACleanup();
    return 0;
}
