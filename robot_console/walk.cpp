#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

//#include "CalcParallelJoint.h"
#include "robot_hydra_id.h"
#include "hydra_types.h"
#include "walk.h"

#include "cthreaddata.h"

using namespace Eigen;

walk::walk(){
    //need to change
//    flag_first_step = true;
//    x_cp_eos.x() = -0.125;
//    x_cp_eos.y() = 0;
//    x_cp_0.x() = -0.1;
//    x_cp_0.y() = 0.125;
    r_leg_move_flag = true;
    l_leg_move_flag = false;
    //need to change
    x_zmp_des.setZero();
    x_zmp_act.setZero();
    x_cp_act.setZero();
//    P_before.setZero();
//    L_before.setZero();
    p_com_act.setZero();
    dp_com_act.setZero();

    x_com_des.setZero();
    dx_com_des.setZero();

    R_body_ref.setZero();
    R_body_ref(0,2) = 1;
    R_body_ref(1,1) = -1;
    R_body_ref(2,0) = 1;
/*    R_body_ref(0,0) = 0.537263;//reqline to right front
    R_body_ref(0,1) = -0.222975;
    R_body_ref(0,2) = 0.813407;
    R_body_ref(1,0) = -0.38332;
    R_body_ref(1,1) = -0.923616;
    R_body_ref(1,2) = -1.30636e-07;
    R_body_ref(2,0) = 0.751275;
    R_body_ref(2,1) = -0.311795;
    R_body_ref(2,2) = -0.581696;*/
    q_ref[2] = -M_PI/12;//r_hip_pitch
    q_ref[4] = M_PI/6;//r_knee
    q_ref[6] = -M_PI/12;//r_ankle_pitch
    q_ref[8] = -M_PI/12;//l_hip_pitch
    q_ref[10] = M_PI/6;//l_knee
    q_ref[12] = -M_PI/12;//l_ankle_pitch

    fixed_joint_num = 12;
    fixed_joint_pos.x() = 0;
    fixed_joint_pos.y() = 0.125;
//    fixed_joint_num = 6;
//    fixed_joint_pos.x() = 0;
//    fixed_joint_pos.y() = -0.125;
    fixed_joint_pos.z() = leg_height;
    fixed_joint_R.setZero();
    fixed_joint_R(0,0) = 1;
    fixed_joint_R(1,2) = 1;
    fixed_joint_R(2,1) = -1;
    STEPset();
    ZMPset();
    CPset();

    t = 0;
    t_plot = 0;
    t_before = 0;
    walk_log.open("log_walk", std::ios::out);//for debug
    zmp_plot.open("plot_zmp.csv", std::ios::out);
    zmp_plot << "t " << "zmp_des.x " << "zmp_des.y " << "zmp_act.x " << "zmp_act.y" << std::endl;
    com_plot.open("plot_com.csv", std::ios::out);
    com_plot << "t " << "com_pos_des.x " << "com_pos_des.y " << "com_pos_act.x " << "com_pos_act.y" << "com_vel_des.x " << "com_vel_des.y " << "com_vel_act.x " << "com_vel_act.y" << std::endl;
    cp_plot.open("plot_cp.csv", std::ios::out);
    cp_plot << "t " << "cp_des.x " << "cp_des.y " << "cp_act.x " << "cp_act.y" << std::endl;
}

walk::~walk(){

}

void walk::walkingcontroll(){
//    auto start = std::chrono::system_clock::now();

    ForwardKinematics_from_fixed_joint(fixed_joint_num, fixed_joint_pos, fixed_joint_R);

    COMcalculate();
    ZMPcalculate();
    CPcalculate();

    ZMPcontrol();//x_zmp_des = update;
//    ZMPprojection();
    COMgenerate();

    OperationalSpace();
    TaskPriority();

//    auto end = std::chrono::system_clock::now();
//    auto dur = end - start;
//    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
//    walk_log << "time step = " << msec << "msec" << std::endl;

    if(t >= dT_step){
        t = 0;
        r_leg_move_flag = !r_leg_move_flag;
        l_leg_move_flag = !l_leg_move_flag;
        if(r_leg_move_flag){
            fixed_joint_num = 12;
        }else{
            fixed_joint_num = 6;
        }
        fixed_joint_pos = jnt[fixed_joint_num].p_W_j;
        fixed_joint_R = jnt[fixed_joint_num].R_W_j;
        step_count++;
        walk_log << std::endl << std::endl;
    }

}

void walk::writeReference_pos(double q[]){
    for(int i=0; i<HYDRA_JNT_MAX; i++){
        q[i] = q_ref(i+1);
    }
}

void walk::STEPset(){
    STEP[0].x() = 0;
    STEP[0].y() = 0.125;
    STEP[1].x() = 0;
    STEP[1].y() = -0.125;
    STEP[2].x() = 0.2;
    STEP[2].y() = 0.125;
    STEP[3].x() = 0.4;
    STEP[3].y() = -0.125;
    STEP[4].x() = 0.6;
    STEP[4].y() = 0.125;
    STEP[5].x() = 0.6;
    STEP[5].y() = -0.125;
}

void walk::ZMPset(){
    ZMP[0].x() = 0;
    ZMP[0].y() = 0;
    ZMP[1].x() = 0;
    ZMP[1].y() = -0.125;
    ZMP[2].x() = 0.2;
    ZMP[2].y() = 0.125;
    ZMP[3].x() = 0.4;
    ZMP[3].y() = -0.125;
    ZMP[4].x() = 0.6;
    ZMP[4].y() = 0.125;
    ZMP[5].x() = 0.6;
    ZMP[5].y() = 0;
}

void walk::CPset(){//need to change
    CP_ini[5] = ZMP[5];
    int i = 4;
    while(i > -1){
        CP_eos[i] = CP_ini[i+1];
        CP_ini[i] = ZMP[i] + (CP_eos[i] - ZMP[i])*exp(-kappa*dT_step);
        i--;
    }
}

void walk::ZMPprojection(){
    int joint_num = 6;
    if(r_leg_move_flag){
        joint_num = 12;//L_ANKLE_YAW
    }

    Vector3d x_zmp_j;
    Vector2d p_0_j;
    Matrix3d A;

    p_0_j.x() = jnt[joint_num].p_0_j.x();
    p_0_j.y() = jnt[joint_num].p_0_j.y();

    A = jnt[joint_num].R_0_j.transpose();
    x_zmp_j = A.block(0,0,3,2)*(x_zmp_des - p_0_j);

    if(x_zmp_j.x() > leg_width){
        x_zmp_j.x() = leg_width;
    }else if(x_zmp_j.x() < -leg_width){
        x_zmp_j.x() = -leg_width;
    }
    if(x_zmp_j.y() > leg_front){
        x_zmp_j.y() = leg_front;
    }else if(x_zmp_j.y() < leg_back){
        x_zmp_j.y() = leg_back;
    }
    x_zmp_j = jnt[joint_num].p_0_j + jnt[joint_num].R_0_j*x_zmp_j;
    x_zmp_des.x() = x_zmp_j.x();
    x_zmp_des.y() = x_zmp_j.y();
}

Vector3d walk::moving_leg_path(double t, Vector2d x_fixed_leg_bef, Vector2d x_fixed_leg_next){
    Vector3d path;
    double temp_value = (t/dT_step - 1/2/M_PI*sin(2*M_PI*t/dT_step));
    path.x() = x_fixed_leg_bef.x() + (x_fixed_leg_next.x() - x_fixed_leg_bef.x())*temp_value;
    path.y() = x_fixed_leg_bef.y() + (x_fixed_leg_next.y() - x_fixed_leg_bef.y())*temp_value;
    path.z() = step_height*(1 - cos(2*M_PI*t/dT_step)) + leg_height;
    return path;
}

Vector3d walk::dmoving_leg_path(double t, Vector2d x_fixed_leg_bef, Vector2d x_fixed_leg_next){
    Vector3d path;
    double temp_value = (1/dT_step - 1/dT_step*cos(2*M_PI*t/dT_step));
    path.x() = (x_fixed_leg_next.x() - x_fixed_leg_bef.x())*temp_value;
    path.y() = (x_fixed_leg_next.y() - x_fixed_leg_bef.y())*temp_value;
    path.z() = step_height*2*M_PI/dT_step*sin(2*M_PI*t/dT_step);
    return path;
}

/*MatrixXd walk::COM_Jacobian(){
    Matrix<double, 3, HYDRA_JNT_MAX+6> J_COM_joint[HYDRA_JNT_MAX+1];
    for(int i=0; i<HYDRA_JNT_MAX+1; i++){
        J_COM_joint[i].setZero();
    }
    Matrix<double, 3, HYDRA_JNT_MAX+6> J_COM;
    J_COM.setZero();
    Vector3d a_W_ij;
    Vector3d p_W_ij;
    int parent_i;

    for(int joint=1; joint<HYDRA_JNT_MAX+1; joint++){//HYDRA_JNT_MAX
        parent_i = joint;
        while(parent_i != 0){
            a_W_ij = jnt[parent_i].parent[0]->R_W_j * jnt[parent_i].R_i_ij_ini*Vector3d::UnitZ();
            p_W_ij = jnt[joint].s_W_j - jnt[parent_i].p_W_j;
            J_COM_joint[joint].col(parent_i + 5) = a_W_ij.cross(p_W_ij);
            parent_i = jnt[parent_i].parent_id;
        }
    }
    for(int i=1; i<HYDRA_JNT_MAX; i++){
        J_COM += jnt[i].m*J_COM_joint[i];
    }

    J_COM.block(0,0,3,1) = jnt[0].m*Vector3d::UnitX();
    J_COM.block(0,1,3,1) = jnt[0].m*Vector3d::UnitY();
    J_COM.block(0,2,3,1) = jnt[0].m*Vector3d::UnitZ();
    J_COM.block(0,3,3,1) = jnt[0].m*Vector3d::UnitX().cross(jnt[0].s_W_j - jnt[0].p_W_j);
    J_COM.block(0,4,3,1) = jnt[0].m*Vector3d::UnitY().cross(jnt[0].s_W_j - jnt[0].p_W_j);
    J_COM.block(0,5,3,1) = jnt[0].m*Vector3d::UnitZ().cross(jnt[0].s_W_j - jnt[0].p_W_j);
    J_COM = J_COM/Hydra_weight;

    Matrix<double, 6, 6> J_0_i;
    J_0_i.block(0,0,3,1) = Vector3d::UnitX();
    J_0_i.block(3,0,3,1) = Vector3d::Zero();
    J_0_i.block(0,1,3,1) = Vector3d::UnitY();
    J_0_i.block(3,1,3,1) = Vector3d::Zero();
    J_0_i.block(0,2,3,1) = Vector3d::UnitZ();
    J_0_i.block(3,2,3,1) = Vector3d::Zero();
    J_0_i.block(0,3,3,1) = Vector3d::UnitX().cross(jnt[fixed_joint_num].p_W_j);
    J_0_i.block(3,3,3,1) = Vector3d::UnitX();
    J_0_i.block(0,4,3,1) = Vector3d::UnitY().cross(jnt[fixed_joint_num].p_W_j);
    J_0_i.block(3,4,3,1) = Vector3d::UnitY();
    J_0_i.block(0,5,3,1) = Vector3d::UnitZ().cross(jnt[fixed_joint_num].p_W_j);
    J_0_i.block(3,5,3,1) = Vector3d::UnitZ();

    Matrix<double, 6, HYDRA_JNT_MAX> J_theta_i;
    J_theta_i.setZero();
    parent_i = jnt[fixed_joint_num].parent_id; //jnt[joint].parent[i];
    while(parent_i != 0){
        a_W_ij = jnt[parent_i].parent[0]->R_W_j * jnt[parent_i].R_i_ij_ini*Vector3d::UnitZ();
        p_W_ij = jnt[fixed_joint_num].p_W_j - jnt[parent_i].p_W_j;
        J_theta_i.block(0, parent_i-1, 3, 1) = a_W_ij.cross(p_W_ij);
        J_theta_i.block(3, parent_i-1, 3, 1) = a_W_ij;
        parent_i = jnt[parent_i].parent_id;
    }
    Matrix<double, HYDRA_JNT_MAX+6, HYDRA_JNT_MAX> Hi;
    Hi.block(0,0,6,HYDRA_JNT_MAX) = -J_0_i.transpose()*J_theta_i;
    Hi.block(6,0,HYDRA_JNT_MAX,HYDRA_JNT_MAX) = MatrixXd::Identity(HYDRA_JNT_MAX,HYDRA_JNT_MAX);

    Matrix<double, 3, HYDRA_JNT_MAX+1> J_COM_i;
    J_COM_i.block(0,1,3,HYDRA_JNT_MAX) = J_COM*Hi;
    J_COM_i.block(0,0,3,1) = Vector3d::Zero();

    return J_COM_i;
}*/

MatrixXd walk::COM_Jacobian(){
    Matrix<double, 3, HYDRA_JNT_MAX+1> J_COM_joint[HYDRA_JNT_MAX+1];
    for(int i=0; i<HYDRA_JNT_MAX+1; i++){
        J_COM_joint[i].setZero();
    }
    Matrix<double, 3, HYDRA_JNT_MAX+1> J_COM;
    J_COM.setZero();
    Vector3d a_0_ij;
    Vector3d p_0_ij;
    int parent_i;

    for(int joint=1; joint<HYDRA_JNT_MAX+1; joint++){//HYDRA_JNT_MAX
        parent_i = joint;
        while(parent_i != 0){
            a_0_ij = jnt[parent_i].parent[0]->R_0_j * jnt[parent_i].R_i_ij_ini*Vector3d::UnitZ();
            p_0_ij = jnt[joint].s_0_j - jnt[parent_i].p_0_j;
            J_COM_joint[joint].col(parent_i) = a_0_ij.cross(p_0_ij);
            parent_i = jnt[parent_i].parent_id;
        }
    }
    for(int i=0; i<HYDRA_JNT_MAX+1; i++){
        J_COM += jnt[i].m*J_COM_joint[i];
    }
    J_COM = J_COM/Hydra_weight;

/*    Vector3d p_0_com;
    p_0_com.setZero();
    for(int i=0; i<HYDRA_JNT_MAX; i++){
        p_0_com += jnt[i].m*jnt[i].s_0_j;
    }
    p_0_com = p_0_com/Hydra_weight;*/

//    J_COM = jnt[0].R_W_j*(J_COM - jnt[fixed_joint_num].I_J_lin + sqew_symmetric(p_0_com - jnt[fixed_joint_num].p_0_j)*(jnt[fixed_joint_num].I_J_rot));
    return J_COM;
}

void walk::COMcalculate(){
    Vector3d c;
    c = Vector3d::Zero();
    for(int i=0; i<HYDRA_JNT_MAX; i++){
        c += jnt[i].m*jnt[i].s_W_j;
    }
    c = c/Hydra_weight;
    if(p_com_act == Vector3d::Zero()){
        dp_com_act.setZero();
    }else{
        dp_com_act = (c - p_com_act)/dt_loop;

    }
    p_com_act = c;
    x_com_act.x() = p_com_act.x();
    x_com_act.y() = p_com_act.y();
    dx_com_act.x() = dp_com_act.x();
    dx_com_act.y() = dp_com_act.y();
}

void walk::ZMPcalculate(){
    double d = 0.02;///////////////////////////////////////////////////////////
    Vector3d x_zmp_R((-FTsensor[0].torque(1) - FTsensor[0].force(0)*d)/FTsensor[0].force(2), (FTsensor[0].torque(0) - FTsensor[0].force(1)*d)/FTsensor[0].force(2), 0);
    Vector3d x_zmp_L((-FTsensor[1].torque(1) - FTsensor[1].force(0)*d)/FTsensor[1].force(2), (FTsensor[1].torque(0) - FTsensor[1].force(1)*d)/FTsensor[1].force(2), 0);
    //foot coordinate to world coordinate
    x_zmp_R = jnt[6].p_W_j + jnt[6].R_W_j*x_zmp_R;
    x_zmp_L = jnt[12].p_W_j + jnt[12].R_W_j*x_zmp_L;
    x_zmp_act.x() = (x_zmp_R.x()*FTsensor[0].force(2) + x_zmp_L.x()*FTsensor[1].force(2))/(FTsensor[0].force(2) + FTsensor[1].force(2));
    x_zmp_act.y() = (x_zmp_R.y()*FTsensor[0].force(2) + x_zmp_L.y()*FTsensor[1].force(2))/(FTsensor[0].force(2) + FTsensor[1].force(2));

//    walk_log << "Right FTsensor = " << FTsensor[0].force << std::endl;
//    walk_log << "Left FTsensor = " << FTsensor[1].force << std::endl;

}

/*void walk::ZMPcalculate(){
    Vector3d P;
    P = Vector3d::Zero();
    Vector3d dc;
    Vector3d L;
    L = Vector3d::Zero();
    Vector3d dP;
    Vector3d dL;

    for(int i = 0; i < HYDRA_JNT_MAX; i++){
        dc = jnt[i].v_W_j + jnt[i].omeg_W_j.cross(jnt[i].R_W_j*jnt[i].s_j_j);
        L += jnt[i].s_W_j.cross(jnt[i].m*dc) + jnt[i].R_W_j*jnt[i].I_j_j*jnt[i].R_W_j.transpose()*jnt[i].omeg_W_j;
        P += jnt[i].m * dc;
    }
    dP = (P - P_before)/dt_loop;
    dL = (L - L_before)/dt_loop;
    P_before = P;
    L_before = L;
    x_zmp_act.x() = (Hydra_weight*g*x_com_act.x() + COM_height*dP.x() - dL.y())/(Hydra_weight*g + dP.z());
    x_zmp_act.y() = (Hydra_weight*g*x_com_act.y() + COM_height*dP.y() + dL.x())/(Hydra_weight*g + dP.z());
}*/

void walk::OperationalSpace(){
    int moving_joint_num;

    if(fixed_joint_num == 6){
        moving_joint_num = 12;
    }else{
        moving_joint_num = 6;
    }

    //Body
    Quaterniond e_des;
    Quaterniond e_act;
    e_des = AngleAxisd(R_body_ref);
    e_act = AngleAxisd(jnt[14].R_W_j);
    Vector3d e_des_eps(e_des.x(), e_des.y(), e_des.z());
    Vector3d e_act_eps(e_act.x(), e_act.y(), e_act.z());
    omeg_body = -Ko_body*(e_des.w()*e_act_eps - e_act.w()*e_des_eps + sqew_symmetric(e_des_eps)*e_act_eps);

    //moving_leg; (need to consider posture
    Vector3d step1(STEP[0].x(), STEP[0].y(), leg_height);
    if(step_count == 0){
        dp_moving_leg_ref = Kp_moving_leg*(step1 - jnt[moving_joint_num].p_W_j);
//        dp_moving_leg_ref = jnt[moving_joint_num].v_W_j + dt_loop*(3*(step1 - jnt[moving_joint_num].p_W_j) - 0.5*jnt[moving_joint_num].v_W_j);
    }else{
        dp_moving_leg_ref = dmoving_leg_path(t, STEP[step_count-1], STEP[step_count+1]) + Kp_moving_leg*(moving_leg_path(t,STEP[step_count-1], STEP[step_count+1]) - jnt[moving_joint_num].p_W_j);
    }
//    dp_moving_leg_ref = WorldtoBase_dp(jnt[moving_joint_num].p_W_j, dp_moving_leg_ref);

    e_des = AngleAxisd(fixed_joint_R);
    e_act = AngleAxisd(jnt[moving_joint_num].R_W_j);
    Vector3d e_des_eps2(e_des.x(), e_des.y(), e_des.z());
    Vector3d e_act_eps2(e_act.x(), e_act.y(), e_act.z());
    omeg_moving_leg_ref = -Ko_moving_leg*(e_des.w()*e_act_eps2 - e_act.w()*e_des_eps2 + sqew_symmetric(e_des_eps2)*e_act_eps2);

    //fixed_leg; (need to consider posture)
/*    Vector3d p_fixed_leg_des;
    p_fixed_leg_des.x() = STEP[step_count].x();
    p_fixed_leg_des.y() = STEP[step_count].y();
    p_fixed_leg_des.z() = leg_height;
    dp_fixed_leg_ref = Kp_fixed_leg*(p_fixed_leg_des - jnt[fixed_joint_num].p_W_j);
    dp_fixed_leg_ref = WorldtoBase_dp(jnt[fixed_joint_num].p_W_j, dp_fixed_leg_ref);
    e_des = AngleAxisd(fixed_joint_R);
    e_act = AngleAxisd(jnt[fixed_joint_num].R_W_j);
    Vector3d e_des_eps3(e_des.x(), e_des.y(), e_des.z());
    Vector3d e_act_eps3(e_act.x(), e_act.y(), e_act.z());
    omeg_fixed_leg_ref = -Ko_body*(e_des.w()*e_act_eps3 - e_act.w()*e_des_eps3 + sqew_symmetric(e_des_eps3)*e_act_eps3);*/

    //COM
    Vector3d p_com_des(x_com_des.x(), x_com_des.y(), COM_height);
    Vector3d dp_com_des(dx_com_des.x(), dx_com_des.y(), 0);
    dp_com_ref = dp_com_des + Kp_com*(p_com_des - p_com_act);
//    dp_com_ref = WorldtoBase_dp(p_com_act, dp_com_ref);

    //Hand;
//    dp_rhand_ref.setZero();//zero at base coordinate
//    dp_lhand_ref.setZero();//zero at base coordinate
}

void walk::TaskPriority(){
    int moving_joint_num;
    if(fixed_joint_num == 6){
        moving_joint_num = 12;
    }else{
        moving_joint_num = 6;
    }
    Vector3d p_0_com;
    p_0_com.setZero();
    for(int i=0; i<HYDRA_JNT_MAX; i++){
        p_0_com += jnt[i].m*jnt[i].s_0_j;
    }
    p_0_com = p_0_com/Hydra_weight;

    dq_ref.setZero();
    P = MatrixXd::Identity(HYDRA_JNT_MAX+1,HYDRA_JNT_MAX+1);
//    dq_ref += NullSpace(World_Jacobian(COM_Jacobian(), p_0_com), dp_com_ref);//in case of "-0.1*Vector3d::UnitY()", it seems to work.
    dq_ref += NullSpace(jnt[14].I_J_rot, omeg_body);//waist roll

//    dq_ref += NullSpace(World_Jacobian(jnt[fixed_joint_num].I_J_lin, jnt[fixed_joint_num].p_0_j), dp_fixed_leg_ref);
    dq_ref += NullSpace(World_Jacobian(jnt[moving_joint_num].I_J_lin, jnt[moving_joint_num].p_0_j), dp_moving_leg_ref);
//    dq_ref += NullSpace(jnt[fixed_joint_num].I_J_lin, dp_fixed_leg_ref);
//    dq_ref += NullSpace(jnt[moving_joint_num].I_J_lin, dp_moving_leg_ref);


//    dq_ref += NullSpace(jnt[fixed_joint_num].I_J_rot, omeg_fixed_leg_ref);//don't use
    dq_ref += NullSpace(jnt[moving_joint_num].I_J_rot, omeg_moving_leg_ref);//don't use

//    dq_ref += NullSpace(jnt[23].I_J_lin, dp_rhand_ref);
//    dq_ref += NullSpace(jnt[31].I_J_lin, dp_lhand_ref);

    q_ref =  q_ref + dq_ref*dt_loop;

    if(step_count >= 4){
        q_ref.setZero();
    }

        walk_log << "t = " << t << std::endl;
//        walk_log << "dp_moving_leg_ref = " << dp_moving_leg_ref << std::endl;
//        walk_log << "dp_fixed_leg_ref = " << dp_fixed_leg_ref << std::endl;
//        walk_log << "x_zmp_act = " << x_zmp_act << std::endl;
//        walk_log << "x_zmp_des = " << x_zmp_des << std::endl;
//        walk_log << "p_com_act = " << p_com_act << std::endl;
//        walk_log << "jnt[0].R_W_j = " << jnt[0].R_W_j << std::endl;
        walk_log << "jnt[12].p_0_j = " << jnt[12].p_0_j << std::endl;
        walk_log << "jnt[12].p_W_j = " << jnt[12].p_W_j << std::endl;
//        walk_log << "jnt[6].R_W_j = " << jnt[6].R_W_j << std::endl;
}
#if 0
void walk::TaskPriority(){
    int moving_joint_num;
    if(fixed_joint_num == 6){
        moving_joint_num = 12;
    }else{
        moving_joint_num = 6;
    }
    dq_ref.setZero();
    P = MatrixXd::Identity(HYDRA_JNT_MAX+1,HYDRA_JNT_MAX+1);
    dq_ref += NullSpace(COM_Jacobian(), dp_com_ref);//in case of "-0.1*Vector3d::UnitY()", it seems to work.
    dq_ref += NullSpace(jnt[14].I_J_rot, omeg_body);//waist roll

    dq_ref += NullSpace(jnt[fixed_joint_num].I_J_lin, dp_fixed_leg_ref);
    dq_ref += NullSpace(jnt[moving_joint_num].I_J_lin, dp_moving_leg_ref);

////    dq_ref += NullSpace(jnt[fixed_joint_num].I_J_rot, omeg_fixed_leg_ref);//don't use
////    dq_ref += NullSpace(jnt[moving_joint_num].I_J_rot, omeg_moving_leg_ref);//don't use

    dq_ref += NullSpace(jnt[23].I_J_lin, dp_rhand_ref);
    dq_ref += NullSpace(jnt[31].I_J_lin, dp_lhand_ref);

    q_ref =  q_ref + dq_ref*dt_loop;
/*    if(step_count == 0){
        q_ref(1) = 4.56/360*2*M_PI;
        q_ref(2) = -26.75/360*2*M_PI;
        q_ref(3) = -0.24/360*2*M_PI;
        q_ref(4) = 54.83/360*2*M_PI;
        q_ref(5) = 8.67/360*2*M_PI;
        q_ref(6) = -23.28/360*2*M_PI;
        q_ref(7) = 4.22/360*2*M_PI;
        q_ref(8) = -17.34/360*2*M_PI;
        q_ref(9) = -1.42/360*2*M_PI;
        q_ref(10) = 49.14/360*2*M_PI;
        q_ref(11) = 8.12/360*2*M_PI;
        q_ref(12) = -26.54/360*2*M_PI;
        q_ref(13) = 4.79/360*2*M_PI;
        q_ref(14) = 5.8/360*2*M_PI;
    }*/
    if(step_count >= 4){
        q_ref.setZero();
    }

/*        Vector3d aaaa(-0.499872,-0.543161,-0.663874);
        Vector3d bbbb = aaaa - jnt[6].p_0_j;
        q_ref = q_ref + 0.00005*PseudoInverse(jnt[6].I_J_lin)*bbbb;
//        q_ref = q_ref + 0.1*jnt[14].I_J_rot.transpose()*omeg_body;

        walk_log << "norm = " << bbbb.dot(bbbb) << std::endl;*/

        walk_log << "t = " << t << std::endl;
//        walk_log << "dp_moving_leg_ref = " << dp_moving_leg_ref << std::endl;
//        walk_log << "dp_fixed_leg_ref = " << dp_fixed_leg_ref << std::endl;
        walk_log << "x_zmp_act = " << x_zmp_act << std::endl;
        walk_log << "x_zmp_des = " << x_zmp_des << std::endl;
        walk_log << "dp_com_ref = " << dp_com_ref << std::endl;
//        walk_log << "jnt[12].R_W_j = " << jnt[12].R_W_j << std::endl;
//        walk_log << "jnt[6].R_W_j = " << jnt[6].R_W_j << std::endl;
}
#endif

VectorXd walk::NullSpace(const MatrixXd& J, const VectorXd &task){
    MatrixXd JP_Pinv;
    MatrixXd JP;
    VectorXd dq(HYDRA_JNT_MAX+1);
    JP = J*P;
//    JP_Pinv = JP.transpose();
    JP_Pinv = PseudoInverse(JP);
    dq = P*JP_Pinv*(task - J*dq_ref);
    P = P*(MatrixXd::Identity(HYDRA_JNT_MAX+1, HYDRA_JNT_MAX+1) - JP_Pinv*J*P);
    return dq;
}

MatrixXd walk::PseudoInverse(const MatrixXd& m){
    double tolerance = 1e-6;
    using namespace Eigen;
    typedef JacobiSVD<MatrixXd> TSVD;
    unsigned int svd_opt(ComputeThinU | ComputeThinV);
    if(m.RowsAtCompileTime!=Dynamic || m.ColsAtCompileTime!=Dynamic) svd_opt= ComputeFullU | ComputeFullV;
    TSVD svd(m, svd_opt);
    const typename TSVD::SingularValuesType &sigma(svd.singularValues());
    typename TSVD::SingularValuesType sigma_inv(sigma.size());
    for(long i=0; i<sigma.size(); i++)
    {
        if(sigma(i) > tolerance){
            sigma_inv(i)= 1.0/sigma(i);
        }else{
            sigma_inv(i)= 0.0;
        }
    }
    return svd.matrixV()*sigma_inv.asDiagonal()*svd.matrixU().transpose();
}
