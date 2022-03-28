#ifndef _SQL_FUNCTION_H
#define _SQL_FUNCTION_H

#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

#include "tinyxml2.h"

using namespace std;
using namespace pqxx;
using namespace tinyxml2;

void createTable(connection* C, const string fileName);
void dropAllTable(connection* C);
void addAccount(connection* C, int account_id, float balance);
void addSymbol(connection* C, const string& sym, int account_id, int num);
result getEligibleOrders(connection* C, const string& sym, int amount,
                         float limit);
void addOrder(connection* C, const string& sym, int amount, float limit,
              int account_id);
void reduceMoneyOrSymbol(connection* C, const string& sym, int account_id,
                         int amount, float limit);
void setOrderExecuted(connection* C, const string& o_sym, int o_amount,
                      float o_limit, int o_account_id, int o_version);
void updateOpenOrder(connection* C, const string& o_sym, int o_remain_amount,
                     float o_limit, int o_account_id, int o_version);

#endif