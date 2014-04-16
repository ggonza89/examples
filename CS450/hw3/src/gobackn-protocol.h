#ifndef _GOBACKN_PROTOCOL_H
#define _GOBACKN_PROTOCOL_H
#include "common.h"

#define N 10

/* c/c++ % computes remainder, not modulo, the formula below
    returns modulo. */
#define MOD(x, m) (x%m + m)%m
#define LAST_SENT_SEQN MOD(current_seqn - 1, N)

#define DATAGRAM_IDENT "DATA"
#define ACK_IDENT "ACK"


/* Implementation of the Go-back-N protocol, N is a compile time constant set above.
    This implementation is bidirectional & maintains different sequence numbers for each
    direction of communication. */
class GoBackNProtocol : public Protocol {
private:

    // char * data_received;
    Packet * buffer; //buffer used for incoming and outgoing messages

    /* client vars */
    Packet * window[N]; //window of size N
    int current_seqn; //sequence number to be assigned to the next packet
    int last_acked_seqn; //last sequence number received from the server
    int num_active; //number of active messages in the window

    /* server vars*/
    int last_sent_ackn; //sequence number of last ACK
    int expected_seqn; //next sequence number expected by the server

    /*
    Puts the datagram into a Packet
    */
    int sendPacket(Packet * packet);
    int sendPacket(Packet * packet, char * data);

    /* returns true if the window still has space to add a message */
    bool canAddToWindow();

    /* creates and adds a message containing the payload b to the window */
    int addToWindow(Packet * data);

    /* returns true if no ACKs are outstanding */
    bool windowEmpty();

    /* listen for an ACK, returns true if the socket does not timeout */
    bool acceptAcks() ;

    /* helper function to set timeout to 5 seconds before listening for an ACK */
    bool listenForAck();

    /* returns true if string pointed to by data_in is a valid ACK
        sets ack_seqn to the parsed sequence number */
    bool parseValidAck(int *ack_seqn, Packet * p);

    /* returns true if the string pointed to by data_in is a valid DATA
        sets memory pointed to by datagram_seqn & payload to the parsed seqn and payload */
    bool parseValidDatagram(int * datagram_seqn, Packet * data_in);

    /* remove mes_seqn from the window and free its memory */
    void removeFromWindow(int mesg_seqn);

    /* send an ACK containing sequence number seqn */
    void sendAck(CS450Header header, int seqn);

    /* resends all active message in the window */
    void resendWindow();

public:
    GoBackNProtocol();
    void sendPackets(Packet * packet, char * data);

    /* implementation of abstract Protocol::sendMessage */
    virtual int sendMessage(Packet * packet[], int amount_of_packets);
    /* implementation of abstract Protocol::receiveMessage */
    virtual Packet * receiveMessage();
};

#endif
