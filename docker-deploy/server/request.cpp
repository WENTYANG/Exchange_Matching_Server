#include "request.h"
#include "server.h"

void CreateRequest::printRequest() {
    for (auto ptr : subRequests) {
        ptr->printSubRequest();
    }
}

void TransRequest::printRequest() {
    cout << "TransRequest" << endl;
    cout << "Orders" << endl;
    for (auto o : orders) {
        cout << "sym: " << o.sym << " amount:" << o.amount
             << " limit:" << o.limit << endl;
    }
    cout << "Queries" << endl;
    for (auto q : queries) {
        cout << q << endl;
    }
    cout << "Cancels" << endl;
    for (auto c : cancels) {
        cout << c << endl;
    }
}

void Account::execute(XMLDocument& response) {
    addAccount(C, account_id, balance, response);
}

void Symbol::execute(XMLDocument& response) {
    addSymbol(C, sym, account_id, num, response);
}

void CreateRequest::executeRequest() {
    for (SubCreateRequest* ptr : subRequests) {
        try {
            ptr->execute(response);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            // TODO: add error info to response;怎么传出去？
            continue;
        }
        // record success.
    }
}
void TransRequest::executeRequest() {}

/*
TODO:
    1. 完成在sql_function.cpp中的addAccount(), addSymbol();
    2. 通过exception抛出报错信息，并存储在response中  外层用引用传进来
vector<string> responses ? http://pqxx.org/development/libpqxx/
    3. 修改了Symbol类，对应调整parser.cpp
*/