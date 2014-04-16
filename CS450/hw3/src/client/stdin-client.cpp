#include <stdio.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include "stdin-client.h"

StdinClient::StdinClient(GoBackNProtocol *p, ClientUDPConnection *c) {
	this->p = p;
	this->c = c;
}

Packet * StdinClient::preparePacket(const char * block_data, int nbytes, const char * filename, int8_t saveFile, int nTotalBytes) {

	char hostname[128];
    gethostname(hostname, sizeof(hostname));
    char ** hostip_list;
    hostip_list = getIpAddress(hostname);
    char * hostip;
    hostip = printIP((unsigned char *)hostip_list[0]);
    // printf("Host ip: %s Server IP: %s\n", hostip, c->relay);
    Packet * packet;
    packet = makePacket(c->relay, hostip, filename, c->cliaddr.sin_port, c->servaddr.sin_port, 1, saveFile, nTotalBytes);
    packet->header.nbytes = nbytes;
    strcpy(packet->data, block_data);
    packet->header.checksum = calcChecksum((void*)packet, PacketSize);

    return packet;

}

Packet ** StdinClient::preparePackets(const char * data, const char * filename, int8_t saveFile, int data_length, int & amount_of_packets) {

    int data_broken = 0;
    char * block_data;
    block_data = (char *)malloc(BLOCKSIZE);

    int amount = PacketSize % data_length;
    amount_of_packets = amount;
    Packet * packets[amount];
    for(int i = 0; i < amount; i++) {

        memset(block_data, 0, BLOCKSIZE);
        // break data into BLOCKSIZE pieces
        if(i == 0)
            strncpy(block_data, data, BLOCKSIZE);
        else
            strncpy(block_data, (data+packets[i-1]->header.nbytes), BLOCKSIZE);

        packets[i] = preparePacket(block_data, strlen(block_data), filename, saveFile, data_length);

    }

    return packets;

}

void StdinClient::start() {

	if (c->connect() < 0) {

		printf("Could not connect\n");
		return;

	}

	p->setConnection(c);

	string input, filename;
    struct stat sb;
    char * data;
    int8_t saveFile;

	while(1) {

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

        // data = (char *)malloc(sb.st_size);
        data = (char *)mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

        if(data == MAP_FAILED) {

            perror("mmap");
            exit(1);

        }

        if(close(fd) < 0) {

            perror("close");
            exit(1);

        }

        cout << "Would you like to save (" + filename +  ") to server (yes/no)? ";
        getline(cin, input);

        if(!input.compare("yes"))
            saveFile = 1;
        else
            saveFile = 0;

        Packet ** packets;
        int amount_of_packets;
        packets = preparePackets(data, filename.c_str(), saveFile, sb.st_size, amount_of_packets);
		p->sendMessage(packets, amount_of_packets);

        // free(packets);

        cout << "Would you like to send another file? ";
        getline(cin, input);

        if(input.compare("yes"))
            break;

    }

}

/* Run a StdinClient over a ClientUDPConnection using the GoBackNProtocol */
int main(int argc,char **argv) {

	/* fetch the server_ip from the cmd line, port is optional */
	int relay_port;
	if (argc <= 1) {
		// no args
		printf("usage: udp-client <server_ip> <port>\n");
		exit(-1);

	}
	else if (argc <= 2)
		// got an ip, but no port
		relay_port = DEFAULT_PORT;
	else
		relay_port = atoi(argv[2]);

	ClientUDPConnection c(relay_port, argv[1], argv[3], atoi(argv[4]));
	GoBackNProtocol p;

	StdinClient client(&p, &c);
	client.start();

}
