#ifndef _GOBACKN_PROTOCOL_H
#define _GOBACKN_PROTOCOL_H
#include "connection.h"

/* Abstract class that defines a protocol wrapping a connection c.
    Use the sendMessage & receiveMessage to interact with the connection
    object using the implemented procotol */
class Protocol {
protected:
    Connection *c;
public:

    /* set connection to interact with */
    void setConnection(Connection *c) {
        this->c = c;
    };

    /* send a message of size t starting at the memory address pointed to by line
        through the implememted protocol */
    virtual int sendMessage(char* line, unsigned int t) = 0;

    /* receive a message through the implemented protocol */
    virtual char* receiveMessage() = 0;
};

