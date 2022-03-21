CC=g++
CFLAGS=-g
EXTRAFLAGS=-lpqxx -lpq -lpthread


all: server

server:  tinyxml2.h server.h exception.h socket.h clientInfo.h sql_function.h
	$(CC) $(CFLAGS)  -o server main.cpp server.cpp tinyxml2.cpp socket.cpp sql_function.cpp $(EXTRAFLAGS)

test: parser.h request.h  tinyxml2.h
	$(CC) $(CFLAGS)  -o test test.cpp parser.cpp request.cpp tinyxml2.cpp $(EXTRAFLAGS)

.PHONY:
	clean
clean:
	rm -rf *.o server test 