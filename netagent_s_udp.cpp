#include "netagent.h"

void NetServerUDP::init() {
    WSADATA wsaData;
    int iResult;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;

    // Initialize Winsock

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        std::cout << "NetServerUDP::NetServerUDP WSAStartup failed with error: " << iResult << std::endl;
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;  // SOCK_STREAM;
    hints.ai_protocol = IPPROTO_UDP; // IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT_UDP, &hints, &result);
    if (iResult != 0) {
        std::cout << "NetServerUDP::NetServerUDP getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cout << "NetServerUDP::NetServerUDP socket failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    iResult = ::bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "NetServerUDP::NetServerUDP bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    /*
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "NetServer::NetServer listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }
    */

    // v non-blocking mode
    unsigned long ul = 1;
    int nRet = ioctlsocket(ListenSocket, FIONBIO, &ul);

    if (nRet == SOCKET_ERROR) {
        std::cout << "ListenSocket failed to put the socket into non-blocking mode\n";
        // Failed to put the socket into non-blocking mode
        return;
    }
    std::cout << "ListenSocket in non-blocking mode\n";
}

NetServerUDP::~NetServerUDP() { // shutdown the connection since we're done
    closesocket(ListenSocket);

    // cleanup
    WSACleanup();
    std::cout << "NetServerUDP shutdown completed\n";
}

/// @param newClient OUT: is this client new client?
/// @return ID from client
int NetServerUDP::recieveData(bool *newClient) {
    *newClient = false;
    char data[MAX_BUF_LEN];
    int len = MAX_BUF_LEN;
    int iResult = recv(ListenSocket, data, MAX_BUF_LEN, 0);

    if (iResult == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
        return recieveData_NO_NEW_DATA;

    if (iResult > 0) {
        len = iResult;
        struct sockaddr_in si_other;
        inet_ntoa(si_other.sin_addr);
        ntohs(si_other.sin_port);

#ifdef CONSOLE_LOGGING_NETWORKING
        cout << "new data from " << si_other.sin_addr.S_un.S_addr << ":" << si_other.sin_port << endl;
#endif
        return recieveData_OK; // success
    }

    // if (iResult == 0)
    //     return recieveData_CONN_CLOSED_BY_CLIENT_ERR;

    // other errors
#ifdef CONSOLE_LOGGING_NETWORKING
    std::cout << "NetServerUDP::recieveData (recv) failed with error: " << WSAGetLastError() << std::endl;
#endif
    return recieveData_ERR;
}

int NetServerUDP::sendData(int clientId, const char *data, int len) {
    string komu = "127.0.0.1";
    u_short port = stoi(DEFAULT_PORT_UDP);

    struct sockaddr_in si_other;
    si_other.sin_family = AF_INET;
    si_other.sin_port = port;
    si_other.sin_addr.S_un.S_addr = inet_addr(komu.c_str());

    int iSendResult = sendto(ListenSocket, data, len, 0, (struct sockaddr *)&si_other, sizeof(si_other));

    if (iSendResult == SOCKET_ERROR)
        return 1;

    return 0;
}
RecievedData *NetServerUDP::getLastData(int clientId) {
    ClientData *s = clientData.at_id(clientId);
    if (s == nullptr)
        return nullptr;

    return &s->recieved;
}