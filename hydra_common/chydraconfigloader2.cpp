#include "chydraconfigloader2.h"

#include <string>
#include <string.h>
#include <stdlib.h>
#include  <iostream>
#include "robot_hydra_id.h"

CHydraConfigLoader2::CHydraConfigLoader2()
{
    num_joints = 0;
    pNode = NULL;
}

int CHydraConfigLoader2::Init(const char model_name[], int num)
{
    model = model_name;
    num_joints = num;
    pgain.resize(num_joints);
    dgain.resize(num_joints);
}

void CHydraConfigLoader2::SetPGain(const double gain[])
{
    for(int i=0; i<num_joints; i++)
    {
        pgain[i] = gain[i];
    }
}

void CHydraConfigLoader2::SetDGain(const double gain[])
{
    for(int i=0; i<num_joints; i++)
    {
        dgain[i] = gain[i];
    }
}

void CHydraConfigLoader2::LoadPGain(double gain[])
{
    for(int i=0; i<num_joints; i++)
    {
        gain[i] = pgain[i];
    }
}

void CHydraConfigLoader2::LoadDGain(double gain[])
{
    for(int i=0; i<num_joints; i++)
    {
        gain[i] = dgain[i];
    }
}

int CHydraConfigLoader2::decode_jointnum(const char jointname[])
{
    for(int i=0; i<JOINT_HYDRA_MAX; i++)
    {
        if(!strcmp(jointname, joint_hydra_names[i]))
            return i;
    }
    return -1;
}

void CHydraConfigLoader2::load_node(const xmlpp::Node* node)
{
    if(const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(node))
    {
        if(nodeElement->get_name()=="model")
        {
            if(nodeElement->get_child_text()->get_content()!=model)
            {
                err_code = E_ERR_MODEL;
                throw std::invalid_argument("model error");
            }
//            std::cout << nodeElement->get_child_text()->get_content() << std::endl;
            return;
        }
        if(nodeElement->get_name()=="config")
        {
//            std::cout << nodeElement->get_name() << std::endl;
        }
        else if(nodeElement->get_name()=="joint")
        {
            //Print attributes:
            const xmlpp::Attribute* attribute = nodeElement->get_attribute("name");
            if(attribute)
            {
                joint = decode_jointnum(attribute->get_value().c_str());
                if(joint==-1)
                {
                    err_code = E_ERR_JNT_NAME;
                    throw std::invalid_argument("invalid joint name");
                }
            }
            else
            {
                err_code = E_ERR_JNT_NAME;
                throw std::invalid_argument("no joint name");
            }
        }
        if(nodeElement->get_name()=="pgain")
        {
            pgain[joint] = atof(nodeElement->get_child_text()->get_content().c_str());
//            std::cout << "P Gain = " << joint << nodeElement->get_child_text()->get_content() << std::endl;
            return;
        }
        else if(nodeElement->get_name()=="dgain")
        {
            dgain[joint] = atof(nodeElement->get_child_text()->get_content().c_str());
//            std::cout << "D Gain = " << joint  <<nodeElement->get_child_text()->get_content() << std::endl;
            return;
        }

    }

    const xmlpp::ContentNode* nodeContent = dynamic_cast<const xmlpp::ContentNode*>(node);
    if(!nodeContent)
    {
        //Recurse through child nodes:
        xmlpp::Node::NodeList list = node->get_children();
        for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
        {
            load_node(*iter); //recursive
        }
    }

}

void CHydraConfigLoader2::set_node(xmlpp::Node* node)
{
    if(xmlpp::Element* nodeElement = dynamic_cast<xmlpp::Element*>(node))
    {
        if(nodeElement->get_name()=="model")
        {
//            std::cout << nodeElement->get_child_text()->get_content() << std::endl;
            return;
        }
        if(nodeElement->get_name()=="config")
        {
//            std::cout << nodeElement->get_name() << std::endl;
        }
        else if(nodeElement->get_name()=="joint")
        {
            const xmlpp::Attribute* attribute = nodeElement->get_attribute("name");
            if(attribute)
            {
                joint = decode_jointnum(attribute->get_value().c_str());
                if(joint==-1)
                {
                    err_code = E_ERR_JNT_NAME;
                    throw std::invalid_argument("invalid joint name");
                }
            }
            else
            {
                err_code = E_ERR_JNT_NAME;
                throw std::invalid_argument("no joint name");
            }
        }
        if(nodeElement->get_name()=="pgain")
        {
            nodeElement->get_child_text()->set_content(std::to_string(pgain[joint]));
            return;
        }
        else if(nodeElement->get_name()=="dgain")
        {
            nodeElement->get_child_text()->set_content(std::to_string(dgain[joint]));
            return;
        }

    }

    xmlpp::ContentNode* nodeContent = dynamic_cast<xmlpp::ContentNode*>(node);
    if(!nodeContent)
    {
        //Recurse through child nodes:
        xmlpp::Node::NodeList list = node->get_children();
        for(xmlpp::Node::NodeList::iterator iter = list.begin(); iter != list.end(); ++iter)
        {
            set_node(*iter); //recursive
        }
    }

}
