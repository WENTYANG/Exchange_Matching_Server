#include "Client.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "exception.h"
#include "functions.h"
#include "socket.h"

using namespace std;

#define MAX_LENGTH 65536
#define N_Thread_CREATE 3  //用于创建并发送create type的线程数量

static vector<string> symbolName = {"Byd", "Tesla", "Xpeng", "Nio", "BMW", "ONE"};

Client::Client(string name, string port, int id) :
    serverName(name), serverPort(port), account_id(id) {
  server_fd = clientRequestConnection(serverName, serverPort);
}

void Client::run() {
  vector<pthread_t> createThreads;

  // send a batch of create type request (add account and symbols)
  for (size_t i = 0; i < N_Thread_CREATE; i++) {
    pthread_t t;
    int res =
        pthread_create(&t, NULL, _thread_run<Client, &Client::sendCreateRequest>, this);
    if (res < 0) {
      std::cerr << "pthread create error.\n";
      exit(EXIT_FAILURE);
    }
    createThreads.push_back(t);
  }
  for (size_t i = 0; i < N_Thread_CREATE; i++) {
    pthread_join(createThreads[i], NULL);
  }

  //发完create后，生成 threads 发送大量的trans request
}

/*
    send "CREATE" request, which include one create account request, and several create symbol request.
*/
void Client::sendCreateRequest() {
  pthread_t self = pthread_self();

  string XMLrequest = getCreateRequest();

  // save request and send it to server
  string requestFileName = "t" + to_string(self) + "_createRequest.xml";
  convertStringToFile(requestFileName, XMLrequest);
  if (send(server_fd, XMLrequest.c_str(), XMLrequest.length(), 0) < 0) {
    throw MyException("Fail to send GET request to server.\n");
  }

  // receive response from server
  string responseFileName = "t" + to_string(self) + "_createReponse.xml";
  vector<char> buffer(MAX_LENGTH, 0);
  int len = recv(server_fd, &(buffer.data()[0]), MAX_LENGTH, 0);
  if (len <= 0) {
    throw MyException("Fail to receive response from server.\n");
  }
  string XMLresponse(buffer.data(), len);
  convertStringToFile(responseFileName, XMLresponse);
}


/*
  generate "create" repquest, which include one create account request, and several create symbol request.
  each account has random balance.
  it will create 2 random symbols with random amount in this account.
*/
string Client::getCreateRequest(){
  //TODO  随机函数在functioins.h已经实现
}
