#ifndef _SQL_FUNCTION_H
#define _SQL_FUNCTION_H

#include <fstream>
#include <iostream>
#include <pqxx/pqxx>
#include <string>

using namespace std;
using namespace pqxx;

void creatTable(connection* C, string fileName);
void dropAllTable(connection* C);

#endif