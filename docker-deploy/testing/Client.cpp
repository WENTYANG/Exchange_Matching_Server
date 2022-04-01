#include "Client.h"

#include <sys/time.h>

#include <fstream>
#include <iostream>
#include <mutex>
#include <vector>

#include "exception.h"
#include "socket.h"

using namespace std;
using namespace tinyxml2;

std::mutex mtx;
double latencySum = 0.0;

void createAccount(int account_id, float balance, XMLDocument & request);

void createSymbol(string sym, int account_id, int amount, XMLDocument & request);

void addOrder(string sym, int amount, float limit_price, XMLDocument & request);

static vector<string> symbolName = {"Byd", "Tesla", "Xpeng", "Nio", "BMW", "ONE"};

void Client::run() {
  vector<pthread_t> createThreads;

  // send a batch of create type request (add account and symbols)
  for (size_t i = 0; i < N_Thread_CREATE; i++) {
    pthread_t t;
    int res = pthread_create(&t, NULL, _thread_run<Client, 1>, this);
    if (res < 0) {
      std::cerr << "pthread create error.\n";
      exit(EXIT_FAILURE);
    }
    createThreads.push_back(t);
  }
  for (size_t i = 0; i < N_Thread_CREATE; i++) {
    pthread_join(createThreads[i], NULL);
  }

  // send a batch of TRANSACTION type request (randomly buy symbols, and
  // randomly sell symbols)
  createThreads.clear();
  for (size_t i = 0; i < N_Thread_TRANS; i++) {
    pthread_t t;
    int res = pthread_create(&t, NULL, _thread_run<Client, 2>, this);
    if (res < 0) {
      std::cerr << "pthread create error.\n";
      exit(EXIT_FAILURE);
    }
    createThreads.push_back(t);
  }
  for (size_t i = 0; i < N_Thread_TRANS; i++) {
    pthread_join(createThreads[i], NULL);
  }
}

/*
    send "CREATE" request, which include one create account request, and several
   create symbol request. Then recv response from sever, save it as xml file.
*/
void Client::sendCreateRequestAndGetResponse() {
  int server_fd = clientRequestConnection(serverName, serverPort);
  pthread_t self = pthread_self();

  string XMLrequest = getCreateRequest();

  timeval t_start, t_end;
  gettimeofday(&t_start, NULL);

  // save request and send it to server
  string requestFileName = "./requests/t" + to_string(self) + "_createRequest.xml";
  convertStringToFile(requestFileName, XMLrequest);
  if (send(server_fd, XMLrequest.c_str(), XMLrequest.length(), 0) < 0) {
    throw MyException("Fail to send XML request to server.\n");
  }

  // receive response from server
  string responseFileName = "./responses/t" + to_string(self) + "_createReponse.xml";
  vector<char> buffer(MAX_LENGTH, 0);
  int len = recv(server_fd, &(buffer.data()[0]), MAX_LENGTH, 0);
  if (len <= 0) {
    throw MyException("Fail to receive response from server.\n");
  }
  string XMLresponse(buffer.data(), len);
  convertStringToFile(responseFileName, XMLresponse);

  // calculate the latency
  gettimeofday(&t_end, NULL);
  double runTime =
      (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
  mtx.lock();
  latencySum += runTime;
  mtx.unlock();

  close(server_fd);
  return;
}

/*
    send "TRANSACTION" request, which include several order requests. client
   will randomy buy symbols, and randomly sell the symbols. Then recv response
   from sever, save it as xml file.
*/
void Client::sendTransRequestAndGetResponse() {
  int server_fd = clientRequestConnection(serverName, serverPort);
  pthread_t self = pthread_self();

  string XMLrequest = getTransRequest();

  // save request and send it to server
  string requestFileName = "./requests/t" + to_string(self) + "_transRequest.xml";
  convertStringToFile(requestFileName, XMLrequest);
  if (send(server_fd, XMLrequest.c_str(), XMLrequest.length(), 0) < 0) {
    throw MyException("Fail to send XML request to server.\n");
  }

  // receive response from server
  string responseFileName = "./responses/t" + to_string(self) + "_transReponse.xml";
  vector<char> buffer(MAX_LENGTH, 0);
  int len = recv(server_fd, &(buffer.data()[0]), MAX_LENGTH, 0);
  if (len <= 0) {
    throw MyException("Fail to receive response from server.\n");
  }
  string XMLresponse(buffer.data(), len);
  convertStringToFile(responseFileName, XMLresponse);

  close(server_fd);
  return;
}

/*
  generate "create" repquest, which include one create account request, and
  several create symbol request. each account has random balance. it will create
  several random symbols with random amount in this account.
*/
string Client::getCreateRequest() {
  XMLDocument request;

  // Add declaration for request xml (e.g. <?xml version="1.0"
  // encoding="utf-8" standalone="yes" ?>)
  XMLDeclaration * declaration = request.NewDeclaration();
  request.InsertFirstChild(declaration);

  // Create root element:<create></create>
  XMLElement * root = request.NewElement("create");
  request.InsertEndChild(root);

  // Generate create account request, track balance in the client object
  createAccount(account_id, INITIAL_BALANCE, request);
  balance = INITIAL_BALANCE;

  // Generate create symbol request, track symbol name:amount in the clients
  for (int i = 0; i < NUM_SYMBOL; i++) {
    string sym = symbolName[getRandomINT(0, symbolName.size() - 1)];
    createSymbol(sym, account_id, INITIAL_SYMBOL_AMOUNT, request);
    if (symbol[sym]) {
      symbol[sym] += INITIAL_SYMBOL_AMOUNT;
    }
    else {
      symbol[sym] = INITIAL_SYMBOL_AMOUNT;
    }
  }

  // Convert XMLDocument to string
  XMLPrinter printer;
  request.Print(&printer);
  string requestStr = printer.CStr();

  // Add the length of request to the head of string
  size_t size = sizeof(char) * (requestStr.length() + 1);
  requestStr = to_string(size) + '\n' + requestStr;

  return requestStr;
}

/*
  generate "transaction"(order) repquest, which include 4 orders per request, 2
  buy + 2 sell
  <transactions  id="ACCOUNT_ID">
    <order sym="SYM" amount="AMT" limit="LMT"/>
  </transactions>
*/
string Client::getTransRequest() {
  XMLDocument request;

  // Add declaration for request xml (e.g. <?xml version="1.0"
  // encoding="utf-8" standalone="yes" ?>)
  XMLDeclaration * declaration = request.NewDeclaration();
  request.InsertFirstChild(declaration);

  // Create root element:<create></create>
  XMLElement * root = request.NewElement("transactions");
  root->SetAttribute("id", account_id);
  request.InsertEndChild(root);
  for (int i = 0; i < NUM_ORDER; i++) {
    // Buy
    string sym = symbolName[getRandomINT(0, symbolName.size() - 1)];
    int amount = getRandomINT(1, INITIAL_SYMBOL_AMOUNT);
    float limit_price = getRandomINT(ORDER_PRICE_LOWERBOUND, ORDER_PRICE_UPPERBOUND);
    addOrder(sym, amount, limit_price, request);

    // Sell
    sym = symbolName[getRandomINT(0, symbolName.size() - 1)];
    amount = getRandomINT(-INITIAL_SYMBOL_AMOUNT, -1);
    limit_price = getRandomINT(ORDER_PRICE_LOWERBOUND, ORDER_PRICE_UPPERBOUND);
    addOrder(sym, amount, limit_price, request);
  }

  // Convert XMLDocument to string
  XMLPrinter printer;
  request.Print(&printer);
  string requestStr = printer.CStr();

  // Add the length of request to the head of string
  size_t size = sizeof(char) * (requestStr.length() + 1);
  requestStr = to_string(size) + '\n' + requestStr;
  return requestStr;
}

/*
  Given an XMLDocument object, add an account node under its root.
  This is a helper function of Client::getCreateRequest()
*/
void createAccount(int account_id, float balance, XMLDocument & request) {
  //<account id="ACCOUNT_ID" balance="BALANCE"/>
  XMLElement * root = request.RootElement();
  XMLElement * account = request.NewElement("account");
  account->SetAttribute("id", account_id);
  account->SetAttribute("balance", balance);
  root->InsertEndChild(account);
}

/*
  Given an XMLDocument object, add a symbol node under its root.
  This is a helper function of Client::getCreateRequest()
*/
void createSymbol(string sym, int account_id, int amount, XMLDocument & request) {
  // <symbol sym="SYM">
  XMLElement * root = request.RootElement();
  XMLElement * symbol = request.NewElement("symbol");
  symbol->SetAttribute("sym", sym.c_str());

  // <account id="ACCOUNT_ID">NUM</account>
  XMLElement * account = request.NewElement("account");
  account->SetAttribute("id", account_id);
  XMLText * NUM = request.NewText(to_string(amount).c_str());
  account->InsertFirstChild(NUM);
  symbol->InsertEndChild(account);

  root->InsertEndChild(symbol);
}

/*
  Given an XMLDocument object, add an order node under its root.
  This is a helper function of Client::getTransRequest()
*/
void addOrder(string sym, int amount, float limit_price, XMLDocument & request) {
  XMLElement * root = request.RootElement();
  XMLElement * order = request.NewElement("order");
  order->SetAttribute("sym", sym.c_str());
  order->SetAttribute("amount", amount);
  order->SetAttribute("limit", limit_price);
  root->InsertEndChild(order);
}

/*
  Given an XMLDocument object, add a query or cancel node under its root.
  This is for future implementation to send query request
*/
void addQueryorCancel(string type, int trans_id, XMLDocument & request) {
  XMLElement * root = request.RootElement();
  XMLElement * query = request.NewElement(type.c_str());
  query->SetAttribute("id", trans_id);
  root->InsertEndChild(query);
}

/*
Print Client information
*/
std::ostream & operator<<(std::ostream & out, const Client & client) {
  out << "Account id: " << client.account_id << endl;
  out << "Balance: " << client.balance << endl;
  out << "Symbols: " << endl;
  for (auto & s : client.symbol) {
    cout << s.first << ":" << s.second << endl;
  }
  return out;
}