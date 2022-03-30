#include <iostream>

#include "Client.h"
#include "exception.h"

using namespace std;

int main() {
    Client MyClient("testServer", "1234", 1);
    MyClient.printCreateRequest();
    cout << MyClient;
    return 0;
}