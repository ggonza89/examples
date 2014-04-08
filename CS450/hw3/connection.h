/*  connection.h

    This file contains UDP connection implementation

    Written by Giancarlo Gonzalez (ggonza20) April 2014 for CS 450 project 4
*/
#ifndef _CONNECTION_H
#define _CONNECTION_H
#include "myutils.h"

/*
UDP connection that defines socket i/o functions
*/
class UDPConnection : public Connection {
protected:
    int sockfd;
    struct sockaddr_in servaddr, clientaddr;
    unsigned int addrlen;

    int datalen;
    char mesg[BUF_SIZE]

    // called so server/client can change sockaddr_in vars
    virtual int onSocketCreated();

public:
    Connection();
    virtual ~Connection();

    // inherited from Connection
    virtual int connect()
    virtual void setTimeout(int secs) = 0;
    virtual int sendData(const void * data, unsigned int length);
    virtual int blocking_receive(char* return_buf);
    virtual void printInfo();
};

#endif
