#include "server.h"

#define MAX_LENGTH 65536

void Server::run() {
  // initialize database
  connection * C = connectDB("exchange_server", "postgres", "passw0rd");
  initializeDB(C);
  disConnectDB(C);

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

    // server receive request
    string XMLrequest;
    recvRequest(client_fd, XMLrequest);
    size_t firstLine = XMLrequest.find('\n', 0);
    XMLrequest = XMLrequest.substr(firstLine + 1);  // get rid of the number

    // generate new thread for each request
    ClientInfo * info = new ClientInfo(client_fd, client_id, XMLrequest);
    Thread_args * args = new Thread_args(this, info);
    pthread_t thread;
    pthread_create(&thread, NULL, _thread_run<Thread_args, Server, 1>, args);

    client_id++;
  }
}

void Server::cleanResource(connection * C, ClientInfo * info) {
  disConnectDB(C);
  delete info;
}

/* ------------------------ "Request related functions ------------------------ */
void Server::handleRequest(void * info) {
  ClientInfo * client_info = (ClientInfo *)info;

  // connect database in each thread
  connection * C = connectDB("exchange_server", "postgres", "passw0rd");

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
    cleanResource(C, client_info);
  }

  // execute request
  r->executeRequest(C);

  // send back response
  try {
    sendResponse(client_info->client_fd, r->getResponseStr());
  }
  catch (const std::exception & e) {
    std::cerr << e.what() << '\n';
    cleanResource(C, client_info);
  }

  // exit successfully
  cleanResource(C, client_info);
}

void Server::recvRequest(int client_fd, string & wholeRequest) {
  vector<char> buffer(MAX_LENGTH, 0);

  //receive first request.
  int len = recv(client_fd,
                 &(buffer.data()[0]),
                 MAX_LENGTH,
                 0);  // len 是recv实际的读取字节数
  if (len <= 0) {
    close(client_fd);
    throw MyException("fail to accept request.\n");
  }
  string firstRequest(buffer.data(), len);

  int contentLength = getContentLength(firstRequest);
  wholeRequest = firstRequest;
  int remainReceiveLen = contentLength - firstRequest.length();
  int receiveLen = 0;
  while (receiveLen < remainReceiveLen) {
    int len = recv(client_fd, &(buffer.data()[0]), MAX_LENGTH, 0);
    if (len < 0)
      break;
    string temp(buffer.data(), len);
    wholeRequest += temp;
    receiveLen += len;
  }
}

void Server::sendResponse(int client_fd, const string & XMLresponse) {
  int res = send(client_fd, &(XMLresponse.data()[0]), XMLresponse.length(), 0);
  if (res <= 0) {
    throw MyException("fail to send back response.\n");
  }
}

/* ------------------------ "DB related functions ------------------------ */
/*
  connect to specific database using the given userName and passWord, reutnr 
  the connection* C. It will throw an exception if fails. 
*/
connection * Server::connectDB(string dbName, string userName, string password) {
  printf("Connect to %s with User: %s, using Password: %s\n",
         dbName.c_str(),
         userName.c_str(),
         password.c_str());
  connection * C =
      new connection("dbname=" + dbName + " user=" + userName + " password=" + password);
  if (C->is_open()) {
    cout << "Opened database successfully: " << C->dbname() << endl;
  }
  else {
    throw MyException("Can't open database.");
  }
  return C;
}

/*
  initialize database when server run at the first time. It will drop all the existing tables,
  and then create new ones.
*/
void Server::initializeDB(connection * C) {
  dropAllTable(C);
  createTable(C, "../sql/table.sql");
}

/*
  close the connection to database.
*/
void Server::disConnectDB(connection * C) {
  C->disconnect();
}