/*  server.cpp

    This program receives a file from a remote client and sends back an
    acknowledgement after the entire file has been received.

    Two possible options for closing the connection:
        (1) Send a close command along with the acknowledgement and then close.
        (2) Hold the connection open waiting for additional files / commands.
        The "persistent" field in the header indicates the client's preference.

    Written by Giancarlo Gonzalez (ggonza20) January 2014 for CS 450 HW1
*/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include "CS450Header.h"
// #include "450Utils.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>

using namespace std;

void* handle_client(void *sock);

int main(int argc, char ** argv) {

    char * start = "Written by Giancarlo Gonzalez (ggonzale) January 2014 for CS 450 HW1";

    printf("%s\n", start);

    char * port = "54321";

    if(argc > 1)
        port = argv[1];

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) {

        perror("Creating socket failed: ");
        exit(1);

    }

    int reuse_true = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_true, sizeof(reuse_true));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));
    addr.sin_addr.s_addr = INADDR_ANY; // listen to all interfaces

    int res = bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    if(res < 0) {
        perror("Error binding to port");
        exit(1);
    }

    struct sockaddr_in remote_addr;
    unsigned int socklen = sizeof(remote_addr);

    // wait for a connection
    res = listen(server_sock,0);
    if(res < 0) {
        perror("Error listening for connection");
        exit(1);
    }

    while(1) {

        int sock;
        sock = accept(server_sock, (struct sockaddr*)&remote_addr, &socklen);
        if(sock < 0) {
            perror("Error accepting connection");
            exit(1);
        }

        // printf("%d\n", sock);

        pthread_t client;
        pthread_create(&client,0,handle_client,(void*)sock);
    }

    shutdown(server_sock,SHUT_RDWR);

}

void to_IP(uint32_t ipAddress) {

    uint8_t  octet[4];
    //string ret = "";
    int x;
    for (x = 0; x < 4; x++)
    {
        octet[x] = (ipAddress >> (x * 8)) & (uint8_t)-1;
        printf("%d", octet[x]);
        if (x != 3) printf(".");
    }
    // return ret;

}

void printHeader(CS450Header header) {

    printf("Version %d UIN %ld HW_number %d transactionNumber %d\n",
        header.version,header.UIN,header.HW_number, header.transactionNumber);
    printf("User ID %s filename %s from IP ", header.ACCC, header.filename);
    to_IP(header.from_IP);
    printf(":%d to IP ", header.from_Port);
    to_IP(header.to_IP);
    printf(":%d\n", header.to_Port);
    if(header.packetType == 2)
        printf("Data received %lu persistent %d file saved %d\n",
            header.nbytes, header.persistent, header.saveFile);
    else
        printf("Data sending %lu persistent %d saveFile %d\n",
            header.nbytes, header.persistent, header.saveFile);

}

void * handle_client(void * arg) {

    int sock = (intptr_t) arg;
    //receive header
    CS450Header header;
    //logging where connection came from and how much data waiting to receive
    // string ipAddress = to_IP(header.from_IP);

    while(1) {

        int recv_count = recv(sock,&header,sizeof(CS450Header),0);

        printHeader(header);

        // printf("Receiving %lu bytes from ", header.nbytes);
        // to_IP(header.from_IP);
        // printf("...\n");

        //receive data from client
        char data[10000];
        int recv_accum = 0;
        ofstream file;

        if(header.saveFile)
            file.open(header.filename);

        while(recv_accum < header.nbytes) {

            memset(&data, 0, sizeof data);
            int recv_count = recv(sock, data, sizeof(data), 0);
            if(recv_count<0) {

                perror("Receive failed");
                exit(1);

            }
            recv_accum+=recv_count;
            //if header.saveFile non-zero write data to file
            if(header.saveFile) {

                file << data;

            }
            // memset(&data, 0, sizeof(data));

        }

        if(recv_accum == header.nbytes)
            printf("\tData transfer of %d bytes successful...\n", recv_accum);

        if(header.saveFile)
            file.close();
        //send acknowledgement of file received
        header.packetType = 2;
        uint32_t from_IP = header.to_IP;
        header.to_IP = header.from_IP;
        header.from_IP = from_IP;
        uint16_t from_port = header.to_Port;
        header.to_Port = header.from_Port;
        header.from_Port = from_port;

        printHeader(header);

        if(send(sock,&header,sizeof(CS450Header),0) < 0)
            perror("send");

        if(!header.persistent)
            break;
    }

    close(sock);
    return 0;

}
