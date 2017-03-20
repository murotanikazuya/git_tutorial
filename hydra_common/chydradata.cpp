#include "chydradata.h"
#include "hydra_types.h"
#include "robot_hydra_id.h"
#include "hydra.h"
#include "CalcParallelJoint.h"
#include "hydra_gains.h"

CHydraData::CHydraData()
{
    jnt.act.pos.resize(HYDRA_JNT_MAX);      std::fill(jnt.act.pos.begin(),jnt.act.pos.end(),0.0);
    jnt.act.vel.resize(HYDRA_JNT_MAX);      std::fill(jnt.act.vel.begin(),jnt.act.vel.end(),0.0);
    jnt.act.tau.resize(HYDRA_JNT_MAX);      std::fill(jnt.act.tau.begin(),jnt.act.tau.end(),0.0);
    jnt.act.enabled.resize(HYDRA_JNT_MAX);  std::fill(jnt.act.enabled.begin(),jnt.act.enabled.end(),false);

    jnt.ref_raw.pos.resize(HYDRA_JNT_MAX);      std::fill(jnt.ref_raw.pos.begin(),jnt.ref_raw.pos.end(),0.0);
    jnt.ref_raw.vel.resize(HYDRA_JNT_MAX);      std::fill(jnt.ref_raw.vel.begin(),jnt.ref_raw.vel.end(),0.0);
    jnt.ref_raw.tau.resize(HYDRA_JNT_MAX);      std::fill(jnt.ref_raw.tau.begin(),jnt.ref_raw.tau.end(),0.0);
    jnt.ref_raw.enabled.resize(HYDRA_JNT_MAX);  std::fill(jnt.ref_raw.enabled.begin(),jnt.ref_raw.enabled.end(),false);

    jnt.ref_checked.pos.resize(HYDRA_JNT_MAX);      std::fill(jnt.ref_checked.pos.begin(),jnt.ref_checked.pos.end(),0.0);
    jnt.ref_checked.vel.resize(HYDRA_JNT_MAX);      std::fill(jnt.ref_checked.vel.begin(),jnt.ref_checked.vel.end(),0.0);
    jnt.ref_checked.tau.resize(HYDRA_JNT_MAX);      std::fill(jnt.ref_checked.tau.begin(),jnt.ref_checked.tau.end(),0.0);
    jnt.ref_checked.enabled.resize(HYDRA_JNT_MAX);  std::fill(jnt.ref_checked.enabled.begin(),jnt.ref_checked.enabled.end(),false);


    eha.act.pos.resize(EHA_MAX);            std::fill(eha.act.pos.begin(),eha.act.pos.end(),0.0);
    eha.act.vel.resize(EHA_MAX);            std::fill(eha.act.vel.begin(),eha.act.vel.end(),0.0);
    eha.act.tau.resize(EHA_MAX);            std::fill(eha.act.tau.begin(),eha.act.tau.end(),0.0);
    eha.act.tau2.resize(EHA_MAX);           std::fill(eha.act.tau2.begin(),eha.act.tau2.end(),0.0);
    eha.act.tau3.resize(EHA_MAX);           std::fill(eha.act.tau3.begin(),eha.act.tau3.end(),0.0);
    eha.act.enc_raw.resize(EHA_MAX);        std::fill(eha.act.enc_raw.begin(),eha.act.enc_raw.end(),0);
    eha.act.status.resize(EHA_MAX);         std::fill(eha.act.status.begin(),eha.act.status.end(),0);

    eha.ref.pos.resize(EHA_MAX);            std::fill(eha.ref.pos.begin(),eha.ref.pos.end(),0.0);
    eha.ref.vel.resize(EHA_MAX);            std::fill(eha.ref.vel.begin(),eha.ref.vel.end(),0.0);
    eha.ref.tau.resize(EHA_MAX);            std::fill(eha.ref.tau.begin(),eha.ref.tau.end(),0.0);
    eha.ref.status.resize(EHA_MAX);         std::fill(eha.ref.status.begin(),eha.ref.status.end(),0);

    fs.left.wrench.resize(6);
    fs.right.wrench.resize(6);

    imu.acc.resize(3);
    imu.gyro.resize(3);

    flags["jnt_check_pos_limit"] = true;
    flags["jnt_check_vel_limit"] = true;


}

void CHydraData::UpdateEHAcmdFromJnt(void)
{
    for(int i=0;i<HYDRA_JNT_MAX;i++){
        for (int j=0;j<joint_to_EHA_power[i][0];j++){
            int k=joint_to_EHA_power[i][j+1];
            eha.ref.status[k] = (jnt.ref_checked.enabled[i])?EHA_CtrlWd_ON[k]:EHA_CtrlWd_OFF[k];
        }
    }
    JointToCylinderAll(jnt.ref_checked.pos.data(),eha.ref.pos.data());

}

void CHydraData::CheckJntRef(void)
{
    if(flags["jnt_check_pos_limit"]){
        for(int i=0;i<HYDRA_JNT_MAX;i++){
            jnt.ref_raw.pos[i] = CHK_LIM(jnt.ref_raw.pos[i],
                                      all_joint_angle_limit[i][0],
                                      all_joint_angle_limit[i][1]);
        }
    }


    if(flags["jnt_check_vel_limit"]){
        for (int i=0;i<HYDRA_JNT_MAX;i++){
            if((jnt.ref_raw.pos[i]-jnt.ref_checked.pos[i])>jnt_pos_step_max)
                jnt.ref_checked.pos[i] += jnt_pos_step_max;
            else if ((jnt.ref_raw.pos[i]-jnt.ref_checked.pos[i])<-jnt_pos_step_max)
                jnt.ref_checked.pos[i] -= jnt_pos_step_max;
            else
                jnt.ref_checked.pos[i] = jnt.ref_raw.pos[i];
        }
    }else{
        jnt.ref_checked.pos = jnt.ref_raw.pos;
    }

    jnt.ref_checked.enabled = jnt.ref_raw.enabled;
    jnt.ref_checked.tau     = jnt.ref_raw.tau;
    jnt.ref_checked.vel     = jnt.ref_raw.vel;

}

void CHydraData::SetAllJointServoSwitch(bool in)
{
    for (int i=0;i<HYDRA_JNT_MAX;i++){
        jnt.ref_raw.enabled[i] = in;
    }
}


