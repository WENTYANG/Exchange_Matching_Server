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
        while (getline(ifs, line))
            sql.append(line);
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
        "DROP TABLE IF EXISTS account;DROP TABLE IF EXISTS symbol;DROP TABLE "
        "IF EXISTS orders;";

    W.exec(sql);
    W.commit();
    cout << "Drop all the existed table" << endl;
}

/*
    insert a row into table Account. This function will throw exception when it
   fails.
*/
void addAccount(connection* C,
                int account_id,
                float balance,
                XMLDocument& response) {
    /*
    Attribute "id"="account_id" 可能setattribute的时候要转成str
    */
    work W(*C);
    stringstream sql;
    sql << "INSERT INTO ACCOUNT(ACCOUNT_ID, BALANCE) VALUES(" << account_id
        << "," << balance << ");";
    W.exec(sql.str());
    W.commit();

    // Add success response
    XMLElement* root = response.RootElement();
    XMLElement* created = response.NewElement("created");
    created->SetAttribute("id", account_id);
    root->InsertEndChild(created);
}

/*
    insert a row into table Symbol. This function will throw exception when it
   fails.
*/
void addSymbol(connection* C,
               const string& sym,
               int account_id,
               int num,
               XMLDocument& response) {
    /*
    Attribute "id"="account_id" 可能setattribute的时候要转成str
    */
    work W(*C);
    stringstream sql;
    sql << "INSERT INTO SYMBOL(ACCOUNT_ID, SYM, AMOUNT) VALUES(" << account_id
        << "," << W.quote(sym) << "," << num << ");";
    W.exec(sql.str());
    W.commit();

    // Add success response
    XMLElement* root = response.RootElement();
    XMLElement* created = response.NewElement("created");
    created->SetAttribute("sym", sym.c_str());
    created->SetAttribute("id", account_id);
    root->InsertEndChild(created);
}