#include "server.h"
#include "function.h"
#include <cstdio>
#include "exception.h"

#include<string>

using namespace std;

void server::run(){
    connectDB("serverdb","postgres","passw0rd");

    // create server socket, listen to port
    int server_fd;
    try{
        server_fd = createServerSocket(portNum);
    }
    catch(const std::exception& e){
        std::cerr << e.what() << '\n';
        return;
    }

    // server keep runnning    
    while(1){

        // server accept new connection
        int client_fd;
        string clientIp;
        try{
            client_fd = serverAcceptConnection(server_fd,clientIp);
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
            continue;
        }

        // accept new XML request


        

    }


    
    

    
}

void server::connectDB(string dbName, string userName, string password){
    printf("connect to %s with user: %s, using password: %s\n",dbName,userName,password);
    return;
}