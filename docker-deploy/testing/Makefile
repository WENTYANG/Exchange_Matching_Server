EXTRAFLAGS=-lpthread

all: client

client:  Client.h ../server/socket.h ../server/exception.h ../server/tinyxml2.h Client.cpp
	g++ -o client main.cpp Client.cpp functions.cpp ../server/socket.cpp ../server/tinyxml2.cpp 	-I ../server/ $(EXTRAFLAGS)

.PHONY:
	clean
clean:
	rm -rf *.o  client  ./requests/*.xml ./responses/*.xml