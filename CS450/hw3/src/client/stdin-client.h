#ifndef _STDIN_CLIENT_H
#define _STDIN_CLIENT_H

#include "common.h"
#include "gobackn-protocol.h"
#include "client-udpconnection.h"

/* Shell client that reads from stdin and interacts with the connection c
	via Protocol p */
class StdinClient {
private:
	GoBackNProtocol *p;
	ClientUDPConnection *c;
    Packet ** preparePackets(const char * data, const char * filename, int8_t saveFile, int data_length, int & amount_of_packets);
    Packet * preparePacket(const char * block_data, int nbytes, const char * filename, int8_t saveFile, int nTotalBytes);

public:
	StdinClient(GoBackNProtocol *p, ClientUDPConnection *c);
	void start();
};
#endif
