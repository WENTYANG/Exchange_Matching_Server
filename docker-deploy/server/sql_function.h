#ifndef _SQL_FUNCTION_H
#define _SQL_FUNCTION_H

#include <pqxx/pqxx>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;
using namespace pqxx;


void creatTable(connection *C, string fileName);
void dropAllTable(connection *C);

#endif