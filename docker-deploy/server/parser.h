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

/* ------------------------ Create request ------------------------ */
class Account {
   public:
    int account_id;
    int balance;
    Account(int id, int balance) : account_id(id), balance(balance){};
};
class Share {
   public:
    int account_id;
    int num;
    Share(int id, int num) : account_id(id), num(num){};
};
class Symbol {
    /* A string of symbol name and a vector of account_ids
    and the number of shares in that account */
   public:
    string sym;
    vector<Share> shares;
    Symbol(string sym) : sym(sym){};
};
class CreateRequest {
   public:
    vector<Account> accounts;
    vector<Symbol> symbols;
    void printRequest() {
        cout << "CreateRequest" << endl;
        cout << "Accounts" << endl;
        for (auto a : accounts) {
            cout << "id: " << a.account_id << " balance:" << a.balance << endl;
        }
        cout << "Symbols" << endl;
        for (auto s : symbols) {
            cout << "sym: " << s.sym << endl;
            for (auto sh : s.shares) {
                cout << "id: " << sh.account_id << " num:" << sh.num << endl;
            }
        }
    }
};

/* ------------------------ Transaction request ------------------------ */
class Order {
   public:
    string sym;
    int amount;
    int limit;
    Order(string sym, int amount, int limit)
        : sym(sym), amount(amount), limit(limit){};
};

class TransRequest {
   public:
    int account_id;
    vector<Order> orders;
    vector<int> queries;
    vector<int> cancels;
    void printRequest() {
        cout << "TransRequest" << endl;
        cout << "Orders" << endl;
        for (auto o : orders) {
            cout << "sym: " << o.sym << " amount:" << o.amount
                 << " limit:" << o.limit << endl;
        }
        cout << "Queries" << endl;
        for (auto q : queries) {
            cout << q << endl;
        }
        cout << "Cancels" << endl;
        for (auto c : cancels) {
            cout << c << endl;
        }
    }
};

/* ------------------------ Parse functions ------------------------ */
XMLDocument* convert_to_file(string xml);
int request_type(XMLDocument* xml);
CreateRequest* parse_create(XMLDocument* xml);
TransRequest* parse_trans(XMLDocument* xml);