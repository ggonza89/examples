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
#include <fstream>

using namespace std;

char * printIP(uint32_t ip) {

    char * ipAddress;
    ipAddress = (char *)calloc(INET_ADDRSTRLEN, sizeof(char));

    inet_ntop(AF_INET, &ip, ipAddress, INET_ADDRSTRLEN);

    // printf("%s\n", ipAddress);

    return ipAddress;

}

char * printIP(unsigned char *ip) {

    char * ipAddress;
    sprintf(ipAddress, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    return ipAddress;

}

void printHeader(CS450Header header) {

    printf("\tVersion %d UIN %ld HW_number %d transactionNumber %d\n",
        header.version,header.UIN,header.HW_number, header.transactionNumber);
    char * to_IP, *from_IP;
    from_IP = printIP(header.from_IP);
    to_IP = printIP(header.to_IP);
    printf("\tUser ID %s from IP %s:%d to IP %s:%d\n", header.ACCC, from_IP, ntohs(header.from_Port), to_IP, ntohs(header.to_Port));
    if(header.packetType != 2)
        printf("\tData sending: %lu Size of data: %d saveFile: %d\n",
            header.nbytes, header.nTotalBytes, header.saveFile);

    printf("\n");

    free(from_IP);
    free(to_IP);

}

int sendAck(CS450Header * header, int server_sock, struct sockaddr_in remote_addr, unsigned int addrlen) {

    Packet packet;
    //send acknowledgement of file received
    packet.header.version = 6;
    packet.header.UIN = header->UIN;
    packet.header.transactionNumber = header->transactionNumber;
    packet.header.ackNumber = 1;
    uint32_t from_IP = header->to_IP;
    packet.header.to_IP = header->from_IP;
    packet.header.from_IP = from_IP;
    uint16_t from_port = header->to_Port;
    packet.header.to_Port = header->from_Port;
    packet.header.from_Port = from_port;
    packet.header.nbytes = 0;
    packet.header.nTotalBytes = 0;
    strcpy(packet.header.filename, header->filename);
    packet.header.HW_number = 2;
    packet.header.packetType = 2;
    strcpy(packet.header.ACCC, header->ACCC);

    // printHeader(packet.header);

    // memset(&packet.data, 0, sizeof(packet.data));
    if(sendto(server_sock, &packet, PacketSize, 0, (struct sockaddr *)&remote_addr, addrlen) < 0) {

        printf("Ack not sent.");
        return 0;

    }
    else
        return 1;

    // printf("WTF\n");

}

void handlePacket(CS450Header header, char * data) {

    printf("WTF!!!!\n");

    if(header.saveFile) {

        char * name = "save_file.txt";
        printf("WTF!!!\n");
        ofstream file;
        file.open(name);

        printf("WTF!!!!!!!\n");
        file << data;

        file.close();

        printf("Data of length %d saved to file %s\n", strlen(data), name);

    }

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
    char hostname[128];
    gethostname(hostname, sizeof(hostname));
    printf("%s\n", hostname);
    char * data;
    int data_length = 0, recv_accum = 0;
    // CS450Header header;
    while(1) {

        recv_count = recvfrom(server_sock, &packet, PacketSize, 0, (struct sockaddr *)&remote_addr, &addrlen);

        if(recv_count <= 0) {

            perror("Error receiving packet.");
            exit(1);

        }

        printHeader(packet.header);

        data = (char *)calloc(packet.header.nTotalBytes, sizeof(char));

        strncpy(data, packet.data, packet.header.nbytes);
        data_length = strlen(data);

        sendAck(&packet.header, server_sock, remote_addr, addrlen);
        printf("Data received: %d\n", data_length);

        if(packet.header.nTotalBytes == packet.header.nbytes)
            handlePacket(packet.header, data);
        else {

            // recv_accum = data_length;
            // printf("Data received: %d\n", packet.header.nTotalBytes);
            while(strlen(data) < packet.header.nTotalBytes) {

                recv_count = recvfrom(server_sock, &packet, PacketSize, 0, (struct sockaddr *)&remote_addr, &addrlen);

                printHeader(packet.header);
                strcpy((data+strlen(data)), packet.data);
                // data_length = strlen(data);
                // printf("Length data: %d\n", strlen(data));

                sendAck(&packet.header, server_sock, remote_addr, addrlen);

                // recv_accum = strlen(data);
                printf("Data received: %d\n", strlen(data));

            }

            handlePacket(packet.header, data);
            // recv_accum = 0;

        }

        data_length = 0;
        free(data);

    }

    close(server_sock);

}
