#include <stdio.h>
#include <memory.h>
#include "common.h"
#include "udp-connection.h"

UDPConnection::UDPConnection() : servaddr {0}, cliaddr{0}{
	sockfd              = -1;
	addrlen             = sizeof(struct sockaddr_in);
	servaddr.sin_family = AF_INET;

	datalen = -1;
	servaddr.sin_port   = htons(DEFAULT_PORT);

	printf("Generic connection created\n");
}

int UDPConnection::connect() {

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("can't open datagram socket");
		return -1;
	}

	if (onSocketCreated() < 0) {
		perror("error in OnSocketCreated\n");
		return -1;
	}

	return 0;
}

void UDPConnection::setTimeout(int secs) {
	struct timeval tval = { 0 };
	tval.tv_sec = secs;

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(timeval));
}

int UDPConnection::send(const void *message, unsigned int length) {

	int data_sent = sendto(sockfd, message, length, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
	printf("UDPConnection send\n");

	return data_sent;

}

int UDPConnection::blocking_receive(Packet * return_buf) {

	datalen = recvfrom(sockfd, mesg, BUF_SIZE, 0, (struct sockaddr *)&servaddr, &addrlen);
	printf("UDPConnection receive got %i bytes\n", datalen);
	memcpy(return_buf, mesg, BUF_SIZE);
	return datalen;
}


int UDPConnection::onSocketCreated() {
	return 0;
}

void UDPConnection::dumpInfo() {
	printf("sockfd: %i addrlen: %i\n", sockfd, addrlen);
}

UDPConnection::~UDPConnection() {
	printf("Generic connection destroyed\n");
}
