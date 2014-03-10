/*  server.cpp

    This program receives a file from a remote client and sends back an
    acknowledgement after the entire file has been received.

    Two possible options for closing the connection:
        (1) Send a close command along with the acknowledgement and then close.
        (2) Hold the connection open waiting for additional files / commands.
        The "persistent" field in the header indicates the client's preference.

    Written by Giancarlo Gonzalez (ggonza20) January 2014 for CS 450 HW1
*/

#include "450UtilsUDP.cpp"
#include <fstream>

int sendNack(CS450Header * header, int server_sock, struct sockaddr_in remote_addr, unsigned int addrlen) {

    Packet * packet;

    char * to_IP, * from_IP;
    to_IP = printIP(header->from_IP);
    from_IP = printIP(header->to_IP);

    packet = makePacket(to_IP, from_IP, header->filename, header->to_Port, header->from_Port, 2, 0, header->nTotalBytes, 0);

    packet->header.transactionNumber = header->transactionNumber;
    packet->header.ackNumber = 0;
    packet->header.packetType = 2;

    printHeader(packet->header);

    // memset(&packet.data, 0, sizeof(packet.data));
    if(sendto(server_sock, packet, PacketSize, 0, (struct sockaddr *)&remote_addr, addrlen) < 0) {

        printf("Ack not sent.");
        return 0;

    }
    else
        return 1;

    free(to_IP);
    free(from_IP);

}

int sendAck(CS450Header * header, int server_sock, struct sockaddr_in remote_addr, unsigned int addrlen) {

    Packet * packet;

    char * to_IP, * from_IP;
    to_IP = printIP(header->from_IP);
    from_IP = printIP(header->to_IP);

    packet = makePacket(to_IP, from_IP, header->filename, header->to_Port, header->from_Port, 2, 0, header->nTotalBytes, 0);

    packet->header.transactionNumber = header->transactionNumber;
    packet->header.ackNumber = 1;

    printHeader(packet->header);

    // memset(&packet.data, 0, sizeof(packet.data));
    if(sendto(server_sock, packet, PacketSize, 0, (struct sockaddr *)&remote_addr, addrlen) < 0) {

        printf("Ack not sent.");
        return 0;

    }
    else
        return 1;

    free(to_IP);
    free(from_IP);

    // printf("WTF\n");

}

void handlePacket(CS450Header header, char * data) {

    printf("WTF!!!!\n");

    if(header.saveFile) {

        char * name;
        name = "server_file.cpp";
        // sprintf(name,, header.filename);
        printf("%s\n", name);
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
    // int data_length = 0, recv_accum = 0;
    // CS450Header header;
    uint32_t dont_handle = 0;
    while(1) {

        recv_count = recvfrom(server_sock, &packet, PacketSize, 0, (struct sockaddr *)&remote_addr, &addrlen);

        if(recv_count <= 0) {

            perror("Error receiving packet.");
            exit(1);

        }

        printHeader(packet.header);

        data = (char *)calloc(packet.header.nTotalBytes, sizeof(char));

        strncpy(data, packet.data, packet.header.nbytes);
        // data_length = strlen(data);

        sendAck(&packet.header, server_sock, remote_addr, addrlen);
        // printf("Data received: %d\n", data_length);

        if(packet.header.nTotalBytes == packet.header.nbytes)
            handlePacket(packet.header, data);
        else {

            // recv_accum = data_length;
            // printf("Data received: %d\n", packet.header.nTotalBytes);
            while(strlen(data) < packet.header.nTotalBytes) {

                recv_count = recvfrom(server_sock, &packet, PacketSize, 0, (struct sockaddr *)&remote_addr, &addrlen);

                printHeader(packet.header);
                strncpy((data+strlen(data)), packet.data, packet.header.nbytes);
                // data_length = strlen(data);
                // printf("Length data: %d\n", strlen(data));

                dont_handle = calcChecksum((void *)&packet, PacketSize);
                printf("Checksum: %d\n", dont_handle);
                if(dont_handle == 0)
                    sendAck(&packet.header, server_sock, remote_addr, addrlen);
                else {

                    sendNack(&packet.header, server_sock, remote_addr, addrlen);
                    dont_handle = 1;
                    break;

                }

                // recv_accum = strlen(data);
                printf("Data received: %d\n", strlen(data));

            }

            if(dont_handle == 0)
                handlePacket(packet.header, data);

        }

        free(data);

    }

    close(server_sock);

}
