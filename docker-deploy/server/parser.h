#ifndef _PARSER_H
#define _PARSER_H
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>
#include <vector>
#include "exception.h"
#include "tinyxml2.h"
#include "request.h"

using namespace tinyxml2;
using namespace std;

/* ------------------------ Parse functions ------------------------ */
XMLDocument *convert_to_file(string xml);
int request_type(XMLDocument *xml);
Request *parse_create(XMLDocument *xml);
Request *parse_trans(XMLDocument *xml);

#endif