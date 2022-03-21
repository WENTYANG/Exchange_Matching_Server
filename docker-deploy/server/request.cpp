#include "request.h"

void CreateRequest::executeRequest() {}
void TransRequest::executeRequest() {}

void CreateRequest::printRequest()
{
    cout << "CreateRequest" << endl;
    cout << "Accounts" << endl;
    for (auto a : accounts)
    {
        cout << "id: " << a.account_id << " balance:" << a.balance << endl;
    }
    cout << "Symbols" << endl;
    for (auto s : symbols)
    {
        cout << "sym: " << s.sym << endl;
        for (auto sh : s.shares)
        {
            cout << "id: " << sh.account_id << " num:" << sh.num << endl;
        }
    }
}
void TransRequest::printRequest()
{
    cout << "TransRequest" << endl;
    cout << "Orders" << endl;
    for (auto o : orders)
    {
        cout << "sym: " << o.sym << " amount:" << o.amount
             << " limit:" << o.limit << endl;
    }
    cout << "Queries" << endl;
    for (auto q : queries)
    {
        cout << q << endl;
    }
    cout << "Cancels" << endl;
    for (auto c : cancels)
    {
        cout << c << endl;
    }
}
