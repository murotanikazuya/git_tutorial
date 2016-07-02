#ifndef _CALCFORCETORQUE_H_
#define _CALCFORCETORQUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <sys/time.h>

#define NumJoint 31
#define NumEncoder 37

//for ankle, hip, waist, shoulder and wrist
typedef struct parallel_2axis_joint{
	//const double q0[2];						//q0:関節角初期位置[rad]
	const char name[32];
	const double a;
	const double b;
	const double h;
	const double l;
	const double x1;
	const double x2;
	const double y1;
	const double y2;
	const double z1_0;
	const double z2_0;
	
	//for waist
	const double s;
	const double t;
}p_2joint_t;

//for knee and elbow
typedef struct parallel_1axis_joint{
	//const double q0[2];						//q0:関節角初期位置[rad]
	const char name[32];
	const double a;
	const double r;
	const double l;
	const double z1_0;
	const double z2_0;
}p_1joint_t;

const p_2joint_t p_2joint_array[9] = {
	{"rhip", -28.5*cos(M_PI/4.0), 48.0, -28.5*sin(M_PI/4.0), 155.0, -14.0, -14.0, 58.5, -58.5, -154.0, -154.0, 0, 0},
	{"rankle", -37.5, 39.0, 10.0, 155.0, -25.5, -25.5, 26.5, -26.5, 160.0, 160.0, 0, 0},
	{"lhip", -28.5*cos(M_PI/4.0), 48.0, -28.5*sin(M_PI/4.0), 155.0, -14.0, -14.0, 58.5, -58.5, -154.0, -154.0, 0, 0},
	{"lankle", -37.5, 39.0, 10.0, 155.0, -25.5, -25.5, 26.5, -26.5, 160.0, 160.0, 0, 0},
	{"waist", 31.55*cos(15.55*M_PI/180.0), 58.5, 31.55*sin(15.55*M_PI/180.0), 155.0, 0, 0, 58.5, -58.5, 152.84, 152.84, -1.0/sqrt(3.0), 2.0*37.3/sqrt(3.0)},
	{"rshoulder", 35.0, 27.0*cos(50*M_PI/180), -27.0*sin(50*M_PI/180), 155.0, 35.0, -35.0, 10.33, 10.33, 154.1, 154.1, 0, 0},
	{"rwrist", 25.4, 47.5, 0.0, 155.0, 0.0, 0.0, 41.25, -41.25, 152.9, 152.9, 0, 0},
	{"lshoulder", 35.0, 27.0*cos(50*M_PI/180), -27.0*sin(50*M_PI/180), 155.0, 35.0, -35.0, 10.33, 10.33, 154.1, 154.1, 0, 0},
	{"lwrist", 25.4, 47.5, 0.0, 155.0, 0.0, 0.0, 41.25, -41.25, 152.9, 152.9, 0, 0},	
};

const p_1joint_t p_1joint_array[4] = {
	{"rknee", 11.0, 27.0, 189.75, 190.0, 190.0},
	{"lknee", 11.0, 27.0, 189.75, 190.0, 190.0},
	{"relbow", 11.0, 27.5, 155.0, 154.8, 154.8},
	{"lelbow", 11.0, 27.5, 155.0, 154.8, 154.8},
};

//th[rad],tau[Nm],f[N]
int TorqueToActForce(const double th_r, const double th_p, const double tau_r, const double tau_p, double *f1, double *f2, p_2joint_t p_joint);
int ActForceToTorque(const double th_r, const double th_p, const double f1, const double f2, double *tau_r, double *tau_p, p_2joint_t p_joint);
int ActForceToKneeTorque(const double th_r, const double f1,const double f2, double *tau_p, p_1joint_t p_joint);
int KneeTorqueToActforce(const double th_p, const double tau_p, double *f1, double *f2, p_1joint_t p_joint);
int ActForceToElbowTorque(const double th_p, const double f, double *tau, p_1joint_t p_joint);
int ElbowTorqueToActForce(const double th_p, const double tau, double *f, p_1joint_t p_joint);

int ActForceToTorqueAll(const double* joint_angle, const double* input, double* output);
int TorqueToActForceAll(const double* joint_angle, const double* input, double* output);

int CalcJacobian(const double q1, const double q2, double *j00, double *j01, double *j10, double *j11, p_2joint_t p_joint);

#endif
