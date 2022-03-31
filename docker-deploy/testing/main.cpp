#include <iostream>

#include "Client.h"
#include "exception.h"
#include<time.h>

#define N_client 100  //生成的client数量

void checkInput(int argc, char * argv[], string & masterName, string & masterPort);
void * initializeClient(void * ptr);

int main(int argc, char * argv[]) {
  // get argc
  string serverName;
  string serverPort;
  checkInput(argc, argv, serverName, serverPort);

  clock_t start,end;
  start = clock();

  // generate clients
  vector<pthread_t> threads;
  for (size_t i = 0; i < N_client; i++) {
    Args * info = new Args(serverName, serverPort, i);
    pthread_t t;
    int res = pthread_create(&t, NULL, initializeClient, info);
    if (res < 0) {
      std::cerr << "pthread create error.\n";
      exit(EXIT_FAILURE);
    }
    threads.push_back(t);
  }
  for (size_t i = 0; i < N_client; i++) {
    pthread_join(threads[i], NULL);
  }
  end=clock();
  cout<<"run time:"<< (double)(end-start)/CLOCKS_PER_SEC <<"s.\n";
  return 0;
}

void checkInput(int argc, char * argv[], string & masterName, string & masterPort) {
  if (argc != 3)
    throw MyException("Invalid Usage. Should be ./client <master_name><port_num>\n");

  masterName = argv[1];
  masterPort = argv[2];
}

void * initializeClient(void * ptr) {
  Args * info = (Args *)ptr;
  Client c(info->serverName, info->serverPort, info->account_id);
  c.run();

  delete info;
  return nullptr;
}