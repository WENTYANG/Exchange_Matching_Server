#ifndef _CLIENT_H
#define _CLIENT_H

#include <pthread.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "functions.h"
#include "tinyxml2.h"

#define MAX_LENGTH 65536
#define N_Thread_CREATE 3  //用于创建并发送create type的线程数量
#define N_Thread_TRANS 3   //用于创建并发送transaction type的线程数量
#define NUM_SYMBOL 2       //每个Create Request中添加的symbol数量
#define INITIAL_SYMBOL_AMOUNT 100
#define INITIAL_BALANCE 1000.0

using namespace tinyxml2;
using namespace std;

class Args {
 public:
  int account_id;
  string serverName;
  string serverPort;

 public:
  Args(string name, string port, int id) :
      serverName(name), serverPort(port), account_id(id) {}
  ~Args() {}
};

class Client {
 private:
  string serverName;
  string serverPort;
  int account_id;
  float balance;
  unordered_map<string, int> symbol;  // symbols:amount owned by this client

 private:
  template<typename TYPE, int C>  //线程启动函数，声明为模板函数
  static void * _thread_run(void * param) {
    TYPE * This = (TYPE *)param;  //传入的是object的this指针，用于启动非静态成员函数
    if (C == 1) {
      This->sendCreateRequestAndGetResponse();
    }
    else if (C == 2) {
      This->sendTransRequestAndGetResponse();
    }
    return nullptr;
  }

 private:
  void sendTransRequestAndGetResponse();
  void sendCreateRequestAndGetResponse();
  string getCreateRequest();

 public:
  Client(const string & Name, const string & Port, int id) :
      serverName(Name), serverPort(Port), account_id(id), balance(INITIAL_BALANCE) {}
  ~Client() {}
  void run();
  void printCreateRequest() {
    string req = getCreateRequest();
    cout << req << endl;
  }

  // for test
  friend std::ostream & operator<<(std::ostream & out, const Client & client);
};

std::ostream & operator<<(std::ostream & out, const Client & client);

#endif