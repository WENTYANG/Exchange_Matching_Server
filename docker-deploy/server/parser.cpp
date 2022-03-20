#include "parser.h"
using namespace tinyxml2;

XMLDocument* convert_to_file(string xml){
    XMLDocument* doc = new XMLDocument;
    doc.Parse(xml);
    return doc;
}

int request_type(XMLDocument* xml){
    XMLElement* root = doc.RootElement();
    if(strcmp(root->value(), "create")==0) return CREATE;
    else if(strcmp(root->value(), "transactions")==0) return TRANSACTION;
    else{
        throw(MyException("Invalid request type"));
    }
}

CreateRequest parse_create(XMLDocument* xml){

}

TransRequest parse_trans(XMLDocument* xml){}