/*  server.cpp

    This program receives a file from a remote client and sends back an
    acknowledgement after the entire file has been received.

    Two possible options for closing the connection:
        (1) Send a close command along with the acknowledgement and then close.
        (2) Hold the connection open waiting for additional files / commands.
        The "persistent" field in the header indicates the client's preference.

    Written by Giancarlo Gonzalez (ggonza20) January 2014 for CS 450 HW1
*/

#include "450UtilsUDP.h"

using namespace std;

char * printIP(uint32_t ip) {

    uint8_t  octet[4];
    char * ipAddress;
    int x;
    for (x = 0; x < 4; x++) {

        octet[x] = (ip >> (x * 8)) & (uint8_t)-1;
        sprintf(ipAddress,"%s%d", ipAddress, octet[x]);

        if (x != 3)
            sprintf(ipAddress, "%s.", ipAddress);

    }

    printf("%s\n", ipAddress);

    return ipAddress;

}

char * printIP(unsigned char *ip) {

    char * ipAddress;
    sprintf(ipAddress, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    return ipAddress;

}

void printHeader(CS450Header header) {

    printf("Version %d UIN %ld HW_number %d transactionNumber %d\n",
        header.version,header.UIN,header.HW_number, header.transactionNumber);
    printf("User ID %s from IP %s:%d to IP %s:%d\n", header.ACCC, printIP(header.from_IP), ntohs(header.from_Port), printIP(header.to_IP), ntohs(header.to_Port));
    if(header.packetType == 2)
        printf("Data received: %lu file saved: %d\n",
            header.nbytes, header.saveFile);
    else
        printf("Data sending: %lu saveFile: %d\n",
            header.nbytes, header.saveFile);

}

int main(int argc, char ** argv) {

    printf("Written by Giancarlo Gonzalez (ggonzale) February 2014 for CS 450 HW2\n");

    char * port = "54323";

    if(argc > 1)
        port = argv[1];

    int server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(server_sock < 0) {

        perror("Creating socket failed");
        exit(1);

    }

    int reuse_true = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_true, sizeof(reuse_true));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = INADDR_ANY;

    int res = bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    if(res < 0) {

        perror("Error binding to port.");
        exit(1);

    }

    struct sockaddr_in remote_addr;
    unsigned int addrlen = sizeof(remote_addr);

    int recv_count;
    Packet packet;
    // CS450Header header;
    while(1) {

        recv_count = recvfrom(server_sock, &packet, PacketSize, 0, (struct sockaddr *)&remote_addr, &addrlen);

        if(recv_count <= 0) {

            perror("Error receiving header.");
            exit(1);

        }

        printHeader(packet.header);

        //send acknowledgement of file received
        packet.header.packetType = 2;
        uint32_t from_IP = packet.header.to_IP;
        packet.header.to_IP = packet.header.from_IP;
        packet.header.from_IP = from_IP;
        uint16_t from_port = packet.header.to_Port;
        packet.header.to_Port = packet.header.from_Port;
        packet.header.from_Port = from_port;

        printHeader(packet.header);

        memset(&packet.data, 0, sizeof(packet.data));
        sendto(server_sock, &packet, PacketSize, 0, (struct sockaddr *)&remote_addr, addrlen);

        // pthread_t client;
        // pthread_create(&client, 9, handle_client, (void *)sock);

    }

    close(server_sock);

}
