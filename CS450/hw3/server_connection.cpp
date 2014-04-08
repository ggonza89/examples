#include <stdio.h>
#include "server_connection.h"

ServerConnection::ServerConnection() {

    printf("Server created\n");

}

ServerConnection::ServerConnection(int port) {

    servaddr.sin_port = htons(port);
    printf("Server created\n");

}

int ServerConnection::onSocketCreated() {

    if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {

        perror("server: can't bind local address");
        return -1;

    }

    printf("server: binding success\n");
    return 0;

}

ServerConnection::~ServerConnection() {

    printf("Server destroyed\n");

}
