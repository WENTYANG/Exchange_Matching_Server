#ifndef _PARSER_H
#define _PARSER_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

#include "exception.h"
#include "request.h"
#include "tinyxml2.h"

using namespace tinyxml2;

/* ------------------------ Parse functions ------------------------ */
XMLDocument* convert_to_file(string xml);
int request_type(XMLDocument* xml);
Request* parse_create(XMLDocument* xml);
Request* parse_trans(XMLDocument* xml);
int getContentLength(const string& data);

#endif