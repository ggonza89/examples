#include <stdio.h>
#include <cstring>
#include <arpa/inet.h>
#include "client-udpconnection.h"

ClientUDPConnection::ClientUDPConnection() {
	printf("Client connection created\n");
}

ClientUDPConnection::ClientUDPConnection(int port, char * relay_ip, char * server_ip, int s_port) {

	servaddr.sin_port = htons(port);
    this->relay = relay_ip;
    this->server = server_ip;
    this->server_port = s_port;
    char ** addr_list;
    char * address;
    address = relay_ip;
    printf("wtf\n");
    int iter = 0;
    addr_list = getIpAddress(address);
    while(inet_aton(address, &servaddr.sin_addr) == 0) {

        try {

            address = printIP((unsigned char *)addr_list[iter]);
            printf("Trying address %s of host %s\n", address, relay);
            ++iter;

        }
        catch(...) {

            printf("inet_aton() failed on host %s\n", relay);
            exit(1);

        }

    }
    strcpy(relay, address);
	printf("Client connection created\n");

}

int ClientUDPConnection::onSocketCreated() {

    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cliaddr.sin_port = htons(atoi("54323"));

    // if(::bind(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) < 0) {
    //     perror("client: can't bind local address");
    //     return -1;
    // }

    // printf("client: binding success\n");
    return 0;
}

ClientUDPConnection::~ClientUDPConnection() {
	printf("Client connection destroyed\n");
}
