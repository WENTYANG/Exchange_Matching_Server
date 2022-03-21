#include <fstream>
#include <iostream>
#include "parser.h"

using namespace std;
using namespace tinyxml2;

string getXMLbyString(string name)
{
    string line, xml;
    ifstream ifs(name.c_str());
    if (ifs.is_open())
    {
        while (getline(ifs, line))
        {
            xml += line;
        }
        ifs.close();
    }
    return xml;
}

void parseXML(string xml)
{
    XMLDocument *doc = convert_to_file(xml);
    int type = request_type(doc);
    if (type == TRANSACTION)
    {
        Request *trans = parse_trans(doc);
        trans->printRequest();
        delete (trans);
    }
    else if (type == CREATE)
    {
        Request *create = parse_create(doc);
        create->printRequest();
        delete (create);
    }
    delete (doc);
}

int main()
{
    string xml = getXMLbyString("../xml/create.txt");
    // string xml = getXMLbyString("trans.txt");
    parseXML(xml);
    return 0;
}

void createXML()
{
    tinyxml2::XMLDocument doc;

    // 1.添加声明
    tinyxml2::XMLDeclaration *declaration = doc.NewDeclaration();
    doc.InsertFirstChild(declaration);

    // 2.创建根节点
    tinyxml2::XMLElement *root = doc.NewElement("school");
    doc.InsertEndChild(root);

    // 3.创建子节点
    tinyxml2::XMLElement *childNodeStu = doc.NewElement("student");
    tinyxml2::XMLElement *childNodeTea = doc.NewElement("teacher");
    tinyxml2::XMLElement *childNodeTeaGender = doc.NewElement("gender");

    // 4.为子节点增加内容
    tinyxml2::XMLText *contentStu = doc.NewText("stu");
    childNodeStu->InsertFirstChild(contentStu);

    tinyxml2::XMLText *contentGender = doc.NewText("man");
    childNodeTeaGender->InsertFirstChild(contentGender);

    // 5.为子节点增加属性
    childNodeStu->SetAttribute("Name", "libai");

    root->InsertEndChild(childNodeStu); // childNodeStu是root的子节点
    root->InsertEndChild(childNodeTea); // childNodeTea是root的子节点
    childNodeTea->InsertEndChild(
        childNodeTeaGender); // childNodeTeaGender是childNodeTea的子节点

    // 6.保存xml文件
    doc.SaveFile("school.xml");
    doc.Print();

    // XML 写到字符串
    XMLPrinter printer;
    doc.Print(&printer);         //将Print打印到Xmlprint类中 即保存在内存中
    string buf = printer.CStr(); //转换成const char*类型
    cout << buf << endl;         // buf即为创建后的XML 字符串。
}