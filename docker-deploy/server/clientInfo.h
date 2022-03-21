#ifndef _CLIENTINFO_H
#define _CLIENTINFO_H

#include <unistd.h>
#include <iostream>
#include <string>

using namespace std;

class ClientInfo {
   public:
    int client_fd;
    int client_id;
    string XMLrequest;

   public:
    ClientInfo(int fd, int id, string request)
        : client_fd(fd), client_id(id), XMLrequest(request) {}
    ~ClientInfo() { close(client_fd); }
    void showInfo() {
        cout << "client_fd: " << client_fd << endl;
        cout << "client_id: " << client_id << endl;
        cout << "XMLrequest: " << XMLrequest << endl;
    }
};

#endif