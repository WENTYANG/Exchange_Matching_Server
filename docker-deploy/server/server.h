#ifndef _SERVER_H
#define _SERVER_H

#include <pthread.h>

#include <cstdio>
#include <iostream>
#include <pqxx/pqxx>
#include <string>
#include <vector>
#include <queue>
#include <atomic>
#include <errno.h>
#include <mutex>    

#include "clientInfo.h"
#include "exception.h"
#include "parser.h"
#include "request.h"
#include "socket.h"
#include "sql_function.h"

using namespace pqxx;

#define N_THREAD_LIMIT 100

// Encapsulation parameters for passing parameters to the thread startup function
class Thread_args {
 public:
  void * This;
  void * arg_1;

 public:
  Thread_args() : This(nullptr), arg_1(nullptr) {}
  Thread_args(void * t, void * arg1) : This(t), arg_1(arg1) {}
};

class Server {
 private:
  string portNum;
  volatile int curRunThreadNum; //record the number of threads run in server
  queue<ClientInfo*> requestQueue;  // save request

 private:
  template<typename TYPE, typename CLASS_TYPE, int C>  //Thread startup function
  static void * _thread_run(void * param) {
    TYPE* ptr = (TYPE*) param;
    CLASS_TYPE * This = (CLASS_TYPE *)ptr->This; 
    if (C == 1) {
      This->handleRequest(ptr->arg_1);
    }

    delete ptr;
    return nullptr;
  }

 public:
  Server(string port) : portNum(port), curRunThreadNum(1){}
  ~Server() {}
  void run();
  void handleRequest(void * info);

 public:
  connection * connectDB(string dbName, string userName, string password);
  void disConnectDB(connection * C);

 private:
  void recvRequest(int client_fd, string & wholeRequest);
  void sendResponse(int client_fd, const string & XMLresponse);
  void initializeDB(connection * C);
  void cleanResource(connection * C, ClientInfo * info);
};

#endif