
all: server

server:  tinyxml2.h server.h
	g++ -g  -o server main.cpp server.cpp tinyxml2.cpp  -lpthread

test: tinyxml2.h
	g++ -o test test.cpp tinyxml2.cpp

.PHONY:
	clean
clean:
	rm -rf *.o server test