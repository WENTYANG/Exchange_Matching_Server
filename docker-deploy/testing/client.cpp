#include "socket.h"
#include "exception.h"
#include <iostream>
#include <string>

using namespace std;

void checkInput(int argc, char *argv[], string &masterName, string &masterPort);

int main(int argc, char *argv[])
{
    string serverName;
    string serverPort;
    try{
        checkInput(argc, argv, serverName, serverPort);
    }
    catch (const std::exception &e){
        std::cerr << e.what();
        exit(EXIT_FAILURE);
    }

    // connect to master and send request
    int server_fd;
    try {
        server_fd = clientRequestConnection(serverName, serverPort);
    }
    catch (const std::exception & e) {
        std::cerr << e.what();
        exit(EXIT_FAILURE);
    }

    //send request to server
    string XMLrequest = "test xml";
    send(server_fd,XMLrequest.c_str(),XMLrequest.length(),0);

    close(server_fd);
    
}

void checkInput(int argc, char *argv[], string &masterName, string &masterPort)
{
    if (argc != 3)
        throw MyException("Invalid Usage. Should be ./player <master_name><port_num>\n");

    masterName = argv[1];
    masterPort = argv[2];
}