#ifndef _REQUEST_H
#define _REQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <vector>
#include "exception.h"
#include "tinyxml2.h"

#define CREATE 1
#define TRANSACTION 0

using namespace tinyxml2;
using namespace std;

/* ------------------------ Create Attribute ------------------------ */
class Account
{
public:
    int account_id;
    int balance;
    Account(int id, int balance) : account_id(id), balance(balance){};
};
class Share
{
public:
    int account_id;
    int num;
    Share(int id, int num) : account_id(id), num(num){};
};
class Symbol
{
    /* A string of symbol name and a vector of account_ids
    and the number of shares in that account */
public:
    string sym;
    vector<Share> shares;
    Symbol(string sym) : sym(sym){};
};

/* ------------------------ Transaction Attribute ------------------------ */
class Order
{
public:
    string sym;
    int amount;
    int limit;
    Order(string sym, int amount, int limit)
        : sym(sym), amount(amount), limit(limit){};
};

/* ------------------------ Abstract Request ------------------------ */
class Request
{
public:
    virtual void printRequest() = 0;
    virtual void executeRequest() = 0;
};

/* ------------------------ "CREATE" Request ------------------------ */
class CreateRequest : public Request
{
public:
    vector<Account> accounts;
    vector<Symbol> symbols;
    virtual void printRequest();
    virtual void executeRequest();
};

/* ------------------------ "TRANSACTION" Request ------------------------ */
class TransRequest : public Request
{
public:
    int account_id;
    vector<Order> orders;
    vector<int> queries;
    vector<int> cancels;
    virtual void printRequest();
    virtual void executeRequest();
};

#endif