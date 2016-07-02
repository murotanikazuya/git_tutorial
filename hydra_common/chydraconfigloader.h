#ifndef __CHYDRACONFIGLOADER_H_
#define __CHYDRACONFIGLOADER_H_
#include <vector>

class CHydraConfigLoader
{
public:
    CHydraConfigLoader();
    int  Init(int num);
    int  LoadConfigXML(const char filename[]);
    void SetPGain(const double gain[]);
    void SetDGain(const double gain[]);
    void LoadPGain(double gain[]);
    void LoadDGain(double gain[]);
    int  WriteConfigXML(const char filename[]);
    enum {
        E_ERR_MODEL = -2,
        E_ERR_NOFILE = -1
    };
private:
    int decode_jointnum(const char jointname[]);
    int num_joints;
    std::vector<double> pgain;
    std::vector<double> dgain;
};

#endif

