EXTRAFLAGS=-lpthread

all: test

test: ../server/socket.h ../server/exception.h ../server/tinyxml2.h
	g++ -o test client.cpp ../server/socket.cpp -I ../server/

unittest:  ../server/socket.h ../server/exception.h ../server/tinyxml2.h Client.cpp test.cpp functions.cpp
	g++ -o unittest test.cpp Client.cpp functions.cpp ../server/socket.cpp ../server/tinyxml2.cpp -I ../server/ $(EXTRAFLAGS)

.PHONY:
	clean
clean:
	rm -rf *.o  test unittest