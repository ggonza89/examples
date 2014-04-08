all: client server

client: client.cpp
	g++ -g -o client client.cpp

server: server.cpp
	g++ -g -o server server.cpp

clean: client server
	rm client
	rm server
	rm -rf client.dSYM server.dSYM
