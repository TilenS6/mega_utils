#include "netagent.h"

NetClientUDP::NetClientUDP(int port, string ip) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        cout << "UDP socket creation failed!\n";
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Fill server address info
    servaddr.sin_family = AF_INET;                     // IPv4
    servaddr.sin_port = htons(port);                   // Server port
    servaddr.sin_addr.s_addr = inet_addr(ip.c_str());  // Server IP

    socketlen = sizeof(servaddr);
}

NetClientUDP::~NetClientUDP() {
    if (sockfd >= 0) close(sockfd);
}

void NetClientUDP::send(const char* data, int len) {
    sendto(sockfd, data, len, MSG_CONFIRM,
           (const struct sockaddr*)&servaddr, sizeof(servaddr));
}

int NetClientUDP::receive(char* buffer, int maxlen) {
    return recvfrom(sockfd, buffer, maxlen, MSG_WAITALL,
                    (struct sockaddr*)&servaddr, &socketlen);
}
