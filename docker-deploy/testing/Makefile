
all: test

test: ../server/socket.h ../server/exception.h
	g++ -o test client.cpp ../server/socket.cpp -I ../server/

.PHONY:
	clean
clean:
	rm -rf *.o  test 