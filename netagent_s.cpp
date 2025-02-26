#include "netagent.h"

void NetServer::init() {
    ClientSockets.set_rollingID(10);
    WSADATA wsaData;
    int iResult;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;

    // Initialize Winsock

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        std::cout << "NetServer::NetServer WSAStartup failed with error: " << iResult << std::endl;
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cout << "NetServer::NetServer getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cout << "NetServer::NetServer socket failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    iResult = ::bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cout << "NetServer::NetServer bind failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cout << "NetServer::NetServer listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    // v non-blocking mode
    unsigned long ul = 1;
    int nRet = ioctlsocket(ListenSocket, FIONBIO, &ul);

    if (nRet == SOCKET_ERROR) {
        std::cout << "ListenSocket failed to put the socket into non-blocking mode\n";
        // Failed to put the socket into non-blocking mode
        return;
    }
#ifdef CONSOLE_LOGGING_NETWORKING
    std::cout << "ListenSocket in non-blocking mode\n";
#endif

    // TCP_NODELAY
    /*
    DWORD tr = TRUE;
    setsockopt(ListenSocket, IPPROTO_TCP, TCP_NODELAY, (const char *)&tr, sizeof(tr));
    */
}

NetServer::~NetServer() { // shutdown the connection since we're done
    while (ClientSockets.size() > 0) {
        int iResult = shutdown(ClientSockets.at_index(0)->socket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
            // closesocket(ClientSocket);
            // WSACleanup();
            // return;
        }
        closesocket(ClientSockets.at_index(0)->socket);
        ClientSockets.remove_index(0);
    }

    // cleanup
    WSACleanup();
    std::cout << "NetServer shutdown completed\n";
}

int NetServer::acceptNewClient() {
    ClientConnection tmp;
    // Accept a client socket
    tmp.socket = accept(ListenSocket, NULL, NULL);
    if (tmp.socket == INVALID_SOCKET) {
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            return -1;
        }
        std::cout << "NetServer::acceptNewClient (accept) failed with error: " << WSAGetLastError() << std::endl;
        // closesocket(ListenSocket);
        // WSACleanup();
        return -1;
    }

    unsigned long ul = 1;
    int nRet = ioctlsocket(tmp.socket, FIONBIO, &ul);

    if (nRet == SOCKET_ERROR)

    {
        std::cout << "Failed to put the socket into non-blocking mode\n";
        // Failed to put the socket into non-blocking mode
        return -1;
    }
#ifdef CONSOLE_LOGGING_NETWORKING
    std::cout << "socket in non-blocking mode\n";
#endif

    // TCP_NODELAY
    DWORD tr = TRUE;
    setsockopt(tmp.socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&tr, sizeof(tr));

    // No longer need server socket
    // closesocket(ListenSocket);
    return ClientSockets.push_back(tmp);
}

int NetServer::recieveData(int clientId) {
    ClientConnection *s = ClientSockets.at_id(clientId);
    if (s == nullptr)
        return recieveData_NO_CLIENT_ERR;

    int iResult = recv(ClientSockets.at_id(clientId)->socket, ClientSockets.at_id(clientId)->recieved.data, MAX_BUF_LEN, 0);

    if (iResult == -1 && WSAGetLastError() == WSAEWOULDBLOCK)
        return recieveData_NO_NEW_DATA;

    if (iResult > 0) {
        ClientSockets.at_id(clientId)->recieved.len = iResult;
        return recieveData_OK; // success
    }

    // if (iResult == 0)
    //     return recieveData_CONN_CLOSED_BY_CLIENT_ERR;

    // other errors
    std::cout << "NetServer::recieveData (recv) failed with error: " << WSAGetLastError() << " (socket is from now on closed)" << std::endl;
    iResult = shutdown(ClientSockets.at_id(clientId)->socket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        // std::cout << "shutdown for client " << clientId << " failed: " << WSAGetLastError() << std::endl;
        closesocket(ClientSockets.at_id(clientId)->socket);
    }
    ClientSockets.remove_id(clientId);
    return recieveData_CONN_CLOSED_BY_CLIENT_ERR;
}

int NetServer::sendData(int clientId, const char *data, int len) {

    int iSendResult = send(ClientSockets.at_id(clientId)->socket, data, len, 0);
    if (iSendResult == SOCKET_ERROR) {
        std::cout << "NetServer::sendData failed with error: " << WSAGetLastError() << " (socket is from now on closed)" << std::endl;
        closesocket(ClientSockets.at_id(clientId)->socket);
        ClientSockets.remove_id(clientId);
        return 1;
    }
    return 0;
}
RecievedData *NetServer::getLastData(int clientId) {
    ClientConnection *s = ClientSockets.at_id(clientId);
    if (s == nullptr)
        return nullptr;

    return &s->recieved;
}

void NetServer::closeConnection(int clientId) {
    int iResult = shutdown(ClientSockets.at_id(clientId)->socket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cout << "E @ NetServer::closeConnection... shutdown failed with error: " << WSAGetLastError() << std::endl;
        // closesocket(ClientSocket);
        // WSACleanup();
        // return;
    }
    closesocket(ClientSockets.at_id(clientId)->socket);

    ClientSockets.remove_id(clientId);
}