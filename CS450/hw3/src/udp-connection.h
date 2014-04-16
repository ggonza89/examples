#ifndef _UDP_CONNECTION
#define _UDP_CONNECTION

/* Abstract UDP implementation of the abstract Connection class*/
class UDPConnection : public Connection {
protected:
	int sockfd;
	unsigned int addrlen;

	int datalen;
	char mesg[BUF_SIZE];

	/* called after the socket is created so that server/client implementations can
		change sockaddr_in vars */
	virtual int onSocketCreated();

public:
	UDPConnection();
	virtual ~UDPConnection();

	/* inherited from Connection */
	virtual int connect();
	virtual void setTimeout(int secs);
	virtual int send(const void *message, unsigned int length);
	virtual int blocking_receive(Packet * return_buf);

	virtual void dumpInfo();
	struct sockaddr_in servaddr, cliaddr;
};

#endif
