CC=g++
CFLAGS=-g
EXTRAFLAGS=-lpqxx -lpq -lpthread


all: server

server:  tinyxml2.h server.h exception.h socket.h clientInfo.h sql_function.h request.h parser.h
	$(CC) $(CFLAGS)  -o server main.cpp parser.cpp request.cpp server.cpp tinyxml2.cpp socket.cpp sql_function.cpp $(EXTRAFLAGS)

test: parser.h request.h tinyxml2.h server.h sql_function.h test.cpp parser.cpp sql_function.cpp
	$(CC) $(CFLAGS)  -o test tinyxml2.cpp socket.cpp sql_function.cpp server.cpp request.cpp parser.cpp test.cpp $(EXTRAFLAGS)

.PHONY:
	clean
clean:
	rm -rf *.o server test 