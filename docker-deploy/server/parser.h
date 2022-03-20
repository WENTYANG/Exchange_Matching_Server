#include "tinyxml2.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <vector>
#include <stdio.h>
#include <exception>

#define CREATE 1
#define TRANSACTION 0

using namespace tinyxml2;
using namespace std;

/* ------------------------ Create request ------------------------ */
class Account{
public:
    int account_id;
    int balance;
}
class Symbol{
    /* A string of symbol name and a vector of account_ids 
    and the number of shares in that account */
public:
    string sym;
    typedef struct share_t{
        int account_id;
        int num;
    } share;
    vector<share> shares;
}
class CreateRequest{
public:
    vector<Account> accounts;
    vector<Symbol> symbols;
}

/* ------------------------ Transaction request ------------------------ */
class Transaction{
public:
    int account_id;
}
class Order:public Transaction{
public:
    string sym;
    int amount;
    int limit_price;
}
class Query:public Transaction{
public:
    int trans_id;
}
class Cancel:public Transaction{
public:
    int trans_id;    
}
class TransRequest{
    vector<Order> orders;
    vector<Query> queries;
    vector<Cancel> cancels;
}

/* ------------------------ Parse functions ------------------------ */
XMLDocument* convert_to_file(string xml);
int request_type(XMLDocument* xml);
CreateRequest parse_create(XMLDocument* xml);
TransRequest parse_trans(XMLDocument* xml);

/* ------------------------ exception helper ------------------------ */
class MyException : public std::exception {
   private:
    std::string msg;

   public:
    MyException(std::string msg);

    const char* what() const throw();
};
