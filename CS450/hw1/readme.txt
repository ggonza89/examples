Written by Giancarlo Gonzalez (ggonza20) January 2014 for CS 450 HW1

server.cpp

    This program receives a file from a remote client and sends back an
    acknowledgement after the entire file has been received.

    Two possible options for closing the connection:
        (1) Send a close command along with the acknowledgement and then close.
        (2) Hold the connection open waiting for additional files / commands.
        The "persistent" field in the header indicates the client's preference.

client.cpp

    This program asks for a filename, asks if user would like file to be saved
    on server, and transfers the file to a remote server, possibly through an
    intermediary relay. The server will respond with an acknowledgement and can
    send more files by user preference.

    This program then calculates the round-trip time from the start of
    transmission to receipt of the acknowledgement, and reports that along
    with the average overall transmission rate and the amount of data sent.

Shortcomings

    Server does not properly save a lengthy file.  I tested it with a 8 KB file,
    a 887 KB file, a 12 MB file, and a trivial 1 Byte file.  All but the 12 MB
    file wrote properly to disk.
    Client does not properly send a lengthy file to professors server.  I
    tested with the same files as with the server and only the 8 KB and 1 Byte
    file worked. It exits at line 261 when I send the data to the server/relay.
    I'm guessing that the professors server closes the sock when an error is
    encountered when writing to file.  Mine does not :/

Run
    Enter "make" command in a terminal.
    myserver
        run "./myserver [port]" in a terminal.  It will have a line with my
        credentials and then a line of data to be received from the IP and
        another line when the transfer is finished succesfully.
    myclient
        run "./myclient [server] [server port] [relay] [relay port]".  Again
        it will provide you with my credentials and prompts to the user for
        filename, option to save file on server, and another to persist.

Update!!
    ofstream is failing to write to the file.  I would have to have a little
    more time to write out a file of 32000 bytes approximately.
