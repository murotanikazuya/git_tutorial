#include "chydraconfigloader.h"
#include "irrXML.h"
#include <string.h>
#include  <iostream>
#include "robot_hydra_id.h"

CHydraConfigLoader::CHydraConfigLoader()
{
    num_joints = 0;
}

int CHydraConfigLoader::Init(int num)
{
    num_joints = num;
    pgain.resize(num_joints);
    dgain.resize(num_joints);
}

int CHydraConfigLoader::LoadConfigXML(const char filename[])
{
    int set_model = 0;
    int set_pgain = 0;
    int set_dgain = 0;
    int joint;

    irr::io::IrrXMLReader *xml = irr::io::createIrrXMLReader(filename);

    if(!xml)
    {
        std::cerr << "Failed to open XML" <<  std::endl;
        return E_ERR_NOFILE;
    }

    while(xml && xml->read())
    {
        switch(xml->getNodeType())
        {
        case irr::io::EXN_TEXT:
            if(set_model)
            {
                if(strcmp(xml->getNodeData(),"HYDRA"))
                    return E_ERR_MODEL;
//                std::cerr << "Config Model " << xml->getNodeData() << std::endl;
                set_model = 0;
            }
            else if(set_pgain)
            {
                pgain[joint] = atof(xml->getNodeData());
//                std::cerr << "P Gain " << joint << ":" << xml->getNodeData() << std::endl;
                set_pgain = 0;
            }
            else if(set_dgain)
            {
                dgain[joint] = atof(xml->getNodeData());
//                std::cerr << "D gain " << joint << ":" << xml->getNodeData() << std::endl;
                set_dgain = 0;
            }
            break;
        case irr::io::EXN_ELEMENT:
            if( !strcmp(xml->getNodeName(),"model"))
                set_model = 1;
            else if( !strcmp(xml->getNodeName(),"joint"))
            {
                int tmp_joint = decode_jointnum(xml->getAttributeValue("name"));
                if(tmp_joint>=0)
                    joint = tmp_joint;
            }
            else if( !strcmp(xml->getNodeName(),"pgain"))
            {
                set_pgain = 1;
            }
            else if( !strcmp(xml->getNodeName(),"dgain"))
            {
                set_dgain = 1;
            }
            break;
        default:
            break;
        }
    }

    delete xml;
    return 0;
}

void CHydraConfigLoader::SetPGain(const double gain[]){
    for(int i=0; i<num_joints; i++)
    {
        pgain[i] = gain[i];
    }
}

void CHydraConfigLoader::SetDGain(const double gain[]){
    for(int i=0; i<num_joints; i++)
    {
        dgain[i] = gain[i];
    }
}

void CHydraConfigLoader::LoadPGain(double gain[]){
    for(int i=0; i<num_joints; i++)
    {
        gain[i] = pgain[i];
    }
}

void CHydraConfigLoader::LoadDGain(double gain[]){
    for(int i=0; i<num_joints; i++)
    {
        gain[i] = dgain[i];
    }
}

int CHydraConfigLoader::decode_jointnum(const char jointname[])
{
    for(int i=0; i<JOINT_HYDRA_MAX; i++)
    {
        if(!strcmp(jointname, joint_names[i]))
            return i;
    }
    return -1;
}
