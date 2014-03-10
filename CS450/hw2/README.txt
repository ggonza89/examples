Written by Giancarlo Gonzalez (ggonza20) March 2014 for CS 450 HW2

client.cpp

    This program sends a file to a remote server and, depending on the protocol, waits for an acknowledgement after each packet.
    For RDT 1.0, the client sends packets assuming no bit errors and no stop and wait. Round trip time for this protocol is
