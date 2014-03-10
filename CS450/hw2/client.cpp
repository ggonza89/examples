#include "450UtilsUDP.cpp"

void usage() {
    cout << "Usage: client [server] [port] [relay] [port]...\nIf any argument is omitted, then all following arguments must also be omitted.\n";
    exit(1);
}

int sendPacket(int relay_sock, char * data, struct sockaddr_in servaddr, int datasize, int transactionNumber, Packet * packet) {

    strcpy(packet->data, data);
    packet->header.nbytes = datasize;
    packet->header.transactionNumber = transactionNumber;
    packet->header.checksum = 0;
    packet->header.checksum = calcChecksum((void*)packet, PacketSize);
    unsigned int servlen = sizeof(servaddr);

    printf("Checksum of packet: %d\n", calcChecksum((void*)packet, PacketSize));
    printHeader(packet->header);

    if(sendto(relay_sock, packet, PacketSize, 0, (struct sockaddr *)&
        servaddr, servlen) < 0) {

        perror("sendto failed");
        return 0;

    }

    Packet acknowledgment;
    if(recvfrom(relay_sock, &acknowledgment, PacketSize, 0, (struct sockaddr *)&servaddr, &servlen) < 0) {

        printf("No acknowledgment received.");
        return 0;

    }

    printHeader(acknowledgment.header);
    return datasize;

}

int sendPackets(int relay_sock, char * data, struct sockaddr_in servaddr, Packet * packet) {

    int sent_accum = 0, data_sent = 0, transactionNumber = 0;
    char * block_data;
    block_data = (char *)calloc(BLOCKSIZE, sizeof(char));
    while(sent_accum < strlen(data)) {

        memset(block_data, 0, BLOCKSIZE);
        // break data into BLOCKSIZE pieces
        if(sent_accum == 0)
            strncpy(block_data, data, BLOCKSIZE);
        else
            strncpy(block_data, (data+sent_accum), BLOCKSIZE);

        data_sent = sendPacket(relay_sock, block_data, servaddr, strlen(block_data), transactionNumber, packet);
        // if(data_sent == 0)
        //     resend data

        sent_accum += data_sent;
        printf("DATA SENT: %d\n", data_sent);
        ++transactionNumber;

    }

    free(block_data);

}

int main(int argc, char ** argv) {

    printf("Written by Giancarlo Gonzalez (ggonzale) February 2014 for CS 450 HW2\n");

    char * server, *server_port, *relay, *relay_port;

    if(argc == 1) {

        relay = "localhost";
        relay_port = "54323";
        // relay = "none";

    }
    else if(argc == 2) {

        relay = argv[1];
        relay_port = "54323";

    }
    else if(argc == 3) {

        relay = argv[1];
        relay_port = argv[2];

    }
    else if(argc == 5) {

        server = argv[1];
        server_port = argv[2];

        relay = argv[3];
        relay_port = argv[4];

    }
    else {

        usage();

    }

    int relay_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(relay_sock < 0) {

        perror("cannot create socket");
        return 1;

    }

    struct sockaddr_in myaddr;
    //bind to an arbitrary return address
    memset((char*)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(atoi("54321"));

    if( bind(relay_sock, (struct sockaddr*)&myaddr, sizeof(myaddr)) < 0 ) {

        perror("bind failed");
        return 1;

    }

    struct sockaddr_in servaddr;
    memset((char *) &servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(relay_port));
    unsigned int servlen = sizeof(servaddr);
    char ** addr_list;
    char * address;
    address = relay;
    int iter = 0;
    addr_list = getIpAddress(relay);
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

    string input, filename;
    struct stat sb;
    char * data;
    int8_t saveFile;

    // while(1) {

        cout << "Name of file to send > ";
        getline(cin, filename);

        int fd = open(filename.c_str(), O_RDONLY);
        if(fd < 0) {

            perror("Opening file");
            exit(1);

        }

        if(fstat(fd, &sb) < 0) {

            perror("fstat");
            exit(1);

        }

        if(!S_ISREG(sb.st_mode)) {

            printf("%s is not a file.\n", input.c_str());
            exit(1);

        }

        data = (char *)mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

        if(data == MAP_FAILED) {

            perror("mmap");
            exit(1);

        }

        if(close(fd) < 0) {

            perror("close");
            exit(1);

        }

        cout << "Would you like to save file (" + filename +  ") to server (yes/no)?";
        getline(cin, input);

        if(!input.compare("yes"))
            saveFile = 1;
        else
            saveFile = 0;

        // make packet

        char hostname[128];
        gethostname(hostname, sizeof(hostname));
        char ** hostip_list;
        hostip_list = getIpAddress(hostname);
        char * hostip;
        hostip = printIP((unsigned char *)hostip_list[0]);
        printf("Host ip: %s Server IP: %s\n", hostip, address);
        Packet * packet;
        printf("WTF\n");
        packet = makePacket(address, hostip, filename, myaddr.sin_port, servaddr.sin_port, 1, saveFile, strlen(data), 0);

        if(sb.st_size <= BLOCKSIZE)
            sendPacket(relay_sock, data, servaddr, sb.st_size, 1, packet);
        else
            sendPackets(relay_sock, data, servaddr, packet);

    // }

    if(strcmp(address, relay) != 0)
        free(address);

    close(relay_sock);

}
