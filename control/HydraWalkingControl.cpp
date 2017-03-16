/**
   @author Shin'ichiro Nakaoka
*/

#include "HydraWalkingControl.h"
#include <vector>
#include <sstream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../RealTimeSim/HydraSimulation/hydra_common/CalcParallelJoint.h"
#include "../RealTimeSim/HydraSimulation/robot_hydra_id.h"

using namespace Eigen;


HydraWalkingControl::HydraWalkingControl(){
    printf("HydraWalkingControl::initialize()\n");

	FILE* fp_dbg;
    fp_dbg = fopen("jvrc-test/HydraControllerRTC/aaa.log","w");
	fprintf(fp_dbg,"aaa\n");
	fclose(fp_dbg);
}

HydraWalkingControl::~HydraWalkingControl(){
    fclose(fp_debugJac);
    fclose(fp_logOSCWalk);
}

void HydraWalkingControl::spamConsole(){
    fprintf(stderr, "SPAM\n");
    fprintf(stderr, "%d\n",numJoints);
}

void HydraWalkingControl::initialize(){
	
    printf("HydraWalkingControl::initialize()\n");

    q_c_des.resize(HYDRA_JNT_MAX); //HYDRA_JNT_MAX
    q_c_des.setZero();

    dq_c_des.resize(HYDRA_JNT_MAX); //HYDRA_JNT_MAX
    dq_c_des.setZero();

    q_c_act.resize(HYDRA_JNT_MAX); //HYDRA_JNT_MAX
    q_c_act.setZero();

    dq_c_act.resize(HYDRA_JNT_MAX); //HYDRA_JNT_MAX
    dq_c_act.setZero();

    q_c_act_prev.resize(HYDRA_JNT_MAX);
    q_c_act_prev.setZero();

    // Footstep Planner
    footsteps.resize(numFootsteps,4);
    footsteps.setZero();
    rotMat.setZero();

    // CP traj generator
    I_r_ZMP_prev.resize(previewHorizon,2);
    I_r_ZMP_prev.setZero();
    I_r_CP_ini.resize(previewHorizon,2);
    I_r_CP_ini.setZero();
    I_r_CP_eos.resize(previewHorizon,2);
    I_r_CP_eos.setZero();
    I_r_CP_ref.resize(2*step_time_discrete,2);
    I_r_CP_ref.setZero();

    // Timers
    curr_iter_walking = 0;
    curr_footstep = 0;

    // Controller
    I_r_ZMP_des.setZero();
    I_ddr_COM_des.setZero();
    I_dr_COM_des.setZero();
    I_r_COM_des.setZero();

    // FTS
    I_r_ZMP.setZero();

    // Preplanning
    I_r_f_pre.resize(numFootsteps,3);
    I_r_f_pre.setZero();
    I_r_ZMP_pre.resize(numFootsteps,3);
    I_r_ZMP_pre.setZero();
    I_r_CP_pre.resize(numFootsteps,3);
    I_r_CP_pre.setZero();
    I_r_CP_des_pre.resize((numFootsteps-1+numConvergence)*step_time_discrete,3);
    I_r_CP_des_pre.setZero();
    I_r_COM_des_pre.resize((numFootsteps-1+numConvergence)*step_time_discrete,3);
    I_r_COM_des_pre.setZero();
    I_dr_COM_des_pre.resize((numFootsteps-1+numConvergence)*step_time_discrete,3);
    I_dr_COM_des_pre.setZero();
    I_r_f_traj.resize((numFootsteps-1+numConvergence)*step_time_discrete,3);
    I_r_f_traj.setZero();
    I_r_fl_traj.resize((numFootsteps-1+numConvergence)*step_time_discrete,3);
    I_r_fl_traj.setZero();
    I_r_fr_traj.resize((numFootsteps-1+numConvergence)*step_time_discrete,3);
    I_r_fr_traj.setZero();

    TEST.resize(8,8);
    TEST.setZero();

    // OSC Left Leg
    I_J_LF.resize(6,58);
    I_J_LF.setZero();
    J_LF.resize(6,6);
    J_LF.setZero();
    JJT_LF.resize(6,6);
    JJT_LF.setZero();
    JJT_LF_inv.resize(6,6);
    JJT_LF_inv.setZero();
    PInv_J_LF.resize(6,6);
    PInv_J_LF.setZero();

    I_task_LF_vel.resize(6,1);
    I_task_LF_vel.setZero();
    dq_LF_des.resize(6,1);
    dq_LF_des.setZero();
    q_LF_des_rest.resize(6,1);
    q_LF_des_rest.setZero();
    I_r_fl_des.setZero();
    I_w_fl_des.setZero();

    q_LF_des_rest(0) = 0.0;
    q_LF_des_rest(1) = -0.2;
    q_LF_des_rest(2) = 0.0;
    q_LF_des_rest(3) = 0.4;
    q_LF_des_rest(4) = 0.0;
    q_LF_des_rest(5) = -0.2;


    // OSC Right Leg
    I_J_RF.resize(6,58);
    I_J_RF.setZero();
    J_RF.resize(6,6);
    J_RF.setZero();
    JJT_RF.resize(6,6);
    JJT_RF.setZero();
    JJT_RF_inv.resize(6,6);
    JJT_RF_inv.setZero();
    PInv_J_RF.resize(6,6);
    PInv_J_RF.setZero();

    I_task_RF_vel.resize(6,1);
    I_task_RF_vel.setZero();
    dq_RF_des.resize(6,1);
    dq_RF_des.setZero();
    q_RF_des_rest.resize(6,1);
    q_RF_des_rest.setZero();
    I_r_fr_des.setZero();
    I_w_fr_des.setZero();

    q_RF_des_rest(0) = 0.0;
    q_RF_des_rest(1) = -0.2;
    q_RF_des_rest(2) = 0.0;
    q_RF_des_rest(3) = 0.4;
    q_RF_des_rest(4) = 0.0;
    q_RF_des_rest(5) = -0.2;


    // OSC Left Hand
    J_LH.resize(6,8);
    J_LH.setZero();
    JJT_LH.resize(6,6);
    JJT_LH.setZero();
    JJT_LH_inv.resize(6,6);
    JJT_LH_inv.setZero();
    PInv_J_LH.resize(8,6);
    PInv_J_LH.setZero();

    I_task_LH_vel.resize(6,1);
    I_task_LH_vel.setZero();
    dq_LH_des.resize(8,1);
    dq_LH_des.setZero();
    q_LH_des_rest.resize(8,1);
    q_LH_des_rest.setZero();
    I_r_hl_des.setZero();
    I_w_hl_des.setZero();

    q_LH_des_rest(0) = 0.0;
    q_LH_des_rest(1) = -0.2;
    q_LH_des_rest(2) = 0.2;
    q_LH_des_rest(3) = 0.0;
    q_LH_des_rest(4) = -0.5;
    q_LH_des_rest(5) = 0.0;
    q_LH_des_rest(6) = 0.0;
    q_LH_des_rest(7) = 0.0;

    // OSC Right Hand
    J_RH.resize(6,8);
    J_RH.setZero();
    JJT_RH.resize(6,6);
    JJT_RH.setZero();
    JJT_RH_inv.resize(6,6);
    JJT_RH_inv.setZero();
    PInv_J_RH.resize(8,6);
    PInv_J_RH.setZero();

    I_task_RH_vel.resize(6,1);
    I_task_RH_vel.setZero();
    dq_RH_des.resize(8,1);
    dq_RH_des.setZero();
    q_RH_des_rest.resize(8,1);
    q_RH_des_rest.setZero();
    I_r_hr_des.setZero();
    I_w_hr_des.setZero();

    q_RH_des_rest(0) = 0.0;
    q_RH_des_rest(1) = 0.2;
    q_RH_des_rest(2) = 0.2;
    q_RH_des_rest(3) = 0.0;
    q_RH_des_rest(4) = -0.5;
    q_RH_des_rest(5) = 0.0;
    q_RH_des_rest(6) = 0.0;
    q_RH_des_rest(7) = 0.0;

    I_r_fl_static.resize(numFootstepsStatic,3);
    I_r_fl_static.setZero();
    I_r_fr_static.resize(numFootstepsStatic,3);
    I_r_fr_static.setZero();
    I_r_fl_traj_static.resize((numFootsteps-1+numConvergence)*step_time_discrete,3);
    I_r_fl_traj_static.setZero();
    I_r_fr_traj_static.resize((numFootsteps-1+numConvergence)*step_time_discrete,3);
    I_r_fr_traj_static.setZero();

    I_J_COG_lin.resize(3,58);
    I_J_COG_lin.setZero();
    B_J_COG_lin.resize(3,58);
    B_J_LF.resize(6,58);
    B_J_LF.setZero();
    B_J_RF.resize(6,58);
    B_J_RF.setZero();

    B_J_LH.resize(6,58);
    B_J_LH.setZero();
    B_J_RH.resize(6,58);
    B_J_RH.setZero();

    I_J_COG_lin_HYDRA.resize(3,HYDRA_JNT_MAX);
    I_J_COG_lin_HYDRA.setZero();
    B_J_COG_lin_HYDRA.resize(3,HYDRA_JNT_MAX);
    B_J_COG_lin_HYDRA.setZero();
    JJT_COG.resize(3,3);
    JJT_COG.setZero();
    JJT_COG_inv.resize(3,3);
    JJT_COG_inv.setZero();
    PInv_J_COG.resize(HYDRA_JNT_MAX,3);
    PInv_J_COG.setZero();

    q_c_rest.resize(HYDRA_JNT_MAX);
    q_c_rest.setZero();
    q_c_rest(1) = -0.3;
    q_c_rest(3) = 0.4;
    q_c_rest(5) = -0.2;

    q_c_rest(7) = -0.3;
    q_c_rest(9) = 0.4;
    q_c_rest(11) = -0.2;

    //q_c_rest(12) = -0.4;

    q_c_rest(15) = 0.0;
    q_c_rest(17) = -0.2;
    q_c_rest(19) = -0.5;

    q_c_rest(23) = 0.0;
    q_c_rest(25) = 0.2;
    q_c_rest(27) = -0.5;

    q_c_rest.setZero();

    I_r_COM.setZero();
    I_dr_COM_ref.setZero();

    B_J_COG_chain_lin.resize(3,58);
    B_J_COG_chain_lin.setZero();
    B_J_COG_chain_lin_HYDRA.resize(3,HYDRA_JNT_MAX);
    B_J_COG_chain_lin_HYDRA.setZero();

    PreplanFootTrajectoriesStatic();

    strcpy(debugJacobianFile, "./debugJacobian");
    if( (fp_debugJac = fopen(debugJacobianFile,"w"))==NULL ) {
        perror("Cannot open debug jacobian file");
    }
    //fprintf(fp_debugJac, "application start\n");
    fflush(fp_debugJac);

    /*
    for(int i = 0; i < numFootstepsStatic - 1 + numConvergence; ++i){
        for(int j = 0; j < step_time_discrete; ++j){
            fprintf(fp_debugJac, "%f,%f,%f,%f,%f,%f \n ", I_r_fl_traj_static(i*step_time_discrete+j,0),I_r_fl_traj_static(i*step_time_discrete+j,1),I_r_fl_traj_static(i*step_time_discrete+j,2),I_r_fr_traj_static(i*step_time_discrete+j,0),I_r_fr_traj_static(i*step_time_discrete+j,1),I_r_fr_traj_static(i*step_time_discrete+j,2));
        }
    }*/


    // Log OSC walk data
    strcpy(logOSCWalkFile, "./LogOSCWalk.csv");
    if( (fp_logOSCWalk = fopen(logOSCWalkFile,"w"))==NULL ) {
        perror("Cannot open OSC Walk file");
    }
    //fprintf(fp_debugJac, "application start\n");
    fflush(fp_logOSCWalk);


    // Function calls
    StepPlanner();
    StepPlanner_prePlanned();


    /*fprintf(fp_debugJac, "Now it works\n");

    // Read desired joint angles from file for feed-forward
    std::ifstream indata;
    indata.open("/JointData.csv", std::ifstream::in);

    fprintf(fp_debugJac, "Now it ... (1) \n");

    std::vector<std::vector<double>> csvData;
    std::vector<double> JointColumn;

    fprintf(fp_debugJac, "Now it ... (2) \n");

    std::string line;
    int rows = 0;
    int cols = 0;
    std::getline(indata, line); // disregard first line

    fprintf(fp_debugJac, "Now it ... (3) \n");
    while (std::getline(indata, line))
    {
        fprintf(fp_debugJac, "Now it ... (i) \n");
        std::stringstream lineStream(line);
        std::string cell;
        cols = 0;
        while (std::getline(lineStream, cell, ','))
        {
            //Process cell
            std::string::size_type sz;
            double tmp = std::stod(cell,&sz);
            JointColumn.push_back(tmp);
            cols++;
        }
        csvData.push_back(JointColumn);
        rows++;
    }
    indata.close();

    fprintf(fp_debugJac, "Now it ... (4) \n");*/

    int rowsZKC = 14801;
    int colsZKC = 59;
    matZKC.resize(rowsZKC, colsZKC);
    matZKC.setZero();
    std::string jointDataLogged = "./JointData.csv";
    matZKC = HydraWalkingControl::readCSV(jointDataLogged, rowsZKC, colsZKC);


    int rowsHYDRA = 14801;
    int colsHYDRA = HYDRA_JNT_MAX;
    matHYDRA.resize(rowsHYDRA, colsHYDRA);
    matHYDRA.setZero();
    ConvertZKCtoHYDRAJointLogMatrix(matZKC, matHYDRA);

    fprintf(fp_debugJac, "\n");
    fprintf(fp_debugJac, "mat = \n");
    for(int i = 0; i < rowsHYDRA; ++i){
        fprintf(fp_debugJac, "[");
        for(int j = 0; j < colsHYDRA; ++j){
            fprintf(fp_debugJac, "%f, ", matHYDRA(i,j));
        }
        fprintf(fp_debugJac, "]\n");
    }


    /*
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            mat(i,j) = csvData[i][j];
        }
    }

    fprintf(fp_debugJac, "Now it doesn't \n");*/

}

void HydraWalkingControl::updateController(CHydraKinematics &hydra_kinematics){

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %% NOTE: ONLY use Operational Space Controller          %%
    // %%       AFTER setting waist base to 'fix' in hydra.wrl %%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //updateOSController(hydra_kinematics);

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // %% NOTE: ONLY use Center of Mass Controller              %%
    // %%       AFTER setting waist base to 'free' in hydra.wrl %%
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    updateCOMController(hydra_kinematics);
}


void HydraWalkingControl::updateCOMController(CHydraKinematics &hydra_kinematics){
    // ======= Update kinematics =======
    hydra_kinematics.GetJointData(joints);
    int joint_foot_contact = 12; // assume left foot in flat contact
    SetIMUEmulated(joint_foot_contact);

    // Update step counter after t_Step has passed
    if( curr_iter_walking%step_time_discrete == 0 && curr_iter_walking > 1 ){
        curr_footstep++;
    }

    // Initialize desired COG position
    if(t_highlevel > (t_activate_legOScontrol-dt_highlevel) && t_highlevel < (t_activate_legOScontrol+dt_highlevel)){
        SetBaseCOGPosition(I_r_COM_init);
        I_r_COM_init = joints[0].R_I_j*I_r_COM_init;
        I_r_COM_des = I_r_COM_init;

        // Commanding foot motion
        I_r_fr_init = joints[6].s_0_j; //JOINT_HYDRA_RANKLE_PITCH
        I_r_fr_des = I_r_fr_init;
        I_r_fr_target = I_r_fr_init;
        I_R_0_FR_target = joints[6].R_0_j;

        // Commanding wrist motion
        I_r_hl_init = joints[31].p_0_j; //JOINT_HYDRA_LWRIST_PITCH
        I_r_hl_des = I_r_hl_init;
        I_r_hl_target = I_r_hl_init + Eigen::Vector3d(I_x_hl_target,I_y_hl_target,I_z_hl_target);
        I_R_0_HL_target = joints[31].R_0_j;
    }

    // Calculate COG Jacobian
    /*for(int joint = 0; joint < 58; ++joint){
        SetLinkBaseCOGJacobian(joint);
    }
    SetBaseCOGJacobian(B_J_COG_lin);
    SetJacobianLinHYDRA(B_J_COG_lin, B_J_COG_lin_HYDRA);
    I_J_COG_lin_HYDRA = joints[0].R_I_j*B_J_COG_lin_HYDRA;*/
    SetCOGJacobian(I_J_COG_lin);
    SetJacobianLinHYDRA(I_J_COG_lin, I_J_COG_lin_HYDRA);

    // Get COG Jacobian Pseudo-Inverse
    JJT_COG = I_J_COG_lin_HYDRA * I_J_COG_lin_HYDRA.transpose();
    SetPInv(JJT_COG,JJT_COG_inv,1.0e-6);
    PInv_J_COG = I_J_COG_lin_HYDRA.transpose() * JJT_COG_inv;

    // Get desired COG motion
    double dx_motion = 0.0;//1.0/100.0*sin(2.0*M_PI*t_highlevel/4.0);
    double dy_motion = 0.0;//1.0/100.0*sin(2.0*M_PI*t_highlevel/4.0);
    double dz_motion = 0.0; //*sin(2.0*M_PI*(t_highlevel-t_activate_control)/10.0);
    I_dr_COM_des = Vector3d(dx_motion,dy_motion,dz_motion);
    I_r_COM_des = I_r_COM_des + dt_highlevel * I_dr_COM_des;

    // Move COG over Foot
    /*if(t_highlevel >= t_activate_control && t_highlevel < t_activate_control + 40.0){
        I_r_COM_target = joints[0].R_I_j.transpose() * joints[12].p_0_j;
        I_r_COM_target(2) = I_r_COM_init(0);
        I_r_COM_target(2) = I_r_COM_init(2);
        I_dr_COM_des = (I_r_COM_target-I_r_COM_init)/40.0;
    }
    I_r_COM_des = I_r_COM_des + dt_highlevel * I_dr_COM_des;*/

    // Get reference COG motion
    SetBaseCOGPosition(I_r_COM);
    I_r_COM = joints[0].R_I_j*I_r_COM;
    I_dr_COM_ref = I_dr_COM_des + 1.0*(I_r_COM_des-I_r_COM);


    // command right foot contact
    // RIGHT ANKLE CONTROLLER
    // ZERO DATA
    I_dr_fr_des.setZero();
    I_w_fr_des.setZero();

    // Get current left foot position (JOINT_HYDRA_LANKLE_PITCH)
    I_r_fr_curr = joints[0].R_I_j*joints[6].s_0_j;

    // Change target position to be incremental waypoint
    /*double t_rel_ankleR = t_highlevel-t_activate_legOScontrol;
    int t_rel_ankleR_discrete = t_rel_ankleR/dt_highlevel;
    if(curr_iter_walking < (numFootstepsStatic)*step_time_discrete){
        I_r_fr_target = I_r_fr_traj_static.block(t_rel_ankleR_discrete,0,1,3).transpose() + (I_r_fl_init+I_r_fr_init)/2.0;
        I_dr_fr_des = (I_r_fr_traj_static.block(t_rel_ankleR_discrete+1,0,1,3).transpose()-I_r_fr_traj_static.block(t_rel_ankleR_discrete,0,1,3).transpose())/dt_highlevel;
    }*/

    // Set desired task space velocity
    if(t_highlevel < t_activate_legOScontrol + t_arriveTarget_RF){
        I_dr_fr_des = (I_r_fr_target-I_r_fr_init)/t_arriveTarget_RF;
    }
    I_dr_fr_des.setZero();

    // Generate desired position via numeric velocity integration (starting from initial pos.)
    I_r_fr_des = I_r_fr_des + dt_highlevel * I_dr_fr_des;

    // Get current and desired orientation quaternions
    Quaterniond quat_RF_curr(joints[0].R_I_j*joints[6].R_0_j);
    Quaterniond quat_RF_des(I_R_0_FR_target);
    GetOrientationError(quat_RF_curr,quat_RF_des,err_ori_RF);

    // Cartesian reference velocity
    I_task_RF_vel.block(0,0,3,1) = I_dr_fr_des + (I_r_fr_des-I_r_fr_curr)*20.0;
    I_task_RF_vel.block(3,0,3,1) = I_w_fr_des - 10.0 * err_ori_RF;

    // Get Jacobian and Pseudo-Inverse
    SetJacRightAnkleEE(Vector3d::Zero(), B_J_RF);
    I_J_RF = joints[0].R_I_j*B_J_RF;
    JJT_RF = I_J_RF * I_J_RF.transpose();
    SetPInv(JJT_RF,JJT_RF_inv,1.0e-6);
    PInv_J_RF = I_J_RF.transpose() * JJT_RF_inv;

    // Desired joint velocity w/ PInv projection of ref. task-space velocity and nullspace projection of ref. posture
    dq_RF_des = PInv_J_RF * I_task_RF_vel;// + 1.0 * (MatrixXd::Identity(6,6) - PInv_J_RF*B_J_RF) * (q_RF_des_rest - q_c_act.block(0,0,6,1));


    // overwrite and comamnd left hand motion
    // LEFT WRIST CONTROLLER
    // ZERO DATA
    I_dr_hl_des.setZero();
    I_w_hl_des.setZero();

    // Get current left foot position (JOINT_HYDRA_LANKLE_PITCH)
    I_r_hl_curr = joints[0].R_I_j*joints[31].p_0_j;

    // Set desired task space velocity
    if(t_highlevel < t_activate_legOScontrol + t_arriveTarget_LH){
        I_dr_hl_des = (I_r_hl_target-I_r_hl_init)/t_arriveTarget_LH;
    }

    // Generate desired position via numeric velocity integration (starting from initial pos.)
    I_r_hl_des = I_r_hl_des + dt_highlevel * I_dr_hl_des;

    // Get current and desired orientation quaternions
    Quaterniond quat_LH_curr(joints[0].R_I_j*joints[31].R_0_j);
    Quaterniond quat_LH_des(I_R_0_HL_target);
    GetOrientationError(quat_LH_curr,quat_LH_des,err_ori_LH);

    // Cartesian reference velocity
    I_task_LH_vel.block(0,0,3,1) = I_dr_hl_des + 10.0 * (I_r_hl_des-I_r_hl_curr);
    I_task_LH_vel.block(3,0,3,1) = I_w_hl_des - 10.0 * err_ori_LH;

    // Get Jacobian and Pseudo-Inverse
    SetJacLeftWristEE(Vector3d::Zero(), B_J_LH);
    I_J_LH = joints[0].R_I_j*B_J_LH;
    JJT_LH = I_J_LH * I_J_LH.transpose();
    SetPInv(JJT_LH,JJT_LH_inv,1.0e-6);
    PInv_J_LH = I_J_LH.transpose() * JJT_LH_inv;

    // Desired joint velocity w/ PInv projection of ref. task-space velocity and nullspace projection of ref. posture
    dq_LH_des = PInv_J_LH * I_task_LH_vel;// + 1.0 * (MatrixXd::Identity(8,8) - PInv_J_LH*J_LH) * (1.0*(q_LH_des_rest - q_c_act.block(23,0,8,1)) + 10.0*(VectorXd::Zero(8)-dq_c_act.block(23,0,8,1)));


    //==================================================
    //=== PD CONTROLLER ON JOINT LEVEL TO INITIALIZE ===
    //==================================================
    if(t_highlevel < t_activate_control){
        // Set desired output position
        for(int i=0; i<HYDRA_JNT_MAX; i++)
        {
            q_c_des[i] = 0.0;
        }

        // Test some configs
        q_c_des[1] = -0.3;
        q_c_des[3] = 0.4;
        q_c_des[5] = -0.2;

        q_c_des[7] = -0.3;
        q_c_des[9] = 0.4;
        q_c_des[11] = -0.2;

        // waist pitch
        //q_c_rest(12) = -0.4;

        // elbow
        q_c_des[17] = -0.2;
        q_c_des[19] = -0.5;

        q_c_des[25] = 0.2;
        q_c_des[27] = -0.5;

        q_c_null = q_c_des;
    }
    //===========================
    //=== COM JACOBIAN MOTION ===
    //===========================
    else{
        // Desired joint positions via numeric integration of des. joint velocities
        //q_c_null.block(0,0,6,1) = q_c_null.block(0,0,6,1) + dt_highlevel * dq_RF_des;
        q_c_null.block(23,0,8,1) = q_c_null.block(23,0,8,1) + dt_highlevel * dq_LH_des;

        dq_c_des = PInv_J_COG * I_dr_COM_ref + (MatrixXd::Identity(HYDRA_JNT_MAX,HYDRA_JNT_MAX)-PInv_J_COG*I_J_COG_lin_HYDRA) * (q_c_null-q_c_act);
        //q_c_des = q_c_des + dt_highlevel * dq_c_des;
    }


    // reading logged data
    if(curr_iter < 0){
        q_c_des = matHYDRA.block(0,0,1,HYDRA_JNT_MAX).transpose();
    }
    else if(curr_iter < matHYDRA.rows()){
        q_c_des = matHYDRA.block(curr_iter,0,1,HYDRA_JNT_MAX).transpose();
    }
    else{
        q_c_des = matHYDRA.block(matHYDRA.rows()-1,0,1,HYDRA_JNT_MAX).transpose();
    }
    slow_motion_counter = slow_motion_counter + 1;
    if(slow_motion_counter == slow_motion){
        curr_iter = curr_iter + 1;
        slow_motion_counter = 0;
    }


    // ======= DEBUG LOGGING =======
    if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "t = %f \n", t_highlevel);
    }    

    /*fprintf(fp_debugJac, "\n");
    fprintf(fp_debugJac, "I_dr_COM_ref = [%f, %f, %f] \n", I_dr_COM_ref(0),I_dr_COM_ref(1), I_dr_COM_ref(2));
    fprintf(fp_debugJac, "\n");*/

    /*
    if(t_highlevel > t_debug_start && t_debug_start + 10.0*dt_highlevel){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "B_J_COM_chain_linT = ");
        for(int i = 0; i < 58; ++i){
            fprintf(fp_debugJac, "[%f, %f, %f] \n", B_J_COG_chain_lin(0,i),B_J_COG_chain_lin(1,i),B_J_COG_chain_lin(2,i));
        }
        fprintf(fp_debugJac, "\n");
    }*/

    /*
    if(t_highlevel > t_debug_start && t_debug_start + 10.0*dt_highlevel){
        for(int j = 0; j < 58; ++j){
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "Joint %d", j);
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "I_J_COM_linT = ");
            for(int i = 0; i < 58; ++i){
                fprintf(fp_debugJac, "[%f, %f, %f] \n", joints[j].B_J_COG_lin(0,i),joints[j].B_J_COG_lin(1,i),joints[j].B_J_COG_lin(2,i));
            }
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "\n");
        }
    }*/


    /*if(t_highlevel > t_debug_start && t_highlevel < t_debug_start + 10.0*dt_highlevel){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "I_J_COM_lin_HYDRAT = ");
        for(int i = 0; i < HYDRA_JNT_MAX; ++i){
            fprintf(fp_debugJac, "[%f, %f, %f] \n", I_J_COG_lin_HYDRA(0,i),I_J_COG_lin_HYDRA(1,i),I_J_COG_lin_HYDRA(2,i));
        }
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "\n");
    }*/

    /*if(t_highlevel > t_debug_start && t_highlevel < t_debug_start + 10.0*dt_highlevel){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "I_J_COG_pinv = ");
        for(int i = 0; i < HYDRA_JNT_MAX; ++i){
            fprintf(fp_debugJac, "[%f, %f, %f] \n", PInv_J_COG(i,0),PInv_J_COG(i,1),PInv_J_COG(i,2));
        }
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "\n");
    }*/

    /*if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "q_c_des = ");
        for(int i = 0; i < HYDRA_JNT_MAX; ++i){
            fprintf(fp_debugJac, "[%f] \n", q_c_des(i));
        }
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "\n");
    }*/


    /*if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "R_I_13 = ");
        for(int i = 0; i < 3; ++i){
            fprintf(fp_debugJac, "[%f, %f, %f] \n", joints[13].R_I_j(i,0), joints[13].R_I_j(i,1), joints[13].R_I_j(i,2));
        }
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "\n");
    }*/

    /*if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "R_0_B = ");
        for(int i = 0; i < 3; ++i){
            fprintf(fp_debugJac, "[%f, %f, %f] \n", joints[0].R_0_j(i,0), joints[0].R_0_j(i,1), joints[0].R_0_j(i,2));
        }
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "\n");
    }*/

    for(int j = 0; j < HYDRA_JNT_MAX; ++j){
        if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "Joint %d: R_0_B =               R_I_B = \n", j);
            for(int i = 0; i < 3; ++i){
                fprintf(fp_debugJac, "[%f, %f, %f] [%f, %f, %f] \n", joints[j].R_0_j(i,0), joints[j].R_0_j(i,1), joints[j].R_0_j(i,2), joints[j].R_I_j(i,0), joints[j].R_I_j(i,1), joints[j].R_I_j(i,2));
            }
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "\n");
        }
    }

    /*Matrix3d I = joints[12].R_0_j.transpose()*joints[12].R_0_j;
    if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "I = ");
        for(int i = 0; i < 3; ++i){
            fprintf(fp_debugJac, "[%f, %f, %f] \n", I(i,0), I(i,1), I(i,2));
        }
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "\n");
    }*/

    /*
    fprintf(fp_debugJac, "\n");
    fprintf(fp_debugJac, "Chain COM des = [%f, %f, %f] \n", I_r_COM_des(0),I_r_COM_des(1), I_r_COM_des(2));
    fprintf(fp_debugJac, "\n");
    fprintf(fp_debugJac, "Chain COM acc = [%f, %f, %f] \n", I_r_COM(0),I_r_COM(1), I_r_COM(2));

    if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "R_0_B = ");
        for(int i = 0; i < 3; ++i){
            fprintf(fp_debugJac, "[%f, %f, %f] \n", joints[0].R_0_j(i,0), joints[0].R_0_j(i,1), joints[0].R_0_j(i,2));
        }
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "\n");
    }*/



    t_highlevel += dt_highlevel;
}

void HydraWalkingControl::updateOldController(){
    // Update step counter after t_Step has passed
    /*if( curr_iter_walking%step_time_discrete == 0 && curr_iter_walking > 1 ){
        curr_footstep++;
    }

    //==================================================
    //=== PD CONTROLLER ON JOINT LEVEL TO INITIALIZE ===
    //==================================================
    if(t_highlevel < t_activate_control){
        // Set desired output position
        for(int i=0; i<HYDRA_JNT_MAX; i++)
        {
            q_c_des[i] = 0.0;
        }
    }
    //============================================================
    //=== ZMP/CP BASED WALKING CONTROLLER AFTER INITIALIZATION ===
    //============================================================
    else{
        //=======================================
        //=== ZMP-CONTROL, ACCELERATION-BASED ===
        //=======================================


        // Probably BUG in here, check at later time

        // Set desired CP trajectory
        if(curr_iter_walking%step_time_discrete == 0 && curr_footstep < numFootsteps){
            CP_preview();
        }

        // Get desired ZMP
        if(curr_footstep < numFootsteps){
            I_r_ZMP_des(0) = k_d*I_r_CP_ref(curr_iter_walking - (curr_footstep*step_time_discrete) + t_ech_discrete,0) - (k_d-1)*I_r_ZMP(0);
            I_r_ZMP_des(1) = k_d*I_r_CP_ref(curr_iter_walking - (curr_footstep*step_time_discrete) + t_ech_discrete,1) - (k_d-1)*I_r_ZMP(1);
        }
        else{
            I_r_ZMP_des(0) = k_d*I_r_CP_ref(step_time_discrete,0) - (k_d-1)*I_r_ZMP(0);
            I_r_ZMP_des(1) = k_d*I_r_CP_ref(step_time_discrete,1) - (k_d-1)*I_r_ZMP(1);
        }

        // ZMP control to get desired acceleration || TODO: implement velocity-based control
        I_ddr_COM_des(0) = k_mf*(gravity_const/com_height)*(I_r_ZMP(0) - I_r_ZMP_des(0));
        I_ddr_COM_des(1) = k_mf*(gravity_const/com_height)*(I_r_ZMP(1) - I_r_ZMP_des(1));
        I_ddr_COM_des(2) = 0.0;

        I_dr_COM_des(0) = I_dr_COM_des(0) + I_ddr_COM_des(0)*dt_highlevel; // cur_comvel.e[0];
        I_dr_COM_des(1) = I_dr_COM_des(1) + I_ddr_COM_des(1)*dt_highlevel; //cur_comvel.e[1];
        I_dr_COM_des(2) = 0.0;

        I_r_COM_des(0) = I_r_COM_des(0) + I_dr_COM_des(0)*dt_highlevel; //+0.5*com_acc_d.e[0]*DT*DT; // included acceleration in calculation of COM position
        I_r_COM_des(1) = I_r_COM_des(1) + I_dr_COM_des(1)*dt_highlevel; //+0.5*com_acc_d.e[1]*DT*DT;

        printf("I_r_COM_des = [%f,%f,%f]\n", I_r_COM_des(0), I_r_COM_des(1), I_r_COM_des(2));
        //printf("I_r_ZMP_des = [%f,%f,%f]\n", I_r_ZMP_des(0), I_r_ZMP_des(1), I_r_ZMP_des(2));
        //printf("I_r_CP_ref = [%f,%f,%f]\n", I_r_CP_ref(curr_iter_walking - (curr_footstep*step_time_discrete) + t_ech_discrete,0), I_r_CP_ref(curr_iter_walking - (curr_footstep*step_time_discrete) + t_ech_discrete,1), I_r_CP_ref(curr_iter_walking - (curr_footstep*step_time_discrete) + t_ech_discrete,2));
        //printf("Step = %d\n", curr_footstep);


        // Pre-planned stuff

        // Set desired foot positions
        if(curr_footstep < numFootsteps){
            // Swing foot = LEFT
            if(curr_footstep%2 == 0){
                I_r_fl_des = I_r_f_traj.block(curr_iter_walking,0,1,3).transpose();
                I_r_fr_des = I_r_f_pre.block(curr_footstep,0,1,3).transpose();

                I_dr_fl_des = (I_r_f_traj.block(curr_iter_walking+1,0,1,3).transpose()-I_r_fl_des)/2.0;
                I_dr_fr_des = Eigen::Vector3d::Zero();
            }
            // Swing foot = RIGHT
            else if(curr_footstep%2 == 1){
                I_r_fl_des = I_r_f_pre.block(curr_footstep,0,1,3).transpose();
                I_r_fr_des = I_r_f_traj.block(curr_iter_walking,0,1,3).transpose();

                I_dr_fl_des = Eigen::Vector3d::Zero();
                I_dr_fr_des = (I_r_f_traj.block(curr_iter_walking+1,0,1,3).transpose()-I_r_fr_des)/2.0;
            }
        }
        // Final stance (check which foot is last step!!!)
        else{
            I_r_fl_des = I_r_f_pre.block(numFootsteps,0,1,3).transpose();
            I_r_fr_des = I_r_f_traj.block(numFootsteps-1,0,1,3).transpose();

            I_dr_fl_des = Eigen::Vector3d::Zero();
            I_dr_fr_des = Eigen::Vector3d::Zero();
        }

        if(curr_iter_walking < (numFootsteps - 1 + numConvergence)*step_time_discrete - 1){
            // Swing foot = LEFT
            if(curr_footstep%2 == 0){
                I_r_fl_des = I_r_fl_traj.block(curr_iter_walking,0,1,3).transpose();
                I_r_fr_des = I_r_fr_traj.block(curr_iter_walking,0,1,3).transpose();

                I_dr_fl_des = (I_r_fl_traj.block(curr_iter_walking+1,0,1,3).transpose()-I_r_fl_des)/2.0;
                I_dr_fr_des = Eigen::Vector3d::Zero();
            }
            // Swing foot = RIGHT
            else if(curr_footstep%2 == 1){
                I_r_fl_des = I_r_fl_traj.block(curr_iter_walking,0,1,3).transpose();
                I_r_fr_des = I_r_fr_traj.block(curr_iter_walking,0,1,3).transpose();

                I_dr_fl_des = Eigen::Vector3d::Zero();
                I_dr_fr_des = (I_r_fr_traj.block(curr_iter_walking+1,0,1,3).transpose()-I_r_fr_des)/2.0;
            }
        }
        // Final stance (check which foot is last step!!!)
        else{
            I_r_fl_des = I_r_fl_traj.block((numFootsteps - 1 + numConvergence)*step_time_discrete -1,0,1,3).transpose();
            I_r_fr_des = I_r_fr_traj.block((numFootsteps - 1 + numConvergence)*step_time_discrete -1,0,1,3).transpose();

            I_dr_fl_des = Eigen::Vector3d::Zero();
            I_dr_fr_des = Eigen::Vector3d::Zero();
        }

        // Set desired COM position and velocity
        if(curr_footstep < numFootsteps+1){
            I_r_COM_des = I_r_COM_des_pre.block(curr_iter_walking,0,1,3).transpose();
            I_dr_COM_des = I_dr_COM_des_pre.block(curr_iter_walking,0,1,3).transpose();
        }
        else{
            I_r_COM_des = I_r_COM_des_pre.block((numFootsteps+1)*step_time_discrete-1,0,1,3).transpose();
            I_dr_COM_des = I_dr_COM_des_pre.block((numFootsteps+1)*step_time_discrete-1,0,1,3).transpose();
        }

        // OVERWRITE AND MAKE ROBOT FALL (OPEN-LOOP)
        for(int i=0; i<HYDRA_JNT_MAX; i++)
        {
            q_c_des[i] = q_c_act[i];
        }

        curr_iter_walking += 1;

        if(curr_iter_walking%step_time_discrete == 0){
            curr_footstep += 1;
        }
    }

    t_highlevel += dt_highlevel;*/
}



void HydraWalkingControl::updateOSController(CHydraKinematics &hydra_kinematics){
    hydra_kinematics.GetJointData(joints);

    dq_c_act = q_c_act - q_c_act_prev;

    if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "t = %f \n", t_highlevel);
    }


    // Update step counter after t_Step has passed
    if( curr_iter_walking%step_time_discrete == 0 && curr_iter_walking > 1 ){
        curr_footstep++;
    }

    if(t_highlevel > (t_activate_legOScontrol-dt_highlevel) && t_highlevel < (t_activate_legOScontrol+dt_highlevel)){
        I_r_fl_init = joints[12].s_0_j; //JOINT_HYDRA_LANKLE_PITCH
        I_r_fl_des = I_r_fl_init;
        I_r_fl_target = I_r_fl_init + Eigen::Vector3d(I_x_fl_target,I_y_fl_target,I_z_fl_target);
        I_R_0_FL_target = joints[12].R_0_j;

        I_r_fr_init = joints[6].s_0_j; //JOINT_HYDRA_RANKLE_PITCH
        I_r_fr_des = I_r_fr_init;
        I_r_fr_target = I_r_fr_init + Eigen::Vector3d(I_x_fr_target,I_y_fr_target,I_z_fr_target);
        I_R_0_FR_target = joints[6].R_0_j;

        I_r_hl_init = joints[31].p_0_j; //JOINT_HYDRA_LWRIST_PITCH
        I_r_hl_des = I_r_hl_init;
        I_r_hl_target = I_r_hl_init + Eigen::Vector3d(I_x_hl_target,I_y_hl_target,I_z_hl_target);
        I_R_0_HL_target = joints[31].R_0_j;

        I_r_hr_init = joints[23].p_0_j; //JOINT_HYDRA_RWRIST_PITCH
        I_r_hr_des = I_r_hr_init;
        I_r_hr_target = I_r_hr_init + Eigen::Vector3d(I_x_hr_target,I_y_hr_target,I_z_hr_target);
        I_R_0_HR_target = joints[23].R_0_j;

        //I_dr_fl_des = (I_r_fl_target-I_r_fl_init)/t_arriveTarget;
        //I_dr_fl_des.setZero();
        //I_w_fl_des.setZero();
        /*printf("HYDRA_JNT_MAX = %d\n", HYDRA_JNT_MAX);
        printf("I_r_fl_init = [%f,%f,%f]\n", I_r_fl_init(0),I_r_fl_init(1),I_r_fl_init(2));
        printf("I_dr_fl_des = [%f,%f,%f]\n", I_dr_fl_des(0),I_dr_fl_des(1),I_dr_fl_des(2));*/
        //printf("q_c_des = [%f,%f,%f,%f]\n", q_c_des(6),q_c_des(7),q_c_des(8),q_c_des(9));
    }

    //==================================================
    //=== PD CONTROLLER ON JOINT LEVEL TO INITIALIZE ===
    //==================================================
    if(t_highlevel < t_activate_legOScontrol){
        // Set desired output position
        for(int i=0; i<HYDRA_JNT_MAX; i++) //HYDRA_JNT_MAX
        {
            q_c_des[i] = 0.0;
        }

        // Test some configs
        q_c_des[7] = -0.2;
        q_c_des[9] = 0.4;
        q_c_des[11] = -0.2;

        q_c_des[1] = -0.2;
        q_c_des[3] = 0.4;
        q_c_des[5] = -0.2;

        // elbow
        q_c_des[25] = 0.2;
        q_c_des[27] = -0.5;

        q_c_des[17] = -0.2;
        q_c_des[19] = -0.5;
    }
    //==============================================================
    //=== Operational Space Leg Control (Position Control Upper) ===
    //==============================================================
    else{
        // LEFT ANKLE CONTROLLER
        // ZERO DATA
        I_dr_fl_des.setZero();
        I_w_fl_des.setZero();

        // Get current left foot position (JOINT_HYDRA_LANKLE_PITCH)
        I_r_fl_curr = joints[12].s_0_j;

        // Change target position to be incremental waypoint
        double t_rel_ankleL = t_highlevel-t_activate_legOScontrol;
        int t_rel_ankleL_discrete = t_rel_ankleL/dt_highlevel;
        if(curr_iter_walking < (numFootstepsStatic)*step_time_discrete){
            I_r_fl_target = I_r_fl_traj_static.block(t_rel_ankleL_discrete,0,1,3).transpose() + (I_r_fl_init+I_r_fr_init)/2.0;
            I_dr_fl_des = (I_r_fl_traj_static.block(t_rel_ankleL_discrete+1,0,1,3).transpose()-I_r_fl_traj_static.block(t_rel_ankleL_discrete,0,1,3).transpose())/dt_highlevel;
        }

        // Set desired task space velocity
        /*if(t_highlevel < t_activate_legOScontrol + t_arriveTarget_LF){
            I_dr_fl_des = (I_r_fl_target-I_r_fl_init)/t_arriveTarget_LF;
        }*/

        // Generate desired position via numeric velocity integration (starting from initial pos.)
        I_r_fl_des = I_r_fl_des + dt_highlevel * I_dr_fl_des;

        // Get current and desired orientation quaternions
        Quaterniond quat_LF_curr(joints[12].R_0_j);
        Quaterniond quat_LF_des(I_R_0_FL_target);
        GetOrientationError(quat_LF_curr,quat_LF_des,err_ori_LF);

        // Cartesian reference velocity
        I_task_LF_vel.block(0,0,3,1) = I_dr_fl_des + (I_r_fl_des-I_r_fl_curr)*20.0;
        I_task_LF_vel.block(3,0,3,1) = I_w_fl_des - 10.0 * err_ori_LF;

        // Get Jacobian and Pseudo-Inverse
        SetJacLeftAnkleEE(Vector3d::Zero(), B_J_LF);
        JJT_LF = B_J_LF * B_J_LF.transpose();
        SetPInv(JJT_LF,JJT_LF_inv,1.0e-6);
        PInv_J_LF = B_J_LF.transpose() * JJT_LF_inv;

        // Desired joint velocity w/ PInv projection of ref. task-space velocity and nullspace projection of ref. posture
        dq_LF_des = PInv_J_LF * I_task_LF_vel + 1.0 * (MatrixXd::Identity(6,6) - PInv_J_LF*B_J_LF) * (q_LF_des_rest - q_c_act.block(6,0,6,1));

        // Desired joint positions via numeric integration of des. joint velocities
        q_c_des.block(6,0,6,1) = q_c_des.block(6,0,6,1) + dt_highlevel * dq_LF_des;


        // RIGHT ANKLE CONTROLLER
        // ZERO DATA
        I_dr_fr_des.setZero();
        I_w_fr_des.setZero();

        // Get current left foot position (JOINT_HYDRA_LANKLE_PITCH)
        I_r_fr_curr = joints[6].s_0_j;

        // Change target position to be incremental waypoint
        double t_rel_ankleR = t_highlevel-t_activate_legOScontrol;
        int t_rel_ankleR_discrete = t_rel_ankleR/dt_highlevel;
        if(curr_iter_walking < (numFootstepsStatic)*step_time_discrete){
            I_r_fr_target = I_r_fr_traj_static.block(t_rel_ankleR_discrete,0,1,3).transpose() + (I_r_fl_init+I_r_fr_init)/2.0;
            I_dr_fr_des = (I_r_fr_traj_static.block(t_rel_ankleR_discrete+1,0,1,3).transpose()-I_r_fr_traj_static.block(t_rel_ankleR_discrete,0,1,3).transpose())/dt_highlevel;
        }

        // Set desired task space velocity
        /*if(t_highlevel < t_activate_legOScontrol + t_arriveTarget_RF){
            I_dr_fr_des = (I_r_fr_target-I_r_fr_init)/t_arriveTarget_RF;
        }*/

        // Generate desired position via numeric velocity integration (starting from initial pos.)
        I_r_fr_des = I_r_fr_des + dt_highlevel * I_dr_fr_des;

        // Get current and desired orientation quaternions
        Quaterniond quat_RF_curr(joints[6].R_0_j);
        Quaterniond quat_RF_des(I_R_0_FR_target);
        GetOrientationError(quat_RF_curr,quat_RF_des,err_ori_RF);

        // Cartesian reference velocity
        I_task_RF_vel.block(0,0,3,1) = I_dr_fr_des + (I_r_fr_des-I_r_fr_curr)*20.0;
        I_task_RF_vel.block(3,0,3,1) = I_w_fr_des - 10.0 * err_ori_RF;

        // Get Jacobian and Pseudo-Inverse
        SetJacRightAnkleEE(Vector3d::Zero(), B_J_RF);
        JJT_RF = B_J_RF * B_J_RF.transpose();
        SetPInv(JJT_RF,JJT_RF_inv,1.0e-6);
        PInv_J_RF = B_J_RF.transpose() * JJT_RF_inv;

        // Desired joint velocity w/ PInv projection of ref. task-space velocity and nullspace projection of ref. posture
        dq_RF_des = PInv_J_RF * I_task_RF_vel;// + 1.0 * (MatrixXd::Identity(6,6) - PInv_J_RF*B_J_RF) * (q_RF_des_rest - q_c_act.block(0,0,6,1));

        // Desired joint positions via numeric integration of des. joint velocities
        q_c_des.block(0,0,6,1) = q_c_des.block(0,0,6,1) + dt_highlevel * dq_RF_des;

        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "I_task_RF_vel = [%f, %f, %f] \n", I_task_RF_vel(0),I_task_RF_vel(1), I_task_RF_vel(2));
        fprintf(fp_debugJac, "\n");






        // LEFT WRIST CONTROLLER
        // ZERO DATA
        I_dr_hl_des.setZero();
        I_w_hl_des.setZero();

        // Get current left foot position (JOINT_HYDRA_LANKLE_PITCH)
        I_r_hl_curr = joints[31].p_0_j;

        // Set desired task space velocity
        if(t_highlevel < t_activate_legOScontrol + t_arriveTarget_LH){
            I_dr_hl_des = (I_r_hl_target-I_r_hl_init)/t_arriveTarget_LH;
        }

        // Generate desired position via numeric velocity integration (starting from initial pos.)
        I_r_hl_des = I_r_hl_des + dt_highlevel * I_dr_hl_des;

        // Get current and desired orientation quaternions
        Quaterniond quat_LH_curr(joints[31].R_0_j);
        Quaterniond quat_LH_des(I_R_0_HL_target);
        GetOrientationError(quat_LH_curr,quat_LH_des,err_ori_LH);

        // Cartesian reference velocity
        I_task_LH_vel.block(0,0,3,1) = I_dr_hl_des + 10.0 * (I_r_hl_des-I_r_hl_curr);
        I_task_LH_vel.block(3,0,3,1) = I_w_hl_des - 10.0 * err_ori_LH;

        // Get Jacobian and Pseudo-Inverse
        SetJacLeftWristEE(Vector3d::Zero(), J_LH);
        JJT_LH = J_LH * J_LH.transpose();
        SetPInv(JJT_LH,JJT_LH_inv,1.0e-6);
        PInv_J_LH = J_LH.transpose() * JJT_LH_inv;

        // Desired joint velocity w/ PInv projection of ref. task-space velocity and nullspace projection of ref. posture
        dq_LH_des = PInv_J_LH * I_task_LH_vel + 1.0 * (MatrixXd::Identity(8,8) - PInv_J_LH*J_LH) * (1.0*(q_LH_des_rest - q_c_act.block(23,0,8,1)) + 10.0*(VectorXd::Zero(8)-dq_c_act.block(23,0,8,1)));

        // Desired joint positions via numeric integration of des. joint velocities
        q_c_des.block(23,0,8,1) = q_c_des.block(23,0,8,1) + dt_highlevel * dq_LH_des;



        // RIGHT WRIST CONTROLLER
        // ZERO DATA
        I_dr_hr_des.setZero();
        I_w_hr_des.setZero();

        // Get current left foot position (JOINT_HYDRA_LANKLE_PITCH)
        I_r_hr_curr = joints[23].p_0_j;

        // Set desired task space velocity
        if(t_highlevel < t_activate_legOScontrol + t_arriveTarget_RH){
            I_dr_hr_des = (I_r_hr_target-I_r_hr_init)/t_arriveTarget_RH;
        }

        // Generate desired position via numeric velocity integration (starting from initial pos.)
        I_r_hr_des = I_r_hr_des + dt_highlevel * I_dr_hr_des;

        // Get current and desired orientation quaternions
        Quaterniond quat_RH_curr(joints[23].R_0_j);
        Quaterniond quat_RH_des(I_R_0_HR_target);
        GetOrientationError(quat_RH_curr,quat_RH_des,err_ori_RH);

        // Cartesian reference velocity
        I_task_RH_vel.block(0,0,3,1) = I_dr_hr_des + 10.0 * (I_r_hr_des-I_r_hr_curr);
        I_task_RH_vel.block(3,0,3,1) = I_w_hr_des - 10.0 * err_ori_RH;

        // Get Jacobian and Pseudo-Inverse
        SetJacRightWristEE(Vector3d::Zero(), J_RH);
        JJT_RH = J_RH * J_RH.transpose();
        SetPInv(JJT_RH,JJT_RH_inv,1.0e-6);
        PInv_J_RH = J_RH.transpose() * JJT_RH_inv;

        // Desired joint velocity w/ PInv projection of ref. task-space velocity and nullspace projection of ref. posture
        dq_RH_des = PInv_J_RH * I_task_RH_vel + 1.0 * (MatrixXd::Identity(8,8) - PInv_J_RH*J_RH) * (1.0*(q_RH_des_rest - q_c_act.block(15,0,8,1)) + 10.0*(VectorXd::Zero(8)-dq_c_act.block(15,0,8,1)));

        // Desired joint positions via numeric integration of des. joint velocities
        q_c_des.block(15,0,8,1) = q_c_des.block(15,0,8,1) + dt_highlevel * dq_RH_des;


        // COG experiments
        /*for(int joint = 0; joint < 58; ++joint){
            SetLinkBaseCOGJacobian(joint);
        }
        SetBaseCOGJacobian(B_J_COG_lin);
        SetJacobianLinHYDRA(B_J_COG_lin, B_J_COG_lin_HYDRA);
        I_J_COG_lin_HYDRA = joints[0].R_0_j.transpose()*B_J_COG_lin_HYDRA;

        JJT_COG = I_J_COG_lin_HYDRA * I_J_COG_lin_HYDRA.transpose();
        SetPInv(JJT_COG,JJT_COG_inv,1.0e-6);
        PInv_J_COG = I_J_COG_lin_HYDRA.transpose() * JJT_COG_inv;

        Vector3d COM_task_vel(0.01,-0.01,-0.01);
        dq_c_des = PInv_J_COG * COM_task_vel;
        q_c_des = q_c_des + dt_highlevel * dq_c_des;*/


        // =========== DEBUGGING ============

        /*if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "J^{+}J = [%f,%f,%f,%f,%f,%f] \n ", TEST(0,0),TEST(0,1),TEST(0,2),TEST(0,3),TEST(0,4),TEST(0,5));
            fprintf(fp_debugJac, "         [%f,%f,%f,%f,%f,%f] \n ", TEST(1,0),TEST(1,1),TEST(1,2),TEST(1,3),TEST(1,4),TEST(1,5));
            fprintf(fp_debugJac, "         [%f,%f,%f,%f,%f,%f] \n ", TEST(2,0),TEST(2,1),TEST(2,2),TEST(2,3),TEST(2,4),TEST(2,5));
            fprintf(fp_debugJac, "         [%f,%f,%f,%f,%f,%f] \n ", TEST(3,0),TEST(3,1),TEST(3,2),TEST(3,3),TEST(3,4),TEST(3,5));
            fprintf(fp_debugJac, "         [%f,%f,%f,%f,%f,%f] \n ", TEST(4,0),TEST(4,1),TEST(4,2),TEST(4,3),TEST(4,4),TEST(4,5));
            fprintf(fp_debugJac, "         [%f,%f,%f,%f,%f,%f] \n ", TEST(5,0),TEST(5,1),TEST(5,2),TEST(5,3),TEST(5,4),TEST(5,5));
            fprintf(fp_debugJac, "\n");
        }*/

        /*if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "J^{+}J = [%f,%f,%f,%f,%f,%f,%f,%f] \n ", TEST(0,0),TEST(0,1),TEST(0,2),TEST(0,3),TEST(0,4),TEST(0,5),TEST(0,6),TEST(0,7));
            fprintf(fp_debugJac, "         [%f,%f,%f,%f,%f,%f,%f,%f] \n ", TEST(1,0),TEST(1,1),TEST(1,2),TEST(1,3),TEST(1,4),TEST(1,5),TEST(1,6),TEST(1,7));
            fprintf(fp_debugJac, "         [%f,%f,%f,%f,%f,%f,%f,%f] \n ", TEST(2,0),TEST(2,1),TEST(2,2),TEST(2,3),TEST(2,4),TEST(2,5),TEST(2,6),TEST(2,7));
            fprintf(fp_debugJac, "         [%f,%f,%f,%f,%f,%f,%f,%f] \n ", TEST(3,0),TEST(3,1),TEST(3,2),TEST(3,3),TEST(3,4),TEST(3,5),TEST(3,6),TEST(3,7));
            fprintf(fp_debugJac, "         [%f,%f,%f,%f,%f,%f,%f,%f] \n ", TEST(4,0),TEST(4,1),TEST(4,2),TEST(4,3),TEST(4,4),TEST(4,5),TEST(4,6),TEST(4,7));
            fprintf(fp_debugJac, "         [%f,%f,%f,%f,%f,%f,%f,%f] \n ", TEST(5,0),TEST(5,1),TEST(5,2),TEST(5,3),TEST(5,4),TEST(5,5),TEST(5,6),TEST(5,7));
            fprintf(fp_debugJac, "\n");
        }*/

        /*if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "EE Error = [%f \n ", I_r_fl_des(0)-I_r_fl_curr(0));
            fprintf(fp_debugJac, "            %f \n ", I_r_fl_des(1)-I_r_fl_curr(1));
            fprintf(fp_debugJac, "            %f]\n ", I_r_fl_des(2)-I_r_fl_curr(2));
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "I_w_fl_des = [%f \n ", I_w_fl_des(0));
            fprintf(fp_debugJac, "              %f \n ", I_w_fl_des(1));
            fprintf(fp_debugJac, "              %f]\n ", I_w_fl_des(2));
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "I_w_fl_curr = [%f \n ", I_w_fl_curr(0));
            fprintf(fp_debugJac, "               %f \n ", I_w_fl_curr(1));
            fprintf(fp_debugJac, "               %f]\n ", I_w_fl_curr(2));
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "err_ori_LF = [%f \n ", err_ori_LF(0));
            fprintf(fp_debugJac, "              %f \n ", err_ori_LF(1));
            fprintf(fp_debugJac, "              %f]\n ", err_ori_LF(2));
            fprintf(fp_debugJac, "\n");
        }*/

        /*if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "EE Error = [%f \n ", I_r_hl_des(0)-I_r_hl_curr(0));
            fprintf(fp_debugJac, "            %f \n ", I_r_hl_des(1)-I_r_hl_curr(1));
            fprintf(fp_debugJac, "            %f]\n ", I_r_hl_des(2)-I_r_hl_curr(2));
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "I_w_hl_des = [%f \n ", I_w_hl_des(0));
            fprintf(fp_debugJac, "              %f \n ", I_w_hl_des(1));
            fprintf(fp_debugJac, "              %f]\n ", I_w_hl_des(2));
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "I_w_hl_curr = [%f \n ", I_w_hl_curr(0));
            fprintf(fp_debugJac, "               %f \n ", I_w_hl_curr(1));
            fprintf(fp_debugJac, "               %f]\n ", I_w_hl_curr(2));
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "err_ori_LH = [%f \n ", err_ori_LH(0));
            fprintf(fp_debugJac, "              %f \n ", err_ori_LH(1));
            fprintf(fp_debugJac, "              %f]\n ", err_ori_LH(2));
            fprintf(fp_debugJac, "\n");
        }*/

        if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "I_r_fl_target = [%f \n ", I_r_fl_target(0));
            fprintf(fp_debugJac, "                 %f \n ", I_r_fl_target(1));
            fprintf(fp_debugJac, "                 %f]\n ", I_r_fl_target(2));
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "I_dr_fl_des = [%f \n ", I_dr_fl_des(0));
            fprintf(fp_debugJac, "               %f \n ", I_dr_fl_des(1));
            fprintf(fp_debugJac, "               %f]\n ", I_dr_fl_des(2));
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "I_r_fr_target = [%f \n ", I_r_fr_target(0));
            fprintf(fp_debugJac, "                 %f \n ", I_r_fr_target(1));
            fprintf(fp_debugJac, "                 %f]\n ", I_r_fr_target(2));
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "I_dr_fr_des = [%f \n ", I_dr_fr_des(0));
            fprintf(fp_debugJac, "               %f \n ", I_dr_fr_des(1));
            fprintf(fp_debugJac, "               %f]\n ", I_dr_fr_des(2));
            fprintf(fp_debugJac, "\n");
        }


        // DEBUGGING START
        TEST = PInv_J_LH*J_LH;
        /*for(int i = 0; i < 6; ++i){
            for(int j = 0; j < 6; ++j){
                if(i==j){
                    if(TEST(i,j) > 1.0 + 1.0e-5 || TEST(i,j) < 1.0 - 1.0e-5){
                        printf("Error: %f,", TEST(i,j));
                    }
                }
                else{
                    if(TEST(i,j) > 0.0 + 1.0e-5 || TEST(i,j) < 0.0 - 1.0e-5){
                        printf("Error: %f,", TEST(i,j));
                    }
                }
                //printf("%f,", TEST(i,j));
            }
            //printf("\n");
        }*/
        //printf("\n");
        //printf("\n");
        // DEBUGGING END


        fprintf(fp_logOSCWalk, "%f,%f,%f,", I_r_fl_curr[0],I_r_fl_curr[1],I_r_fl_curr[2]);
        fprintf(fp_logOSCWalk, "%f,%f,%f,", I_r_fl_des[0],I_r_fl_des[1],I_r_fl_des[2]);
        fprintf(fp_logOSCWalk, "%f,%f,%f,%f,", quat_LF_curr.w(),quat_LF_curr.x(),quat_LF_curr.y(),quat_LF_curr.z());
        fprintf(fp_logOSCWalk, "%f,%f,%f,%f,", quat_LF_des.w(),quat_LF_des.x(),quat_LF_des.y(),quat_LF_des.z());

        fprintf(fp_logOSCWalk, "%f,%f,%f,", I_r_fr_curr[0],I_r_fr_curr[1],I_r_fr_curr[2]);
        fprintf(fp_logOSCWalk, "%f,%f,%f,", I_r_fr_des[0],I_r_fr_des[1],I_r_fr_des[2]);
        fprintf(fp_logOSCWalk, "%f,%f,%f,%f,", quat_RF_curr.w(),quat_RF_curr.x(),quat_RF_curr.y(),quat_RF_curr.z());
        fprintf(fp_logOSCWalk, "%f,%f,%f,%f,", quat_RF_des.w(),quat_RF_des.x(),quat_RF_des.y(),quat_RF_des.z());

        fprintf(fp_logOSCWalk, "\n");


        curr_iter_walking += 1;

        if(curr_iter_walking%step_time_discrete == 0){
            curr_footstep += 1;
        }
    }

    q_c_act_prev = q_c_act;

    t_highlevel += dt_highlevel;
}



void HydraWalkingControl::SetJacLeftAnkleEE(Vector3d EE_r_EEtoPoint, MatrixXd &B_J_LF){
    // Erase for safety
    J_LF.setZero();

    // Temp. matrix (full DOF forward kinematics)
    MatrixXd J_LF_kin;
    J_LF_kin.resize(6,58);
    J_LF_kin.setZero();

    // Hard-coded joint indices
    int joint_Base = 0;  // Waist Base
    int joint_Hip = 7;   // Hip Yaw Left
    int joint_Foot = 12; // Ankle Pitch Left

    // Allocate position vectors
    Vector3d p_0_BaseToPoint = joints[joint_Foot].p_0_j + joints[joint_Foot].R_0_j*EE_r_EEtoPoint - joints[joint_Base].p_0_j;
    Vector3d p_0_BaseToJoint;
    Vector3d p_0_JointToPoint;

    // Allocate vector for joint rotation in world frame
    Vector3d z_0_Joint;

    // Start iteration from ankle joint until we reach waist base (better via .parent w/ new .wrl model)
    int joint_i = joint_Foot;
    while(joint_i != joint_Hip-1){ // up until 7 = Left Hip Yaw (inclusive)
        z_0_Joint = joints[joint_i].R_0_j * Vector3d::UnitZ();

        p_0_BaseToJoint = joints[joint_i].p_0_j - joints[joint_Base].p_0_j;
        p_0_JointToPoint = p_0_BaseToPoint - p_0_BaseToJoint;

        // Get linear jacobian
        J_LF_kin.block(0,joint_i,3,1) = z_0_Joint.cross(p_0_JointToPoint);

        // Get rotational jacobian
        J_LF_kin.block(3,joint_i,3,1) = z_0_Joint;

        // move up kinematic chain
        joint_i = joint_i-1;
    }
    // Write from full jacobian to task specific jacobian (6x6)
    for(int i = 0; i < 6; ++i){
        B_J_LF.block(0,i,3,1) = J_LF_kin.block(0,i+6+1,3,1);
        B_J_LF.block(3,i,3,1) = J_LF_kin.block(3,i+6+1,3,1);
    }
}

void HydraWalkingControl::SetJacRightAnkleEE(Vector3d EE_r_EEtoPoint, MatrixXd &B_J_RF){
    // Erase for safety
    B_J_RF.setZero();

    // Temp. matrix (full DOF forward kinematics)
    MatrixXd J_RF_kin;
    J_RF_kin.resize(6,58);
    J_RF_kin.setZero();

    // Hard-coded joint indices
    int joint_Base = 0;  // Waist Base
    int joint_Hip = 1;   // Hip Yaw Right
    int joint_Foot = 6; // Ankle Pitch Right

    // Allocate position vectors
    Vector3d p_0_BaseToPoint = joints[joint_Foot].p_0_j + joints[joint_Foot].R_0_j*EE_r_EEtoPoint - joints[joint_Base].p_0_j;
    Vector3d p_0_BaseToJoint;
    Vector3d p_0_JointToPoint;

    // Allocate vector for joint rotation in world frame
    Vector3d z_0_Joint;

    // Start iteration from ankle joint until we reach waist base (better via .parent w/ new .wrl model)
    int joint_i = joint_Foot;
    while(joint_i != joint_Hip-1){ // up until 1 = Right Hip Yaw (inclusive)
        z_0_Joint = joints[joint_i].R_0_j * Vector3d::UnitZ();

        p_0_BaseToJoint = joints[joint_i].p_0_j - joints[joint_Base].p_0_j;
        p_0_JointToPoint = p_0_BaseToPoint - p_0_BaseToJoint;

        // Get linear jacobian
        J_RF_kin.block(0,joint_i,3,1) = z_0_Joint.cross(p_0_JointToPoint);

        // Get rotational jacobian
        J_RF_kin.block(3,joint_i,3,1) = z_0_Joint;

        // move up kinematic chain
        joint_i = joint_i-1;
    }
    // Write from full jacobian to task specific jacobian (6x6)
    for(int i = 0; i < 6; ++i){
        B_J_RF.block(0,i,3,1) = J_RF_kin.block(0,i+1,3,1);
        B_J_RF.block(3,i,3,1) = J_RF_kin.block(3,i+1,3,1);
    }
}

// EE_r_EEtoPoint =  point on wrist in wrist frame (EE = Wrist Pitch)
void HydraWalkingControl::SetJacLeftWristEE(Vector3d EE_r_EEtoPoint, MatrixXd &J_LH){
    // Erase for safety
    J_LH.setZero();

    // Temp. matrix (full DOF forward kinematics)
    MatrixXd J_LH_kin;
    J_LH_kin.resize(6,58);
    J_LH_kin.setZero();

    // Hard-coded joint indices
    int joint_Base = 0;   // Waist Base
    int joint_Torso = 24; // Waist Roll
    int joint_Hand = 31;  // Wrist Pitch Left

    // Allocate position vectors
    Vector3d p_0_BaseToPoint = joints[joint_Hand].p_0_j + joints[joint_Hand].R_0_j*EE_r_EEtoPoint - joints[joint_Base].p_0_j;
    Vector3d p_0_BaseToJoint;
    Vector3d p_0_JointToPoint;

    // Allocate vector for joint rotation in world frame
    Vector3d z_0_Joint;

    // Start iteration from wrist joint until we reach torso (better via .parent w/ new .wrl model)
    int joint_i = joint_Hand;
    while(joint_i != joint_Torso-1){ // up until 24 = Scaplae Yaw Left (inclusive)
        z_0_Joint = joints[joint_i].R_0_j * Vector3d::UnitZ();

        p_0_BaseToJoint = joints[joint_i].p_0_j - joints[joint_Base].p_0_j;
        p_0_JointToPoint = p_0_BaseToPoint - p_0_BaseToJoint;

        // Get linear jacobian
        J_LH_kin.block(0,joint_i,3,1) = z_0_Joint.cross(p_0_JointToPoint);

        // Get rotational jacobian
        J_LH_kin.block(3,joint_i,3,1) = z_0_Joint;

        // move up kinematic chain
        joint_i = joint_i-1;
    }
    // Write from full jacobian to task specific jacobian (6x8)
    for(int i = 0; i < 8; ++i){
        J_LH.block(0,i,3,1) = J_LH_kin.block(0,i+23+1,3,1);
        J_LH.block(3,i,3,1) = J_LH_kin.block(3,i+23+1,3,1);
    }
}



// EE_r_EEtoPoint =  point on wrist in wrist frame (EE = Wrist Pitch)
void HydraWalkingControl::SetJacRightWristEE(Vector3d EE_r_EEtoPoint, MatrixXd &J_RH){
    // Erase for safety
    J_RH.setZero();

    // Temp. matrix (full DOF forward kinematics)
    MatrixXd J_RH_kin;
    J_RH_kin.resize(6,58);
    J_RH_kin.setZero();

    // Hard-coded joint indices
    int joint_Base = 0;   // Waist Base
    int joint_Torso = 16; // Waist Roll
    int joint_Hand = 23;  // Wrist Pitch Right

    // Allocate position vectors
    Vector3d p_0_BaseToPoint = joints[joint_Hand].p_0_j + joints[joint_Hand].R_0_j*EE_r_EEtoPoint - joints[joint_Base].p_0_j;
    Vector3d p_0_BaseToJoint;
    Vector3d p_0_JointToPoint;

    // Allocate vector for joint rotation in world frame
    Vector3d z_0_Joint;

    // Start iteration from wrist joint until we reach torso (better via .parent w/ new .wrl model)
    int joint_i = joint_Hand;
    while(joint_i != joint_Torso-1){ // up until 16 = Scaplae Yaw Right (inclusive)
        z_0_Joint = joints[joint_i].R_0_j * Vector3d::UnitZ();

        p_0_BaseToJoint = joints[joint_i].p_0_j - joints[joint_Base].p_0_j;
        p_0_JointToPoint = p_0_BaseToPoint - p_0_BaseToJoint;

        // Get linear jacobian
        J_RH_kin.block(0,joint_i,3,1) = z_0_Joint.cross(p_0_JointToPoint);

        // Get rotational jacobian
        J_RH_kin.block(3,joint_i,3,1) = z_0_Joint;

        // move up kinematic chain
        joint_i = joint_i-1;
    }
    // Write from full jacobian to task specific jacobian (6x8)
    for(int i = 0; i < 8; ++i){
        J_RH.block(0,i,3,1) = J_RH_kin.block(0,i+15+1,3,1);
        J_RH.block(3,i,3,1) = J_RH_kin.block(3,i+15+1,3,1);
    }
}

void HydraWalkingControl::SetBaseJacLeftAnkleEE(MatrixXd &J_LF, MatrixXd &B_J_LF){
    // Erase for safety
    B_J_LF.setZero();
    // Transform Jacobian
    B_J_LF = joints[0].R_0_j.transpose()*J_LF;
}

void HydraWalkingControl::SetBaseJacRightAnkleEE(MatrixXd &J_RF, MatrixXd &B_J_RF){
    // Erase for safety
    B_J_RF.setZero();
    // Transform Jacobian
    B_J_RF = joints[0].R_0_j.transpose()*J_RF;
}



// Hack to emulate IMU, assuming foot ground contact
void HydraWalkingControl::SetIMUEmulated(int joint_foot_contact){
    R_I_0.setZero();
    R_0_foot.setZero();
    if(t_highlevel < 2*dt_highlevel){
        R_I_foot.setZero();
        R_I_foot = joints[joint_foot_contact].R_0_j;
    }
    R_0_foot = joints[joint_foot_contact].R_0_j;
    R_I_0 = R_I_foot * R_0_foot.transpose();

    for(int i = 0; i < HYDRA_JNT_MAX; ++i){
        joints[i].R_I_j = R_I_0 * joints[i].R_0_j;
    }
}



// ==================== FUNCTIONS RELATED TO COG JACOBIAN ====================

void HydraWalkingControl::SetLinkBaseCOGJacobian(int joint){
    // Erase for safety
    joints[joint].B_J_COG_lin.setZero();

    // Hard-coded joint indices
    int joint_Base = 0;   // Waist Base

    // Allocate position vectors
    Vector3d p_0_BaseToLinkCOG = joints[joint].s_0_j - joints[joint_Base].p_0_j;
    Vector3d p_0_BaseToJoint;
    Vector3d p_0_JointToLinkCOG;

    // Allocate vector for joint rotation in world frame
    Vector3d z_0_Joint;

    // Start iteration from wrist joint until we reach torso (better via .parent w/ new .wrl model)
    int joint_i = joint;
    while(joint_i != -1){
        z_0_Joint = joints[joint_i].R_0_j * Vector3d::UnitZ();

        p_0_BaseToJoint = joints[joint_i].p_0_j - joints[joint_Base].p_0_j;
        p_0_JointToLinkCOG = p_0_BaseToLinkCOG - p_0_BaseToJoint;

        // Get linear jacobian
        joints[joint].B_J_COG_lin.block(0,joint_i,3,1) = z_0_Joint.cross(p_0_JointToLinkCOG);

        // move up kinematic chain
        joint_i = joints[joint_i].parent_joint;
    }
}

void HydraWalkingControl::SetChainBaseCOGJacobian(MatrixXd &B_J_COG_chain_lin, int joint){
    B_J_COG_chain_lin.setZero();

    double m_total = 0.0;
    int joint_i = joint;
    while(joint_i != -1){
        B_J_COG_chain_lin += joints[joint_i].m * joints[joint_i].B_J_COG_lin;
        m_total += joints[joint_i].m;

        // move up kinematic chain
        joint_i = joints[joint_i].parent_joint;
    }
    B_J_COG_chain_lin = B_J_COG_chain_lin / m_total;
}

void HydraWalkingControl::SetBaseCOGJacobian(MatrixXd &B_J_COG_lin){
    double m_total = 0.0;
    for(int joint = 0; joint < 58; ++joint){
        B_J_COG_lin += joints[joint].m * joints[joint].B_J_COG_lin;
        m_total += joints[joint].m;
    }
    B_J_COG_lin = B_J_COG_lin / m_total;
}

void HydraWalkingControl::SetBaseCOGPosition(Vector3d &B_r_COG){
    double m_total = 0.0;
    for(int joint = 0; joint < 58; ++joint){
        B_r_COG += joints[joint].m * joints[joint].s_0_j;
        m_total += joints[joint].m;
    }
    B_r_COG = B_r_COG / m_total;
}

void HydraWalkingControl::SetBaseChainCOGPosition(Vector3d &B_r_COG_chain, int joint){
    B_r_COG_chain.setZero();

    double m_total = 0.0;
    int joint_i = joint;
    while(joint_i != -1){
        B_r_COG_chain += joints[joint_i].m * joints[joint_i].s_0_j;
        m_total += joints[joint_i].m;

        // move up kinematic chain
        joint_i = joints[joint_i].parent_joint;
    }
    B_r_COG_chain = B_r_COG_chain / m_total;
}

void HydraWalkingControl::SetCOGJacobian(MatrixXd &I_J_COG_lin){
    SetBaseCOGPosition(B_r_COG);
    for(int joint = 0; joint < 58; ++joint){
        SetLinkBaseCOGJacobian(joint);
    }
    SetBaseCOGJacobian(B_J_COG_lin);

    SetJacLeftAnkleEE(Vector3d::Zero(), B_J_LF);
    SetJacRightAnkleEE(Vector3d::Zero(), B_J_RF);

    // Left Leg is Stance Leg
    Vector3d B_r_SupportFootToCOG = B_r_COG - joints[12].p_0_j;
    Matrix3d B_r_SupportFootToCOG_skew;
    GetSkew(B_r_SupportFootToCOG, B_r_SupportFootToCOG_skew);
    I_J_COG_lin = joints[0].R_I_j*(B_J_COG_lin - B_J_LF.block(0,0,3,58) + B_r_SupportFootToCOG_skew*B_J_LF.block(3,0,3,58));
}

void HydraWalkingControl::SetJacobianLinHYDRA(MatrixXd &J_lin, MatrixXd &J_lin_HYDRA){
    // RIGHT LEG 6 DOF
    J_lin_HYDRA.block(0,0,3,6) = J_lin.block(0,1,3,6);

    // LEFT LEG 6 DOF
    J_lin_HYDRA.block(0,6,3,6) = J_lin.block(0,7,3,6);

    // WAIST PITCH & ROLL 2 DOF
    J_lin_HYDRA.block(0,12,3,1) = J_lin.block(0,13,3,1);
    J_lin_HYDRA.block(0,13,3,1) = J_lin.block(0,14,3,1);

    // NECK PITCH
    J_lin_HYDRA.block(0,14,3,1) = J_lin.block(0,15,3,1);

    // RIGHT ARM 8 DOF
    J_lin_HYDRA.block(0,15,3,8) = J_lin.block(0,16,3,8);

    // LEFT ARM 8 DOF
    J_lin_HYDRA.block(0,23,3,8) = J_lin.block(0,24,3,8);

    // RIGHT HAND THUMB 2 DOF
    J_lin_HYDRA.block(0,31,3,2) = J_lin.block(0,32,3,2);

    // RIGHT HAND INDEX 1 DOF
    J_lin_HYDRA.block(0,33,3,1) = J_lin.block(0,36,3,1);

    // RIGHT HAND MIDDLE 1 DOF
    J_lin_HYDRA.block(0,34,3,1) = J_lin.block(0,39,3,1);

    // RIGHT HAND RING 1 DOF
    J_lin_HYDRA.block(0,35,3,1) = J_lin.block(0,42,3,1);

    // LEFT HAND THUMB 2 DOF
    J_lin_HYDRA.block(0,36,3,2) = J_lin.block(0,45,3,2);

    // LEFT HAND INDEX 1 DOF
    J_lin_HYDRA.block(0,38,3,1) = J_lin.block(0,49,3,1);

    // LEFT HAND MIDDLE 1 DOF
    J_lin_HYDRA.block(0,39,3,1) = J_lin.block(0,52,3,1);

    // LEFT HAND RING 1 DOF
    J_lin_HYDRA.block(0,40,3,1) = J_lin.block(0,55,3,1);
}



// ==================== TOOL FUNCTIONS ====================

void HydraWalkingControl::SetPInv(MatrixXd &mat, MatrixXd &mat_pinv, double svd_tol){
    // SVD
    JacobiSVD<MatrixXd> svd_mat(mat, ComputeFullU | ComputeFullV);

    VectorXd singularValues_inv = svd_mat.singularValues();
    for(int i = 0; i < svd_mat.singularValues().rows(); ++i){
        if((svd_mat.singularValues())(i) > svd_tol){
            singularValues_inv(i) = 1.0 / (svd_mat.singularValues())(i);
        }
        else{
            singularValues_inv(i) = 0.0;
        }
    }
    mat_pinv = svd_mat.matrixV() * singularValues_inv.asDiagonal() * svd_mat.matrixU().transpose();
}

void HydraWalkingControl::GetSkew(Vector3d vec, Matrix3d &mat){
    mat(0,0) = 0.0;
    mat(0,1) = -vec[2];
    mat(0,2) = vec[1];

    mat(1,0) = vec[2];
    mat(1,1) = 0.0;
    mat(1,2) = -vec[0];

    mat(2,0) = -vec[1];
    mat(2,1) = vec[0];
    mat(2,2) = 0.0;
}

void HydraWalkingControl::GetOrientationError(Quaterniond quat_act, Quaterniond quat_des, Vector3d &err_ori){
    Vector3d quat_act_vec(quat_act.x(),quat_act.y(),quat_act.z());
    Vector3d quat_des_vec(quat_des.x(),quat_des.y(),quat_des.z());
    Matrix3d quat_des_vec_skew;
    GetSkew(quat_des_vec, quat_des_vec_skew);

    err_ori = quat_des.w() * quat_act_vec - quat_act.w() * quat_des_vec + quat_des_vec_skew * quat_act_vec;
}



// ==================== FUNCTIONS RELATED TO COMMUNICATION ====================

void HydraWalkingControl::getJointDes(joint_cmd_t  jnt_cmd[]){
    for(int j=0; j<HYDRA_JNT_MAX; j++) {
        jnt_cmd[j].DATA.pos_ref = q_c_des[j];
    }
}

void HydraWalkingControl::setJointAct(joint_state_t  jnt_state[]){
    for(int j=0; j<HYDRA_JNT_MAX; j++) { //HYDRA_JNT_MAX
        q_c_act[j] = jnt_state[j].DATA.pos_act;
    }
}



// ==================== FUNCTIONS RELATING TO MOTION PLANNING ====================

void HydraWalkingControl::PreplanFootTrajectoriesStatic(){
    I_r_fl_static.block(0,0,1,3) = Vector3d(0.0,step_width/2.0,0.0).transpose();
    I_r_fl_static.block(1,0,1,3) = Vector3d(0.0,step_width/2.0,0.0).transpose();
    I_r_fl_static.block(2,0,1,3) = Vector3d(step_length/4.0,step_width/2.0,0.0).transpose();
    I_r_fl_static.block(3,0,1,3) = Vector3d(-step_length/4.0,step_width/2.0,0.0).transpose();
    I_r_fl_static.block(4,0,1,3) = Vector3d(step_length/4.0,step_width/2.0,0.0).transpose();
    I_r_fl_static.block(5,0,1,3) = Vector3d(-step_length/4.0,step_width/2.0,0.0).transpose();
    I_r_fl_static.block(6,0,1,3) = Vector3d(step_length/4.0,step_width/2.0,0.0).transpose();
    I_r_fl_static.block(7,0,1,3) = Vector3d(-step_length/4.0,step_width/2.0,0.0).transpose();
    I_r_fl_static.block(8,0,1,3) = Vector3d(step_length/4.0,step_width/2.0,0.0).transpose();
    I_r_fl_static.block(9,0,1,3) = Vector3d(-step_length/4.0,step_width/2.0,0.0).transpose();

    I_r_fr_static.block(0,0,1,3) = Vector3d(0.0,-step_width/2.0,0.0).transpose();
    I_r_fr_static.block(1,0,1,3) = Vector3d(step_length/4.0,-step_width/2.0,0.0).transpose();
    I_r_fr_static.block(2,0,1,3) = Vector3d(-step_length/4.0,-step_width/2.0,0.0).transpose();
    I_r_fr_static.block(3,0,1,3) = Vector3d(step_length/4.0,-step_width/2.0,0.0).transpose();
    I_r_fr_static.block(4,0,1,3) = Vector3d(-step_length/4.0,-step_width/2.0,0.0).transpose();
    I_r_fr_static.block(5,0,1,3) = Vector3d(step_length/4.0,-step_width/2.0,0.0).transpose();
    I_r_fr_static.block(6,0,1,3) = Vector3d(-step_length/4.0,-step_width/2.0,0.0).transpose();
    I_r_fr_static.block(7,0,1,3) = Vector3d(step_length/4.0,-step_width/2.0,0.0).transpose();
    I_r_fr_static.block(8,0,1,3) = Vector3d(-step_length/4.0,-step_width/2.0,0.0).transpose();
    I_r_fr_static.block(9,0,1,3) = Vector3d(step_length/4.0,-step_width/2.0,0.0).transpose();

    double t_curr;
    for(int i = 0; i < numFootstepsStatic - 1 + numConvergence; ++i){
        t_curr = 0.0;
        for(int j = 0; j < step_time_discrete; ++j){
            if(i==0){
                I_r_fl_traj_static.block(i*step_time_discrete+j,0,1,3) = Vector3d(0.0,step_width/2.0,0.0).transpose();
                I_r_fr_traj_static.block(i*step_time_discrete+j,0,1,3) = Vector3d(0.0,-step_width/2.0,0.0).transpose();
            }
            else if(i==1){
                I_r_fl_traj_static.block(i*step_time_discrete+j,0,1,3) = I_r_fl_static.block(i-1,0,1,3);
                double I_x_fr_des = 1.0/2.0*(I_r_fr_static(i,0)-I_r_fr_static(i-1,0))*(1.0-cos(t_curr/step_time*M_PI));
                double I_y_fr_des = 0.0;
                double I_z_fr_des = step_height * sin(t_curr/step_time*M_PI);
                I_r_fr_traj_static.block(i*step_time_discrete+j,0,1,3) = I_r_fr_static.block(i-1,0,1,3) + Vector3d(I_x_fr_des,I_y_fr_des,I_z_fr_des).transpose();
            }
            else if(i > numFootstepsStatic - 1){
                I_r_fl_traj_static.block(i*step_time_discrete+j,0,1,3) = I_r_fl_static.block(9,0,1,3);
                I_r_fr_traj_static.block(i*step_time_discrete+j,0,1,3) = I_r_fr_static.block(9,0,1,3);
            }
            else{
                if((i+1)%2==1){
                    double I_x_fl_des = 1.0/2.0*(I_r_fl_static(i,0)-I_r_fl_static(i-1,0))*(1.0-cos(t_curr/step_time*M_PI));
                    double I_y_fl_des = 0.0;
                    double I_z_fl_des = step_height * sin(t_curr/step_time*M_PI);
                    I_r_fl_traj_static.block(i*step_time_discrete+j,0,1,3) = I_r_fl_static.block(i-1,0,1,3) + Vector3d(I_x_fl_des,I_y_fl_des,I_z_fl_des).transpose();
                    I_r_fr_traj_static.block(i*step_time_discrete+j,0,1,3) = I_r_fr_static.block(i-1,0,1,3) - Vector3d(step_length/2.0,0,0).transpose()*j/step_time_discrete;
                }
                else{
                    I_r_fl_traj_static.block(i*step_time_discrete+j,0,1,3) = I_r_fl_static.block(i-1,0,1,3) - Vector3d(step_length/2.0,0,0).transpose()*j/step_time_discrete;
                    double I_x_fr_des = 1.0/2.0*(I_r_fr_static(i,0)-I_r_fr_static(i-1,0))*(1.0-cos(t_curr/step_time*M_PI));
                    double I_y_fr_des = 0.0;
                    double I_z_fr_des = step_height * sin(t_curr/step_time*M_PI);
                    I_r_fr_traj_static.block(i*step_time_discrete+j,0,1,3) = I_r_fr_static.block(i-1,0,1,3) + Vector3d(I_x_fr_des,I_y_fr_des,I_z_fr_des).transpose();
                }
            }
            t_curr = t_curr + dt_highlevel;
        }
    }
}

void HydraWalkingControl::StepPlanner(){
    footsteps(0,0) = 0;
    footsteps(0,1) = 0;
    footsteps(0,2) = 0;
    footsteps(0,3) = 0;//neither of foot

    footsteps(1,0) = 0;
    footsteps(1,1) = step_width/2.0;// cannot move
    footsteps(1,2) = 0;
    footsteps(1,3) = 1;

    for(int i=2;i<numFootsteps;i++){
        footsteps(i,0) = step_length*(double)(i-1); // x
    }
    //double rotMat[2][2];
    for(int i=2;i<numFootsteps;i++){
        footsteps(i,1) = cosMag * 0.5 * (1 - cos(2*M_PI*footsteps(i,0)/((numFootsteps-2)*step_length))); // y
        footsteps(i,2) = cosMag * 0.5 * 2 * sin(2*M_PI*footsteps(i,0)/((numFootsteps-2)*step_length)) * M_PI / ((numFootsteps-2)*step_length); // rotation
        footsteps(i,3) = 1; // l or r

        rotMat(0,0) = cos(footsteps(i,2));
        rotMat(0,1) = -sin(footsteps(i,2));
        rotMat(1,0) = sin(footsteps(i,2));
        rotMat(1,1) = cos(footsteps(i,2));

        if(i%2==0){
            footsteps(i,0) += rotMat(0,1) * (-step_width/2.0);
            footsteps(i,1) += rotMat(1,1) * (-step_width/2.0);
            footsteps(i,3) = -1;
        }
        else{
            footsteps(i,0) += rotMat(0,1) * (step_width/2.0);
            footsteps(i,1) += rotMat(1,1) * (step_width/2.0);
            footsteps(i,3) = 1;
        }
    }

    if(numFootsteps%2==0){
        footsteps(numFootsteps,0) = footsteps(numFootsteps-1,0) + rotMat(0,1) * (-step_width/2.0);
        footsteps(numFootsteps,1) = footsteps(numFootsteps-1,1) + rotMat(1,1) * (-step_width/2.0);
        footsteps(numFootsteps,2) = footsteps(numFootsteps-1,2);
        footsteps(numFootsteps,3) = 0; // neither foot
    }
    else{
        footsteps(numFootsteps,0) = footsteps(numFootsteps-1,0) + rotMat(0,1) * (step_width/2.0);
        footsteps(numFootsteps,1) = footsteps(numFootsteps-1,1) + rotMat(1,1) * (step_width/2.0);
        footsteps(numFootsteps,2) = footsteps(numFootsteps-1,2);
        footsteps(numFootsteps,3) = 0; // neither foot
    }
}

void HydraWalkingControl::CP_preview(){
    if(curr_footstep + previewHorizon - 1 <= numFootsteps){
        for(int i = 0; i < previewHorizon; ++i){
            I_r_ZMP_prev(i,0) = footsteps(curr_footstep+i,0);
            I_r_ZMP_prev(i,1) = footsteps(curr_footstep+i,1);
        }
    }
    else{
        int i = curr_footstep + previewHorizon - numFootsteps - 1;
        if(i > previewHorizon){
            i = previewHorizon;
        }
        int j = previewHorizon - i;
        while(i > 0){
            I_r_ZMP_prev(previewHorizon-i,0) = footsteps(numFootsteps,0);
            I_r_ZMP_prev(previewHorizon-i,1) = footsteps(numFootsteps,1);
            --i;
        }
        while(j > 0){
            I_r_ZMP_prev(j-1,0) = footsteps(curr_footstep+j-1,0);
            I_r_ZMP_prev(j-1,1) = footsteps(curr_footstep+j-1,1);
            --j;
        }
    }
    I_r_CP_ini(previewHorizon-1,0) = I_r_ZMP_prev(previewHorizon-1,0);
    I_r_CP_ini(previewHorizon-1,1) = I_r_ZMP_prev(previewHorizon-1,1);
    int i = previewHorizon - 1;
    while(i > 0){
        I_r_CP_eos(i-1,0) = I_r_CP_ini(i,0);
        I_r_CP_eos(i-1,1) = I_r_CP_ini(i,1);
        I_r_CP_ini(i-1,0) = I_r_ZMP_prev(i-1,0) + (I_r_CP_eos(i-1,0) - I_r_ZMP_prev(i-1,0))/exp(omega*step_time);
        I_r_CP_ini(i-1,1) = I_r_ZMP_prev(i-1,1) + (I_r_CP_eos(i-1,1) - I_r_ZMP_prev(i-1,1))/exp(omega*step_time);
        --i;
    }
    for(int t = 0; t < step_time_discrete; t++){
        I_r_CP_ref(t,0) = I_r_ZMP_prev(0,0)+exp(omega*(double)t*0.001)*(I_r_CP_ini(0,0) - I_r_ZMP_prev(0,0));
        I_r_CP_ref(t,1) = I_r_ZMP_prev(0,1)+exp(omega*(double)t*0.001)*(I_r_CP_ini(0,1) - I_r_ZMP_prev(0,1));
    }
    for(int t = 0; t < step_time_discrete; t++){
        I_r_CP_ref(t+step_time_discrete,0) = I_r_ZMP_prev(1,0)+exp(omega*(double)t*0.001)*(I_r_CP_ini(1,0) - I_r_ZMP_prev(1,0));
        I_r_CP_ref(t+step_time_discrete,1) = I_r_ZMP_prev(1,1)+exp(omega*(double)t*0.001)*(I_r_CP_ini(1,1) - I_r_ZMP_prev(1,1));
    }
}

void HydraWalkingControl::StepPlanner_prePlanned(){
    I_r_f_pre.block(0,0,1,3) = Eigen::Vector3d(0,-step_width/2.0,0).transpose();
    I_r_f_pre.block(1,0,1,3) = Eigen::Vector3d(0,step_width/2.0,0).transpose();
    I_r_f_pre.block(2,0,1,3) = Eigen::Vector3d(2.0*step_length/2.0,-step_width/2.0,0).transpose();
    I_r_f_pre.block(3,0,1,3) = I_r_f_pre.block(1,0,1,3) + Eigen::Vector3d(2.0*step_length,0,0).transpose();
    I_r_f_pre.block(4,0,1,3) = I_r_f_pre.block(2,0,1,3) + Eigen::Vector3d(2.0*step_length,0,0).transpose();
    I_r_f_pre.block(5,0,1,3) = I_r_f_pre.block(3,0,1,3) + Eigen::Vector3d(2.0*step_length,0,0).transpose();
    I_r_f_pre.block(6,0,1,3) = I_r_f_pre.block(4,0,1,3) + Eigen::Vector3d(2.0*step_length,0,0).transpose();
    I_r_f_pre.block(7,0,1,3) = I_r_f_pre.block(5,0,1,3) + Eigen::Vector3d(2.0*step_length,0,0).transpose();
    I_r_f_pre.block(8,0,1,3) = I_r_f_pre.block(6,0,1,3) + Eigen::Vector3d(2.0*step_length,0,0).transpose();
    I_r_f_pre.block(9,0,1,3) = I_r_f_pre.block(7,0,1,3) + Eigen::Vector3d(2.0*step_length/2.0,0,0).transpose();

    I_r_ZMP_pre.block(0,0,1,3) = Eigen::Vector3d::Zero().transpose();
    for(int i = 1; i < numFootsteps-1; ++i){
        I_r_ZMP_pre.block(i,0,1,3) = I_r_f_pre.block(i,0,1,3);
    }
    I_r_ZMP_pre.block(numFootsteps-1,0,1,3) = (I_r_f_pre.block(numFootsteps-2,0,1,3)+I_r_f_pre.block(numFootsteps-1,0,1,3))/2.0;

    I_r_CP_pre.block(numFootsteps-1,0,1,3) = I_r_ZMP_pre.block(numFootsteps-1,0,1,3);
    for(int i = 2; i < numFootsteps; ++i){
        I_r_CP_pre.block(numFootsteps-i,0,1,3) = I_r_ZMP_pre.block(numFootsteps-i,0,1,3) + (I_r_CP_pre.block(numFootsteps+1-i,0,1,3) - I_r_ZMP_pre.block(numFootsteps-i,0,1,3))/exp(omega*step_time);
    }

    double t_curr = 0.0;
    for(int i = 0; i < numFootsteps-1+numConvergence; ++i){
        t_curr = 0.0;
        for(int j = 0; j < step_time_discrete; ++j){
            if(i < numFootsteps-1){
                I_r_CP_des_pre.block(i*step_time_discrete+j,0,1,3) = exp(omega*t_curr) * I_r_CP_pre.block(i,0,1,3) + (1-exp(omega*t_curr))*I_r_ZMP_pre.block(i,0,1,3);
            }
            else{
                I_r_CP_des_pre.block(i*step_time_discrete+j,0,1,3) = I_r_CP_des_pre.block((numFootsteps-1)*step_time_discrete-1,0,1,3);
            }
            t_curr = t_curr + 0.001;
        }
    }

    I_r_COM_des_pre.block(0,0,1,3) = Eigen::Vector3d(0,0,0).transpose();
    for(int i = 1; i < (numFootsteps-1)*step_time_discrete; ++i){
        I_r_COM_des_pre.block(i,0,1,3) = I_r_COM_des_pre.block(i-1,0,1,3) + 0.001 * I_dr_COM_des_pre.block(i-1,0,1,3);
        I_dr_COM_des_pre.block(i,0,1,3) = -omega*(I_r_COM_des_pre.block(i,0,1,3) - I_r_CP_des_pre.block(i,0,1,3));
    }
    for(int i = (numFootsteps-1)*step_time_discrete; i < (numFootsteps-1+numConvergence)*step_time_discrete; ++i){
        I_r_COM_des_pre.block(i,0,1,3) = I_r_COM_des_pre.block(i-1,0,1,3) + 0.001 * I_dr_COM_des_pre.block(i-1,0,1,3);
        I_dr_COM_des_pre.block(i,0,1,3) = -omega*(I_r_COM_des_pre.block(i,0,1,3) - I_r_CP_des_pre.block((numFootsteps-1)*step_time_discrete-1,0,1,3));
    }
    for(int i = 0; i < (numFootsteps-1+numConvergence)*step_time_discrete; ++i){
        I_r_COM_des_pre.block(i,0,1,3) = I_r_COM_des_pre.block(i,0,1,3) + Eigen::Vector3d(0,0,com_height).transpose();
    }


    // Preplan swing-foot trajectories
    /*double I_x_f_traj = 0.0;
    double I_y_f_traj = 0.0;
    double I_z_f_traj = 0.0;
    for(int i = 0; i < numFootsteps; ++i){
        t_curr = 0.0;
        for(int j = 0; j < step_time_discrete; ++j){
            if(i == 0){
                I_x_f_traj = 0.0;
                I_y_f_traj = step_width/2;
                I_z_f_traj = 0.0;
            }
            else if(i > numFootsteps - 2){
                I_x_f_traj = I_r_f_pre(i,1);
                I_y_f_traj = I_r_f_pre(i,2);
                I_z_f_traj = I_r_f_pre(i,3);
            }
            else{
                I_x_f_traj = I_r_f_pre(i,1) + 1/2*step_length*(1-cos(t_curr/step_time*M_PI));
                I_y_f_traj = I_r_f_pre(i,2);
                I_z_f_traj = I_r_f_pre(i,3) + step_height * sin(t_curr/step_time*M_PI);
            }
            I_r_f_traj.block(i*step_time_discrete+j,0,1,3) = Eigen::Vector3d(I_x_f_traj,I_y_f_traj,I_z_f_traj).transpose();
            t_curr = t_curr + 0.001;
        }
    }*/

    double I_x_fl_traj = 0.0;
    double I_y_fl_traj = 0.0;
    double I_z_fl_traj = 0.0;
    double I_x_fr_traj = 0.0;
    double I_y_fr_traj = 0.0;
    double I_z_fr_traj = 0.0;
    for(int i = 0; i < numFootsteps + numConvergence; ++i){
        t_curr = 0.0;
        for(int j = 0; j < step_time_discrete; ++j){
            if(i == 0){
                I_x_fl_traj = 0.0;
                I_y_fl_traj = step_width/2;
                I_z_fl_traj = 0.0;

                I_x_fr_traj = 0.0;
                I_y_fr_traj = -step_width/2;
                I_z_fr_traj = 0.0;
            }
            else if(i > numFootsteps - 2){
                I_x_fl_traj = I_r_f_pre(9,1);
                I_y_fl_traj = I_r_f_pre(9,2);
                I_z_fl_traj = I_r_f_pre(9,3);

                I_x_fr_traj = I_r_f_pre(8,1);
                I_y_fr_traj = I_r_f_pre(8,2);
                I_z_fr_traj = I_r_f_pre(8,3);
            }
            else{
                if((i+1%2) == 1){
                    I_x_fl_traj = I_r_f_pre(i,1) + 1/2*(I_r_f_pre(i+2,0)-I_r_f_pre(i,0))*(1-cos(t_curr/step_time*M_PI));
                    I_y_fl_traj = I_r_f_pre(i,2);
                    I_z_fl_traj = I_r_f_pre(i,3) + step_height * sin(t_curr/step_time*M_PI);

                    I_x_fr_traj = I_r_f_pre(i+1,1);
                    I_y_fr_traj = I_r_f_pre(i+1,2);
                    I_z_fr_traj = I_r_f_pre(i+1,3);
                }
                else{
                    I_x_fl_traj = I_r_f_pre(i+1,1);
                    I_y_fl_traj = I_r_f_pre(i+1,2);
                    I_z_fl_traj = I_r_f_pre(i+1,3);

                    I_x_fr_traj = I_r_f_pre(i,1) + 1/2*(I_r_f_pre(i+2,0)-I_r_f_pre(i,0))*(1-cos(t_curr/step_time*M_PI));
                    I_y_fr_traj = I_r_f_pre(i,2);
                    I_z_fr_traj = I_r_f_pre(i,3) + step_height * sin(t_curr/step_time*M_PI);
                }

            }
            I_r_fl_traj.block(i*step_time_discrete+j,0,1,3) = Eigen::Vector3d(I_x_fl_traj,I_y_fl_traj,I_z_fl_traj).transpose();
            I_r_fr_traj.block(i*step_time_discrete+j,0,1,3) = Eigen::Vector3d(I_x_fr_traj,I_y_fr_traj,I_z_fr_traj).transpose();
            t_curr = t_curr + 0.001;
        }
    }

}

// ==================== OLD FUNCTION FOR EE JACOBIANS ====================

void HydraWalkingControl::SetJacLF(){
    // Set up matrix
    MatrixXd J_LF_kin;
    J_LF_kin.resize(6,58);
    J_LF_kin.setZero();
    J_LF.setZero();

    int joint_Foot = 12; // = Ankle Pitch Left
    int joint_Base = 0;
    int joint_Knee = 10;
    int joint_Hip = 7;
    Vector3d p_0_BaseToAnkle = joints[joint_Foot].p_0_j - joints[joint_Base].p_0_j;
    Vector3d p_0_BaseToi;
    Vector3d p_0_iToAnkle;
    Vector3d a_0_ij;
    int joint_i = joint_Foot;

    while(joint_i != joint_Hip-1){ // up until 6 = left hip yaw
        /*if(joint_i == joint_Hip){
            a_0_ij = joints[joint_Base].R_0_j * Vector3d::UnitZ(); //joints[joint_i].a_i_ij;
        }
        else{
            a_0_ij = joints[joint_i-1].R_0_j * Vector3d::UnitZ(); //joints[joint_i].a_i_ij;
        }*/
        a_0_ij = joints[joint_i].R_0_j * Vector3d::UnitZ();

        //printf("a_0_ij[%d] = [%f,%f,%f]\n", joint_i,a_0_ij(0),a_0_ij(1),a_0_ij(2));
        //printf("a_i_ij[%d] = [%f,%f,%f]\n", joint_i,joints[joint_i].a_i_ij(0),joints[joint_i].a_i_ij(1),joints[joint_i].a_i_ij(2));
        //printf("R_0_j[%d] = [%f,%f,%f]\n", joint_i,joints[joint_i-1].R_0_j(0,0),joints[joint_i-1].R_0_j(1,1),joints[joint_i-1].R_0_j(2,2));

        p_0_BaseToi = joints[joint_i].p_0_j - joints[joint_Base].p_0_j;
        p_0_iToAnkle = p_0_BaseToAnkle - p_0_BaseToi;

        if(joint_i==10){
            //printf("p_0_iToAnkle = [%f,%f,%f]\n", p_0_iToAnkle(0),p_0_iToAnkle(1),p_0_iToAnkle(2));
        }

        // Get linear jacobian
        J_LF_kin.block(0,joint_i,3,1) = a_0_ij.cross(p_0_iToAnkle);

        // Get rotational jacobian
        J_LF_kin.block(3,joint_i,3,1) = a_0_ij;

        /*if(t_highlevel > 5.0 && t_highlevel < 6.0){
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "p_0_iToAnkle[%d] = [%f \n ", joint_i, p_0_iToAnkle(0));
            fprintf(fp_debugJac, "                    %f \n ", p_0_iToAnkle(1));
            fprintf(fp_debugJac, "                    %f] \n ", p_0_iToAnkle(2));
            fprintf(fp_debugJac, "\n");
            fprintf(fp_debugJac, "a_0_ij[%d] = [%f \n ", joint_i, a_0_ij(0));
            fprintf(fp_debugJac, "              %f \n ", a_0_ij(1));
            fprintf(fp_debugJac, "              %f] \n ", a_0_ij(2));
            fprintf(fp_debugJac, "\n");
        }*/

        // move up kinematic chain
        joint_i = joint_i-1;
        //printf("joints[0].a_i_ij = [%f,%f,%f]\n", joints[0].R_0_j(0,0),joints[0].R_0_j(1,1),joints[0].R_0_j(2,2));
    }
    for(int i = 0; i < 6; ++i){
        J_LF.block(0,i,3,1) = J_LF_kin.block(0,i+6+1,3,1);
        J_LF.block(3,i,3,1) = J_LF_kin.block(3,i+6+1,3,1);
    }

    // PRINT TO FILE
    /*if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "J = [%f, %f, %f, %f, %f, %f \n ", J_LF(0,0), J_LF(0,1), J_LF(0,2), J_LF(0,3), J_LF(0,4), J_LF(0,5));
        fprintf(fp_debugJac, "     %f, %f, %f, %f, %f, %f \n ", J_LF(1,0), J_LF(1,1), J_LF(1,2), J_LF(1,3), J_LF(1,4), J_LF(1,5));
        fprintf(fp_debugJac, "     %f, %f, %f, %f, %f, %f] \n ", J_LF(2,0), J_LF(2,1), J_LF(2,2), J_LF(2,3), J_LF(2,4), J_LF(2,5));
        fprintf(fp_debugJac, "     %f, %f, %f, %f, %f, %f] \n ", J_LF(3,0), J_LF(3,1), J_LF(3,2), J_LF(3,3), J_LF(3,4), J_LF(3,5));
        fprintf(fp_debugJac, "     %f, %f, %f, %f, %f, %f] \n ", J_LF(4,0), J_LF(4,1), J_LF(4,2), J_LF(4,3), J_LF(4,4), J_LF(4,5));
        fprintf(fp_debugJac, "     %f, %f, %f, %f, %f, %f] \n ", J_LF(5,0), J_LF(5,1), J_LF(5,2), J_LF(5,3), J_LF(5,4), J_LF(5,5));
        fprintf(fp_debugJac, "\n");
    }*/
}

void HydraWalkingControl::SetJacLH(){
    // Set up matrix
    MatrixXd J_LH_kin;
    J_LH_kin.resize(6,58);
    J_LH_kin.setZero();
    J_LH.setZero();

    int joint_Hand = 31; // = Wrist Pitch Left
    int joint_Base = 0;
    int joint_Torso = 14;
    Vector3d p_0_BaseToWrist = joints[joint_Hand].p_0_j - joints[joint_Base].p_0_j;
    Vector3d p_0_BaseToi;
    Vector3d p_0_iToWrist;
    Vector3d a_0_ij;
    int joint_i = joint_Hand;

    while(joint_i != joint_Torso-1){ // up until 6 = left hip yaw
        a_0_ij = joints[joint_i].R_0_j * Vector3d::UnitZ();

        p_0_BaseToi = joints[joint_i].p_0_j - joints[joint_Base].p_0_j;
        p_0_iToWrist = p_0_BaseToWrist - p_0_BaseToi;

        // Get linear jacobian
        J_LH_kin.block(0,joint_i,3,1) = a_0_ij.cross(p_0_iToWrist);

        // Get rotational jacobian
        J_LH_kin.block(3,joint_i,3,1) = a_0_ij;

        // move up kinematic chain
        joint_i = joint_i-1;
    }
    for(int i = 0; i < 8; ++i){
        J_LH.block(0,i,3,1) = J_LH_kin.block(0,i+23+1,3,1);
        J_LH.block(3,i,3,1) = J_LH_kin.block(3,i+23+1,3,1);
    }

    // PRINT TO FILE
    /*if(t_highlevel > t_debug_start && t_highlevel < t_debug_end){
        fprintf(fp_debugJac, "\n");
        fprintf(fp_debugJac, "J_LH = [%f, %f, %f, %f, %f, %f, %f, %f] \n ", J_LH(0,0), J_LH(0,1), J_LH(0,2), J_LH(0,3), J_LH(0,4), J_LH(0,5), J_LH(0,6), J_LH(0,7));
        fprintf(fp_debugJac, "       [%f, %f, %f, %f, %f, %f, %f, %f] \n ", J_LH(1,0), J_LH(1,1), J_LH(1,2), J_LH(1,3), J_LH(1,4), J_LH(1,5), J_LH(1,6), J_LH(1,7));
        fprintf(fp_debugJac, "       [%f, %f, %f, %f, %f, %f, %f, %f] \n ", J_LH(2,0), J_LH(2,1), J_LH(2,2), J_LH(2,3), J_LH(2,4), J_LH(2,5), J_LH(2,6), J_LH(2,7));
        fprintf(fp_debugJac, "       [%f, %f, %f, %f, %f, %f, %f, %f] \n ", J_LH(3,0), J_LH(3,1), J_LH(3,2), J_LH(3,3), J_LH(3,4), J_LH(3,5), J_LH(3,6), J_LH(3,7));
        fprintf(fp_debugJac, "       [%f, %f, %f, %f, %f, %f, %f, %f] \n ", J_LH(4,0), J_LH(4,1), J_LH(4,2), J_LH(4,3), J_LH(4,4), J_LH(4,5), J_LH(4,6), J_LH(4,7));
        fprintf(fp_debugJac, "       [%f, %f, %f, %f, %f, %f, %f, %f] \n ", J_LH(5,0), J_LH(5,1), J_LH(5,2), J_LH(5,3), J_LH(5,4), J_LH(5,5), J_LH(5,6), J_LH(5,7));
        fprintf(fp_debugJac, "\n");
    }*/
}



// =================================== BACKUP ===================================
/*void HydraWalkingControl::SetJacLeftAnkleEE(Vector3d EE_r_EEtoPoint, MatrixXd &J_LF, MatrixXd &I_J_LF){
    // Erase for safety
    J_LF.setZero();

    // Temp. matrix (full DOF forward kinematics)
    MatrixXd J_LF_kin;
    J_LF_kin.resize(6,58);
    J_LF_kin.setZero();

    // Hard-coded joint indices
    int joint_Base = 0;  // Waist Base
    int joint_Hip = 7;   // Hip Yaw Left
    int joint_Foot = 12; // Ankle Pitch Left

    // Allocate position vectors
    Vector3d p_0_BaseToPoint = joints[joint_Foot].p_0_j + joints[joint_Foot].R_0_j*EE_r_EEtoPoint - joints[joint_Base].p_0_j;
    Vector3d p_0_BaseToJoint;
    Vector3d p_0_JointToPoint;

    // Allocate vector for joint rotation in world frame
    Vector3d z_0_Joint;

    // Start iteration from ankle joint until we reach waist base (better via .parent w/ new .wrl model)
    int joint_i = joint_Foot;
    while(joint_i != joint_Hip-1){ // up until 7 = Left Hip Yaw (inclusive)
        z_0_Joint = joints[joint_i].R_0_j * Vector3d::UnitZ();

        p_0_BaseToJoint = joints[joint_i].p_0_j - joints[joint_Base].p_0_j;
        p_0_JointToPoint = p_0_BaseToPoint - p_0_BaseToJoint;

        // Get linear jacobian
        J_LF_kin.block(0,joint_i,3,1) = z_0_Joint.cross(p_0_JointToPoint);

        // Get rotational jacobian
        J_LF_kin.block(3,joint_i,3,1) = z_0_Joint;

        // move up kinematic chain
        joint_i = joint_i-1;
    }
    // Write from full jacobian to task specific jacobian (6x6)
    for(int i = 0; i < 6; ++i){
        J_LF.block(0,i,3,1) = J_LF_kin.block(0,i+6+1,3,1);
        J_LF.block(3,i,3,1) = J_LF_kin.block(3,i+6+1,3,1);
    }
    I_J_LF = J_LF_kin;
}

void HydraWalkingControl::SetJacRightAnkleEE(Vector3d EE_r_EEtoPoint, MatrixXd &J_RF, MatrixXd &I_J_RF){
    // Erase for safety
    J_RF.setZero();

    // Temp. matrix (full DOF forward kinematics)
    MatrixXd J_RF_kin;
    J_RF_kin.resize(6,58);
    J_RF_kin.setZero();

    // Hard-coded joint indices
    int joint_Base = 0;  // Waist Base
    int joint_Hip = 1;   // Hip Yaw Right
    int joint_Foot = 6; // Ankle Pitch Right

    // Allocate position vectors
    Vector3d p_0_BaseToPoint = joints[joint_Foot].p_0_j + joints[joint_Foot].R_0_j*EE_r_EEtoPoint - joints[joint_Base].p_0_j;
    Vector3d p_0_BaseToJoint;
    Vector3d p_0_JointToPoint;

    // Allocate vector for joint rotation in world frame
    Vector3d z_0_Joint;

    // Start iteration from ankle joint until we reach waist base (better via .parent w/ new .wrl model)
    int joint_i = joint_Foot;
    while(joint_i != joint_Hip-1){ // up until 1 = Right Hip Yaw (inclusive)
        z_0_Joint = joints[joint_i].R_0_j * Vector3d::UnitZ();

        p_0_BaseToJoint = joints[joint_i].p_0_j - joints[joint_Base].p_0_j;
        p_0_JointToPoint = p_0_BaseToPoint - p_0_BaseToJoint;

        // Get linear jacobian
        J_RF_kin.block(0,joint_i,3,1) = z_0_Joint.cross(p_0_JointToPoint);

        // Get rotational jacobian
        J_RF_kin.block(3,joint_i,3,1) = z_0_Joint;

        // move up kinematic chain
        joint_i = joint_i-1;
    }
    // Write from full jacobian to task specific jacobian (6x6)
    for(int i = 0; i < 6; ++i){
        J_RF.block(0,i,3,1) = J_RF_kin.block(0,i+1,3,1);
        J_RF.block(3,i,3,1) = J_RF_kin.block(3,i+1,3,1);
    }
    I_J_RF = J_RF_kin;
}

// EE_r_EEtoPoint =  point on wrist in wrist frame (EE = Wrist Pitch)
void HydraWalkingControl::SetJacLeftWristEE(Vector3d EE_r_EEtoPoint, MatrixXd &J_LH){
    // Erase for safety
    J_LH.setZero();

    // Temp. matrix (full DOF forward kinematics)
    MatrixXd J_LH_kin;
    J_LH_kin.resize(6,58);
    J_LH_kin.setZero();

    // Hard-coded joint indices
    int joint_Base = 0;   // Waist Base
    int joint_Torso = 24; // Waist Roll
    int joint_Hand = 31;  // Wrist Pitch Left

    // Allocate position vectors
    Vector3d p_0_BaseToPoint = joints[joint_Hand].p_0_j + joints[joint_Hand].R_0_j*EE_r_EEtoPoint - joints[joint_Base].p_0_j;
    Vector3d p_0_BaseToJoint;
    Vector3d p_0_JointToPoint;

    // Allocate vector for joint rotation in world frame
    Vector3d z_0_Joint;

    // Start iteration from wrist joint until we reach torso (better via .parent w/ new .wrl model)
    int joint_i = joint_Hand;
    while(joint_i != joint_Torso-1){ // up until 24 = Scaplae Yaw Left (inclusive)
        z_0_Joint = joints[joint_i].R_0_j * Vector3d::UnitZ();

        p_0_BaseToJoint = joints[joint_i].p_0_j - joints[joint_Base].p_0_j;
        p_0_JointToPoint = p_0_BaseToPoint - p_0_BaseToJoint;

        // Get linear jacobian
        J_LH_kin.block(0,joint_i,3,1) = z_0_Joint.cross(p_0_JointToPoint);

        // Get rotational jacobian
        J_LH_kin.block(3,joint_i,3,1) = z_0_Joint;

        // move up kinematic chain
        joint_i = joint_i-1;
    }
    // Write from full jacobian to task specific jacobian (6x8)
    for(int i = 0; i < 8; ++i){
        J_LH.block(0,i,3,1) = J_LH_kin.block(0,i+23+1,3,1);
        J_LH.block(3,i,3,1) = J_LH_kin.block(3,i+23+1,3,1);
    }
}



// EE_r_EEtoPoint =  point on wrist in wrist frame (EE = Wrist Pitch)
void HydraWalkingControl::SetJacRightWristEE(Vector3d EE_r_EEtoPoint, MatrixXd &J_RH){
    // Erase for safety
    J_RH.setZero();

    // Temp. matrix (full DOF forward kinematics)
    MatrixXd J_RH_kin;
    J_RH_kin.resize(6,58);
    J_RH_kin.setZero();

    // Hard-coded joint indices
    int joint_Base = 0;   // Waist Base
    int joint_Torso = 16; // Waist Roll
    int joint_Hand = 23;  // Wrist Pitch Right

    // Allocate position vectors
    Vector3d p_0_BaseToPoint = joints[joint_Hand].p_0_j + joints[joint_Hand].R_0_j*EE_r_EEtoPoint - joints[joint_Base].p_0_j;
    Vector3d p_0_BaseToJoint;
    Vector3d p_0_JointToPoint;

    // Allocate vector for joint rotation in world frame
    Vector3d z_0_Joint;

    // Start iteration from wrist joint until we reach torso (better via .parent w/ new .wrl model)
    int joint_i = joint_Hand;
    while(joint_i != joint_Torso-1){ // up until 16 = Scaplae Yaw Right (inclusive)
        z_0_Joint = joints[joint_i].R_0_j * Vector3d::UnitZ();

        p_0_BaseToJoint = joints[joint_i].p_0_j - joints[joint_Base].p_0_j;
        p_0_JointToPoint = p_0_BaseToPoint - p_0_BaseToJoint;

        // Get linear jacobian
        J_RH_kin.block(0,joint_i,3,1) = z_0_Joint.cross(p_0_JointToPoint);

        // Get rotational jacobian
        J_RH_kin.block(3,joint_i,3,1) = z_0_Joint;

        // move up kinematic chain
        joint_i = joint_i-1;
    }
    // Write from full jacobian to task specific jacobian (6x8)
    for(int i = 0; i < 8; ++i){
        J_RH.block(0,i,3,1) = J_RH_kin.block(0,i+15+1,3,1);
        J_RH.block(3,i,3,1) = J_RH_kin.block(3,i+15+1,3,1);
    }
}*/


// Taken from "GitHub" user Robert Eisele ("infusion") ; slightly modified
Eigen::MatrixXd HydraWalkingControl::readCSV(std::string file, int rows, int cols){

    std::ifstream in;
    in.open(file.c_str());

    std::string line;

    int row = 0;
    int col = 0;

    Eigen::MatrixXd res = Eigen::MatrixXd(rows, cols);

    if (in.is_open()) {

        while (std::getline(in, line)) {

          char *ptr = (char *) line.c_str();
          int len = line.length();

          col = 0;

          char *start = ptr;
          for (int i = 0; i < len; i++) {

            if (ptr[i] == ',') {
              res(row, col++) = atof(start);
              start = ptr + i + 1;
            }
          }
          res(row, col) = atof(start);

          row++;
        }

        in.close();
    }
    else{
        fprintf(fp_debugJac, "FAILED\n");
    }

    fprintf(fp_debugJac, "\n");
    fprintf(fp_debugJac, "res = \n");
    for(int i = 0; i < rows; ++i){
        fprintf(fp_debugJac, "[");
        for(int j = 0; j < cols; ++j){
            fprintf(fp_debugJac, "%f, ", res(i,j));
        }
        fprintf(fp_debugJac, "]\n");
    }

    return res;
}

void HydraWalkingControl::ConvertZKCtoHYDRAJointLogMatrix(MatrixXd &jointMatZKC, MatrixXd &jointMatHYDRA){
    for(int i = 0; i < jointMatZKC.rows(); ++i){
        // RIGHT LEG 6 DOF
        jointMatHYDRA.block(i,0,1,6) = jointMatZKC.block(i,0,1,6);

        // LEFT LEG 6 DOF
        jointMatHYDRA.block(i,6,1,6) = jointMatZKC.block(i,6,1,6);


        // WAIST PITCH & ROLL 2 DOF
        jointMatHYDRA.block(i,12,1,1) = jointMatZKC.block(i,12,1,1);
        jointMatHYDRA.block(i,13,1,1) = jointMatZKC.block(i,13,1,1);

        // NECK PITCH
        jointMatHYDRA.block(i,14,1,1) = jointMatZKC.block(i,14,1,1);


        // RIGHT ARM 8 DOF
        jointMatHYDRA.block(i,15,1,8) = jointMatZKC.block(i,16,1,8);

        // LEFT ARM 8 DOF
        jointMatHYDRA.block(i,23,1,8) = jointMatZKC.block(i,24,1,8);

        // RIGHT HAND THUMB 2 DOF
        jointMatHYDRA.block(i,31,1,2) = jointMatZKC.block(i,32,1,2);

        // RIGHT HAND INDEX 1 DOF
        jointMatHYDRA.block(i,33,1,1) = jointMatZKC.block(i,36,1,1);

        // RIGHT HAND MIDDLE 1 DOF
        jointMatHYDRA.block(i,34,1,1) = jointMatZKC.block(i,39,1,1);

        // RIGHT HAND RING 1 DOF
        jointMatHYDRA.block(i,35,1,1) = jointMatZKC.block(i,42,1,1);

        // LEFT HAND THUMB 2 DOF
        jointMatHYDRA.block(i,36,1,2) = jointMatZKC.block(i,45,1,2);

        // LEFT HAND INDEX 1 DOF
        jointMatHYDRA.block(i,38,1,1) = jointMatZKC.block(i,49,1,1);

        // LEFT HAND MIDDLE 1 DOF
        jointMatHYDRA.block(i,39,1,1) = jointMatZKC.block(i,52,1,1);

        // LEFT HAND RING 1 DOF
        jointMatHYDRA.block(i,40,1,1) = jointMatZKC.block(i,55,1,1);
    }
}
