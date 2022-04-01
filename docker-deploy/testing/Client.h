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
#define N_Thread_CREATE 1  //用于创建并发送create type的线程数量
#define N_Thread_TRANS 0    //用于创建并发送transaction type的线程数量
#define NUM_SYMBOL 2        //每个Create Request中添加的symbol数量
#define INITIAL_SYMBOL_AMOUNT 100  //每个symbol的初始amount
#define INITIAL_BALANCE 1000.0     // account的初始balance
#define NUM_ORDER 2  //每个account分别发送buy和sell的数量 (共NUM_ORDER*2个order)
#define ORDER_PRICE_LOWERBOUND 5   //交易时价格的最低值
#define ORDER_PRICE_UPPERBOUND 30  //交易时价格的最高值

using namespace tinyxml2;
using namespace std;

extern double latencySum;

class Args {
   public:
    int account_id;
    string serverName;
    string serverPort;

   public:
    Args(string name, string port, int id)
        : serverName(name), serverPort(port), account_id(id) {}
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
    template <typename TYPE, int C>  // Thread startup function
    static void* _thread_run(void* param) {
        TYPE* This =
            (TYPE*)param; 
        if (C == 1) {
            This->sendCreateRequestAndGetResponse();
        } else if (C == 2) {
            This->sendTransRequestAndGetResponse();
        }
        return nullptr;
    }

   private:
    void sendTransRequestAndGetResponse();
    void sendCreateRequestAndGetResponse();
    string getCreateRequest();
    string getTransRequest();

   public:
    Client(const string& Name, const string& Port, int id)
        : serverName(Name),
          serverPort(Port),
          account_id(id),
          balance(INITIAL_BALANCE) {
          }
    ~Client() {}
    void run();
    void printCreateRequest() {
        string req = getCreateRequest();
        cout << req << endl;
    }

    // for test
    friend std::ostream& operator<<(std::ostream& out, const Client& client);
};

std::ostream& operator<<(std::ostream& out, const Client& client);

#endif