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
    reduceMoneyOrSymbol(C, sym, account_id, amount, limit);
    reportSuccess(response);

    // match current order with other possible orders
    result list = getEligibleOrders(C, sym, amount, limit);
    if (list.empty()) {  // not eligible Orders
        addOrder(C, sym, amount, limit, account_id);
        return;
    }
    for (auto l : list) {
        // Print for test
        cout << "Potential Matchings:" << endl;
        cout << "     "
             << "TRANS_ID ACCOUNT_ID SYM AMOUNT LIMIT_PRICE STATE TIME VERSION"
             << endl;
        cout << "     " << l[0].as<int>() << " " << l[1].as<int>() << " "
             << l[2].as<string>() << " " << l[3].as<int>() << " "
             << l[4].as<float>() << " " << l[5].as<string>() << " "
             << l[6].as<string>() << " " << l[7].as<int>() << endl;

        // 更新为乐观锁：执行时读取version，确保数据库中的version和result中的version匹配，并改写version

        // 逐个匹配，直到当前订单的amount为0.
        // 每匹配成功一个，则修改对应的EligibleOrder，解锁,对应的账户增加钱或者symbols.
        // 同时向表中插入买卖双方的executed order(记录可能的spilt情况)
        // 全部匹配完成后，如果当前订单amount>0,则再将其插入表中（open）
        // 最后unlock所有剩余的eligible order
    }
}

/*
Validate the order, throws exception if the order is invalid. An order is
invalid if: 1) The account id doesn't exist 2) There is not enough money on the
account for a buy order 3) There is not enough shares of symbol on the account
for a sell order
*/
bool Order::isValid() {
    nontransaction N(*C);
    // Account id exists
    stringstream sql;
    sql << "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT_ID=" << account_id << ";";
    result B(N.exec(sql.str()));
    if (B.empty()) {
        throw MyException("Invalid Account");
    }

    sql.clear();
    sql.str("");

    if (amount > 0) {
        // For buy order, ensure enough money on account
        if (B[0][0].as<float>() < amount * limit) {
            throw MyException("Insufficient balance on buyer's account");
        }
    } else if (amount < 0) {
        // For sell order, ensure enough symbol on account
        sql << "SELECT AMOUNT FROM SYMBOL WHERE ACCOUNT_ID=" << account_id
            << " AND SYM=" << N.quote(sym) << ";";
        result S(N.exec(sql.str()));
        if (S.empty() || S[0][0].as<int>() < -amount) {
            throw MyException("Insufficient symbol on seller's Account");
        }
    } else {
        throw MyException("Invalid amount: amount cannot be 0");
    }
    return true;
}

/*
    Add a "opened" node into the response xml of request
    <opened sym="SYM" amount="AMT" limit="LMT" id="TRANS_ID"/>
*/
void Order::reportSuccess(XMLDocument& response) {
    XMLElement* root = response.RootElement();
    XMLElement* opened = response.NewElement("opened");
    opened->SetAttribute("sym", sym.c_str());
    opened->SetAttribute("amount", amount);
    opened->SetAttribute("limit", limit);
    root->InsertEndChild(opened);
}

/*
    Add an "error" node into the response xml of request
    <error sym="SYM" amount="AMT" limit="LMT">Message</error>
*/
void Order::reportError(XMLDocument& response, string msg) {
    XMLElement* root = response.RootElement();
    XMLElement* error = response.NewElement("error");
    error->SetAttribute("sym", sym.c_str());
    error->SetAttribute("amount", amount);
    error->SetAttribute("limit", limit);
    XMLText* content = response.NewText(msg.c_str());
    error->InsertFirstChild(content);
    root->InsertEndChild(error);
}

/* ------------------------ "Query" Function ------------------------ */
void Query::execute(XMLDocument& response) {}

/*
    Add the query result into the response xml of request
    <status id="TRANS_ID">
        <open shares=.../>
        <canceled shares=... time=.../>
        <executed shares=... price=... time=.../>
    </status>
*/
void Query::reportSuccess(XMLDocument& response) {}

void Query::reportError(XMLDocument& response, string msg) {}

/* ------------------------ "Cancel" Function ------------------------ */
void Cancel::execute(XMLDocument& response) {}

/*
    Add the cancel result into the response xml of request
    <canceled id="TRANS_ID">
        <canceled shares=... time=.../>
        <executed shares=... price=... time=.../>
    </canceled>
*/
void Cancel::reportSuccess(XMLDocument& response) {}

void Cancel::reportError(XMLDocument& response, string msg) {}