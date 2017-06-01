#ifndef __CHYDRA_KINEMATICS_H_
#define __CHYDRA_KINEMATICS_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <fstream>

using namespace Eigen;
typedef struct _Joint
{
    //variable
    double  q_ij;
    double dq_ij;

    //constants
    Vector3d p_i_ij;
    Vector3d v_i_ij;
    Matrix3d R_i_ij_ini;
    Vector3d s_j_j;
    double  m;
    Vector3d a_i_ij;
    int num_parent;
    int parent_id;
    struct _Joint *parent[5];
    //int parent[5];
    int num_child;
    struct _Joint *child[5];
    //int child[5];

    //calculation
    Matrix3d R_i_ij;
    Matrix3d R_0_ij;
    Vector3d p_0_ij;
    Vector3d p_0_j;
    Vector3d v_0_ij;
    Vector3d v_0_j;
    Matrix3d R_0_j;
    Vector3d omeg_0_j;//2017/05/12 murotani
    Vector3d s_0_jj;
    Vector3d s_0_j;
    Matrix3d I_j_j;//2017/05/12 murotani
    Matrix3d R_W_j;//2017/05/22 murotani
    Vector3d p_W_j;//2017/05/22 murotani
    Vector3d p_W_ij;//2017/05/22 murotani
    Vector3d omeg_W_j;//2017/05/22 murotani
    Vector3d v_W_ij;
    Vector3d v_W_j;
    Vector3d s_W_j;
    Vector3d s_W_jj;

    Vector3d fg_0_j;

    Vector3d N_0_j;
    Vector3d F_0_j;

    MatrixXd I_J_lin;
    MatrixXd I_J_rot;

} Joint;

typedef struct _IMU{//2017/05/10 murotani
    Vector3d acc;
    Vector3d gyro;
} IMU;

typedef struct _ForceSensor{
    Vector3d force;
    Vector3d torque;
} ForceSensor;

class CHydraKinematics
{
public:
    CHydraKinematics();
    ~CHydraKinematics();
    void SetJointPosition(double q[]);
    void SetJointVelocity(double dq[]);//2017/05/10 murotani
    void SetIMUacc(double acc[]);//2017/05/10 murotani
    void SetIMUgyro(double gyro[]);//2017/05/10 murotani
    void SetForceSensor(int i, float force[]);
    void ForwardKinematics_imu();
    void ForwardKinematics_from_body(void);
    void ForwardKinematics_from_fixed_joint(int joint, Vector3d pos, Matrix3d R);//fixed joint num, world position/posture of that joint.
    void GetJointAbsPos(double pos[][3]);
    void GetCoMAbsPos(double pos[][3]);
    void GetJointTorque(double tau[]);
    void GetJointLinJac(int joint);
    void GetJointRotJac(int joint);
    MatrixXd GetJlinTEST(int j);
    MatrixXd GetJrotTEST(int j);
    const double grav_acc = 9.80665;
    const int num_joints_hydra = 41;
    const int num_joints_choreonoid = 58;

//    std::ofstream log_kinema;

protected://private -> protected. murotani 2017/05/04
    Joint jnt[58];
    IMU imu;//2017/05/10 murotani
    ForceSensor FTsensor[4];//0:RF 1:LF 2:RH 3:LH
    Vector3d p_com;
    double dt_loop = 1e-3;//2017/05/10 murotani
    Matrix3d Rodrigues(Vector3d w, double dt);
    void CalcStatics(int i);
    void CalcStatics(int i, int j);
    void CalcStatics(int i, int j1, int j2);
    void CalcStatics(int i, int j1, int j2, int j3);
    void CalcStatics(int i, int nj, int j[]);
};

    
#endif //__CHYDRA_KINEMATICS_H_


