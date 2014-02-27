/* client.cpp

    This program transfers a file to a remote server, possibly through an
    intermediary relay.  The server will respond with an acknowledgement.

    This program then calculates the round-trip time from the start of
    transmission to receipt of the acknowledgement, and reports that along
    with the average overall transmission rate.

    Written by Giancarlo Gonzalez (ggonza20) January 2014 for CS 450 HW1
*/

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <string>

#include "CS450Header.h"
// #include "450Utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/uio.h>

using namespace std;

void to_IP(uint32_t ipAddress) {

    uint8_t  octet[4];
    // char * ret;
    int x;
    for (x = 0; x < 4; x++)
    {
        octet[x] = (ipAddress >> (x * 8)) & (uint8_t)-1;
        printf("%d", octet[x]);
        if (x != 3) printf(".");
    }
    // printf("\n");
    // return 0;

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

void printHeader(CS450Header * header) {

    printf("Version %d UIN %ld HW_number %d transactionNumber %d\n",
        header->version,header->UIN,header->HW_number, header->transactionNumber);
    printf("User ID %s filename %s from IP ", header->ACCC, header->filename);
    to_IP(header->from_IP);
    printf(":%d to IP ", header->from_Port);
    to_IP(header->to_IP);
    printf(":%d\n", header->to_Port);
    if(header->packetType == 2)
        printf("Data received %lu persistent %d file saved %d\n",
            header->nbytes, header->persistent, header->saveFile);
    else
        printf("Data sending %lu persistent %d saveFile %d\n",
            header->nbytes, header->persistent, header->saveFile);

}

void usage() {
    cout << "Usage: myclient [server] [port] [relay]...\nIf any argument is omitted, then all following arguments must also be omitted.\n";
    exit(1);
}

int main(int argc, char** argv)
{

    char * start = "Written by Giancarlo Gonzalez (ggonzale) January 2014 for CS 450 HW1";

    printf("%s\n", start);

    char * server, *server_port, *relay, *relay_port;
    struct stat sb;
    char *data;
    string filename, server_save;
    CS450Header header;
    // memset(header, 0, sizeof CS450Header);
    int fd, sock,transNumber = 0;

    struct addrinfo hints, *relay_info, *p, *servinfo;
    struct sockaddr_in* saddr;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;

    struct hostent *se;
    struct in_addr **serv_list;

    if(argc == 1) {

        relay = "127.0.0.1";
        relay_port = "54321";
        // relay = "none";

    }
    else if(argc == 3) {

        relay = argv[1];
        relay_port = argv[2];

    }
    else if(argc == 5) {

        server = argv[1];
        server_port = argv[2];

        // char servername[128];
        // gethostname(servername, sizeof servername);

        // printf("%s\n", servername);

        if ((se = gethostbyname(server)) == NULL) {  // get the host info
            herror("getserverbyname");
            return 2;
        }

        serv_list = (struct in_addr **)se->h_addr_list;

        relay = argv[3];
        relay_port = argv[4];

    }
    else {

        usage();

    }

    if ((rv = getaddrinfo(relay, relay_port, &hints, &relay_info)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for(p = relay_info; p != NULL; p = p->ai_next) {
        if ((sock = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect(sock, p->ai_addr, p->ai_addrlen) == -1) {
            close(sock);
            perror("connect");
            continue;
        }

        break; // if we get here, we must have connected successfully
    }

    if (p == NULL) {
        // looped off the end of the list with no connection
        fprintf(stderr, "failed to connect\n");
        return 1;
    }

    if(argc <= 3) {

        if ((se = gethostbyname(relay)) == NULL) {  // get the host info
            herror("getserverbyname");
            return 2;
        }

        serv_list = (struct in_addr **)se->h_addr_list;



    }

    // char hostname[128];
    // gethostname(hostname, sizeof hostname);

    char * hostname = "127.0.0.1";

    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(hostname)) == NULL) {  // get the host info
        herror("local host");
        return 2;
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    printf("%s\n", inet_ntoa(*addr_list[0]));
    printf("%s\n", relay);

    int data_sent = 0, data_elapsed = 0;
    double avg_response_rate = 0.00, responses = 0.00;

    while(1) {

        cout << "Name of file to send: ";
        getline(cin, filename);

        fd = open(filename.c_str(),O_RDONLY);
        if(fd < 0) {

            perror ("open");
            exit(1);

        }

        if(fstat (fd, &sb) < 0) {

            perror ("fstat");
            return 1;

        }

        if(!S_ISREG (sb.st_mode)) {

            fprintf (stderr, "%s is not a file\n", filename.c_str());
            exit(1);

        }

        data = (char*)mmap (0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

        if(data == MAP_FAILED) {

            perror ("mmap");
            exit(1);

        }

        if (close (fd) < 0) {

           perror ("close");
           return 1;

        }

        cout << "Would you like to save this file to server (yes/no)? ";
        getline(cin, filename);

        if(!filename.compare("yes")) {

            cout<< "\tWhat would you like to name the file on server? ";
            getline(cin, server_save);
            header.saveFile = 1;

        }
        else
            header.saveFile = 0;

        cout << "Would you like to keep this connection open (yes/no)? ";
        getline(cin, filename);

        if(!filename.compare("yes"))
            header.persistent = 1;
        else
            header.persistent = 0;

        header.version = 4;
        header.UIN = 665799950;
        header.HW_number = 1;
        header.transactionNumber = ++transNumber;
        strcpy(header.ACCC, "ggonza20");
        strcpy(header.filename, server_save.c_str());
        inet_pton(AF_INET,inet_ntoa(*addr_list[0]), &header.from_IP);
        inet_pton(AF_INET,inet_ntoa(*serv_list[0]), &header.to_IP);
        header.from_Port = atoi("54321");
        header.to_Port = atoi(relay_port);
        // to_IP(header.to_IP);
        // to_IP(header.from_IP);
        header.packetType = 1;
        // header.relayCommand
        header.nbytes = sb.st_size;
        // header.nbytes = 0;

        printHeader(header);

        //printf("bytes of data sending: %lu\n", header.nbytes);

        if((data_sent = send(sock,&header,sizeof(CS450Header),0)) <= 0)
            fprintf(stderr, "failed to send header\n");

        // data_elapsed += data_sent;

        clock_t begin = clock();
        // printf("wtf!\n");
        if((data_sent = send(sock,data,sb.st_size,0)) <= 0) {
            fprintf(stderr, "failed sending data.\n");
            close(sock);
            return 1;
        }
        // printf("%d\n",data_elapsed);
        data_elapsed += data_sent;

        if (munmap ((void*)data, sb.st_size) < 0) {

            fprintf(stderr, "munmap\n");
            return 1;

        }

        CS450Header * acknowledgement;

        if(recv(sock,acknowledgement,sizeof(CS450Header),0) <= 0) {

            fprintf(stderr,"ack not received.\n");
            close(sock);
            return 1;

        }

        printHeader(acknowledgement);

        ++responses;

        clock_t end = clock();
        double rtt = double(end-begin)/CLOCKS_PER_SEC;
        printf("Round trip time: %f\n", rtt);
        avg_response_rate += rtt;

        if(!acknowledgement->persistent)
            break;

    }

    close(sock);

    avg_response_rate /= responses;
    printf("Average response rate: %f\n", avg_response_rate);
    printf("Data sent: %d\n", data_elapsed);

    freeaddrinfo(relay_info); // all done with this structure

}
