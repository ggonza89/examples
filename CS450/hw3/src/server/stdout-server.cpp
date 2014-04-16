#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include "stdout-server.h"
#include "gobackn-protocol.h"
#include "server-udpconnection.h"

StdoutServer::StdoutServer(Protocol *p, Connection *c) {
	this->p = p;
	this->c = c;
}

void StdoutServer::start() {
	if (c->connect() < 0) {
		printf("Could not connect\n");
		return;
	}

	p->setConnection(c);

	Packet * packet;
	while(1) {

		packet = p->receiveMessage();

		if(packet->header.saveFile > 0) {

			char * name;
	        name = "server_file.cpp";
	        // sprintf(name,, header.filename);
	        ofstream file;
	        file.open(name);

	        file << packet->data;

	        file.close();

	        printf("Data of length %d saved to file %s\n", packet->header.nbytes, name);

		}

	}

}

/* Run a StdoutServer over a ServerUDPConnection using the GoBackNProtocol */
int main(int argc,char **argv) {

	printf("Written by Giancarlo Gonzalez (ggonzale) February 2014 for CS 450 HW3\n");
	/* optionally fetch the port from the cmd line, otherwise use the default */
	int port;
	if (argc <= 1) { // no args

		printf("usage: udp-server <port>\n");
		printf("using default port %i\n", DEFAULT_PORT);
		port = DEFAULT_PORT;

	}
	else
		port = atoi(argv[1]);

	ServerUDPConnection c(port);
	GoBackNProtocol p;

	StdoutServer server(&p, &c);
	server.start();

}
