#pragma once
#ifndef __linux__ 

#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <iostream>

#include "FastCont.h"
// #include <stdlib.h>

#define MAX_BUF_LEN 65536

#define DEFAULT_PORT "27014"
#define DEFAULT_PORT_UDP "27016"

class NetClient;

class NetClient {
    SOCKET ConnectSocket;
    int err;

public:
    char recvbuf[MAX_BUF_LEN];
    int init_recvbuflen = MAX_BUF_LEN;
    int recvbuflen = MAX_BUF_LEN;
    NetClient(std::string);
    NetClient();
    ~NetClient();
    int init(std::string);
    void closeConnection();

    int getConnectionStatus();

    int sendData(const char *, int);
    int recieveData();
};

enum recieveData_ret {
    recieveData_SUCCESS = 0,
    recieveData_CONNECTION_CLOSED = -1
};

enum getConnectionStatus_ret {
    getConnectionStatus_GOOD = 0,
    getConnectionStatus_NO_HOST = -1,
    getConnectionStatus_ERR = -2,
};

enum recieveData_err {
    recieveData_OK = 0,
    recieveData_NO_NEW_DATA = -1,
    recieveData_CONN_CLOSED_BY_CLIENT_ERR = -2,
    recieveData_NO_CLIENT_ERR = -3,
    recieveData_ERR = -4,
};

// --------------------------------

struct ClientConnection;
class NetServer;

struct RecievedData {
    char data[MAX_BUF_LEN];
    int len = MAX_BUF_LEN;
};
struct ClientConnection {
    SOCKET socket = INVALID_SOCKET;
    RecievedData recieved;
};
class NetServer {
    SOCKET ListenSocket = INVALID_SOCKET;
    FastCont<ClientConnection> ClientSockets;

public:
    ~NetServer();
    void init();

    int acceptNewClient(); // return: -1 no_new_connection/error,  >= 0 ID of new accepted client
    int recieveData(int);
    int sendData(int, const char *, int);
    RecievedData *getLastData(int);

    void closeConnection(int);
};

// TODO unused, ne dela se, zgruntej kako client-server ID bo delu, nism se testiru nic src=https://www.binarytides.com/udp-socket-programming-in-winsock/
struct ClientData {
    RecievedData recieved;
    string ip;
    int port;
};
class NetServerUDP {
    FastCont<ClientData> clientData;
    SOCKET ListenSocket = INVALID_SOCKET;

public:
    ~NetServerUDP();
    void init();
    int recieveData(bool *);
    int sendData(int, const char *, int);

    RecievedData *getLastData(int);
};

#include "netagent_c.cpp"
#include "netagent_s.cpp"
#include "netagent_s_udp.cpp"

#endif // __linux__