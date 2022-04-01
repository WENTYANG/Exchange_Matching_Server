#include <sys/time.h>

#include <iostream>

#include "Client.h"
#include "exception.h"

#define N_client 1  // the number of clients

void checkInput(int argc, char * argv[], string & masterName, string & masterPort);
void * initializeClient(void * ptr);

int main(int argc, char * argv[]) {
  // get argc
  string serverName;
  string serverPort;
  checkInput(argc, argv, serverName, serverPort);

  timeval t_start, t_end;
  gettimeofday(&t_start, NULL);

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

  // get performance metrics
  gettimeofday(&t_end, NULL);
  double runTime =
      (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_usec - t_start.tv_usec) / 1000000.0;
  cout << "total run time:" << runTime << "s.\n";
  int N_totalRequest = N_client * (N_Thread_CREATE + N_Thread_TRANS);
  cout << "avg latency:" << latencySum / N_totalRequest << "s.\n";

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