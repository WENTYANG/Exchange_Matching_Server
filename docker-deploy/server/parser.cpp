#include "parser.h"

/*
    Convert xml string to XMLDocument object(allocated memory)
    After parsing, xmlDocument object needs to be deleted.
*/
XMLDocument* convert_to_file(string xml) {
    tinyxml2::XMLDocument* doc = new XMLDocument();
    doc->Parse(xml.c_str());
    return doc;
}

/*
    Determine the type of request: create or transaction
    return int value;
*/
int request_type(XMLDocument* xml) {
    XMLElement* root = xml->RootElement();
    if (root == nullptr) {
        cout << "NULL!" << endl;
    }
    if (strcmp(root->Name(), "create") == 0)
        return CREATE;
    else if (strcmp(root->Name(), "transactions") == 0)
        return TRANSACTION;
    else {
        throw(MyException("Invalid request type"));
    }
}

/*
    parse Create type request. return Request object. This object needs to be
    deleted manually.
*/
Request* parse_create(XMLDocument* xml) {
    CreateRequest* request = new CreateRequest();
    XMLElement* root = xml->RootElement();
    XMLElement* currElem = root->FirstChildElement();
    while (currElem) {
        // const XMLAttribute *attribute=currElem->FirstAttribute();
        // const XMLAttribute* id_attr = currElem->FindAttribute("account_id")

        if (strcmp(currElem->Name(), "account") == 0) {  // Create accounts
            int id = currElem->FirstAttribute()->IntValue();
            // currElem->FirstAttribute()->Next()
            int balance = currElem->FindAttribute("balance")->IntValue();
            Account* newAccount = new Account(id, balance);
            request->subRequests.push_back(newAccount);

        } else if (strcmp(currElem->Name(), "symbol") == 0) {  // Create symbols
            string sym = currElem->FirstAttribute()->Value();
            XMLElement* currAcount = currElem->FirstChildElement();
            while (currAcount) {
                int id = currAcount->FirstAttribute()->IntValue();
                int num = atoi(currAcount->GetText());
                Symbol* symbol = new Symbol(sym, id, num);
                currAcount = currAcount->NextSiblingElement();
                request->subRequests.push_back(symbol);
            }
        }
        currElem = currElem->NextSiblingElement();
    }
    return request;
}

/*
    parse Transaction type request. return Request object. This object needs to
   be deleted manually.
*/
Request* parse_trans(XMLDocument* xml) {
    TransRequest* request = new TransRequest();
    XMLElement* root = xml->RootElement();
    request->account_id = root->FirstAttribute()->IntValue();
    XMLElement* currElem = root->FirstChildElement();
    while (currElem) {
        if (strcmp(currElem->Name(), "order") == 0) {
            string sym = currElem->FirstAttribute()->Value();
            int amount = currElem->FindAttribute("amount")->IntValue();
            int limit = currElem->FindAttribute("limit")->IntValue();
            Order order(sym, amount, limit);
            request->orders.push_back(order);
        }
        if (strcmp(currElem->Name(), "query") == 0) {
            int trans_id = currElem->FirstAttribute()->IntValue();
            request->queries.push_back(trans_id);
        }
        if (strcmp(currElem->Name(), "cancel") == 0) {
            int trans_id = currElem->FirstAttribute()->IntValue();
            request->cancels.push_back(trans_id);
        }
        currElem = currElem->NextSiblingElement();
    }
    return request;
}