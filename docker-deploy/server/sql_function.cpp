#include "sql_function.h"

#include "exception.h"

/*
    read sql command from the file and then create tabel using connection *C.
    If fails, it will throw exception.
*/
void createTable(connection* C, string fileName) {
    string sql;
    ifstream ifs(fileName.c_str(), ifstream::in);
    if (ifs.is_open() == true) {
        string line;
        while (getline(ifs, line)) sql.append(line);
    } else {
        throw MyException("fail to open file.");
    }

    work W(*C);
    W.exec(sql);
    W.commit();
}

/*
    Drop all the table in the DataBase. Using for test.
*/
void dropAllTable(connection* C) {
    work W(*C);
    string sql =
        "DROP TABLE IF EXISTS symbol;DROP TABLE IF EXISTS account;DROP TABLE "
        "IF EXISTS orders;";

    W.exec(sql);
    W.commit();
    cout << "Drop all the existed table" << endl;
}

/*
    insert a row into table Account. This function will throw exception when it
    fails.
*/
void addAccount(connection* C, int account_id, float balance) {
    work W(*C);
    stringstream sql;
    sql << "INSERT INTO ACCOUNT(ACCOUNT_ID, BALANCE) VALUES(" << account_id
        << "," << balance << ");";
    W.exec(sql.str());
    W.commit();
}

/*
    Insert a new row into table Symbol. If account does not exist, it will fail
   and throw exception. If the SYM already exist in this account, it will update
   the old value.
   */
void addSymbol(connection* C, const string& sym, int account_id, int num) {
    /*
    Attribute "id"="account_id" 可能setattribute的时候要转成str
    */
    work W(*C);
    stringstream sql;
    sql << "INSERT INTO SYMBOL(ACCOUNT_ID, SYM, AMOUNT) VALUES(" << account_id
        << "," << W.quote(sym) << "," << num
        << ") ON CONFLICT ON CONSTRAINT symbol_pk DO UPDATE SET AMOUNT = "
        << num << "+"
        << "SYMBOL.AMOUNT"
        << ";";
    W.exec(sql.str());
    W.commit();
}

/*
    if current order is a sell order, get all buy orders for the same
   symbol.(price descending sort) if current order is a buy order, get all sell
   orders for the same symbol.(price descending sort) this function will set all
   the eligible orders into lock status(RWlock).
*/
result getEligibleOrders(connection* C, const string& sym, float limit) {}

/*
    insert an order into Order table
*/
void addOrder(connection* C, const string& sym, int amount, float limit,
              int account_id) {}

/*
    reduce the monry or symbol from the corresponding account based on order info.
*/
void reduceMoneyOrSymbol(connection* C, const string& sym, int account_id,
                         int amount, float limit) {}