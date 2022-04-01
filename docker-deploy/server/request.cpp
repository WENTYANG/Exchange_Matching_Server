#include "request.h"

#include "server.h"

/* ------------------------ "Account" Function ------------------------ */
void Account::execute(XMLDocument & response, connection * C) {
  try {
    addAccount(C, account_id, balance);
  }
  catch (const std::exception & e) {
    reportError(response, e.what());
    return;
  }
  reportSuccess(response);
}

/*
    Add a "created" node into the response xml of request
    <created id="ACCOUNT_ID"/>
*/
void Account::reportSuccess(XMLDocument & response) {
  /*
        Attribute "id"="account_id" 可能setattribute的时候要转成str
        */
  XMLElement * root = response.RootElement();
  XMLElement * created = response.NewElement("created");
  created->SetAttribute("id", account_id);
  root->InsertEndChild(created);
}

/*
    Add an "error" node into the response xml of request
    <error id="ACCOUNT_ID">Msg</error>
*/
void Account::reportError(XMLDocument & response, string msg) {
  XMLElement * root = response.RootElement();
  XMLElement * error = response.NewElement("error");
  error->SetAttribute("id", account_id);
  XMLText * content = response.NewText(msg.c_str());
  error->InsertFirstChild(content);
  root->InsertEndChild(error);
}

/* ------------------------ "Symbol" Function ------------------------ */
void Symbol::execute(XMLDocument & response, connection * C) {
  try {
    addSymbol(C, sym, account_id, num);
  }
  catch (const std::exception & e) {
    reportError(response, e.what());
    return;
  }
  reportSuccess(response);
}

/*
    Add a "created" node into the response xml of request
    <created sym="SYM" id="ACCOUNT_ID"/>
*/
void Symbol::reportSuccess(XMLDocument & response) {
  XMLElement * root = response.RootElement();
  XMLElement * created = response.NewElement("created");
  created->SetAttribute("sym", sym.c_str());
  created->SetAttribute("id", account_id);
  root->InsertEndChild(created);
}

/*
    Add an "error" node into the response xml of request
    <error  sym="SYM"  id="ACCOUNT_ID">Msg</error>
*/
void Symbol::reportError(XMLDocument & response, string msg) {
  XMLElement * root = response.RootElement();
  XMLElement * error = response.NewElement("error");
  error->SetAttribute("sym", sym.c_str());
  error->SetAttribute("id", account_id);
  XMLText * content = response.NewText(msg.c_str());
  error->InsertFirstChild(content);
  root->InsertEndChild(error);
}

/* ------------------------ "Order" Function ------------------------ */
void Order::execute(XMLDocument & response, connection * C) {
  int version;
  while (1) {
    try {
      isValid(version, C);  // check the validity of the order.
      reduceMoneyOrSymbol(C, sym, account_id, amount, limit, version);
      break;
    }
    catch (const VersionErrorException & e) {
      continue;
    }
    catch (const std::exception & e) {  // handle invalid order
      reportError(response, e.what());
      return;
    }
  }

  // get possible orders and then try to match. using optimistic lock and roll
  // back to control version.
  while (1) {
    try {
      result list = getEligibleOrders(C, sym, amount, limit);
      if (list.empty()) {  // no eligible Orders
        addOrder(C,
                 this->trans_id,
                 this->amount,
                 this->limit,
                 this->account_id,
                 this->sym,
                 "open");
        reportSuccess(response);
        // cout << "no eligible Orders\n";
        return;
      }
      for (auto const & order : list) {
        float o_limit = order[4].as<float>();
        int o_version = order[7].as<int>();
        int o_amount = order[3].as<int>();
        int o_account_id = order[1].as<int>();
        int o_trans_id = order[0].as<int>();
        string o_time = order[6].as<string>();

        if (amount > 0) {  // buy order
          if (limit - o_limit > 1e-6 ||
              abs(limit - o_limit) < 1e-6) {  // match successfully(>=)
            match(C, o_trans_id, o_time, o_amount, o_limit, o_account_id, o_version);
          }
          else {  // match unsuccessfully.
            break;
          }
        }
        else if (amount < 0) {  // sell order
          if (limit - o_limit < 1e-6 ||
              abs(limit - o_limit) < 1e-6) {  // match successfully((<=))
            match(C, o_trans_id, o_time, o_amount, o_limit, o_account_id, o_version);
          }
          else {  // match unsuccessfully.
            break;
          }
        }
        if (amount == 0) {  //all the amount has been matched and executed, stop matching 
          break;
        }
      }
      if (abs(amount) > 0) {  // remain unmatch portion
        // cout << "remain unmatch portion\n";
        addOrder(C,
                 this->trans_id,
                 this->amount,
                 this->limit,
                 this->account_id,
                 this->sym,
                 "open");
      }
      break;
    }
    catch (const VersionErrorException & e) {
      continue;
    }
    catch (const std::exception & e) {
      std::cerr << e.what() << '\n';
    }
  }
  reportSuccess(response);
}

/*
    Compare the number of both sides. One with smaller amount
   becomes executed. The other with larger amount will modify its amount and
   spilt a sub-order. Then executed orders for both sides.
*/
void Order::match(connection * C,
                  int o_trans_id,
                  const string & o_time,
                  int o_amount,
                  float o_limit,
                  int o_account_id,
                  int o_version) {
  int myAmount = abs(amount);
  int opponentAmount = abs(o_amount);
  char myStatus = amount > 0 ? 'B' : 'S';

  if (myAmount >= opponentAmount) {
    setOrderExecuted(C,
                     o_trans_id,
                     o_time,
                     o_version);  // 将对方订单整个设置为executed（primary key located）
    addOrder(C,
             this->trans_id,
             -1 * o_amount,
             o_limit,
             this->account_id,
             this->sym,
             "executed");  // 插入一个我的executed订单，amount为部分成交额

    int myExecutedAmount = myStatus == 'B' ? opponentAmount : -1 * opponentAmount;
    executeOrder(C,
                 this->account_id,
                 this->sym,
                 o_limit,
                 myExecutedAmount);  // 执行我的订单
    executeOrder(C, o_account_id, this->sym, o_limit,
                 o_amount);              // 执行对方的订单
    if (this->limit - o_limit > 1e-6) {  // 我是买单时，对成交差价部分进行退款
      refund(C, this->limit - o_limit, opponentAmount, this->account_id);
    }
    myAmount -= opponentAmount;
  }
  else {
    int o_remain_amount =
        myStatus == 'B' ? -1 * (opponentAmount - myAmount) : (opponentAmount - myAmount);
    updateOpenOrder(C,
                    o_remain_amount,
                    o_trans_id,
                    o_time,
                    o_version);  // 更新对方订单，amount调整为剩余数量
    addOrder(C,
             this->trans_id,
             this->amount,
             o_limit,
             this->account_id,
             this->sym,
             "executed");  // 插入一个我的executed订单，amount为全部成交额
    addOrder(C,
             o_trans_id,
             -1 * this->amount,
             o_limit,
             o_account_id,
             this->sym,
             "executed");  // 插入一个对方部分成交的executed订单

    int opponentExecutedAmount = myStatus == 'B' ? -1 * myAmount : myAmount;
    executeOrder(C, this->account_id, this->sym, o_limit,
                 this->amount);  // 执行我的订单
    executeOrder(C,
                 o_account_id,
                 this->sym,
                 o_limit,
                 opponentExecutedAmount);  //执行对面的订单
    if (this->limit - o_limit > 1e-6) {  // 我是买单时，对成交差价部分进行退款
      refund(C, this->limit - o_limit, myAmount, this->account_id);
    }
    myAmount = 0;
  }

  amount = myStatus == 'B' ? myAmount : -1 * myAmount;
}

/*
    Validate the order, throws exception if the order is invalid. An order is
   invalid if: 1) The account id doesn't exist 2) There is not enough money on
   the account for a buy order 3) There is not enough shares of symbol on the
   account for a sell order.
   Return the related version number through reference. If it is a sell order,
   return related version number in SYMBOL table, else return realted verion
   number in ACCOUNT table
*/
bool Order::isValid(int & version, connection * C) {
  nontransaction N(*C);
  // Account id exists
  stringstream sql;
  sql << "SELECT BALANCE, VERSION FROM ACCOUNT WHERE ACCOUNT_ID=" << account_id << ";";
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
    version = B[0][1].as<int>();
  }
  else if (amount < 0) {
    // For sell order, ensure enough symbol on account
    sql << "SELECT AMOUNT, VERSION FROM SYMBOL WHERE ACCOUNT_ID=" << account_id
        << " AND SYM=" << N.quote(sym) << ";";
    result S(N.exec(sql.str()));
    if (S.empty() || S[0][0].as<int>() < -amount) {
      throw MyException("Insufficient symbol on seller's Account");
    }
    version = S[0][1].as<int>();
  }
  else {
    throw MyException("Invalid amount: amount cannot be 0");
  }
  return true;
}

/*
    Add a "opened" node into the response xml of request
    <opened sym="SYM" amount="AMT" limit="LMT" id="TRANS_ID"/>
*/
void Order::reportSuccess(XMLDocument & response) {
  XMLElement * root = response.RootElement();
  XMLElement * opened = response.NewElement("opened");
  opened->SetAttribute("sym", sym.c_str());
  opened->SetAttribute("amount", amount);
  opened->SetAttribute("limit", limit);
  opened->SetAttribute("id", trans_id);
  root->InsertEndChild(opened);
}

/*
    Add an "error" node into the response xml of request
    <error sym="SYM" amount="AMT" limit="LMT">Message</error>
*/
void Order::reportError(XMLDocument & response, string msg) {
  XMLElement * root = response.RootElement();
  XMLElement * error = response.NewElement("error");
  error->SetAttribute("sym", sym.c_str());
  error->SetAttribute("amount", amount);
  error->SetAttribute("limit", limit);
  XMLText * content = response.NewText(msg.c_str());
  error->InsertFirstChild(content);
  root->InsertEndChild(error);
}

/* ------------------------ "Query" Function ------------------------ */
void Query::execute(XMLDocument & response, connection * C) {
  query_result = searchOrders(C, trans_id);
  if (query_result.empty()) {
    reportError(response, "Invalid query: Trans_id doesn't exist");
  }
  reportSuccess(response);
}

/*
    Add the query result into the response xml of request
    <status id="TRANS_ID">
        <open shares=.../>
        <canceled shares=... time=.../>
        <executed shares=... price=... time=.../>
    </status>
*/
void Query::reportSuccess(XMLDocument & response) {
  XMLElement * root = response.RootElement();
  //<status id="TRANS_ID">
  XMLElement * status = response.NewElement("status");
  status->SetAttribute("id", trans_id);

  for (auto const & order : query_result) {
    string state = order[0].as<string>();
    int shares = order[1].as<int>();

    //<open shares=.../>
    if (state == "open") {
      XMLElement * open = response.NewElement("open");
      open->SetAttribute("shares", shares);
      status->InsertEndChild(open);
    }
    else {
      float price = order[2].as<float>();
      string time = order[3].as<string>();

      //<canceled shares=... time=.../>
      if (state == "canceled") {
        XMLElement * canceled = response.NewElement("canceled");
        canceled->SetAttribute("shares", shares);
        canceled->SetAttribute("time", time.c_str());
        status->InsertEndChild(canceled);
      }

      else {
        //<executed shares=... price=... time=.../>
        XMLElement * executed = response.NewElement("executed");
        executed->SetAttribute("shares", shares);
        executed->SetAttribute("price", price);
        executed->SetAttribute("time", time.c_str());
        status->InsertEndChild(executed);
      }
    }
  }
  root->InsertEndChild(status);
}

/*
    Add an error query node into the response xml of request
    <error id="TRANS_ID"></error>
*/
void Query::reportError(XMLDocument & response, string msg) {
  XMLElement * root = response.RootElement();
  XMLElement * error = response.NewElement("error");
  error->SetAttribute("id", trans_id);
  XMLText * content = response.NewText(msg.c_str());
  error->InsertFirstChild(content);
  root->InsertEndChild(error);
}

/* ------------------------ "Cancel" Function ------------------------ */
void Cancel::execute(XMLDocument & response, connection * C) {
  result R;
  try {
    // cout << "Canceling order: " << trans_id << endl;
    cancelOrder(C, trans_id);
  }
  catch (const std::exception & e) {
    reportError(response, e.what());
    return;
  }
  reportSuccess(response);
}

/*
    Add the cancel result into the response xml of request
    <canceled id="TRANS_ID">
        <canceled shares=... time=.../>
        <executed shares=... price=... time=.../>
    </canceled>
*/
void Cancel::reportSuccess(XMLDocument & response) {
  XMLElement * root = response.RootElement();

  //<canceled id="TRANS_ID">
  XMLElement * canceled = response.NewElement("canceled");
  canceled->SetAttribute("id", trans_id);
  for (auto const & order : subOrders) {
    string state = order[0].as<string>();
    int shares = order[1].as<int>();
    float price = order[2].as<float>();
    string time = order[3].as<string>();

    //<canceled shares=... time=.../>: only one canceled part
    if (state == "canceled") {
      XMLElement * canceled_part = response.NewElement("canceled");
      canceled_part->SetAttribute("shares", shares);
      canceled_part->SetAttribute("time", time.c_str());
      canceled->InsertEndChild(canceled_part);
    }

    //<executed shares=... price=... time=.../>: maybe 0 or multiple
    // executed parts
    else if (state == "executed") {
      XMLElement * executed = response.NewElement("executed");
      executed->SetAttribute("shares", shares);
      executed->SetAttribute("price", price);
      executed->SetAttribute("time", time.c_str());
      canceled->InsertEndChild(executed);
    }
  }
  root->InsertEndChild(canceled);
}

/*
    Add an error cancel node into the response xml of request
    <error id="TRANS_ID"></error>
*/
void Cancel::reportError(XMLDocument & response, string msg) {
  XMLElement * root = response.RootElement();
  XMLElement * error = response.NewElement("error");
  error->SetAttribute("id", trans_id);
  XMLText * content = response.NewText(msg.c_str());
  error->InsertFirstChild(content);
  root->InsertEndChild(error);
}