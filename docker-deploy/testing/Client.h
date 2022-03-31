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

using namespace tinyxml2;
using namespace std;

class Args {
 public:
  int account_id;
  string serverName;
  string serverPort;
  
 public:
  Args(string name, string port, int id) :
      serverName(name.c_str()), serverPort(port.c_str()), account_id(id) {}
  ~Args() {}
};

class Client {
 private:
  int account_id;
  int server_fd;
  float balance;
  unordered_map<string, int> symbol; /*symbols:amount owned by this client*/

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
  Client(int id);
  ~Client() { close(server_fd); }
  void run(const string & serverName, const string & serverPort);
  void printCreateRequest() {
    string req = getCreateRequest();
    cout << req << endl;
  }

  // for test
  friend std::ostream & operator<<(std::ostream & out, const Client & client);
};

std::ostream & operator<<(std::ostream & out, const Client & client);

#endif