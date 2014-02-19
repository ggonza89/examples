/*  450Utils.h

    This file contains a number of useful functions for use in CS 450 HW

    Written by John T. Bell January 2014 for CS 450
*/

#ifndef UTILS450_H
#define UTILS450_H

#include <cstdlib>
#include <cstdio>

#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <netinet/in.h>

#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <iostream>
#include <ctime>
#include <string>
// #include <fstream>

#include "CS450Header.h"

using namespace std;

void printIP( ostream & out, uint32_t IP );

// ostream & operator << ( ostream & out, const CS450Header & h );
ostream & operator << ( ostream & out, const struct sockaddr_in & addr );

void packHeader( CS450Header & h, int version, long int UIN, int HW_number,
    int transactionNumber, const char * ACCC, const char * filename,
    uint32_t from_IP, uint32_t to_IP, int packetType, unsigned long nbytes,
    int relayCommand, int persistent, int saveFile, uint16_t from_Port,
    uint16_t to_Port, uint32_t trueFromIP, uint32_t trueToIP );

void unpackHeader( const CS450Header & h, int  & version, long int & UIN,
    int & HW_number, int & transactionNumber, char * ACCC, char * filename,
    uint32_t & from_IP, uint32_t & to_IP, int & packetType,
    unsigned long & nbytes, int & relayCommand, int & persistent,
    int & saveFile, uint16_t & from_Port, uint16_t & to_Port,
    uint32_t & trueFromIP, uint32_t & trueToIP );

#endif
