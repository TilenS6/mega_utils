#include "netagent.h"

NetClient::NetClient() {};
int NetClient::init(std::string server) {
    err = 0;
    WSADATA wsaData;
    ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
        *ptr = NULL,
        hints;
    int iResult;

    // Validate the parameters

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed with error:" << iResult << std::endl;
        err = getConnectionStatus_ERR;
        return -1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(server.c_str(), DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "getaddrinfo failed with error:" << iResult << std::endl;
        WSACleanup();
        err = getConnectionStatus_NO_HOST;
        return -1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            std::cout << "socket failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            err = getConnectionStatus_ERR;
            return -1;
        }

        // set timeout to 1sec
        int ReceiveTimeout = 1000;
        int e = setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&ReceiveTimeout, sizeof(int));

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            err = getConnectionStatus_ERR;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
#ifdef CONSOLE_LOGGING_NETWORKING
        std::cout << "Unable to connect to server!\n";
#endif
        WSACleanup();
        err = getConnectionStatus_ERR;
        return -1;
    }

    unsigned long ul = 1;
    int nRet = ioctlsocket(ConnectSocket, FIONBIO, &ul);

    if (nRet == SOCKET_ERROR) {
        std::cout << "ConnectSocket failed to put the socket into non-blocking mode\n";
        // Failed to put the socket into non-blocking mode
        return -1;
    }
#ifdef CONSOLE_LOGGING_NETWORKING
    std::cout << "ConnectSocket in non-blocking mode\n";
#endif

    // TCP_NODELAY
    /*
    DWORD tr = TRUE;
    setsockopt(ConnectSocket, IPPROTO_TCP, TCP_NODELAY, (const char *)&tr, sizeof(tr));
    */
    return 0;
}
NetClient::NetClient(std::string server) {
    init(server);
}

NetClient::~NetClient() {
    closeConnection();
}
void NetClient::closeConnection() {
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();
}

int NetClient::sendData(const char *data, int len) {
    int iResult = send(ConnectSocket, data, len, 0);

    if (iResult == SOCKET_ERROR) {
        std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        err = getConnectionStatus_ERR;
        return -1;
    }

#ifdef CONSOLE_LOGGING_NETWORKING
    std::cout << "Bytes Sent: " << iResult << std::endl;
#endif
    return 0;
}

int NetClient::recieveData() {
    int iResult = recv(ConnectSocket, recvbuf, MAX_BUF_LEN, 0);

    if (iResult == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
        return recieveData_NO_NEW_DATA;

    if (iResult > 0) {
        recvbuflen = iResult;
        return recieveData_OK;
    }

    if (iResult == 0) {
#ifdef CONSOLE_LOGGING_NETWORKING
        std::cout << "Connection closed\n";
#endif
        err = getConnectionStatus_ERR;
        return recieveData_ERR;
    }
#ifdef CONSOLE_LOGGING_NETWORKING
    std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
#endif
    err = getConnectionStatus_ERR;
    return recieveData_ERR;
}

int NetClient::getConnectionStatus() {
    return err;
}
