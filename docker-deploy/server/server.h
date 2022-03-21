#ifndef _SERVER_H
#define _SERVER_H

#include <pqxx/pqxx>
#include "socket.h"
#include "clientInfo.h"
#include "exception.h"
#include "sql_function.h"
#include <pthread.h>
#include <string>
#include <cstdio>
#include <vector>
#include <iostream>

using namespace pqxx;

extern connection *C;

class Server
{
private:
    string portNum;

public:
    Server(string port) : portNum(port) {}
    ~Server(){};
    void run();
    void connectDB(string dbName, string userName, string password);
    static void *handleRequest(void *info);
};

#endif