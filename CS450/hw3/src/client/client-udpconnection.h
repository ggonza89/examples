#ifndef _CLIENT_UDPCONNECTION_H
#define _CLIENT_UDPCONNECTION_H

#include "common.h"
#include "udp-connection.h"

/* client implementation of UDPConnection */
class ClientUDPConnection : public UDPConnection {
protected:
    virtual int onSocketCreated();
public:
	ClientUDPConnection();
	ClientUDPConnection(int port, char * relay_ip, char * server_ip, int s_port);
	virtual ~ClientUDPConnection();
        // ips and ports for the relay and server
    char * relay;
    int relay_port;
    char * server;
    int server_port;
};

#endif
