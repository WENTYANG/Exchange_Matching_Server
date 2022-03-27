#include "request.h"

#include "server.h"

/* ------------------------ "Account" Function ------------------------ */
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

/* ------------------------ "Symbol" Function ------------------------ */
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

/* ------------------------ "Order" Function ------------------------ */
void Order::execute(XMLDocument& response) {
    // check the validity of the order.
    try {
        isValid();
    } catch (const std::exception& e) {
        reportError(response, e.what());
        return;
    }
    reduceMoneyOrSymbol(C,sym,account_id,amount,limit);
    reportSuccess(response);

    // match current order with other possible orders
    result list = getEligibleOrders(C, sym, limit);
    if (list.empty()) {  // not eligible Orders
        addOrder(C, sym, amount, limit, account_id);
        return;
    }
    for(auto l:list){
       // 逐个匹配，直到当前订单的amount为0.
       // 每匹配成功一个，则修改对应的EligibleOrder，解锁,对应的账户增加钱或者symbols. 同时向表中插入买卖双方的executed order(记录可能的spilt情况)
       // 全部匹配完成后，如果当前订单amount>0,则再将其插入表中（open）
       // 最后unlock所有剩余的eligible order
    }
    
}

void Order::reportSuccess(XMLDocument& response){}

void Order::reportError(XMLDocument& response, string msg){}

/* ------------------------ "Query" Function ------------------------ */
void Query::execute(XMLDocument& response) {}

void Query::reportSuccess(XMLDocument& response){}

void Query::reportError(XMLDocument& response, string msg){}

/* ------------------------ "Cancel" Function ------------------------ */
void Cancel::execute(XMLDocument& response) {}

void Cancel::reportSuccess(XMLDocument& response){}

void Cancel::reportError(XMLDocument& response, string msg){}