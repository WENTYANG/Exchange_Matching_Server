#ifndef _REQUEST_H
#define _REQUEST_H

#include <cstdio>
#include <cstdlib>
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

/* ------------------------ Abstract Request ------------------------ */
class SubRequest {
   public:
    int account_id;

   public:
    SubRequest(int id) : account_id(id) {}
    virtual ~SubRequest(){};
    virtual void execute(XMLDocument& response) = 0;
    virtual void printSubRequest() = 0;
    virtual void reportSuccess(XMLDocument& response) = 0;
    virtual void reportError(XMLDocument& response, string msg) = 0;
};

class Request {
   public:
    XMLDocument response;
    vector<SubRequest*> subRequests;

   public:
    virtual void printRequest() {
        for (auto ptr : subRequests) {
            ptr->printSubRequest();
        }
    }
    virtual void executeRequest() {
        for (SubRequest* ptr : subRequests) {
            ptr->execute(response);
        }
    }
    Request() {
        // Add declaration for response xml (e.g. <?xml version="1.0"
        // encoding="utf-8" standalone="yes" ?>)
        tinyxml2::XMLDeclaration* declaration = response.NewDeclaration();
        response.InsertFirstChild(declaration);
        // Create root element:<results></results>
        XMLElement* root = response.NewElement("results");
        response.InsertEndChild(root);
    }
    virtual ~Request() {
        for (auto ptr : subRequests)
            delete (ptr);
    }

    // for test
    virtual void saveResponse() {
        response.SaveFile("test-response.xml");
        // response.Print();
    }
};

/* ------------------------ "CREATE" Attribute ------------------------ */
class Account : public SubRequest {
   public:
    float balance;

   public:
    Account(int id, int balance) : SubRequest(id), balance(balance) {}
    virtual void execute(XMLDocument& response);
    virtual void reportSuccess(XMLDocument& response);
    virtual void reportError(XMLDocument& response, string msg);
    virtual void printSubRequest() {
        cout << "Accounts id: " << account_id << " balance:" << balance << endl;
    };
};

class Symbol : public SubRequest {
   public:
    string sym;  // symbol name
    int num;

   public:
    Symbol(string sym, int id, int n) : SubRequest(id), sym(sym), num(n) {}
    virtual void execute(XMLDocument& response);
    virtual void reportSuccess(XMLDocument& response);
    virtual void reportError(XMLDocument& response, string msg);
    virtual void printSubRequest() {
        cout << "sym: " << sym << " account_id: " << account_id
             << " num:" << num << endl;
    };
};

/* ------------------------ "TRANSACTION" Attribute ------------------------ */
class Order : public SubRequest {
 public:
  string sym;
  int amount;
  float limit;
  int trans_id;

 public:
  Order(int id, string sym, int amount, int limit) :
      SubRequest(id), sym(sym), amount(amount), limit(limit), trans_id(-1) {}
  virtual void execute(XMLDocument & response);
  virtual void printSubRequest() {
    cout << "Order:" << endl;
    cout << "sym: " << sym << " amount:" << amount << " limit:" << limit << endl;
  }
  virtual void reportSuccess(XMLDocument & response);
  virtual void reportError(XMLDocument & response, string msg);

 private:
  bool isValid();
  void match(int o_trans_id,
             const string & o_time,
             int o_amount,
             float o_limit,
             int o_account_id,
             int o_version);
};

class Query : public SubRequest {
   public:
    int trans_id;

   public:
    Query(int accountID, int transId)
        : SubRequest(accountID), trans_id(transId) {}
    virtual void execute(XMLDocument& response);
    virtual void printSubRequest() {
        cout << "Query:" << endl;
        cout << trans_id << endl;
    }
    virtual void reportSuccess(XMLDocument& response);
    virtual void reportError(XMLDocument& response, string msg);
};

class Cancel : public SubRequest {
   public:
    int trans_id;

   public:
    Cancel(int accountID, int transId)
        : SubRequest(accountID), trans_id(transId) {}
    virtual void execute(XMLDocument& response);
    virtual void printSubRequest() {
        cout << "Cancel:" << endl;
        cout << trans_id << endl;
    }
    virtual void reportSuccess(XMLDocument& response);
    virtual void reportError(XMLDocument& response, string msg);
};

/* ------------------------ "CREATE" Request ------------------------ */
class CreateRequest : public Request {};

/* ------------------------ "TRANSACTION" Request ------------------------ */
class TransRequest : public Request {};

#endif