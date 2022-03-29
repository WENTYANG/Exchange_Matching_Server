#include "server.h"

#define MAX_LENGTH 65536

connection * C;

void Server::run() {
  connectDB("exchange_server", "postgres", "passw0rd");

  // create server socket, listen to port
  int server_fd;
  try {
    server_fd = createServerSocket(portNum);
  }
  catch (const std::exception & e) {
    std::cerr << e.what() << '\n';
    return;
  }

  // server keep runnning
  int client_id = 1;
  while (1) {
    // server accept new connection
    int client_fd;
    string clientIp;
    try {
      client_fd = serverAcceptConnection(server_fd, clientIp);
    }
    catch (const std::exception & e) {
      std::cerr << e.what() << '\n';
      continue;
    }

    // accept new XML request
    vector<char> buffer(MAX_LENGTH, 0);
    int len = recv(client_fd,
                   &(buffer.data()[0]),
                   MAX_LENGTH,
                   0);  // len 是recv实际的读取字节数
    if (len <= 0) {
      std::cerr << "fail to accept request." << '\n';
      close(client_fd);
      continue;
    }

    // generate new thread for each request
    string XMLrequest(buffer.data(), len);
    ClientInfo * info = new ClientInfo(client_fd, client_id, XMLrequest);
    pthread_t thread;
    pthread_create(&thread, NULL, handleRequest, info);

    client_id++;
  }
}

void Server::connectDB(string dbName, string userName, string password) {
  printf("Connect to %s with User: %s, using Password: %s\n",
         dbName.c_str(),
         userName.c_str(),
         password.c_str());
  C = new connection("dbname=" + dbName + " user=" + userName + " password=" + password);
  if (C->is_open()) {
    cout << "Opened database successfully: " << C->dbname() << endl;
  }
  else {
    throw MyException("Can't open database.");
  }

  dropAllTable(C);

  createTable(C, "../sql/table.sql");
}

void * Server::handleRequest(void * info) {
  ClientInfo * client_info = (ClientInfo *)info;

  // parse request
  unique_ptr<Request> r(nullptr);
  try {
    unique_ptr<XMLDocument> xml(convert_to_file(client_info->XMLrequest));
    int type = request_type(xml.get());
    if (type == CREATE) {
      unique_ptr<Request> temp_r(parse_create(xml.get()));
      r = std::move(temp_r);
    }
    else if (type == TRANSACTION) {
      unique_ptr<Request> temp_r(parse_trans(xml.get()));
      r = std::move(temp_r);
    }
  }
  catch (const std::exception & e) {
    std::cerr << e.what() << '\n';
    delete client_info;
    return nullptr;
  }

  // execute request
  r->executeRequest();

  //TODO: send back response
  

  delete client_info;
  return nullptr;
}