#include "chydrakinematics.h"
#include "hydra_kinematics.h"
#include "hydra_types.h"
#include <fstream>
#include <iostream>
#include <sstream>

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
        jnt[i].parent_id = parent[i][1];
        for(int j=0; j<jnt[i].num_parent; j++){
            jnt[i].parent[j] = &jnt[parent[i][j+1]];
        }
        jnt[i].num_child = child[i][0];
        for(int j=0; j<jnt[i].num_child; j++){
            jnt[i].child[j] = &jnt[child[i][j+1]];
        }

        jnt[i].I_J_lin.resize(3,HYDRA_JNT_MAX+1);//58+6 -> HYDRA_JNT_MAX 2017/05/17 murotani
        jnt[i].I_J_lin.setZero();
        jnt[i].I_J_rot.resize(3,HYDRA_JNT_MAX+1);//58+6 -> HYDRA_JNT_MAX 2017/05/17 murotani
        jnt[i].I_J_rot.setZero();

        //2017/05/12 murotani
        for(int j=0; j<3; j++){
            for(int k=0; k<3; k++){
                jnt[i].I_j_j(j,k) = Iii[i][j][k];
            }
        }
//        log_kinema.open("log_kinema", std::ios::out);
//        log_kinema << "opened" << std::endl;
        //2017/05/12 murotani
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

void CHydraKinematics::SetJointVelocity(double dq[])//2017/05/10 murotani
{
    for(int i=0; i<HYDRA_JNT_MAX; i++)
    {
        jnt[i+1].dq_ij = dq[i];
    }
}

void CHydraKinematics::SetIMUacc(double acc[])//2017/05/10 murotani
{
    for(int i=0; i<3; i++){
        imu.acc[i] = acc[i];
    }
    imu.acc = jnt[0].R_W_j*imu.acc;//2017/05/15 murotani
}

void CHydraKinematics::SetIMUgyro(double gyro[])//2017/05/10 murotani
{
    for(int i=0; i<3; i++){
        imu.gyro[i] = gyro[i];
    }
    imu.gyro = jnt[0].R_W_j*imu.gyro;//2017/05/15 murotani
}

void CHydraKinematics::SetForceSensor(int i, float force[]){
    for(int j=0; j<3; j++){
        FTsensor[i].force(j) = force[j];
        FTsensor[i].torque(j) = force[j+3];
    }
}

void CHydraKinematics::ForwardKinematics_imu(){//not yet completed
    jnt[0].R_i_ij = Rodrigues(imu.gyro, dt_loop) * jnt[0].R_W_j;////////////////////////////////////incorrect.
    jnt[0].v_W_ij += imu.acc*dt_loop;//need to consider gravity
    jnt[0].v_W_j = jnt[0].v_W_ij;
    jnt[0].p_W_ij += jnt[0].v_W_j*dt_loop;
    jnt[0].p_W_j = jnt[0].p_W_ij;
    jnt[0].omeg_W_j = imu.gyro;

    jnt[0].R_W_j  = jnt[0].R_i_ij;
    jnt[0].p_W_j  = jnt[0].p_W_ij;

    jnt[0].s_W_jj = jnt[0].R_W_j * jnt[0].s_j_j;
    jnt[0].s_W_j  = jnt[0].p_W_j + jnt[0].s_W_jj;

    for(int i=1; i<58; i++)
    {
        jnt[i].R_i_ij = jnt[i].R_i_ij_ini * AngleAxisd(jnt[i].q_ij,Vector3d::UnitZ());

        jnt[i].R_W_j  = jnt[i].parent[0]->R_W_j * jnt[i].R_i_ij;
        jnt[i].p_W_ij = jnt[i].parent[0]->R_W_j * jnt[i].p_i_ij;
        jnt[i].p_W_j  = jnt[i].parent[0]->p_W_j + jnt[i].p_W_ij;

        jnt[i].v_W_j = jnt[i].parent[0]->v_W_j + jnt[i].parent[0]->omeg_W_j.cross(jnt[i].p_W_ij);
        jnt[i].omeg_W_j = jnt[i].parent[0]->omeg_W_j + jnt[i].parent[0]->R_W_j*jnt[i].R_i_ij_ini*Vector3d::UnitZ()*jnt[i].dq_ij;
    }
}

void CHydraKinematics::ForwardKinematics_from_body()
{
    int i;
    jnt[0].v_0_j.setZero();
    jnt[0].p_0_j.setZero();
    jnt[0].omeg_0_j.setZero();

    jnt[0].R_0_j  = Matrix3d::Identity();

    jnt[0].s_0_jj = jnt[0].R_0_j * jnt[0].s_j_j;
    jnt[0].s_0_j  = jnt[0].p_0_j + jnt[0].s_0_jj;

    for(int i=1; i<58; i++)
    {
//        jnt[i].R_i_ij = jnt[i].R_i_ij_ini * Rodrigues(Vector3d::UnitZ(), jnt[i].q_ij);
        jnt[i].R_i_ij = jnt[i].R_i_ij_ini * AngleAxisd(jnt[i].q_ij,Vector3d::UnitZ());

        jnt[i].R_0_j  = jnt[i].parent[0]->R_0_j * jnt[i].R_i_ij;
        jnt[i].p_0_ij = jnt[i].parent[0]->R_0_j * jnt[i].p_i_ij;
        jnt[i].p_0_j  = jnt[i].parent[0]->p_0_j + jnt[i].p_0_ij;

        jnt[i].s_0_jj = jnt[i].R_0_j * jnt[i].s_j_j;
        jnt[i].s_0_j  = jnt[i].p_0_j + jnt[i].s_0_jj;

        jnt[i].v_0_j = jnt[i].parent[0]->v_0_j + jnt[i].parent[0]->omeg_0_j.cross(jnt[i].p_0_ij);//2017/05/12 murotani
        jnt[i].omeg_0_j = jnt[i].parent[0]->omeg_0_j + jnt[i].parent[0]->R_0_j*jnt[i].R_i_ij_ini*Vector3d::UnitZ()*jnt[i].dq_ij;//2017/05/18 murotani
    }


    // Calculate jacobians
    for(int i=1; i<HYDRA_JNT_MAX; i++){//2017/05/12 murotani
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

void CHydraKinematics::ForwardKinematics_from_fixed_joint(int joint, Vector3d pos, Matrix3d R){
//    int n = jnt[joint].parent_id;
    int n = joint;
    std::ofstream log_kinema;
    log_kinema.open("log_kinema", std::ios::app);
//    log_kinema << "n = " << n << std::endl;
//    log_kinema << "joint = " << joint << std::endl;
//    log_kinema << "pos = " << pos << std::endl;
//    log_kinema << "R = " << R << std::endl;

    Vector3d p_j_ji;
    jnt[joint].v_W_j.setZero();
    jnt[joint].omeg_W_j.setZero();
    jnt[joint].p_W_j = pos;
    jnt[joint].R_W_j = R;
    jnt[joint].s_W_jj = jnt[joint].R_W_j * jnt[joint].s_j_j;
    jnt[joint].s_W_j  = jnt[joint].p_W_j + jnt[joint].s_W_jj;

    for(int i=0; i<58; i++){
        jnt[i].R_i_ij = jnt[i].R_i_ij_ini * AngleAxisd(jnt[i].q_ij,Vector3d::UnitZ());
    }

    while(n > 0){//backward iteration for calculating body position/posture, velocity/angular velocity.
        p_j_ji = -jnt[n].R_i_ij.transpose()*jnt[n].p_i_ij;

        jnt[n].parent[0]->p_W_j = jnt[n].p_W_j + jnt[n].R_W_j*p_j_ji;
        jnt[n].parent[0]->R_W_j = jnt[n].R_W_j*jnt[n].R_i_ij.transpose();

//        jnt[n].s_W_jj = jnt[n].R_W_j * jnt[n].s_j_j;
//        jnt[n].s_W_j  = jnt[n].p_W_j + jnt[n].s_W_jj;

        jnt[n].parent[0]->v_W_j = jnt[n].v_W_j + jnt[n].omeg_W_j.cross(jnt[n].R_W_j*p_j_ji);
        jnt[n].parent[0]->omeg_W_j = jnt[n].omeg_W_j + jnt[n].R_W_j*Vector3d::UnitZ()*jnt[n].dq_ij;
        log_kinema << "jnt[" << n << "].parent[0]->R_W_j = " << jnt[n].parent[0]->R_W_j << std::endl;
        n = jnt[n].parent_id;




/*        p_j_ji = -jnt[n].child[0]->R_i_ij.transpose()*jnt[n].child[0]->p_i_ij;

        jnt[n].p_W_j = jnt[n].child[0]->p_W_j + jnt[n].child[0]->R_W_j*p_j_ji;
        jnt[n].R_W_j = jnt[n].child[0]->R_W_j*jnt[n].child[0]->R_i_ij.transpose();

//        jnt[n].s_W_jj = jnt[n].R_W_j * jnt[n].s_j_j;
//        jnt[n].s_W_j  = jnt[n].p_W_j + jnt[n].s_W_jj;

        jnt[n].v_W_j = jnt[n].child[0]->v_W_j + jnt[n].child[0]->omeg_W_j.cross(jnt[n].child[0]->R_W_j*p_j_ji);
        jnt[n].omeg_W_j = jnt[n].child[0]->omeg_W_j + jnt[n].child[0]->R_W_j*Vector3d::UnitZ()*jnt[n].child[0]->dq_ij;
//        log_kinema << "jnt[" << n << "].R_W_j = " << jnt[n].R_W_j << std::endl;
        n = jnt[n].parent_id;*/
    }
    for(int i=1; i<58; i++)
    {
        jnt[i].R_W_j  = jnt[i].parent[0]->R_W_j * jnt[i].R_i_ij;
        jnt[i].p_W_ij = jnt[i].parent[0]->R_W_j * jnt[i].p_i_ij;
        jnt[i].p_W_j  = jnt[i].parent[0]->p_W_j + jnt[i].p_W_ij;

        jnt[i].s_W_jj = jnt[i].R_W_j * jnt[i].s_j_j;
        jnt[i].s_W_j  = jnt[i].p_W_j + jnt[i].s_W_jj;

        jnt[i].v_W_j = jnt[i].parent[0]->v_W_j + jnt[i].parent[0]->omeg_W_j.cross(jnt[i].p_W_ij);
        jnt[i].omeg_W_j = jnt[i].parent[0]->omeg_W_j + jnt[i].parent[0]->R_W_j*jnt[i].R_i_ij_ini*Vector3d::UnitZ()*jnt[i].dq_ij;
    }
}


void CHydraKinematics::GetJointLinJac(int joint){//2017/05/12 murotani
    Vector3d a_0_ij;
    Vector3d p_0_ij;
    int parent_i = parent[joint][1]; //jnt[joint].parent[i];
    while(parent_i != 0){
        a_0_ij = jnt[parent_i].parent[0]->R_0_j * jnt[parent_i].R_i_ij_ini*Vector3d::UnitZ();
        p_0_ij = jnt[joint].p_0_j - jnt[parent_i].p_0_j;
        jnt[joint].I_J_lin.col(parent_i) = a_0_ij.cross(p_0_ij);
        parent_i = parent[parent_i][1];
    }
}

void CHydraKinematics::GetJointRotJac(int joint){//2017/05/16 murotani
    Vector3d a_0_ij;
    a_0_ij = jnt[joint].parent[0]->R_0_j * jnt[joint].R_i_ij_ini*Vector3d::UnitZ();
    jnt[joint].I_J_rot.col(joint) = a_0_ij;
    int parent_i = parent[joint][1]; //jnt[joint].parent[i];
    while(parent_i != 0){
        a_0_ij = jnt[parent_i].parent[0]->R_0_j * jnt[parent_i].R_i_ij_ini*Vector3d::UnitZ();
        jnt[joint].I_J_rot.col(parent_i) = a_0_ij;
        parent_i = parent[parent_i][1];
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
    Matrix3d R = Matrix3d::Identity();
    Vector3d wn;
    Matrix3d w_dash;
    double norm_w = sqrt(w.dot(w));
    double theta = norm_w*dt;

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
