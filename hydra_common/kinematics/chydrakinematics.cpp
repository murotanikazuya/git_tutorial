#include "chydrakinematics.h"
#include "hydra_kinematics.h"
#include "hydra_types.h"

CHydraKinematics::CHydraKinematics()
{
    for(int i=0; i<58; i++)
    {
        jnt[i].p_i_ij.x() = pij[i][0];
        jnt[i].p_i_ij.y() = pij[i][1];
        jnt[i].p_i_ij.z() = pij[i][2];
        jnt[i].v_i_ij.x() = pij[i][0]; // define v here
        jnt[i].v_i_ij.y() = pij[i][1]; // define v here
        jnt[i].v_i_ij.z() = pij[i][2]; // define v here
        jnt[i].a_i_ij.x() = aij[i][0];
        jnt[i].a_i_ij.y() = aij[i][1];
        jnt[i].a_i_ij.z() = aij[i][2];
        jnt[i].R_i_ij_ini = Rodrigues(jnt[i].a_i_ij, aij[i][3]);
        jnt[i].q_ij = 0;
        jnt[i].dq_ij = 0;
        jnt[i].s_j_j.x() = sij[i][0];
        jnt[i].s_j_j.y() = sij[i][1];
        jnt[i].s_j_j.z() = sij[i][2];
        jnt[i].m         = mass[i];
        jnt[i].num_parent = parent[i][0];
        for(int j=0; j<jnt[i].num_parent; j++){
            jnt[i].parent[j] = &jnt[parent[i][j+1]];
        }
        jnt[i].num_child = child[i][0];
        for(int j=0; j<jnt[i].num_child; j++){
            jnt[i].child[j] = &jnt[child[i][j+1]];
        }

        jnt[i].I_J_lin.resize(3,58+6);
        jnt[i].I_J_lin.setZero();
        jnt[i].I_J_rot.resize(3,58+6);
        jnt[i].I_J_rot.setZero();
    }
}
CHydraKinematics::~CHydraKinematics()
{
}

void CHydraKinematics::SetJointPosition(double q[])
{
//    for(int i=0; i<58; i++)  ///  <- this access jnt[58], which DO NOT EXIST!
    for(int i=0; i<HYDRA_JNT_MAX; i++)
    {
        jnt[i+1].q_ij = q[i];
    }
}

void CHydraKinematics::ForwardKinematics()
{
    int i, j, j1, j2, j3;
    jnt[0].R_i_ij = jnt[0].R_i_ij_ini * AngleAxisd(jnt[0].q_ij, Vector3d::UnitZ());//Rodrigues( Vector3d::UnitZ(), jnt[0].q_ij);
    jnt[0].R_0_j  = jnt[0].R_i_ij;
    jnt[0].p_0_ij = jnt[0].R_0_j * jnt[0].p_i_ij;
    jnt[0].p_0_j  = jnt[0].p_0_ij;

    jnt[0].s_0_jj = jnt[0].R_0_j * jnt[0].s_j_j;
    jnt[0].s_0_j  = jnt[0].p_0_j + jnt[0].s_0_jj;

    for(int i=1; i<58; i++)
    {
//        jnt[i].R_i_ij = jnt[i].R_i_ij_ini * Rodrigues(Vector3d::UnitZ(), jnt[i].q_ij);
        jnt[i].R_i_ij = jnt[i].R_i_ij_ini * AngleAxisd(jnt[i].q_ij,Vector3d::UnitZ());

        jnt[i].R_0_j  = jnt[i].parent[0]->R_0_j * jnt[i].R_i_ij;
        jnt[i].p_0_ij = jnt[i].parent[0]->R_0_j * jnt[i].p_i_ij;
        jnt[i].p_0_j  = jnt[i].parent[0]->p_0_j + jnt[i].p_0_ij;
#if 0
        jnt[i].R_0_j  = jnt[parent[i][1]].R_0_j * jnt[i].R_i_ij;
        jnt[i].p_0_ij = jnt[parent[i][1]].R_0_j * jnt[i].p_i_ij;
        jnt[i].p_0_j  = jnt[parent[i][1]].p_0_j + jnt[i].p_0_ij;
#endif
        jnt[i].s_0_jj = jnt[i].R_0_j * jnt[i].s_j_j;
        jnt[i].s_0_j  = jnt[i].p_0_j + jnt[i].s_0_jj;
    }


    // Calculate jacobians
    for(int i=1; i<58; i++){
        GetJointLinJac(i);
        GetJointRotJac(i);
    }


    //JOINT_HYDRA_LWRIST_PITCH
    i = 31;
    CalcStatics(i);

    for(int i=57; i>0; i--)
    {
        switch(jnt[i].num_child)
        {
        case 0:
            CalcStatics(i);
            break;
        case 1:
            CalcStatics(i,child[i][1]);
            break;
        case 2:
            CalcStatics(i,child[i][1],child[i][2]);
            break;
        case 3:
            CalcStatics(i,child[i][1],child[i][2],child[i][3]);
            break;
        default:
            CalcStatics(i,child[i][0],&(child[i][1]));
            break;
        }
    }
}

void CHydraKinematics::GetJointLinJac(int joint){
    jnt[joint].I_J_lin.block(0,0,3,3) = Matrix<double,3,3>::Identity();
    int parent_i = -1;
    for(int i = 0; i < jnt[joint].num_parent; ++i){
        parent_i = parent[joint][1+i]; //jnt[joint].parent[i];
        Vector3d a_0_ij = jnt[parent_i].R_0_j * jnt[parent_i].a_i_ij; // could probably be put into kinematics
        Vector3d p_0_ij = jnt[joint].p_0_j - jnt[parent_i].p_0_j;
        jnt[joint].I_J_lin.block(0,parent_i,3,1) = a_0_ij.cross(p_0_ij);
    }
}

void CHydraKinematics::GetJointRotJac(int joint){
    jnt[joint].I_J_rot.block(0,3,3,3) = Matrix<double,3,3>::Identity();
    int parent_i = -1;
    for(int i = 0; i < jnt[joint].num_parent; ++i){
        parent_i = parent[joint][1+i]; //jnt[joint].parent[i];
        Vector3d a_0_ij = jnt[parent_i].R_0_j * jnt[parent_i].a_i_ij;
        jnt[joint].I_J_lin.block(0,parent_i,3,1) = a_0_ij;
    }
}

MatrixXd CHydraKinematics::GetJlinTEST(int j){
    return jnt[j].I_J_lin;
}

MatrixXd CHydraKinematics::GetJrotTEST(int j){
    return jnt[j].I_J_rot;
}

Matrix3d CHydraKinematics::Rodrigues(Vector3d w, double dt)
{
    Matrix3d R;
    Vector3d wn;
    Matrix3d w_dash;
    double norm_w = sqrt(w(0)*w(0)+w(1)*w(1)+w(2)*w(2));
    double theta = norm_w*dt;

    R(0,0) = 1;
    R(1,1) = 1;
    R(2,2) = 1;
    R(0,1) = 0;
    R(0,2) = 0;
    R(1,0) = 0;
    R(1,2) = 0;
    R(2,0) = 0;
    R(2,1) = 0;
    if(norm_w != 0)
    {
        wn = w / norm_w;
        w_dash(0,0) = 0;
        w_dash(0,1) = -1*wn(2);
        w_dash(0,2) = wn(1);
        w_dash(1,0) = wn(2);
        w_dash(1,1) = 0;
        w_dash(1,2) = -1*wn(0);
        w_dash(2,0) = -1*wn(1);
        w_dash(2,1) = wn(0);
        w_dash(2,2) = 0;

        R = R + w_dash*sin(theta)+w_dash*w_dash*(1-cos(theta));
    }

    return R;

}

void CHydraKinematics::GetJointAbsPos(double pos[][3])
{
    for(int j=0; j<31; j++)
    {
        pos[j][0] = jnt[j+1].p_0_j(0);
        pos[j][1] = jnt[j+1].p_0_j(1);
        pos[j][2] = jnt[j+1].p_0_j(2);
    }
}

void CHydraKinematics::GetCoMAbsPos(double pos[][3])
{
    for(int j=0; j<31; j++)
    {
        pos[j][0] = jnt[j+1].s_0_j(0);
        pos[j][1] = jnt[j+1].s_0_j(1);
        pos[j][2] = jnt[j+1].s_0_j(2);
    }
}

void CHydraKinematics::GetJointTorque(double tau[])
{
    Vector3d a;
    for(int k=0; k<31; k++)
    {
        a = jnt[k+1].R_0_j.block(0,2,3,1);
//        a.x() = jnt[k+1].R_0_j(0,2);
//        a.y() = jnt[k+1].R_0_j(1,2);
//        a.z() = jnt[k+1].R_0_j(2,2);
        tau[k] = -a.dot(jnt[k+1].N_0_j);
    }

}

void CHydraKinematics::CalcStatics(int i)
{
    jnt[i].fg_0_j = Vector3d::UnitZ()*(-1)*jnt[i].m*CHydraKinematics::grav_acc;
    jnt[i].F_0_j  = jnt[i].fg_0_j;
    jnt[i].N_0_j  = jnt[i].s_0_jj.cross(jnt[i].fg_0_j);
}

void CHydraKinematics::CalcStatics(int i, int j)
{
    jnt[i].fg_0_j = Vector3d::UnitZ()*(-1)*jnt[i].m*CHydraKinematics::grav_acc;
    jnt[i].F_0_j  = jnt[i].fg_0_j + jnt[j].F_0_j;
    jnt[i].N_0_j  = jnt[j].N_0_j + jnt[i].s_0_jj.cross(jnt[i].fg_0_j) + jnt[j].p_0_ij.cross(jnt[j].F_0_j);
}

void CHydraKinematics::CalcStatics(int i, int j1, int j2)
{
    jnt[i].fg_0_j = Vector3d::UnitZ()*(-1)*jnt[i].m*CHydraKinematics::grav_acc;
    jnt[i].F_0_j  = jnt[i].fg_0_j + jnt[j1].F_0_j + jnt[j2].F_0_j;
    jnt[i].N_0_j  = jnt[j1].N_0_j + jnt[j2].N_0_j
    + jnt[i].s_0_jj.cross(jnt[i].fg_0_j)
    + jnt[j1].p_0_ij.cross(jnt[j1].F_0_j)+ jnt[j2].p_0_ij.cross(jnt[j2].F_0_j);
}

void CHydraKinematics::CalcStatics(int i, int j1, int j2, int j3)
{
    jnt[i].fg_0_j = Vector3d::UnitZ()*(-1)*jnt[i].m*CHydraKinematics::grav_acc;
    jnt[i].F_0_j  = jnt[i].fg_0_j + jnt[j1].F_0_j + jnt[j2].F_0_j + jnt[j3].F_0_j;
    jnt[i].N_0_j  = jnt[j1].N_0_j + jnt[j2].N_0_j + jnt[j3].N_0_j
    + jnt[i].s_0_jj.cross(jnt[i].fg_0_j)
    + jnt[j1].p_0_ij.cross(jnt[j1].F_0_j) + jnt[j2].p_0_ij.cross(jnt[j2].F_0_j) + jnt[j3].p_0_ij.cross(jnt[j3].F_0_j);
}

void CHydraKinematics::CalcStatics(int i, int nj, int j[])
{
    jnt[i].fg_0_j = Vector3d::UnitZ()*(-1)*jnt[i].m*CHydraKinematics::grav_acc;
    jnt[i].F_0_j  = jnt[i].fg_0_j;
    jnt[i].N_0_j  = jnt[i].s_0_jj.cross(jnt[i].fg_0_j);
    for(int k=0; k<nj; k++)
    {
        jnt[i].F_0_j  += jnt[j[k]].F_0_j;
        jnt[i].N_0_j  += jnt[j[k]].N_0_j + jnt[j[k]].p_0_ij.cross(jnt[j[k]].F_0_j);
    }
}
