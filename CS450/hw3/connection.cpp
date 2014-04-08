#include <memory.h>
#include <sys/types.h>

Connection::Connection() : servaddr{0}, clientaddr{0} {

    sockfd = -1;
    addrlen = sizeof(struct sockaddr_in);
    servaddr.sin_family = AF_INET;

    servaddr.sin_port = htons(DEFAULT_PORT);

    printf("UPD connection created\n");

}

int Connection::connect() {

    if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {

        perror("error opening datagram socket");
        return -1;

    }

    if (onSocketCreated() < 0) {

        perror("error in OnSocketCreated\n");
        return -1;

    }

    return 0;

}

int Connection::onSocketCreated() {

    return 0;

}

void Connection::setTimeout(int secs) {

    struct timeval tval = {0};
    tval.tv_sec = secs;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(struct timeval));

}

int Connection::send(const void *data, unsigned int length) {

    sendto(sockfd, message, length, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("Sent data");

}

int Connection::blocking_receive(char* return_buf) {

    datalen = recvfrom(sockfd, mesg, BUF_SIZE, 0, (struct sockaddr *)&servaddr, &addrlen);

    memcpy(return_buf, mesg, BUF_SIZE);
    printf("Connection receive got %i bytes\n", datalen);

    return datalen;

}

void Connection::printInfo() {

    printf("sockfd: %i addrlen: %i\n", sockfd, addrlen);

}

Connection::~Connection() {
    printf("UDP connection destroyed\n");
}
