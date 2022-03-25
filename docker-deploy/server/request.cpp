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
    try {
        addAccount(C, account_id, balance);
    } catch (const std::exception& e) {
        reportError(response, e.what());
        return;
    }
    reportSuccess(response);
}

/*
    Add a "created" node into the response xml of request
    <created id="ACCOUNT_ID"/>
*/
void Account::reportSuccess(XMLDocument& response) {
    /*
    Attribute "id"="account_id" 可能setattribute的时候要转成str
    */
    XMLElement* root = response.RootElement();
    XMLElement* created = response.NewElement("created");
    created->SetAttribute("id", account_id);
    root->InsertEndChild(created);
}

/*
    Add an "error" node into the response xml of request
    <error id="ACCOUNT_ID">Msg</error>
*/
void Account::reportError(XMLDocument& response, string msg) {
    XMLElement* root = response.RootElement();
    XMLElement* error = response.NewElement("error");
    error->SetAttribute("id", account_id);
    XMLText* content = response.NewText(msg.c_str());
    error->InsertFirstChild(content);
    root->InsertEndChild(error);
}

void Symbol::execute(XMLDocument& response) {
    try {
        addSymbol(C, sym, account_id, num);
    } catch (const std::exception& e) {
        reportError(response, e.what());
        return;
    }
    reportSuccess(response);
}

/*
    Add a "created" node into the response xml of request
    <created sym="SYM" id="ACCOUNT_ID"/>
*/
void Symbol::reportSuccess(XMLDocument& response) {
    XMLElement* root = response.RootElement();
    XMLElement* created = response.NewElement("created");
    created->SetAttribute("sym", sym.c_str());
    created->SetAttribute("id", account_id);
    root->InsertEndChild(created);
}

/*
    Add an "error" node into the response xml of request
    <error  sym="SYM"  id="ACCOUNT_ID">Msg</error>
*/
void Symbol::reportError(XMLDocument& response, string msg) {
    XMLElement* root = response.RootElement();
    XMLElement* error = response.NewElement("error");
    error->SetAttribute("sym", sym.c_str());
    error->SetAttribute("id", account_id);
    XMLText* content = response.NewText(msg.c_str());
    error->InsertFirstChild(content);
    root->InsertEndChild(error);
}

void CreateRequest::executeRequest() {
    for (SubCreateRequest* ptr : subRequests) {
        ptr->execute(response);
    }
}

void TransRequest::executeRequest() {}

/*
TODO:
    1. sql_function.cpp中的 addSymbol() 账户存在时，update symbol。
    atomic sql完成 或者 read-write(加一个version列)
    卖的时候不删除，amount改成0.
    2. test



*/