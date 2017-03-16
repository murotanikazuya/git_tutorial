/**
   @author Shin'ichiro Nakaoka
*/
#ifndef __HYDRAWALKINGCONTROL_H_
#define __HYDRAWALKINGCONTROL_H_

#include <vector>

#include <stdio.h>
#include <unistd.h>

#include <semaphore.h>

//#include "hydra_types.h"
#include "../RealTimeSim/HydraSimulation/hydra_common/hydra_types.h"
//#include "ihydraipc.h"
#include "../RealTimeSim/HydraSimulation/hydra_common/ihydraipc.h"
//#include "chydrashmserver.h"
#include "../RealTimeSim/HydraSimulation/hydra_common/chydrashmserver.h"

//#include "chydradatamanager.h"
#include "../RealTimeSim/HydraSimulation/hydra_common/chydradatamanager.h"

//#include "chydradatarec.h"
#include "../RealTimeSim/HydraSimulation/hydra_common/chydradatarec.h"

//#include "chydraconfigloader.h"
#include "../RealTimeSim/HydraSimulation/hydra_common/chydraconfigloader.h"

#include "../RealTimeSim/HydraSimulation/hydra_common/kinematics/chydrakinematics.h"
//#include "kinematics/chydra_kinematics.h"
//#include "kinematics/hydra_kinematics.h"

#include <math.h>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

//#include <kindr/Core> // ==> ETH RSL library for kinematics & dynamics


using namespace Eigen;

class HydraWalkingControl
{


private:
    void stepPlanner();

    // Desired joint space vector (c = actuated)
    VectorXd q_c_des;
    VectorXd dq_c_des;

    VectorXd q_c_act;
    VectorXd dq_c_act;

    VectorXd q_c_act_prev;

    VectorXd q_c_null;


    int numJoints;
    int curr_iter_walking;
    int curr_footstep;

    // Time constants
    const double t_activate_control = 5.0;
    const double t_activate_legOScontrol = 5.0;
    const double dt_highlevel = 0.001;
    double t_highlevel = 0.0;

    void StepPlanner();
    void CP_preview();

    // Footstepp parameters
    const int numFootsteps = 10;
    const int numConvergence = 2;
    MatrixXd footsteps;
    double step_width = 0.25;
    double step_length = 0.25;
    double step_height = 0.10;
    const double step_time = 2.0; //0.6;
    const int step_time_discrete = (int)(step_time/dt_highlevel);
    const double t_ech = 0.08; // ech = End of Control Horizon
    const int t_ech_discrete=(int)(t_ech/dt_highlevel);
    Matrix2d rotMat;
    double cosMag = 0.0;

    // CP trajectory generator
    const int previewHorizon = 4;
    MatrixXd I_r_ZMP_prev;
    MatrixXd I_r_CP_ini;
    MatrixXd I_r_CP_eos;
    MatrixXd I_r_CP_ref;

    // Controller
    Vector3d I_r_ZMP_des;
    Vector3d I_ddr_COM_des;
    Vector3d I_dr_COM_des;
    Vector3d I_r_COM_des;
    Vector3d I_r_COM_init;
    Vector3d I_r_COM_target;


    const double gravity_const = 9.81; // (m/s^2)
    const double com_height = 0.955; // (m)
    const double omega = sqrt(gravity_const/com_height); // (1/s)
    const double k_d = 1.0/(1.0-exp(omega*t_ech));
    const double k_mf = 40;

    // FTS ZMP
    Vector3d I_r_ZMP;

    // Preplan everything
    // Preplanning
    void StepPlanner_prePlanned();
    MatrixXd I_r_f_pre;
    MatrixXd I_r_ZMP_pre;
    MatrixXd I_r_CP_pre;
    MatrixXd I_r_CP_des_pre;
    MatrixXd I_r_COM_des_pre;
    MatrixXd I_dr_COM_des_pre;

    MatrixXd I_r_f_traj;
    MatrixXd I_r_fl_traj;
    Vector3d I_r_fl_des;
    Vector3d I_dr_fl_des;
    MatrixXd I_r_fr_traj;
    Vector3d I_r_fr_des;
    Vector3d I_dr_fr_des;

    Joint joints[58];

    // Operational Space Control Legs

    // Left LEG OSC
    const double t_arriveTarget_LF = 4.0; //[s]
    const double I_x_fl_target = -0.20;
    const double I_y_fl_target = 0.20;
    const double I_z_fl_target = 0.30;
    const double I_z_fl_tolerance = 0.005;
    MatrixXd J_LF;
    MatrixXd JJT_LF;
    MatrixXd JJT_LF_inv;
    MatrixXd PInv_J_LF;
    void SetJacLF();
    Vector3d I_r_fl_init;
    Vector3d I_r_fl_curr;
    Vector3d I_r_fl_target;
    Vector3d I_w_fl_des;
    Vector3d I_w_fl_curr;
    VectorXd I_task_LF_vel;
    VectorXd dq_LF_des;
    VectorXd q_LF_des_rest;
    Vector3d err_ori_LF;
    Matrix3d I_R_0_FL_target;

    // Right LEG OSC
    const double t_arriveTarget_RF = 4.0; //[s]
    const double I_x_fr_target = 0.20;
    const double I_y_fr_target = -0.20;
    const double I_z_fr_target = 0.30;
    const double I_z_fr_tolerance = 0.005;
    MatrixXd J_RF;
    MatrixXd JJT_RF;
    MatrixXd JJT_RF_inv;
    MatrixXd PInv_J_RF;
    void SetJacRF();
    Vector3d I_r_fr_init;
    Vector3d I_r_fr_curr;
    Vector3d I_r_fr_target;
    Vector3d I_w_fr_des;
    Vector3d I_w_fr_curr;
    VectorXd I_task_RF_vel;
    VectorXd dq_RF_des;
    VectorXd q_RF_des_rest;
    Vector3d err_ori_RF;
    Matrix3d I_R_0_FR_target;

    // Left ARM OSC
    const double t_arriveTarget_LH = 4.0; //[s]
    const double I_x_hl_target = 0.40;
    const double I_y_hl_target = -0.10;
    const double I_z_hl_target = 0.60;
    MatrixXd J_LH;
    MatrixXd JJT_LH;
    MatrixXd JJT_LH_inv;
    MatrixXd PInv_J_LH;
    void SetJacLH();
    Vector3d I_r_hl_init;
    Vector3d I_r_hl_curr;
    Vector3d I_r_hl_target;
    Vector3d I_r_hl_des;
    Vector3d I_dr_hl_des;
    Vector3d I_w_hl_des;
    Vector3d I_w_hl_curr;
    VectorXd I_task_LH_vel;
    VectorXd dq_LH_des;
    VectorXd q_LH_des_rest;
    Vector3d err_ori_LH;
    Matrix3d I_R_0_HL_target;

    // Right ARM OSC
    const double t_arriveTarget_RH = 4.0; //[s]
    const double I_x_hr_target = -0.40;
    const double I_y_hr_target = -0.10;
    const double I_z_hr_target = 0.60;
    MatrixXd J_RH;
    MatrixXd JJT_RH;
    MatrixXd JJT_RH_inv;
    MatrixXd PInv_J_RH;
    void SetJacRH();
    Vector3d I_r_hr_init;
    Vector3d I_r_hr_curr;
    Vector3d I_r_hr_target;
    Vector3d I_r_hr_des;
    Vector3d I_dr_hr_des;
    Vector3d I_w_hr_des;
    Vector3d I_w_hr_curr;
    VectorXd I_task_RH_vel;
    VectorXd dq_RH_des;
    VectorXd q_RH_des_rest;
    Vector3d err_ori_RH;
    Matrix3d I_R_0_HR_target;

    const int numFootstepsStatic = 10;
    MatrixXd I_r_fl_static;
    MatrixXd I_r_fr_static;
    MatrixXd I_r_fl_traj_static;
    MatrixXd I_r_fr_traj_static;
    void PreplanFootTrajectoriesStatic();

    // General
    void SetJacLeftWristEE(Vector3d EE_r_EEtoPoint, MatrixXd &J_LH);
    void SetJacRightWristEE(Vector3d EE_r_EEtoPoint, MatrixXd &J_RH);
    void SetJacLeftAnkleEE(Vector3d EE_r_EEtoPoint, MatrixXd &B_J_LF);
    void SetJacRightAnkleEE(Vector3d EE_r_EEtoPoint, MatrixXd &B_J_RF);

    void SetPInv(MatrixXd &mat, MatrixXd &mat_pinv, double svd_tol);
    void GetSkew(Vector3d vec, Matrix3d &mat);
    void GetOrientationError(Quaterniond quat_act, Quaterniond quat_des, Vector3d &err_ori);

    void SetLinkBaseCOGJacobian(int joint);
    void SetCOGJacobian(MatrixXd &I_J_COG_lin);
    MatrixXd I_J_COG_lin;
    void SetBaseCOGJacobian(MatrixXd &B_J_COG_lin);
    MatrixXd B_J_COG_lin;
    MatrixXd B_J_COG_lin_HYDRA;
    void SetChainBaseCOGJacobian(MatrixXd &B_J_COG_chain_lin, int joint);
    MatrixXd B_J_COG_chain_lin;
    MatrixXd B_J_COG_chain_lin_HYDRA;
    void SetBaseCOGPosition(Vector3d &B_r_COG);
    Vector3d B_r_COG;
    void SetBaseChainCOGPosition(Vector3d &B_r_COG_chain, int joint);
    Vector3d B_r_COG_chain;
    void SetBaseJacLeftAnkleEE(MatrixXd &J_LF,MatrixXd &B_J_LF);
    MatrixXd I_J_LF;
    MatrixXd B_J_LF;
    void SetBaseJacRightAnkleEE(MatrixXd &J_RF,MatrixXd &B_J_RF);
    MatrixXd I_J_RF;
    MatrixXd B_J_RF;
    void SetJacobianLinHYDRA(MatrixXd &J_lin, MatrixXd &J_lin_HYDRA);
    MatrixXd I_J_COG_lin_HYDRA;

    MatrixXd I_J_LH;
    MatrixXd B_J_LH;
    MatrixXd I_J_RH;
    MatrixXd B_J_RH;

    MatrixXd JJT_COG;
    MatrixXd JJT_COG_inv;
    MatrixXd PInv_J_COG;
    VectorXd q_c_rest;

    Vector3d I_r_COM;
    Vector3d I_dr_COM_ref;

    void SetIMUEmulated(int joint_foot_contact);
    Matrix3d R_I_foot;
    Matrix3d R_0_foot;
    Matrix3d R_I_0;

    MatrixXd TEST;

    char debugJacobianFile[256];
    FILE *fp_debugJac=NULL;
    double t_debug_start = t_activate_control;
    double t_debug_end = t_debug_start+3.0;

    char logOSCWalkFile[256];
    FILE *fp_logOSCWalk=NULL;

    Eigen::MatrixXd matZKC;
    Eigen::MatrixXd matHYDRA;
    int slow_motion = 1;
    int slow_motion_counter = 0;
    double t_start = 7.0;
    int curr_iter = (int)1000*(-t_start);


public:
    HydraWalkingControl();
    ~HydraWalkingControl();
    void spamConsole();
    void initialize();
    void updateController(CHydraKinematics &hydra_kinematics);
    void updateOldController();
    void updateOSController(CHydraKinematics &hydra_kinematics);
    void updateCOMController(CHydraKinematics &hydra_kinematics);
    void getJointDes(joint_cmd_t  jnt_cmd[]);
    void setJointAct(joint_state_t  jnt_state[]);

    Eigen::MatrixXd readCSV(std::string file, int rows, int cols);
    void ConvertZKCtoHYDRAJointLogMatrix(MatrixXd &jointMatZKC, MatrixXd &jointMatHYDRA);

private:

};

#endif //__HYDRAWALKINGCONTROL_H_

