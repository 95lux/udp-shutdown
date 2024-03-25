#pragma once
#ifndef NETWORK_H
#define NETWORK_H

#include <winsock2.h>
#include <string>

extern SOCKET InitializeSocket(int port, std::string shutdown_cmd);
extern void CloseSocket(SOCKET s);

#endif // NETWORK_H
