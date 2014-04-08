Written by Giancarlo Gonzalez (ggonza20) March 2014 for CS 450 HW2

client.cpp

    This program sends a file to a remote server and, depending on the protocol, waits for an acknowledgement after each packet.
    For RDT 1.0, the client sends packets assuming no bit errors and no stop and wait. Round trip time for this protocol is

    Cannot send files of 12mb.  Cannot find out why.  In my minut knowledge of programming, I did not care to make all the calls to a function as proper as it should be in C++, most likely the data was not properly sent to the function.

    Protocol 2.1 locally ran
        With a 7kb size file
        Round trip time: 0.000098
        Round trip time: 0.000127
        Average response rate: 0.000112

        With a 13 byte file
        Round trip time: 0.000144
        Average response rate: 0.000144

    Protocol 2.1 ran between my client and professors server
        With 7kb size file
        Round trip time: 0.000253
        Round trip time: 0.000235
        Average response rate: 0.000244

        With 13 byte file
        Round trip time: 0.000193
        Average response rate: 0.000193

    Protocol 2.0 ran locally
        With 7kb size file
        Round trip time: 0.000183
        Round trip time: 0.000129
        Average response rate: 0.000156

        With 13 byte file
        Round trip time: 0.000092
        Average response rate: 0.000092

    Protocol 2.0 between my client and professors server
        With 7kb size file
        Round trip time: 0.000250
        Round trip time: 0.000198
        Average response rate: 0.000224

        With 13 byte file
        Round trip time: 0.000146
        Average response rate: 0.000146

    Protocol 1.0 ran locally
        With 7kb size file
        Round trip time: 0.000096
        Round trip time: 0.000033
        Average response rate: 0.000064

        With 13 byte file
        Round trip time: 0.000060
        Average response rate: 0.000060

    Protocol 1.0 between my client and professors server
        With 7kb size file
        Round trip time: 0.000108
        Round trip time: 0.000065

        With 13 byte file
        Round trip time: 0.000062
        Average response rate: 0.000062

server.cpp

    This program receives a file from a remote client and sends back an acknowledgement after each packet has been received.
    If the saveFile is 1 then it saves the file to disk.
    ran diff on all saved files and originals and there were no issues.
