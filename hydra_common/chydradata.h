#ifndef CHYDRADATA_H
#define CHYDRADATA_H

#include <vector>
#include <map>

#include "hydra_types.h"


class CHydraData
{
    typedef struct {
        std::vector<double> pos;
        std::vector<double> vel;
        //std::vector<double> acc;
        std::vector<double> tau;
        std::vector<bool>   enabled;
    } jnt_state;

    typedef struct {
        jnt_state act;
        jnt_state ref_raw;
        jnt_state ref_checked;
    } jnt_data;

    typedef struct {
        std::vector<double> pos;
        std::vector<double> vel;
        std::vector<double> tau;
        std::vector<double> tau2;
        std::vector<double> tau3;

        std::vector<int>    enc_raw;
        std::vector<int>   status;
    } eha_state;

    typedef struct {
        eha_state act;
        eha_state ref;
    } eha_data;

    typedef struct{
        std::vector<float> wrench;
    } fs_state;

    typedef struct{
        fs_state left;
        fs_state right;
    } fs_data;

    typedef struct{
        std::vector<double> gyro;
        std::vector<double> acc;
    } imu_data;

public:
    CHydraData();
    virtual ~CHydraData(){}
    void UpdateEHAcmdFromJnt(void);
    void CheckJntRef(void);
    void SetAllJointServoSwitch(bool in);


    double time;
    jnt_data jnt;
    eha_data eha;
    fs_data  fs;
    imu_data imu;

    std::map<std::string,bool> flags;

    const double jnt_pos_step_max = DEG2RAD(0.1);

};

#endif // CHYDRADATA_H
