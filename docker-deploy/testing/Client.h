#ifndef _CLIENT_H
#define _CLIENT_H

#include <pthread.h>
#include <unistd.h>
#include <string>

#include <iostream>
#include <string>
#include <vector>

#include <unordered_map>
#include "functions.h"
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

class Args {
   public:
    string serverName;
    string serverPort;
    int account_id;

   public:
    Args(string name, string port, int id)
        : serverName(name), serverPort(port), account_id(id) {}
    ~Args();
};

class Client {
   private:
    string serverName;
    string serverPort;
    int account_id;
    int server_fd;
    float balance;
    unordered_map<string, int> symbol; /*symbols:amount owned by this client*/

   private:
    void sendTransRequestAndGetResponse();
    void sendCreateRequestAndGetResponse();
    string getCreateRequest();

   public:
    Client(string name, string port, int id);
    ~Client() { close(server_fd); }
    void run();
    void sendCreateRequest();
    void printCreateRequest() {
        string req = getCreateRequest();
        cout << req << endl;
    }

    // for test
    friend std::ostream& operator<<(std::ostream& out, const Client& client);
};

//线程启动函数，声明为模板函数
template <typename TYPE, void (TYPE::*_RunThread)()>
void* _thread_run(void* param) {
    TYPE* This =
        (TYPE*)param;  //传入的是object的this指针，用于启动非静态成员函数
    This->_RunThread();
    return NULL;
}

std::ostream& operator<<(std::ostream& out, const Client& client);

#endif