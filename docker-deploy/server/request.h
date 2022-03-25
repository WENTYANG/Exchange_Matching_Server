#ifndef _REQUEST_H
#define _REQUEST_H

#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <iostream>
#include <vector>

#include "exception.h"
#include "sql_function.h"
#include "tinyxml2.h"

#define CREATE 1
#define TRANSACTION 0

using namespace std;
using namespace tinyxml2;

/* ------------------------ "CREATE" Attribute ------------------------ */
class SubCreateRequest {
   public:
    virtual void execute() = 0;
    virtual void printSubRequest() = 0;
};

class Account : public SubCreateRequest {
   public:
    int account_id;
    float balance;

   public:
    Account(int id, int balance) : account_id(id), balance(balance) {}
    virtual void execute();
    virtual void printSubRequest() {
        cout << "Accounts id: " << account_id << " balance:" << balance << endl;
    };
};

class Symbol : public SubCreateRequest {
   public:
    string sym;  // symbol name
    int account_id;
    int num;

   public:
    Symbol(string sym, int id, int n) : sym(sym), account_id(id), num(n) {}
    virtual void execute();
    virtual void printSubRequest() {
        cout << "sym: " << sym << " account_id: " << account_id
             << " num:" << num << endl;
    };
};

/* ------------------------ "TRANSACTION" Attribute ------------------------ */
class Order {
   public:
    string sym;
    int amount;
    float limit;

   public:
    Order(string sym, int amount, int limit)
        : sym(sym), amount(amount), limit(limit) {}
};

/* ------------------------ Abstract Request ------------------------ */
class Request {
   public:
    virtual void printRequest() = 0;
    virtual void executeRequest() = 0;
};

/* ------------------------ "CREATE" Request ------------------------ */
class CreateRequest : public Request {
   public:
    vector<SubCreateRequest*> subRequests;

   public:
    CreateRequest() {}
    ~CreateRequest() {
        for (auto ptr : subRequests)
            delete (ptr);
    }
    virtual void printRequest();
    virtual void executeRequest();
};

/* ------------------------ "TRANSACTION" Request ------------------------ */
class TransRequest : public Request {
   public:
    int account_id;
    vector<Order> orders;
    vector<int> queries;
    vector<int> cancels;

   public:
    virtual void printRequest();
    virtual void executeRequest();
};

#endif