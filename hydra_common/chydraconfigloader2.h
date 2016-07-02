#ifndef __CHYDRACONFIGLOADER2_H_
#define __CHYDRACONFIGLOADER2_H_
#include <vector>
#include <libxml++/libxml++.h>
#include "cxmlconfig.h"

class CHydraConfigLoader2 : public CXMLConfig
{
public:
    CHydraConfigLoader2();
    int  Init(const char model_name[], int num);
    void SetPGain(const double gain[]);
    void SetDGain(const double gain[]);
    void LoadPGain(double gain[]);
    void LoadDGain(double gain[]);
    enum {
        E_ERR_GENERIC  = -1,
        E_ERR_MODEL    = -2,
        E_ERR_NOFILE   = -3,
        E_ERR_JNT_NAME = -4
    };
private:
    int decode_jointnum(const char jointname[]);
    void load_node(const xmlpp::Node* node);
    void set_node(xmlpp::Node* node);

    int num_joints;
    std::vector<double> pgain;
    std::vector<double> dgain;
    int joint;
    std::string model;

};

#endif

