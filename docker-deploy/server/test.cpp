#include <fstream>
#include <iostream>
#include "parser.h"
#include "request.h"
#include "server.h"
#include "sql_function.h"

string getXMLbyString(string name) {
    string line, xml;
    ifstream ifs(name.c_str());
    if (ifs.is_open()) {
        while (getline(ifs, line)) {
            xml += line;
        }
        ifs.close();
    }
    return xml;
}

void parseXML(string xml) {
    XMLDocument* doc = convert_to_file(xml);
    int type = request_type(doc);
    if (type == TRANSACTION) {
        Request* trans = parse_trans(doc);
        trans->printRequest();
        delete (trans);
    } else if (type == CREATE) {
        Request* create = parse_create(doc);
        create->printRequest();
        delete (create);
    }
    delete (doc);
}

Request* getParsed(string xml) {
    XMLDocument* doc = convert_to_file(xml);
    int type = request_type(doc);
    Request* req = nullptr;
    if (type == TRANSACTION) {
        req = parse_trans(doc);
    } else if (type == CREATE) {
        req = parse_create(doc);
    }
    delete doc;
    return req;
}

void testAdd() {
    Server s("12345");
    s.connectDB("exchange_server", "postgres", "passw0rd");
    addAccount(C, 1, 100);
    addAccount(C, 2, 200);
    addSymbol(C, "Duke", 1, 10);
    addSymbol(C, "Duke", 1, 10);
    addSymbol(C, "Duke", 2, 10);
}

void testExecute() {
    Server s("12345");
    s.connectDB("exchange_server", "postgres", "passw0rd");
    string xml = getXMLbyString("../xml/create.txt");
    // string xml = getXMLbyString("../xml/trans.txt");
    Request* req = getParsed(xml);
    req->executeRequest();
    req->saveResponse();
}

void testOrderNoMatch() {
    Server s("12345");
    s.connectDB("exchange_server", "postgres", "passw0rd");
    string xml = getXMLbyString("../xml/create2.xml");
    Request* req = getParsed(xml);
    req->executeRequest();

    xml = getXMLbyString("../xml/order_test1.xml");
    req = getParsed(xml);
    req->executeRequest();
    req->saveResponse();
}

void testOrderMatch() {
    Server s("12345");
    s.connectDB("exchange_server", "postgres", "passw0rd");

    // 初始化账户和对应symbol
    string xml = getXMLbyString("../xml/create.xml");
    Request* req = getParsed(xml);
    req->executeRequest();

    // 读取并执行order
    xml = getXMLbyString("../xml/order_TestForMatch.xml");
    req = getParsed(xml);
    req->executeRequest();
    req->saveResponse();
}

void testQueryResponse() {
    Server s("12345");
    s.connectDB("exchange_server", "postgres", "passw0rd");
    string xml = getXMLbyString("../xml/create2.xml");
    Request* req = getParsed(xml);
    req->executeRequest();

    xml = getXMLbyString("../xml/order_test1.xml");
    req = getParsed(xml);
    req->executeRequest();

    xml = getXMLbyString("../xml/query.xml");
    req = getParsed(xml);
    req->executeRequest();
    req->saveResponse();
}

void testCancel() {
    Server s("12345");
    s.connectDB("exchange_server", "postgres", "passw0rd");
    string xml = getXMLbyString("../xml/create2.xml");

    size_t size = sizeof(char) * (xml.length() + 1);
    xml = to_string(size) + xml;
    int len = getContentLength(xml);
    cout << "length: " << len << endl;

    Request* req = getParsed(xml);
    req->executeRequest();

    xml = getXMLbyString("../xml/order_test1.xml");

    size = sizeof(char) * (xml.length() + 1);
    xml = to_string(size) + xml;
    len = getContentLength(xml);
    cout << "length: " << len << endl;

    req = getParsed(xml);
    req->executeRequest();

    xml = getXMLbyString("../xml/cancel1.xml");

    size = sizeof(char) * (xml.length() + 1);
    xml = to_string(size) + xml;
    len = getContentLength(xml);
    cout << "length: " << len << endl;

    req = getParsed(xml);
    req->executeRequest();
    req->saveResponse();
}

int main() {
    testCancel();
    return 0;
}

void createXML() {
    tinyxml2::XMLDocument doc;

    // 1.添加声明
    tinyxml2::XMLDeclaration* declaration = doc.NewDeclaration();
    doc.InsertFirstChild(declaration);

    // 2.创建根节点
    tinyxml2::XMLElement* root = doc.NewElement("school");
    doc.InsertEndChild(root);

    // 3.创建子节点
    tinyxml2::XMLElement* childNodeStu = doc.NewElement("student");
    tinyxml2::XMLElement* childNodeTea = doc.NewElement("teacher");
    tinyxml2::XMLElement* childNodeTeaGender = doc.NewElement("gender");

    // 4.为子节点增加内容
    tinyxml2::XMLText* contentStu = doc.NewText("stu");
    childNodeStu->InsertFirstChild(contentStu);

    tinyxml2::XMLText* contentGender = doc.NewText("man");
    childNodeTeaGender->InsertFirstChild(contentGender);

    // 5.为子节点增加属性
    childNodeStu->SetAttribute("Name", "libai");

    root->InsertEndChild(childNodeStu);  // childNodeStu是root的子节点
    root->InsertEndChild(childNodeTea);  // childNodeTea是root的子节点
    childNodeTea->InsertEndChild(
        childNodeTeaGender);  // childNodeTeaGender是childNodeTea的子节点

    // 6.保存xml文件
    doc.SaveFile("school.xml");
    doc.Print();

    // XML 写到字符串
    XMLPrinter printer;
    doc.Print(&printer);  //将Print打印到Xmlprint类中 即保存在内存中
    string buf = printer.CStr();  //转换成const char*类型
    cout << buf << endl;          // buf即为创建后的XML 字符串。
}