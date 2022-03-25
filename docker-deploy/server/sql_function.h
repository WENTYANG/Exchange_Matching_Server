#ifndef _SQL_FUNCTION_H
#define _SQL_FUNCTION_H

#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

using namespace std;
using namespace pqxx;

void createTable(connection* C, const string fileName);
void dropAllTable(connection* C);
void addAccount(connection* C, int account_id, float balance);
void addSymbol(connection* C, const string& sym, int account_id, int num);

#endif