#ifndef WALK_H
#define WALK_H

#include "kinematics/chydrakinematics.h"
#include "hydra_types.h"
//#include "kinematics/hydra_kinematics.h" //murotani 2017/05/04

#include <fstream>
#include <math.h>
#include <chrono>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

using namespace Eigen;

class walk : public CHydraKinematics
{
public:
    walk();
    ~walk();
    //main function of walking controll;
    void walkingcontroll();
    void writeReference_pos(double q[]);

    //controll function;
    void STEPset();
    void ZMPset();
    void CPset();
    void ZMPcontrol();
    void ZMPprojection();
    Vector3d moving_leg_path(double t, Vector2d x_fixed_leg_bef, Vector2d x_fixed_leg_next);
    Vector3d dmoving_leg_path(double t, Vector2d x_fixed_leg_bef, Vector2d x_fixed_leg_next);
    void COMgenerate();
    void OperationalSpace();
    Vector3d WorldtoBase_dp(Vector3d pos_act, Vector3d vel_des);
    void TaskPriority();
//    VectorXd q_act();
    VectorXd NullSpace(const MatrixXd &J, const VectorXd &task);
    void ZMPcalculate();
    void COMcalculate();
    MatrixXd COM_Jacobian();
    MatrixXd World_Jacobian_lin(const MatrixXd &J, const Vector3d &p_0_j);
    MatrixXd World_Jacobian_rot(const MatrixXd &J);
    void CPcalculate();
    void t_update();

    //mathmatical function
    MatrixXd PseudoInverse(const MatrixXd& m);
    Matrix3d sqew_symmetric(Vector3d vector);

    std::ofstream walk_log;
    std::ofstream zmp_plot;
    std::ofstream com_plot;
    std::ofstream cp_plot;


private:
    double t;
    double t_plot;
    double t_before;
    std::chrono::system_clock::time_point t_start;
    std::chrono::system_clock::time_point t_end;

    //walking parameters
    const double dT_step = 0.6;
    const double dT_ech = 0.08;
    const double kf = 0.5;//force controll coefficient;
    const double COM_height = 0.935;
    const double waist_height = 1.11;
    const double Hydra_weight = 103;
    const double g = 9.8;
    const double kappa = sqrt(g/COM_height);
    const double leg_width = 0.07;
    const double leg_front = 0.2;
    const double leg_back = -0.1;
    const double leg_height = 0.03;
    const double step_height = 0.03;

    //CP_control parameters
    int step_count = 0;
    Vector2d ZMP[6];
    Vector2d STEP[6];
    Vector2d CP_eos[6];
    Vector2d CP_ini[6];
    Vector2d x_cp_eos;
    Vector2d x_cp_0;
//    Vector2d x_fixed_leg_bef;
//    Vector2d x_fixed_leg_now;
//    Vector2d x_fixed_leg_next;
    Vector2d x_zmp_des;
    Vector2d x_zmp_act;
    Vector2d x_cp_act;
    Vector3d P_before;
    Vector3d L_before;

    bool r_leg_move_flag;
    bool l_leg_move_flag;

    int fixed_joint_num;
    Vector3d fixed_joint_pos;
    Matrix3d fixed_joint_R;

    Vector2d ddx_com_des;
    Vector2d dx_com_des;
    Vector2d x_com_des;
    Vector2d x_com_act;
    Vector2d dx_com_act;
    Vector3d p_com_act;
    Vector3d dp_com_act;
    Vector3d dp_com_ref;

    //Operational_space parameters
    Matrix3d Kp_moving_leg = 0*Matrix3d::Identity();
    Matrix3d Ko_moving_leg = 0*Matrix3d::Identity();

    Vector3d dp_moving_leg_ref;
    Vector3d omeg_moving_leg_ref;

    Matrix3d Kp_fixed_leg = 0*Matrix3d::Identity();
//    Matrix3d Ko_fixed_leg = 0.3*Matrix3d::Identity();

    Vector3d dp_fixed_leg_ref;
    Vector3d omeg_fixed_leg_ref;

    Matrix3d Kp_com = 0.3*Matrix3d::Identity();//maybe fixed to Matrix3d
    Vector3d omeg_body;
    Matrix3d R_body_ref;
    Matrix3d Ko_body = 2*Matrix3d::Identity();

    Vector3d dp_rhand_ref;
    Vector3d dp_lhand_ref;



    Matrix<double, HYDRA_JNT_MAX+1, HYDRA_JNT_MAX+1> P;
    Matrix<double, HYDRA_JNT_MAX+1, HYDRA_JNT_MAX+1> _P;


    //configuration_control parameters
    VectorXd q_ref = VectorXd::Zero(HYDRA_JNT_MAX+1);
    VectorXd dq_ref = VectorXd::Zero(HYDRA_JNT_MAX+1);
};

inline void walk::ZMPcontrol(){
    double k1 = exp(kappa*dT_ech);
    double k2 = exp(kappa*t)*k1;
    x_zmp_des = 1/(1 - k1)*(k2*CP_ini[step_count] + (1 - k2)*ZMP[step_count]) - k1/(1 - k1)*x_cp_act;

    //zmp_log
    zmp_plot << t_plot << " ";
    zmp_plot << x_zmp_des.transpose() << " ";
    zmp_plot << x_zmp_act.transpose() << std::endl;
}

inline void walk::COMgenerate(){
    ddx_com_des = kf*Hydra_weight*g/COM_height*(x_zmp_act - x_zmp_des);
    dx_com_des =  dx_com_act + ddx_com_des*dt_loop;
    x_com_des = x_com_act + dx_com_des*dt_loop;

    //com_log
    com_plot << t_plot << " ";
    com_plot << x_com_des.transpose() << " ";
    com_plot << x_com_act.transpose() << " ";
    com_plot << dx_com_des.transpose() << " ";
    com_plot << dx_com_act.transpose() << std::endl;
}



inline Vector3d walk::WorldtoBase_dp(Vector3d pos_act, Vector3d vel_des){
    Vector3d dp;
    dp = jnt[0].R_W_j.transpose()*(vel_des - jnt[0].v_W_j - jnt[0].omeg_W_j.cross(pos_act - jnt[0].p_W_j));
    return dp;
}

inline void walk::CPcalculate(){
    x_cp_act = x_com_act + dx_com_act/kappa;

    //cp_log
    double k = exp(kappa*t);
    cp_plot << t_plot << " ";
    Vector2d plot = k*CP_ini[step_count] + (1 - k)*ZMP[step_count];
    cp_plot << plot.transpose() << " ";
    cp_plot << x_cp_act.transpose() << std::endl;
}

inline MatrixXd walk::World_Jacobian_lin(const MatrixXd &J, const Vector3d &p_0_j){
    MatrixXd World_J;
    World_J = jnt[0].R_W_j*(J - jnt[fixed_joint_num].I_J_lin + sqew_symmetric(p_0_j - jnt[fixed_joint_num].p_0_j)*(jnt[fixed_joint_num].I_J_rot));
    return World_J;
}

inline MatrixXd walk::World_Jacobian_rot(const MatrixXd &J){
    MatrixXd World_J_rot;
    World_J_rot = jnt[0].R_W_j*(J - jnt[fixed_joint_num].I_J_rot);
    return World_J_rot;
}

/*inline VectorXd walk::q_act(){
    VectorXd q_act;
    q_act.resize(HYDRA_JNT_MAX+1);
    for(int i=0; i<HYDRA_JNT_MAX+1; i++){
        q_act(i) = jnt[i].q_ij;
    }
    return q_act;
}*/

inline Matrix3d walk::sqew_symmetric(Vector3d vector){
    Matrix3d A;
    A(0,0) = 0;
    A(0,1) = -vector(2);
    A(0,2) = vector(1);

    A(1,0) = vector(2);
    A(1,1) = 0;
    A(1,2) = -vector(0);

    A(2,0) = -vector(1);
    A(2,1) = vector(0);
    A(2,2) = 0;
    return A;
}

inline void walk::t_update(){
    t_end = std::chrono::system_clock::now();
    if(t == 0 && step_count == 0){
        dt_loop = 1e-3;
    }else{
        dt_loop = 1e-3*std::chrono::duration_cast<std::chrono::milliseconds>(t_end-t_start).count();
    }
    t_start = t_end;

    dt_loop = 2e-3;

//    dt_loop = time - t_before;
//    dt_loop = 1e-3;///////////////////////////////////////
    t += dt_loop;
    t_plot += dt_loop;
    t_before = t;
}

#endif // WALK_H
