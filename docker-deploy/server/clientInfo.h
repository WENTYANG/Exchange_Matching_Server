#ifndef _CLIENTINFO_H
#define _CLIENTINFO_H

#include <string>
#include <iostream>

using namespace std;

class clientInfo
{
public:
    int client_fd;
    int client_id;
    string XMLrequest;

public:
    clientInfo(int fd, int id, string request) : client_fd(fd), client_id(id), XMLrequest(request) {}
    ~clientInfo() {
        close(client_fd);
    }
    void showInfo()
    {
        cout << "client_fd: " << client_fd << endl;
        cout << "client_id: " << client_id << endl;
        cout << "XMLrequest: " << XMLrequest << endl;
    }
};


#endif