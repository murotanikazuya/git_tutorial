#ifndef _CALCPARALLELJOINT_H_
#define _CALCPARALLELJOINT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "CalcForceTorque.h"

int JointToCylinder(const double q_cmd1, const double q_cmd2, double *z_ans1, double *z_ans2, p_2joint_t p_joint);
int CylinderToJoint(const double z_cmd1, const double z_cmd2, double *q_ans1, double *q_ans2, const double pre_z1, const double pre_z2, const int zd_max, p_2joint_t p_joint);
int CylinderToKnee(const double z_cmd1,const double z_cmd2, double *q_ans, p_1joint_t p_joint);
int KneeToCylinder(const double q_cmd, double *z_ans1, double *z_ans2, p_1joint_t p_joint);
int CylinderToElbow(const double z_cmd, double *q_ans, p_1joint_t p_joint);
int ElbowToCylinder(const double q_cmd, double *z_ans, p_1joint_t p_joint);

int CylinderToJointAll(const double* input, double* output, const double* pre_input);
int JointToCylinderAll(const double* input, double* output);

#endif
