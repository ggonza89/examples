#ifndef _SERVER_CONNECTION_H
#define _SERVER_CONNECTION_H

#include "connection.h"

/* Server implementation of Connection */
class ServerConnection : public Connection {
protected:
    virtual int onSocketCreated();

public:
    ServerConnection();
    ServerConnection(int port);
    virtual ~ServerConnection();
};

#endif
