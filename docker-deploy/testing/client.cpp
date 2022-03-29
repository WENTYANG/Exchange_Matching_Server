#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "exception.h"
#include "socket.h"

using namespace std;

#define MAX_LENGTH 65536

void convertStringToFile(string fileName, string s);
void checkInput(int argc, char * argv[], string & masterName, string & masterPort);

int main(int argc, char * argv[]) {
  // get argc
  string serverName;
  string serverPort;
  checkInput(argc, argv, serverName, serverPort);

  // connect to server and send request
  int server_fd = clientRequestConnection(serverName, serverPort);

  //send request to server
  string XMLrequest = "test xml";
  if (send(server_fd, XMLrequest.c_str(), XMLrequest.length(), 0) <= 0) {
    close(server_fd);
    throw MyException("Fail to send XML request to server.\n");
  }

  //receive response from server
  vector<char> buffer(MAX_LENGTH, 0);
  int len = recv(server_fd, &(buffer.data()[0]), MAX_LENGTH, 0);
  if (len <= 0) {
    throw MyException("Fail to receive response from server.\n");
  }
  string XMLresponse(buffer.data(), len);

  convertStringToFile("",XMLresponse);

  close(server_fd);
}

void checkInput(int argc, char * argv[], string & masterName, string & masterPort) {
  if (argc != 3)
    throw MyException("Invalid Usage. Should be ./player <master_name><port_num>\n");

  masterName = argv[1];
  masterPort = argv[2];
}

void convertStringToFile(string fileName, string s) {
  fstream out(fileName.c_str(), ios::out);
  if (out.is_open()) {
    out << s;
  }
  out.close();
}