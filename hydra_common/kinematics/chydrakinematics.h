#ifndef __CHYDRA_KINEMATICS_H_
#define __CHYDRA_KINEMATICS_H_

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>

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
    Vector3d s_0_jj;
    Vector3d s_0_j;

    Vector3d fg_0_j;

    Vector3d N_0_j;
    Vector3d F_0_j;

    MatrixXd I_J_lin;
    MatrixXd I_J_rot;

} Joint;

class CHydraKinematics
{
public:
    CHydraKinematics();
    ~CHydraKinematics();
    void SetJointPosition(double q[]);
    void ForwardKinematics(void);
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

private:
    Joint jnt[58];
    Matrix3d Rodrigues(Vector3d w, double dt);
    void CalcStatics(int i);
    void CalcStatics(int i, int j);
    void CalcStatics(int i, int j1, int j2);
    void CalcStatics(int i, int j1, int j2, int j3);
    void CalcStatics(int i, int nj, int j[]);
};

    
#endif //__CHYDRA_KINEMATICS_H_


