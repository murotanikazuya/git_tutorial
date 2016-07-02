#ifndef CXMLCONFIG_H
#define CXMLCONFIG_H
#include <libxml++/libxml++.h>

class CXMLConfig
{
public:
    CXMLConfig();
    int  LoadConfigXML(const char filename[]);
    int  WriteConfigXML(const char filename[]);

protected:
    virtual void load_node(const xmlpp::Node* node) = 0;
    virtual void set_node(xmlpp::Node* node) = 0;

    int err_code;
    xmlpp::DomParser parser;
    xmlpp::Node      *pNode;
};



#endif // CXMLCONFIG_H
