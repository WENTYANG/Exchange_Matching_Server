
all: server

server:  tinyxml2.h server.h exception.h socket.h clientInfo.h
	g++ -g  -o server main.cpp server.cpp tinyxml2.cpp socket.cpp  -lpthread

test: test.cpp parser.cpp parser.h request.cpp request.h tinyxml2.cpp tinyxml2.h
	g++ -g  -o test test.cpp parser.cpp request.cpp tinyxml2.cpp

.PHONY:
	clean
clean:
	rm -rf *.o server test 