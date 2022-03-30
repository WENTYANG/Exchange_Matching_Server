#ifndef _CLIENT_H
#define _CLIENT_H

#include <pthread.h>
#include <unistd.h>

#include <string>
#include <vector>

using namespace std;

class Args {
 public:
  string serverName;
  string serverPort;
  int account_id;

 public:
  Args(string name, string port, int id) :
      serverName(name), serverPort(port), account_id(id) {}
  ~Args();
};

class Client {
 private:
  string serverName;
  string serverPort;
  int account_id;
  int server_fd;

 public:
  Client(string name, string port, int id);
  ~Client() { close(server_fd); }
  void run();
  

 private:
  void sendCreateRequestAndGetResponse();
  string getCreateRequest();
  void sendTransRequestAndGetResponse();
};

//线程启动函数，声明为模板函数
template<typename TYPE, void (TYPE::*_RunThread)()>
void * _thread_run(void * param) {
  TYPE * This = (TYPE *)param;  //传入的是object的this指针，用于启动非静态成员函数
  This->_RunThread();
  return NULL;
}

#endif