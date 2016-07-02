#include "cxmlconfig.h"
#include <iostream>

CXMLConfig::CXMLConfig()
{
    pNode = NULL;
}

int CXMLConfig::LoadConfigXML(const char filename[])
{
//    int return_code = 0;
    err_code = 0;
    try
    {
        parser.set_validate();
        parser.set_substitute_entities(true);
        parser.parse_file(filename);

        if(parser)
        {

            pNode = parser.get_document()->get_root_node(); //deleted by DomParser.
        }
        if(parser)
        {
            //Walk the tree:
            load_node(pNode);
        }
    }
    catch(const std::exception& ex)
    {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
        if(err_code==0)
            err_code = -1;
    }

    return err_code;
}

int CXMLConfig::WriteConfigXML(const char filename[])
{
    err_code = 0;
    try
    {
        if(parser)
        {
            set_node(pNode);
            parser.get_document()->write_to_file(filename);
        }
    }
    catch(const std::exception& ex)
    {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
        if(err_code==0)
            err_code = -1;
    }
    return err_code;
}
