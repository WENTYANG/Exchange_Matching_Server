#ifndef _SERVER_H
#define _SERVER_H

#include <pthread.h>
#include <cstdio>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include "clientInfo.h"
#include "exception.h"
#include "socket.h"
#include "sql_function.h"
using namespace pqxx;

extern connection* C;

class Server {
   private:
    string portNum;

   public:
    Server(string port) : portNum(port) {}
    ~Server(){};
    void run();
    void connectDB(string dbName, string userName, string password);
    static void* handleRequest(void* info);
};

#endif