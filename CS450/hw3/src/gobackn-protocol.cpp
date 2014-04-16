#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <exception>
#include "gobackn-protocol.h"

GoBackNProtocol::GoBackNProtocol() {

    c               = NULL;
    current_seqn    = 0;
    expected_seqn   = 0;
    last_sent_ackn  = 0;
    num_active      = 0;
    last_acked_seqn = LAST_SENT_SEQN;

    for(int i = 0; i < N; ++i)
        window[i] = (Packet *)malloc(PacketSize);

}

int GoBackNProtocol::sendPacket(Packet * packet) {

	printf("Sending:\n");
	printHeader(packet->header);
	printf("\n");

	c->send(packet, PacketSize);

    return 0;

}

int GoBackNProtocol::sendPacket(Packet * packet, char * data) {

    strcpy(packet->data, data);
    packet->header.nbytes = strlen(data);
    packet->header.checksum = calcChecksum((void*)packet, PacketSize);

    // printHeader(packet->header);
    // printf("\nData:\n %s\n", packet->data);

    return 5;

}

void GoBackNProtocol::sendPackets(Packet * packet, char * data) {

    int sent_accum = 0, data_sent = 0, sent = 0;
    char * block_data;
    time_t start, end;
    double rtt, avg_response_rate = 0.0;
    block_data = (char *)malloc(BLOCKSIZE);
    // printf("%d\n", strlen(data));
    while(sent_accum < strlen(data)) {

        memset(block_data, 0, BLOCKSIZE);
        // break data into BLOCKSIZE pieces
        if(sent_accum == 0)
            strncpy(block_data, data, BLOCKSIZE);
        else
            strncpy(block_data, (data+sent_accum), BLOCKSIZE);

        start = clock();
        data_sent = sendPacket(packet, block_data);
        end = clock();
        rtt = double(end-start)/CLOCKS_PER_SEC;
        printf("Round trip time: %f\n", rtt);
        avg_response_rate += rtt;

        sent_accum += data_sent;

        sent++;

    }

    avg_response_rate /= double(sent);
    printf("Average response rate: %f\n", avg_response_rate);

    free(block_data);

}

int GoBackNProtocol::addToWindow(Packet * packet) {

    Packet * pTemp;
    memcpy(pTemp, packet, PacketSize);

    pTemp->header.sequenceNumber = current_seqn;
    memcpy(window[current_seqn], pTemp, PacketSize);
    current_seqn = MOD(current_seqn+1, N);
    num_active++;

    return sendPacket(pTemp);

}

bool GoBackNProtocol::canAddToWindow() {

    printf("current_seqn: %i last_acked_seqn: %i, num_active(%i)\n", current_seqn, last_acked_seqn, num_active);

    return num_active < N;

}

bool GoBackNProtocol::windowEmpty() {

    printf("last_acked_seqn %i  == LAST_SENT_SEQN %i\n", last_acked_seqn, LAST_SENT_SEQN);

    return last_acked_seqn == LAST_SENT_SEQN;

}

void GoBackNProtocol::resendWindow() {

    int num_sending;
    num_sending = num_active;
    printf("\tresending %i packets\n", num_active);

    for( int i = 1; i <= num_sending; i++) {

        int current = MOD(last_acked_seqn+i, N);
        printf("\t");
        sendPacket(window[current]);

    }

}

int GoBackNProtocol::sendMessage(Packet * packets[], int amount_of_packets) {

    if(c == NULL)
        throw std::exception();

    /*
    loop while we have more to send or are still waiting for outstanding ACKs
    */
    int packets_in_window = 0;
    printHeader(packets[packets_in_window]->header);
    // printf("Total Bytes to send: %d\n", packets[packets_in_window]->header.nTotalBytes);
    Packet * pTemp;
    pTemp = (Packet *)malloc(PacketSize);

    while(!windowEmpty() || packets_in_window < amount_of_packets) {

        // fill the window so we can send as much as possible
        while(canAddToWindow() && packets_in_window < amount_of_packets) {

            pTemp->header = packets[packets_in_window]->header;
            printHeader(pTemp->header);
            addToWindow(packets[packets_in_window]);
            ++packets_in_window;

        }

        // if timed out, resend window again
        if(!acceptAcks())
            resendWindow();

    }

    printf("WINDOW EMPTY & ALL PACKETS SENT\n");

    return 0;

}

bool GoBackNProtocol::acceptAcks() {

    bool didNotTimeout;

    c->setTimeout(5);
    didNotTimeout = listenForAck();
    c->setTimeout(0);

    return didNotTimeout;

}

void GoBackNProtocol::removeFromWindow(int mesg_seqn) {

    int num_deleting;

    num_deleting = MOD(mesg_seqn - last_acked_seqn, N);
    printf("num_deleting %i\n", num_deleting);

    if(num_deleting > 0)
        printf("\nRECEIVED: ACK %i\n", mesg_seqn);

    for(int i = 1; i <= num_deleting; i++ ) {

        int deleting = MOD(last_acked_seqn+i, N);
        free(window[deleting]);
        num_active--;
        printf("\tdeleting %i from window\n", deleting);

    }

    last_acked_seqn = mesg_seqn;

}

bool GoBackNProtocol::parseValidAck(int *ack_seqn, Packet * p) {

    if(calcChecksum((void *)p, PacketSize) != 0 || *ack_seqn < 0 || *ack_seqn >= N)
        return false;

    return true;

}

bool GoBackNProtocol::listenForAck() {

    int mesg_seqn;

    //listen for incoming packets, stop if it times out
    if( c->blocking_receive(buffer) == -1 ) {

        printf("TIMED OUT\n");
        return false;

    }

    // we got a packet, validate it
    if(!parseValidAck(&mesg_seqn, buffer)) {

        printf("\tBAD ACK ");
        printHeader(buffer->header);
        printf(", RELISTENING\n");

        //ACK invalid reset timeout
        return listenForAck();

    }

    // server ACKed mesg_seqn, update window
    removeFromWindow(mesg_seqn);

    return true;

}

bool GoBackNProtocol::parseValidDatagram(int * datagram_seqn, Packet * data_in) {

    if(calcChecksum((void *)data_in, PacketSize) != 0)
        return false;

    datagram_seqn = &data_in->header.sequenceNumber;
    if(*datagram_seqn != expected_seqn)
        return false;

    return true;

}

void GoBackNProtocol::sendAck(CS450Header header, int seqn) {

    Packet * packet;

    char * to_IP, * from_IP;
    to_IP = printIP(header.from_IP);
    from_IP = printIP(header.to_IP);

    packet = makePacket(to_IP, from_IP, header.filename, header.to_Port, header.from_Port, 2, 0, header.nTotalBytes);

    packet->header.transactionNumber = header.transactionNumber;
    packet->header.ackNumber = 1;
    packet->header.sequenceNumber = seqn;
    packet->header.checksum = calcChecksum((void*)packet, PacketSize);

    printHeader(packet->header);

    c->send(packet, PacketSize);

    free(to_IP);
    free(from_IP);
    free(packet);

}

Packet* GoBackNProtocol::receiveMessage() {

    int datagram_seqn;

    if(c == NULL)
        throw std::exception();

    while(c->blocking_receive(buffer) != -1) {

        if(parseValidDatagram(&datagram_seqn, buffer)) {

            expected_seqn = MOD(expected_seqn+1, N);
            printf("Got payload %i\n", datagram_seqn);
            printHeader(buffer->header);
            printf("\n");
            sendAck(buffer->header, datagram_seqn);

        }
        else
            sendAck(buffer->header, last_sent_ackn);

    }

    return buffer;

}
