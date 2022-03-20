
#ifndef _SERVER_H
#define _SERVER_H

#include <iostream>

using namespace std;

class server
{
private:
    string portNum;

public:
    server(string port) : portNum(port) {}
    ~server(){};
    void run();
    void connectDB(string dbName, string userName, string password);
    static void *handleRequest(void *info);
};

#endif