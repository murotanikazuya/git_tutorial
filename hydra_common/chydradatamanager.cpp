#include "chydradatamanager.h"
#include "hydra.h"
#include "hydra_gains.h"

CHydraDataManager::CHydraDataManager():
    state_q(HYDRA_JNT_MAX_SIM),
    jnt_pos_step_max(DEG2RAD(0.1)),
    joint_check_limit_en(true)
{
    buffer_index = 0;
}

void CHydraDataManager::SetAllJointServoSwitch(bool in)
{
    for(int i=0;i<(HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX);i++){
        jnt_cmd[buffer_index][i].DATA.enable = in?1:0;
    }
}

void CHydraDataManager::UpdateEHAcmdFromJnt(void)
{

    for(int i=0;i<HYDRA_JNT_MAX;i++){
        for (int j=0;j<joint_to_EHA_power[i][0];j++){
            int k=joint_to_EHA_power[i][j+1];
            GetEHACmdPtr()[k].DATA.ctlword =
                    (GetJointCmdPtr()[i].DATA.enable==1)?EHA_CtrlWd_ON[k]:EHA_CtrlWd_OFF[k];
                    //(GetJointCmdPtr()[j].DATA.enable)?i:0;
        }
    }

    double jnt_pos[HYDRA_JNT_MAX],eha_pos[EHA_MAX];

    for(int i=0;i<HYDRA_JNT_MAX;i++){
        jnt_pos[i] = GetJointCmdPtr()[i].DATA.pos_ref;
    }
    JointToCylinderAll(jnt_pos,eha_pos);
    for(int i=0;i<EHA_MAX;i++){
        GetEHACmdPtr()[i].DATA.pos_ref = eha_pos[i];
    }
}

std::vector<double> CHydraDataManager::getState_q()
{
    // update the newest value from joint state
    for(int i=0;i<HYDRA_JNT_MAX_SIM;i++)
    {
        state_q[i] = jnt_state[buffer_index][i].DATA.pos_act;
    }
    return state_q;
}

void CHydraDataManager::setJointPosCmd(double* all_joint_refpos_to_send)
{
    double all_joint_pos_tgt[HYDRA_JNT_MAX];

    if(joint_check_limit_en){
    //check limit
        for(int loop = 0; loop < HYDRA_JNT_MAX; loop++) {
             all_joint_pos_tgt[loop] = CHK_LIM(all_joint_refpos_to_send[loop]
//                                               +(all_joint_ampl[loop]*sin(cnt/1000.0*all_joint_freq_hz[loop]*2*M_PI+all_joint_phase[loop]))
                                               ,
                                               all_joint_angle_limit[loop][0],all_joint_angle_limit[loop][1]);
        }
    }else{
        for (int loop = 0; loop < HYDRA_JNT_MAX; loop++) {
            all_joint_pos_tgt[loop] = all_joint_refpos_to_send[loop];
        }

    }


    for(int loop=0; loop<HYDRA_JNT_MAX; loop++){
        if((this->GetJointCmdPtr(0)[loop].DATA.pos_ref - all_joint_pos_tgt[loop])>jnt_pos_step_max)
                this->GetJointCmdPtr(1)[loop].DATA.pos_ref = this->GetJointCmdPtr(0)[loop].DATA.pos_ref - jnt_pos_step_max;
            else if((all_joint_pos_tgt[loop] - this->GetJointCmdPtr(0)[loop].DATA.pos_ref)>jnt_pos_step_max)
                this->GetJointCmdPtr(1)[loop].DATA.pos_ref = this->GetJointCmdPtr(0)[loop].DATA.pos_ref + jnt_pos_step_max;
            else
                this->GetJointCmdPtr(1)[loop].DATA.pos_ref = all_joint_pos_tgt[loop];
    }
}

void CHydraDataManager::setJointTauCmd(double* all_joint_tau_tgt)
{
    for(int loop=0; loop<HYDRA_JNT_MAX; loop++){
        this->GetJointCmdPtr(1)[loop].DATA.tau_ref = all_joint_tau_tgt[loop];
    }
}



