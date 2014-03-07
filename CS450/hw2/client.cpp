#include "450UtilsUDP.h"

using namespace std;

void usage() {
    cout << "Usage: client [server] [port] [relay] [port]...\nIf any argument is omitted, then all following arguments must also be omitted.\n";
    exit(1);
}

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
    printf("%s\n", ipAddress);
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

char * getIpAddress(char * host) {

    struct hostent *hp;
    int i;

    hp = gethostbyname(host);
    if(!hp) {

        fprintf(stderr, "could not obtain address of %s\n", host);
        return NULL;

    }

    // printf("Ip address of host %s\n", host);
    // printIP((unsigned char*)hp->h_addr_list[0]);

    return hp->h_addr_list[0];

}

Packet * makePacket(char * data, int transactionNumber, char * relay, char * hostip, string filename, uint16_t from_Port, uint16_t to_Port, int packetType, int saveFile, int nTotalBytes, int nbytes) {

    Packet * packet;
    strcpy(packet->data, data);

    packet->header.version = 6;
    packet->header.UIN = 665799950;
    packet->header.HW_number = 2;
    packet->header.transactionNumber = 1;
    strcpy(packet->header.ACCC, "ggonza20");
    inet_pton(AF_INET, relay, &packet->header.to_IP);
    // printf("%s\n", printIP((unsigned char *)packet->header.to_IP));

    inet_pton(AF_INET, hostip, &packet->header.from_IP);
    packet->header.from_Port = from_Port;
    packet->header.to_Port = to_Port;
    strcpy(packet->header.filename, filename.c_str());
    packet->header.packetType = 1;
    packet->header.saveFile = saveFile;
    packet->header.nbytes = nbytes;
    packet->header.nTotalBytes = nTotalBytes;

    return packet;

}

int sendPacket(int relay_sock, char * data, int8_t saveFile, struct sockaddr_in servaddr, struct sockaddr_in myaddr, string filename, char * relay, int nbytes) {

    char hostname[128];
    gethostname(hostname, sizeof(hostname));
    char * hostip = getIpAddress(hostname);
    hostip = printIP((unsigned char *)hostip);
    printf("%s\n", hostip);
    unsigned int servlen = sizeof(servaddr);

    Packet packet;
    strcpy(packet.data, data);

    packet.header.version = 6;
    packet.header.UIN = 665799950;
    packet.header.HW_number = 2;
    packet.header.transactionNumber = 1;
    strcpy(packet.header.ACCC, "ggonza20");
    inet_pton(AF_INET, relay, &packet.header.to_IP);
    inet_pton(AF_INET, hostip, &packet.header.from_IP);
    packet.header.from_Port = myaddr.sin_port;
    packet.header.to_Port = servaddr.sin_port;
    strcpy(packet.header.filename, filename.c_str());
    packet.header.packetType = 1;
    packet.header.saveFile = saveFile;
    packet.header.nbytes = nbytes;
    packet.header.nTotalBytes = nbytes;

    // packet = makePacket(data, 1, relay, hostip, filename, myaddr.sin_port, servaddr.sin_port, 1, saveFile, datasize, datasize);

    printHeader(packet.header);

    if(sendto(relay_sock, &packet, PacketSize, 0, (struct sockaddr *)&
        servaddr, servlen) < 0) {

        perror("sendto failed");
        return 1;

    }

    Packet acknowledgment;
    if(recvfrom(relay_sock, &acknowledgment, PacketSize, 0, (struct sockaddr *)&servaddr, &servlen) < 0) {

        printf("No acknowledgment received.");
        return 1;

    }

    printHeader(acknowledgment.header);
    return 0;

}

int main(int argc, char ** argv) {

    printf("Written by Giancarlo Gonzalez (ggonzale) February 2014 for CS 450 HW2\n");

    char * server, *server_port, *relay, *relay_port;

    if(argc == 1) {

        relay = "127.0.0.1";
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
    if (inet_aton(relay, &servaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    char hostname[128];
    gethostname(hostname, sizeof(hostname));

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

        cout << "Would you like to save file (" + filename +  ") to server (yes/no)? ";
        getline(cin, input);

        if(!input.compare("yes"))
            saveFile = 1;
        else
            saveFile = 0;

        // make packet
        if(sb.st_size <= BLOCKSIZE) {

            // make function that sends the one piece and waits for ack
            sendPacket(relay_sock, data, saveFile, servaddr, myaddr, filename, relay, sb.st_size);

        }
        // else
            // sendPackets(); // function that sends a piece at a time
            // ;

    // }

    close(relay_sock);

}
