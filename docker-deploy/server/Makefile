CC=g++
CFLAGS=-O3

all: server

server:  tinyxml2.h server.h
	$(CC) -g  -o server main.cpp server.cpp tinyxml2.cpp  -lpthread

test: tinyxml2.h parser.h parser.cpp test.cpp
	$(CC) -o test test.cpp tinyxml2.cpp parser.cpp

.PHONY:
	clean
clean:
	rm -rf *.o server test