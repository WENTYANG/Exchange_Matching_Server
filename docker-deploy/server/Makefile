CC=g++
CFLAGS=-g
EXTRAFLAGS=-lpqxx -lpq -lpthread


all: server

server:  tinyxml2.h server.h exception.h socket.h clientInfo.h sql_function.h request.h parser.h
	$(CC) $(CFLAGS)  -o server main.cpp parser.cpp request.cpp server.cpp tinyxml2.cpp socket.cpp sql_function.cpp $(EXTRAFLAGS)

.PHONY:
	clean
clean:
	rm -rf *.o server  