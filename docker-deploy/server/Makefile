CC=g++
CFLAGS=-O3

all: server

server:  tinyxml2.h server.h exception.h socket.h clientInfo.h
	g++ -g  -o server main.cpp server.cpp tinyxml2.cpp socket.cpp  -lpthread

.PHONY:
	clean
clean:
	rm -rf *.o server test 