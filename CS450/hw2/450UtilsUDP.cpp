#include "450UtilsUDP.h"

using namespace std;

char * printIP(uint32_t ip) {

    char * ipAddress;
    ipAddress = (char *)calloc(INET_ADDRSTRLEN, sizeof(char));
    inet_ntop(AF_INET, &ip, ipAddress, INET_ADDRSTRLEN);

    return ipAddress;

}

char * printIP(unsigned char *ip) {

    char * ipAddress;
    ipAddress = (char *)calloc(10, sizeof(char));
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
        printf("\tData sending: %lu Size of data: %d saveFile: %d checksum: %d sequenceNumber: %d\n",
            header.nbytes, header.nTotalBytes, header.saveFile, header.checksum, header.sequenceNumber);
    else if(header.ackNumber == (-1))
        printf("\tThis packet was garbled\n");

    printf("\n");

    free(from_IP);
    free(to_IP);

}

char ** getIpAddress(char * host) {

    struct hostent *hp;
    int i;

    hp = gethostbyname(host);
    if(!hp) {

        fprintf(stderr, "could not obtain address of %s\n", host);
        return NULL;

    }

    return hp->h_addr_list;

}

Packet * makePacket(char * relay, char * hostip, string filename, uint16_t from_Port, uint16_t to_Port, int packetType, int saveFile, int nTotalBytes, int checksum) {

    Packet * packet;

    packet = (Packet *)malloc(PacketSize);
    packet->header.version = 6;
    packet->header.UIN = 665799950;
    packet->header.HW_number = 2;
    strcpy(packet->header.ACCC, "ggonza20");
    inet_pton(AF_INET, relay, &packet->header.to_IP);
    inet_pton(AF_INET, hostip, &packet->header.from_IP);
    packet->header.from_Port = from_Port;
    packet->header.to_Port = to_Port;
    strcpy(packet->header.filename, filename.c_str());
    packet->header.packetType = 1;
    packet->header.saveFile = saveFile;
    packet->header.nTotalBytes = nTotalBytes;
    packet->header.ackNumber = 0;
    packet->header.sequenceNumber = 0;

    return packet;

}

uint16_t calcChecksum( void *data, int nbytes ) {

    uint16_t * data16 = (uint16_t *)data;
    uint8_t * data8 = (uint8_t *)data;
    uint32_t sum = 0, max16 = 0xffff;
    int i;
    for(i = 0; i < nbytes/2; ++i) {

        sum += data16[i];
        if(sum > max16)
            sum -= max16;
        if((nbytes % 2) != 0) {

            sum += data8[nbytes-1];
            if(sum > max16)
                sum -= max16;

        }

    }

    return ~sum;

}
