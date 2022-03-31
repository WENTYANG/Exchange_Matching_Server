#include "Client.h"

#include <fstream>
#include <iostream>
#include <vector>

#include "exception.h"
#include "socket.h"

using namespace std;
using namespace tinyxml2;


void createAccount(int account_id, float balance, XMLDocument & request);

void createSymbol(string sym, int account_id, int amount, XMLDocument & request);

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

  // send a batch of TRANSACTION type request (randomly buy symbols, and randomly sell symbols)
  createThreads.clear();
  for (size_t i = 0; i < N_Thread_CREATE; i++) {
    pthread_t t;
    int res = pthread_create(&t, NULL, _thread_run<Client, 2>, this);
    if (res < 0) {
      std::cerr << "pthread create error.\n";
      exit(EXIT_FAILURE);
    }
    createThreads.push_back(t);
  }
  for (size_t i = 0; i < N_Thread_CREATE; i++) {
    pthread_join(createThreads[i], NULL);
  }
}

/*
    send "CREATE" request, which include one create account request, and several create symbol request.
    Then recv response from sever, save it as xml file.
*/
void Client::sendCreateRequestAndGetResponse() {
  int server_fd = clientRequestConnection(serverName, serverPort);
  pthread_t self = pthread_self();

  string XMLrequest = getCreateRequest();

  // save request and send it to server
  string requestFileName = "./results/t" + to_string(self) + "_createRequest.xml";
  convertStringToFile(requestFileName, XMLrequest);
  if (send(server_fd, XMLrequest.c_str(), XMLrequest.length(), 0) < 0) {
    throw MyException("Fail to send XML request to server.\n");
  }

  // receive response from server
  string responseFileName = "./results/t" + to_string(self) + "_createReponse.xml";
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

std::ostream & operator<<(std::ostream & out, const Client & client) {
  out << "Account id: " << client.account_id << endl;
  out << "Balance: " << client.balance << endl;
  out << "Symbols: " << endl;
  for (auto & s : client.symbol) {
    cout << s.first << ":" << s.second << endl;
  }
  return out;
}

/*
    send "TRANSACTION" request, which include several order requests. client will randomy buy symbols, and randomly 
    sell the symbols. Then recv response from sever, save it as xml file.
*/
void Client::sendTransRequestAndGetResponse() {
}