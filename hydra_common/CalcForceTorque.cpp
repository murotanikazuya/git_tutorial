#include "CalcForceTorque.h"

int CalcJacobian(const double o1, const double o2, double *j00, double *j01, double *j10, double *j11, p_2joint_t p_joint){
	if(!strcmp(p_joint.name,"lhip") || !strcmp(p_joint.name,"rhip")){
		*j00 = ((-p_joint.a*cos(o1)*sin(o2)+p_joint.h*cos(o1)*cos(o2)+p_joint.b*sin(o1))*(p_joint.y1-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1)))/sqrt(-pow(p_joint.y1-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1),2)-pow(p_joint.x1-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l) + p_joint.a*sin(o1)*sin(o2)-p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1);
		*j01 = -(-2*(-p_joint.a*sin(o1)*cos(o2)-p_joint.h*sin(o1)*sin(o2))*(p_joint.y1-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1))-2.0*(p_joint.a*sin(o2)-p_joint.h*cos(o2))*(p_joint.x1-p_joint.h*sin(o2)-p_joint.a*cos(o2)))/(2.0*sqrt(-pow(p_joint.y1-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1),2)-pow(p_joint.x1-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l))-p_joint.h*cos(o1)*sin(o2)-p_joint.a*cos(o1)*cos(o2);
		*j10 = ((-p_joint.a*cos(o1)*sin(o2)+p_joint.h*cos(o1)*cos(o2)-p_joint.b*sin(o1))*(p_joint.y2-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1)))/sqrt(-pow(p_joint.y2-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1),2)-pow(p_joint.x2-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l)+p_joint.a*sin(o1)*sin(o2)-p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1);
		*j11 = -(-2*(-p_joint.a*sin(o1)*cos(o2)-p_joint.h*sin(o1)*sin(o2))*(p_joint.y2-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1))-2.0*(p_joint.a*sin(o2)-p_joint.h*cos(o2))*(p_joint.x2-p_joint.h*sin(o2)-p_joint.a*cos(o2)))/(2.0*sqrt(-pow(p_joint.y2-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1),2)-pow(p_joint.x2-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l))-p_joint.h*cos(o1)*sin(o2)-p_joint.a*cos(o1)*cos(o2);
	}else if(!strcmp(p_joint.name,"rshoulder") || !strcmp(p_joint.name,"lshoulder")){
		*j00 = -p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1)*cos(o2)+1/2/sqrt(pow(p_joint.a*sin(o2),2)-2*p_joint.a*cos(o2)*p_joint.h*cos(o1)*sin(o2)-2*p_joint.b*sin(o1)*cos(o2)*p_joint.a*sin(o2)+pow(p_joint.h*cos(o1)*cos(o2),2)+2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.b*sin(o1)+pow(p_joint.b*sin(o1)*cos(o2),2)-p_joint.x1*p_joint.x1-p_joint.y1*p_joint.y1+2*p_joint.y1*p_joint.b*cos(o1)+2*p_joint.x1*p_joint.b*sin(o1)*sin(o2)-2*p_joint.y1*p_joint.h*sin(o1)-p_joint.h*p_joint.h-p_joint.a*p_joint.a-p_joint.b*p_joint.b+p_joint.l*p_joint.l+2*p_joint.x1*p_joint.h*cos(o1)*sin(o2)+2*p_joint.x1*p_joint.a*cos(o2))*(2*p_joint.a*cos(o2)*p_joint.h*sin(o1)*sin(o2)-2*p_joint.b*cos(o1)*cos(o2)*p_joint.a*sin(o2)-2*p_joint.h*p_joint.h*cos(o1)*pow(cos(o2),2)*sin(o1)-2*p_joint.h*pow(sin(o1)*cos(o2),2)*p_joint.b+2*p_joint.h*pow(cos(o1)*cos(o2),2)*p_joint.b+2*p_joint.b*p_joint.b*sin(o1)*pow(cos(o2),2)*cos(o1)-2*p_joint.y1*p_joint.b*sin(o1)+2*p_joint.x1*p_joint.b*cos(o1)*sin(o2)-2*p_joint.y1*p_joint.h*cos(o1)-2*p_joint.x1*p_joint.h*sin(o1)*sin(o2));
		*j01 = -p_joint.a*cos(o2)-p_joint.h*cos(o1)*sin(o2)-p_joint.b*sin(o1)*sin(o2)+1/2/sqrt(pow(p_joint.a*sin(o2),2)-2*p_joint.a*cos(o2)*p_joint.h*cos(o1)*sin(o2)-2*p_joint.b*sin(o1)*cos(o2)*p_joint.a*sin(o2)+pow(p_joint.h*cos(o1)*cos(o2),2)+2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.b*sin(o1)+pow(p_joint.b*sin(o1)*cos(o2),2)-p_joint.x1*p_joint.x1-p_joint.y1*p_joint.y1+2*p_joint.y1*p_joint.b*cos(o1)+2*p_joint.x1*p_joint.b*sin(o1)*sin(o2)-2*p_joint.y1*p_joint.h*sin(o1)-p_joint.h*p_joint.h-p_joint.a*p_joint.a-p_joint.b*p_joint.b+p_joint.l*p_joint.l+2*p_joint.x1*p_joint.h*cos(o1)*sin(o2)+2*p_joint.x1*p_joint.a*cos(o2))*(-2*pow(p_joint.h*cos(o1),2)*cos(o2)*sin(o2)-4*p_joint.h*cos(o1)*cos(o2)*p_joint.b*sin(o1)*sin(o2)+2*p_joint.a*pow(sin(o2),2)*p_joint.h*cos(o1)-2*p_joint.a*pow(cos(o2),2)*p_joint.h*cos(o1)-2*pow(p_joint.b*sin(o1),2)*cos(o2)*sin(o2)+2*p_joint.b*sin(o1)*pow(sin(o2),2)*p_joint.a-2*p_joint.b*sin(o1)*pow(cos(o2),2)*p_joint.a+2*pow(p_joint.a,2)*sin(o2)*cos(o2)-2*p_joint.x1*p_joint.a*sin(o2)+2*p_joint.x1*p_joint.b*sin(o1)*cos(o2)+2*p_joint.x1*p_joint.h*cos(o1)*cos(o2)); 
		*j10 = p_joint.b*cos(o1)*cos(o2)-p_joint.h*sin(o1)*cos(o2)+1/2/sqrt(pow(p_joint.b*sin(o1)*cos(o2),2)+2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.b*sin(o1)+2*p_joint.b*sin(o1)*cos(o2)*p_joint.a*sin(o2)+pow(p_joint.h*cos(o1)*cos(o2),2)+2*p_joint.a*cos(o2)*p_joint.h*cos(o1)*sin(o2)+pow(p_joint.a*sin(o2),2)+2*p_joint.x2*p_joint.b*sin(o1)*sin(o2)+2*p_joint.y2*p_joint.b*cos(o1)-p_joint.a*p_joint.a-p_joint.b*p_joint.b+p_joint.l*p_joint.l-p_joint.h*p_joint.h-p_joint.y2*p_joint.y2-2*p_joint.x2*p_joint.a*cos(o2)+2*p_joint.x2*p_joint.h*cos(o1)*sin(o2)-p_joint.x2*p_joint.x2-2*p_joint.y2*p_joint.h*sin(o1))*(2*p_joint.b*p_joint.b*sin(o1)*pow(cos(o2),2)*cos(o1)-2*p_joint.h*pow(sin(o1)*cos(o2),2)*p_joint.b+2*p_joint.h*pow(cos(o1)*cos(o2),2)*p_joint.b+2*p_joint.b*cos(o1)*cos(o2)*p_joint.a*sin(o2)-2*p_joint.h*p_joint.h*cos(o1)*pow(cos(o2),2)*sin(o1)-2*p_joint.a*cos(o2)*p_joint.h*sin(o1)*sin(o2)+2*p_joint.x2*p_joint.b*cos(o1)*sin(o2)-2*p_joint.y2*p_joint.b*sin(o1)-2*p_joint.x2*p_joint.h*sin(o1)*sin(o2)-2*p_joint.y2*p_joint.h*cos(o1));
		*j11 = -p_joint.b*sin(o1)*sin(o2)-p_joint.h*cos(o1)*sin(o2)+p_joint.a*cos(o2)+1/2/sqrt(pow(p_joint.b*sin(o1)*cos(o2),2)+2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.b*sin(o1)+2*p_joint.b*sin(o1)*cos(o2)*p_joint.a*sin(o2)+pow(p_joint.h*cos(o1)*cos(o2),2)+2*p_joint.a*cos(o2)*p_joint.h*cos(o1)*sin(o2)+pow(p_joint.a*sin(o2),2)+2*p_joint.x2*p_joint.b*sin(o1)*sin(o2)+2*p_joint.y2*p_joint.b*cos(o1)-p_joint.a*p_joint.a-p_joint.b*p_joint.b+p_joint.l*p_joint.l-p_joint.h*p_joint.h-p_joint.y2*p_joint.y2-2*p_joint.x2*p_joint.a*cos(o2)+2*p_joint.x2*p_joint.h*cos(o1)*sin(o2)-p_joint.x2*p_joint.x2-2*p_joint.y2*p_joint.h*sin(o1))*(2*p_joint.a*p_joint.a*sin(o2)*cos(o2)-2*p_joint.b*sin(o1)*pow(sin(o2),2)*p_joint.a+2*p_joint.b*sin(o1)*pow(cos(o2),2)*p_joint.a-2*p_joint.a*pow(sin(o2),2)*p_joint.h*cos(o1)+2*p_joint.a*pow(cos(o2),2)*p_joint.h*cos(o1)-2*p_joint.b*p_joint.b*pow(sin(o1),2)*cos(o2)*sin(o2)-4*p_joint.h*cos(o1)*cos(o2)*p_joint.b*sin(o1)*sin(o2)-2*pow(p_joint.h*cos(o1),2)*cos(o2)*sin(o2)+2*p_joint.x2*p_joint.h*cos(o1)*cos(o2)+2*p_joint.x2*p_joint.a*sin(o2)+2*p_joint.x2*p_joint.b*sin(o1)*cos(o2));
	}else if(!strcmp(p_joint.name, "waist")){
		*j00 = 1/2.0/(1+p_joint.s*p_joint.s)*(-2*p_joint.a*sin(o1)*sin(o2)+2*p_joint.b*cos(o1)-2*p_joint.h*sin(o1)*cos(o2)+1/sqrt(pow(p_joint.h*cos(o1)*cos(o2),2)+pow(p_joint.a*cos(o1)*sin(o2),2)+2*p_joint.y1*p_joint.b*cos(o1)+2*p_joint.t*p_joint.h*sin(o2)-2*p_joint.t*p_joint.a*cos(o2)+p_joint.l*p_joint.l-2*p_joint.y1*p_joint.a*sin(o1)*sin(o2)-2*p_joint.y1*p_joint.h*sin(o1)*cos(o2)+pow(p_joint.b*sin(o1),2)-p_joint.h*p_joint.h-p_joint.a*p_joint.a-p_joint.y1*p_joint.y1+2*p_joint.a*pow(cos(o1),2)*sin(o2)*p_joint.h*cos(o2)-p_joint.b*p_joint.b+pow(p_joint.s*p_joint.l,2)-pow(p_joint.s*p_joint.b,2)-pow(p_joint.s*p_joint.h,2)-pow(p_joint.s*p_joint.y1,2)-pow(p_joint.s*p_joint.a,2)-2*p_joint.b*sin(o1)*p_joint.s*p_joint.t+2*p_joint.s*p_joint.s*p_joint.y1*p_joint.b*cos(o1)+2*p_joint.a*sin(o1)*sin(o2)*p_joint.b*cos(o1)+2*p_joint.b*cos(o1)*p_joint.h*sin(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.y1*p_joint.h*sin(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.y1*p_joint.a*sin(o1)*sin(o2)-2*p_joint.a*cos(o1)*sin(o2)*p_joint.s*p_joint.t+2*p_joint.a*cos(o1)*pow(sin(o2),2)*p_joint.s*p_joint.h-2*p_joint.a*p_joint.a*cos(o1)*sin(o2)*p_joint.s*cos(o2)+2*p_joint.b*sin(o1)*p_joint.s*p_joint.h*sin(o2)-2*p_joint.b*sin(o1)*p_joint.s*p_joint.a*cos(o2)-2*p_joint.s*p_joint.t*p_joint.h*cos(o1)*cos(o2)+2*p_joint.s*p_joint.h*p_joint.h*sin(o2)*cos(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.h*sin(o2)*p_joint.a*cos(o2)-2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.s*p_joint.a-p_joint.t*p_joint.t+pow(p_joint.s*p_joint.h*sin(o2),2)+pow(p_joint.s*p_joint.a*cos(o2),2))*(-2*p_joint.h*p_joint.h*cos(o1)*pow(cos(o2),2)*sin(o1)-2*p_joint.a*p_joint.a*cos(o1)*pow(sin(o2),2)*sin(o1)-2*p_joint.y1*p_joint.b*sin(o1)-2*p_joint.y1*p_joint.a*cos(o1)*sin(o2)-2*p_joint.y1*p_joint.h*cos(o1)*cos(o2)+2*p_joint.b*p_joint.b*sin(o1)*cos(o1)-4*p_joint.a*cos(o1)*sin(o2)*p_joint.h*cos(o2)*sin(o1)-2*p_joint.b*cos(o1)*p_joint.s*p_joint.t-2*p_joint.s*p_joint.s*p_joint.y1*p_joint.b*sin(o1)+2*p_joint.a*pow(cos(o1),2)*sin(o2)*p_joint.b-2*p_joint.a*pow(sin(o1),2)*sin(o2)*p_joint.b-2*p_joint.b*pow(sin(o1),2)*p_joint.h*cos(o2)+2*p_joint.b*pow(cos(o1),2)*p_joint.h*cos(o2)-2*p_joint.s*p_joint.s*p_joint.y1*p_joint.h*cos(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.y1*p_joint.a*cos(o1)*sin(o2)+2*p_joint.a*sin(o1)*sin(o2)*p_joint.s*p_joint.t-2*p_joint.a*sin(o1)*pow(sin(o2),2)*p_joint.s*p_joint.h+2*p_joint.a*p_joint.a*sin(o1)*sin(o2)*p_joint.s*cos(o2)+2*p_joint.b*cos(o1)*p_joint.s*p_joint.h*sin(o2)-2*p_joint.b*cos(o1)*p_joint.s*p_joint.a*cos(o2)+2*p_joint.s*p_joint.t*p_joint.h*sin(o1)*cos(o2)-2*p_joint.s*p_joint.h*p_joint.h*sin(o2)*sin(o1)*cos(o2)+2*p_joint.h*sin(o1)*pow(cos(o2),2)*p_joint.s*p_joint.a));
		*j01 = 1/2.0/(1+p_joint.s*p_joint.s)*(2*p_joint.a*cos(o1)*cos(o2)+2*p_joint.s*p_joint.h*cos(o2)-2*p_joint.h*cos(o1)*sin(o2)+2*p_joint.s*p_joint.a*sin(o2)+1/sqrt(pow(p_joint.h*cos(o1)*cos(o2),2)+pow(p_joint.a*cos(o1)*sin(o2),2)+2*p_joint.y1*p_joint.b*cos(o1)+2*p_joint.t*p_joint.h*sin(o2)-2*p_joint.t*p_joint.a*cos(o2)+p_joint.l*p_joint.l-2*p_joint.y1*p_joint.a*sin(o1)*sin(o2)-2*p_joint.y1*p_joint.h*sin(o1)*cos(o2)+pow(p_joint.b*sin(o1),2)-p_joint.h*p_joint.h-p_joint.a*p_joint.a-p_joint.y1*p_joint.y1+2*p_joint.a*pow(cos(o1),2)*sin(o2)*p_joint.h*cos(o2)-p_joint.b*p_joint.b+pow(p_joint.s*p_joint.l,2)-pow(p_joint.s*p_joint.b,2)-pow(p_joint.s*p_joint.h,2)-pow(p_joint.s*p_joint.y1,2)-pow(p_joint.s*p_joint.a,2)-2*p_joint.b*sin(o1)*p_joint.s*p_joint.t+2*p_joint.s*p_joint.s*p_joint.y1*p_joint.b*cos(o1)+2*p_joint.a*sin(o1)*sin(o2)*p_joint.b*cos(o1)+2*p_joint.b*cos(o1)*p_joint.h*sin(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.y1*p_joint.h*sin(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.y1*p_joint.a*sin(o1)*sin(o2)-2*p_joint.a*cos(o1)*sin(o2)*p_joint.s*p_joint.t+2*p_joint.a*cos(o1)*pow(sin(o2),2)*p_joint.s*p_joint.h-2*p_joint.a*p_joint.a*cos(o1)*sin(o2)*p_joint.s*cos(o2)+2*p_joint.b*sin(o1)*p_joint.s*p_joint.h*sin(o2)-2*p_joint.b*sin(o1)*p_joint.s*p_joint.a*cos(o2)-2*p_joint.s*p_joint.t*p_joint.h*cos(o1)*cos(o2)+2*p_joint.s*p_joint.h*p_joint.h*sin(o2)*cos(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.h*sin(o2)*p_joint.a*cos(o2)-2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.s*p_joint.a-p_joint.t*p_joint.t+pow(p_joint.s*p_joint.h*sin(o2),2)+pow(p_joint.s*p_joint.a*cos(o2),2))*(8*p_joint.a*cos(o1)*sin(o2)*p_joint.s*p_joint.h*cos(o2)-2*p_joint.a*p_joint.a*cos(o1)*pow(cos(o2),2)*p_joint.s+2*p_joint.a*p_joint.a*cos(o1)*pow(sin(o2),2)*p_joint.s+2*p_joint.b*sin(o1)*p_joint.s*p_joint.h*cos(o2)+2*p_joint.s*p_joint.s*p_joint.y1*p_joint.h*sin(o1)*sin(o2)-2*p_joint.s*p_joint.s*p_joint.y1*p_joint.a*sin(o1)*cos(o2)+2*p_joint.b*sin(o1)*p_joint.s*p_joint.a*sin(o2)+2*p_joint.s*p_joint.t*p_joint.h*cos(o1)*sin(o2)-2*p_joint.s*pow(p_joint.h*sin(o2),2)*cos(o1)+2*p_joint.s*pow(p_joint.h*cos(o2),2)*cos(o1)-2*p_joint.s*p_joint.s*p_joint.h*pow(cos(o2),2)*p_joint.a-2*p_joint.a*cos(o1)*cos(o2)*p_joint.s*p_joint.t+2*p_joint.t*p_joint.h*cos(o2)-2*pow(p_joint.h*cos(o1),2)*cos(o2)*sin(o2)+2*pow(p_joint.a*cos(o1),2)*sin(o2)*cos(o2)-2*p_joint.y1*p_joint.a*sin(o1)*cos(o2)+2*p_joint.t*p_joint.a*sin(o2)+2*p_joint.y1*p_joint.h*sin(o1)*sin(o2)-2*p_joint.a*pow(cos(o1)*sin(o2),2)*p_joint.h+2*p_joint.s*p_joint.s*p_joint.h*pow(sin(o2),2)*p_joint.a+2*pow(p_joint.s*p_joint.h,2)*sin(o2)*cos(o2)-2*pow(p_joint.s*p_joint.a,2)*cos(o2)*sin(o2)+2*p_joint.a*pow(cos(o1)*cos(o2),2)*p_joint.h+2*p_joint.a*sin(o1)*cos(o2)*p_joint.b*cos(o1)-2*p_joint.b*cos(o1)*p_joint.h*sin(o1)*sin(o2)));
		*j10 = 1/2.0/(1+p_joint.s*p_joint.s)*(-2*p_joint.h*sin(o1)*cos(o2)-2*p_joint.a*sin(o1)*sin(o2)-2*p_joint.b*cos(o1)+1/sqrt(pow(p_joint.h*cos(o1)*cos(o2),2)+pow(p_joint.a*cos(o1)*sin(o2),2)+2*p_joint.t*p_joint.h*sin(o2)-2*p_joint.t*p_joint.a*cos(o2)-p_joint.y2*p_joint.y2-2*p_joint.y2*p_joint.b*cos(o1)+p_joint.l*p_joint.l+pow(p_joint.b*sin(o1),2)-p_joint.h*p_joint.h-p_joint.a*p_joint.a+2*p_joint.a*pow(cos(o1),2)*sin(o2)*p_joint.h*cos(o2)-p_joint.b*p_joint.b-2*p_joint.y2*p_joint.a*sin(o1)*sin(o2)-2*p_joint.y2*p_joint.h*sin(o1)*cos(o2)+pow(p_joint.s*p_joint.l,2)-pow(p_joint.s*p_joint.b,2)-pow(p_joint.s*p_joint.h,2)-pow(p_joint.s*p_joint.a,2)+2*p_joint.b*sin(o1)*p_joint.s*p_joint.t-pow(p_joint.s*p_joint.y2,2)-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.h*sin(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.b*cos(o1)-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.a*sin(o1)*sin(o2)-2*p_joint.a*sin(o1)*sin(o2)*p_joint.b*cos(o1)-2*p_joint.b*cos(o1)*p_joint.h*sin(o1)*cos(o2)-2*p_joint.a*cos(o1)*sin(o2)*p_joint.s*p_joint.t+2*p_joint.a*cos(o1)*pow(sin(o2),2)*p_joint.s*p_joint.h-2*p_joint.a*p_joint.a*cos(o1)*sin(o2)*p_joint.s*cos(o2)-2*p_joint.b*sin(o1)*p_joint.s*p_joint.h*sin(o2)+2*p_joint.b*sin(o1)*p_joint.s*p_joint.a*cos(o2)-2*p_joint.s*p_joint.t*p_joint.h*cos(o1)*cos(o2)+2*p_joint.s*p_joint.h*p_joint.h*sin(o2)*cos(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.h*sin(o2)*p_joint.a*cos(o2)-2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.s*p_joint.a-p_joint.t*p_joint.t+pow(p_joint.s*p_joint.h*sin(o2),2)+pow(p_joint.s*p_joint.a*cos(o2),2))*(-2*p_joint.h*p_joint.h*cos(o1)*pow(cos(o2),2)*sin(o1)-2*p_joint.a*p_joint.a*cos(o1)*pow(sin(o2),2)*sin(o1)+2*p_joint.y2*p_joint.b*sin(o1)+2*p_joint.b*p_joint.b*sin(o1)*cos(o1)-4*p_joint.a*cos(o1)*sin(o2)*p_joint.h*cos(o2)*sin(o1)-2*p_joint.y2*p_joint.a*cos(o1)*sin(o2)-2*p_joint.y2*p_joint.h*cos(o1)*cos(o2)+2*p_joint.b*cos(o1)*p_joint.s*p_joint.t-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.h*cos(o1)*cos(o2)+2*p_joint.s*p_joint.s*p_joint.y2*p_joint.b*sin(o1)-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.a*cos(o1)*sin(o2)-2*p_joint.a*pow(cos(o1),2)*sin(o2)*p_joint.b+2*p_joint.a*pow(sin(o1),2)*sin(o2)*p_joint.b+2*p_joint.b*pow(sin(o1),2)*p_joint.h*cos(o2)-2*p_joint.b*pow(cos(o1),2)*p_joint.h*cos(o2)+2*p_joint.a*sin(o1)*sin(o2)*p_joint.s*p_joint.t-2*p_joint.a*sin(o1)*pow(sin(o2),2)*p_joint.s*p_joint.h+2*p_joint.a*p_joint.a*sin(o1)*sin(o2)*p_joint.s*cos(o2)-2*p_joint.b*cos(o1)*p_joint.s*p_joint.h*sin(o2)+2*p_joint.b*cos(o1)*p_joint.s*p_joint.a*cos(o2)+2*p_joint.s*p_joint.t*p_joint.h*sin(o1)*cos(o2)-2*p_joint.s*p_joint.h*p_joint.h*sin(o2)*sin(o1)*cos(o2)+2*p_joint.h*sin(o1)*pow(cos(o2),2)*p_joint.s*p_joint.a));
		*j11 = 1/2.0/(1+p_joint.s*p_joint.s)*(-2*p_joint.h*cos(o1)*sin(o2)+2*p_joint.a*cos(o1)*cos(o2)+2*p_joint.s*p_joint.a*sin(o2)+2*p_joint.s*p_joint.h*cos(o2)+1/sqrt(pow(p_joint.h*cos(o1)*cos(o2),2)+pow(p_joint.a*cos(o1)*sin(o2),2)+2*p_joint.t*p_joint.h*sin(o2)-2*p_joint.t*p_joint.a*cos(o2)-p_joint.y2*p_joint.y2-2*p_joint.y2*p_joint.b*cos(o1)+p_joint.l*p_joint.l+pow(p_joint.b*sin(o1),2)-p_joint.h*p_joint.h-p_joint.a*p_joint.a+2*p_joint.a*pow(cos(o1),2)*sin(o2)*p_joint.h*cos(o2)-p_joint.b*p_joint.b-2*p_joint.y2*p_joint.a*sin(o1)*sin(o2)-2*p_joint.y2*p_joint.h*sin(o1)*cos(o2)+pow(p_joint.s*p_joint.l,2)-pow(p_joint.s*p_joint.b,2)-pow(p_joint.s*p_joint.h,2)-pow(p_joint.s*p_joint.a,2)+2*p_joint.b*sin(o1)*p_joint.s*p_joint.t-pow(p_joint.s*p_joint.y2,2)-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.h*sin(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.b*cos(o1)-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.a*sin(o1)*sin(o2)-2*p_joint.a*sin(o1)*sin(o2)*p_joint.b*cos(o1)-2*p_joint.b*cos(o1)*p_joint.h*sin(o1)*cos(o2)-2*p_joint.a*cos(o1)*sin(o2)*p_joint.s*p_joint.t+2*p_joint.a*cos(o1)*pow(sin(o2),2)*p_joint.s*p_joint.h-2*p_joint.a*p_joint.a*cos(o1)*sin(o2)*p_joint.s*cos(o2)-2*p_joint.b*sin(o1)*p_joint.s*p_joint.h*sin(o2)+2*p_joint.b*sin(o1)*p_joint.s*p_joint.a*cos(o2)-2*p_joint.s*p_joint.t*p_joint.h*cos(o1)*cos(o2)+2*p_joint.s*p_joint.h*p_joint.h*sin(o2)*cos(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.h*sin(o2)*p_joint.a*cos(o2)-2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.s*p_joint.a-p_joint.t*p_joint.t+pow(p_joint.s*p_joint.h*sin(o2),2)+pow(p_joint.s*p_joint.a*cos(o2),2))*(8*p_joint.a*cos(o1)*sin(o2)*p_joint.s*p_joint.h*cos(o2)-2*p_joint.a*p_joint.a*cos(o1)*pow(cos(o2),2)*p_joint.s+2*p_joint.a*p_joint.a*cos(o1)*pow(sin(o2),2)*p_joint.s-2*p_joint.b*sin(o1)*p_joint.s*p_joint.h*cos(o2)-2*p_joint.b*sin(o1)*p_joint.s*p_joint.a*sin(o2)+2*p_joint.s*p_joint.t*p_joint.h*cos(o1)*sin(o2)-2*p_joint.s*pow(p_joint.h*sin(o2),2)*cos(o1)+2*p_joint.s*pow(p_joint.h*cos(o2),2)*cos(o1)-2*p_joint.s*p_joint.s*p_joint.h*pow(cos(o2),2)*p_joint.a-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.a*sin(o1)*cos(o2)-2*p_joint.a*cos(o1)*cos(o2)*p_joint.s*p_joint.t-2*p_joint.y2*p_joint.a*sin(o1)*cos(o2)+2*p_joint.y2*p_joint.h*sin(o1)*sin(o2)+2*p_joint.t*p_joint.h*cos(o2)+2*p_joint.s*p_joint.s*p_joint.y2*p_joint.h*sin(o1)*sin(o2)-2*pow(p_joint.h*cos(o1),2)*cos(o2)*sin(o2)+2*pow(p_joint.a*cos(o1),2)*sin(o2)*cos(o2)+2*p_joint.t*p_joint.a*sin(o2)-2*p_joint.a*pow(cos(o1)*sin(o2),2)*p_joint.h+2*p_joint.s*p_joint.s*p_joint.h*pow(sin(o2),2)*p_joint.a+2*pow(p_joint.s*p_joint.h,2)*sin(o2)*cos(o2)-2*pow(p_joint.s*p_joint.a,2)*cos(o2)*sin(o2)+2*p_joint.a*pow(cos(o1)*cos(o2),2)*p_joint.h-2*p_joint.a*sin(o1)*cos(o2)*p_joint.b*cos(o1)+2*p_joint.b*cos(o1)*p_joint.h*sin(o1)*sin(o2)));
	}else if(!strcmp(p_joint.name,"rankle") || !strcmp(p_joint.name,"lankle") || !strcmp(p_joint.name,"rwrist") || !strcmp(p_joint.name,"lwrist")){
		*j00 = -((-p_joint.a*cos(o1)*sin(o2)+p_joint.h*cos(o1)*cos(o2)+p_joint.b*sin(o1))*(p_joint.y1-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1)))/sqrt(-pow(p_joint.y1-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1),2)-pow(p_joint.x1-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l) + p_joint.a*sin(o1)*sin(o2)-p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1);
		*j01 = (-2*(-p_joint.a*sin(o1)*cos(o2)-p_joint.h*sin(o1)*sin(o2))*(p_joint.y1-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1))-2*(p_joint.a*sin(o2)-p_joint.h*cos(o2))*(p_joint.x1-p_joint.h*sin(o2)-p_joint.a*cos(o2)))/(2*sqrt(-pow(p_joint.y1-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1),2)-pow(p_joint.x1-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l))-p_joint.h*cos(o1)*sin(o2)-p_joint.a*cos(o1)*cos(o2);
		*j10 = -((-p_joint.a*cos(o1)*sin(o2)+p_joint.h*cos(o1)*cos(o2)-p_joint.b*sin(o1))*(p_joint.y2-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1)))/sqrt(-pow(p_joint.y2-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1),2)-pow(p_joint.x2-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l)+p_joint.a*sin(o1)*sin(o2)-p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1);
		*j11 = (-2*(-p_joint.a*sin(o1)*cos(o2)-p_joint.h*sin(o1)*sin(o2))*(p_joint.y2-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1))-2*(p_joint.a*sin(o2)-p_joint.h*cos(o2))*(p_joint.x2-p_joint.h*sin(o2)-p_joint.a*cos(o2)))/(2*sqrt(-pow(p_joint.y2-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1),2)-pow(p_joint.x2-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l))-p_joint.h*cos(o1)*sin(o2)-p_joint.a*cos(o1)*cos(o2);
	}else{
		return -1;
	}
	return 0;
}



int TorqueToActForce(const double th_r, const double th_p, const double tau_r, const double tau_p, double* f1, double* f2, p_2joint_t p_joint){
	double J[2][2],JTinv[2][2];
	double detJ;
	double o1 = th_r;
	double o2 = th_p;
	
	if(!strcmp(p_joint.name, "lhip") || !strcmp(p_joint.name, "rhip") || !strcmp(p_joint.name, "waist")){
		o1 = -o1;
		o2 = -o2;
		CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
	}else if(!strcmp(p_joint.name, "rshoulder")){
		o1 = -o1;
		CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
	}else if(!strcmp(p_joint.name, "lshoulder")){
		CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
	}else if(!strcmp(p_joint.name, "waist")){
		CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
	}else if(!strcmp(p_joint.name, "rankle") || !strcmp(p_joint.name, "lankle") || !strcmp(p_joint.name, "rwrist") || !strcmp(p_joint.name, "lwrist")){
		CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
	}else{
		return -1;
	}
	detJ = J[0][0]*J[1][1] - J[0][1]*J[1][0];
	JTinv[0][0] = J[1][1] / detJ;
	JTinv[0][1] = - J[1][0] /detJ;
	JTinv[1][0] = - J[0][1] / detJ;
	JTinv[1][1] = J[0][0] / detJ;
	
	//calc force and [kN] -> [N]
	if(!strcmp(p_joint.name, "lhip") || !strcmp(p_joint.name, "rhip") || !strcmp(p_joint.name, "waist")){
		*f1 = -1000.0*(JTinv[0][0]*tau_r + JTinv[0][1]*tau_p);
		*f2 = -1000.0*(JTinv[1][0]*tau_r + JTinv[1][1]*tau_p);
	}else if(!strcmp(p_joint.name, "rshoulder")){
		*f1 = 1000.0*(JTinv[0][0]*(-tau_r) + JTinv[0][1]*tau_p);
		*f2 = 1000.0*(JTinv[1][0]*(-tau_r) + JTinv[1][1]*tau_p);
	}else{
		*f1 = 1000.0*(JTinv[0][0]*tau_r + JTinv[0][1]*tau_p);
		*f2 = 1000.0*(JTinv[1][0]*tau_r + JTinv[1][1]*tau_p);
	}
	return 0;
}

int ActForceToTorque(const double th_r, const double th_p, const double f1, const double f2, double *tau_r, double *tau_p, p_2joint_t p_joint){
	double J[2][2];
	double o1,o2;
	
	o1 = th_r;o2 = th_p;
	
	if(!strcmp(p_joint.name, "lhip") || !strcmp(p_joint.name, "rhip") || !strcmp(p_joint.name, "waist")){
		o1 = -o1;
		o2 = -o2;
		CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
	}
	else if(!strcmp(p_joint.name, "rshoulder")){
		o1 = -o1;
		CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
	}else if(!strcmp(p_joint.name, "lshoulder")){
		CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
	}else if(!strcmp(p_joint.name, "waist")){
		CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
	}else if(!strcmp(p_joint.name, "rankle") || !strcmp(p_joint.name, "lankle") || !strcmp(p_joint.name, "rwrist") || !strcmp(p_joint.name, "lwrist")){
		CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
	}else{
		return -1;
	}
	
	//calc tarque and [Nmm] -> [Nm] 
	if(!strcmp(p_joint.name, "lhip") || !strcmp(p_joint.name, "rhip") || !strcmp(p_joint.name, "waist")){
		*tau_r = -(J[0][0]*f1 + J[1][0]*f2) / 1000.0;
		*tau_p = -(J[0][1]*f1 + J[1][1]*f2) / 1000.0;
	}else if(!strcmp(p_joint.name, "rshoulder")){
		*tau_r = -(J[0][0]*f1 + J[1][0]*f2) / 1000.0;
		*tau_p = (J[0][1]*f1 + J[1][1]*f2) / 1000.0;
	}else{
		*tau_r = (J[0][0]*f1 + J[1][0]*f2) / 1000.0;
		*tau_p = (J[0][1]*f1 + J[1][1]*f2) / 1000.0;
	}

	return 0;
}

int ActForceToKneeTorque(const double th_p, const double f1,const double f2, double *tau_p, p_1joint_t p_joint){
	double J1,J2;
	
	if(!strcmp(p_joint.name, "lknee")){
		J1 = p_joint.r*cos(th_p - 55*M_PI/180) + p_joint.r*sin(th_p - 55*M_PI/180)*(-p_joint.a + cos(th_p - 55*M_PI/180))/sqrt(p_joint.l*p_joint.l - pow((-p_joint.a + p_joint.r*cos(th_p - 55*M_PI/180)),2));
		J2 = -p_joint.r*cos(th_p - 55*M_PI/180) - p_joint.r*sin(th_p - 55*M_PI/180)*(p_joint.a - cos(th_p - 55*M_PI/180))/sqrt(p_joint.l*p_joint.l - pow((p_joint.a - p_joint.r*cos(th_p - 55*M_PI/180)),2));
		*tau_p = (J1*f1 + J2*f2) / 1000.0; 
	}else if(!strcmp(p_joint.name, "rknee")){
		J1 = -p_joint.r*cos(th_p - 55*M_PI/180) - p_joint.r*sin(th_p - 55*M_PI/180)*(p_joint.a - cos(th_p - 55*M_PI/180))/sqrt(p_joint.l*p_joint.l - pow((p_joint.a - p_joint.r*cos(th_p - 55*M_PI/180)),2));		
		J2 = p_joint.r*cos(th_p - 55*M_PI/180) + p_joint.r*sin(th_p - 55*M_PI/180)*(-p_joint.a + cos(th_p - 55*M_PI/180))/sqrt(p_joint.l*p_joint.l - pow((-p_joint.a + p_joint.r*cos(th_p - 55*M_PI/180)),2));
		*tau_p = (J1*f1 + J2*f2) / 1000.0;
	}else{
		return -1;
	}

	return 0;
}

int KneeTorqueToActForce(const double th_p, const double tau_p, double *f1, double *f2, p_1joint_t p_joint){
	double J1,J2;
	
	if(!strcmp(p_joint.name, "lknee")){
		J1 = p_joint.r*cos(th_p - 55*M_PI/180) + p_joint.r*sin(th_p - 55*M_PI/180)*(-p_joint.a + cos(th_p - 55*M_PI/180))/sqrt(p_joint.l*p_joint.l - pow((-p_joint.a + p_joint.r*cos(th_p - 55*M_PI/180)),2));
		J2 = -p_joint.r*cos(th_p - 55*M_PI/180) - p_joint.r*sin(th_p - 55*M_PI/180)*(p_joint.a - cos(th_p - 55*M_PI/180))/sqrt(p_joint.l*p_joint.l - pow((p_joint.a - p_joint.r*cos(th_p - 55*M_PI/180)),2));
		//*z_ans1 = p_joint.r*sin(q_cmd - 55*M_PI/180) + sqrt(p_joint.l*p_joint.l - pow((-p_joint.a + p_joint.r*cos(q_cmd - 55*M_PI/180)),2)) - p_joint.z1_0;
		//*z_ans2 = -p_joint.r*sin(q_cmd - 55*M_PI/180) + sqrt(p_joint.l*p_joint.l - pow((p_joint.a - p_joint.r*cos(q_cmd - 55*M_PI/180)),2)) - p_joint.z2_0;
		*f1 = tau_p / 2.0 / (J1 / 1000.0);
		*f2 = tau_p / 2.0 / (J2 / 1000.0);
	}else if(!strcmp(p_joint.name, "rknee")){
		J1 = -p_joint.r*cos(th_p - 55*M_PI/180) - p_joint.r*sin(th_p - 55*M_PI/180)*(p_joint.a - cos(th_p - 55*M_PI/180))/sqrt(p_joint.l*p_joint.l - pow((p_joint.a - p_joint.r*cos(th_p - 55*M_PI/180)),2));		
		J2 = p_joint.r*cos(th_p - 55*M_PI/180) + p_joint.r*sin(th_p - 55*M_PI/180)*(-p_joint.a + cos(th_p - 55*M_PI/180))/sqrt(p_joint.l*p_joint.l - pow((-p_joint.a + p_joint.r*cos(th_p - 55*M_PI/180)),2));
		//*z_ans1 = -p_joint.r*sin(q_cmd - 55*M_PI/180) + sqrt(p_joint.l*p_joint.l - pow((p_joint.a - p_joint.r*cos(q_cmd - 55*M_PI/180)),2)) - p_joint.z1_0;
		//*z_ans2 = p_joint.r*sin(q_cmd - 55*M_PI/180) + sqrt(p_joint.l*p_joint.l - pow((-p_joint.a + p_joint.r*cos(q_cmd - 55*M_PI/180)),2)) - p_joint.z2_0;
		*f1 = tau_p / 2.0 / (J1 / 1000.0);
		*f2 = tau_p / 2.0 / (J2 / 1000.0);
	}else{
		return -1;
	}
	return 0;
}

int ActForceToElbowTorque(const double th_p, const double f, double *tau_p, p_1joint_t p_joint){
	double J;
	if(!strcmp(p_joint.name, "relbow") || !strcmp(p_joint.name, "lelbow")){
		J = -p_joint.r*cos(th_p + + 56*M_PI/180) + p_joint.r*sin(th_p + 56*M_PI/180)*(-p_joint.a + p_joint.r*cos(th_p + 56*M_PI/180))/sqrt(p_joint.l*p_joint.l - pow(-p_joint.a + p_joint.r*cos(th_p + + 56*M_PI/180),2));
		*tau_p = f*J/1000.0;
	}else{
		return -1;
	}
		return 0;
}

int ElbowTorqueToActForce(const double th_p, const double tau_p, double *f, p_1joint_t p_joint){
	double J;
	if(!strcmp(p_joint.name, "relbow") || !strcmp(p_joint.name, "lelbow")){
		J = -p_joint.r*cos(th_p + + 56*M_PI/180) + p_joint.r*sin(th_p + 56*M_PI/180)*(-p_joint.a + p_joint.r*cos(th_p + 56*M_PI/180))/sqrt(p_joint.l*p_joint.l - pow(-p_joint.a + p_joint.r*cos(th_p + + 56*M_PI/180),2));
		*f = tau_p / (J / 1000.0);
	}else{
		return -1;
	}
		return 0;
}


/*
Encoder(v:vane, c:cylinder)
0:v rhip
1:c1 rhip
2:c2 rhip
3:c2 rknee
4:c1 rknee
5:dummy rknee
6:c1 rankle
7:c2 rankle
8:v lhip
9:c1 lhip
10:c2 lhip
11:c1 lknee
12:c2 lknee
13:dummy lknee
14:c1 lankle
15:c2 lankle
16:c1 waist
17:c2 waist
18:neck
19:v rscalpae
20:c1 rshoulder
21:c2 rshoulder
22:v rshoulder
23:c relbow
24:dummy relbow
25:v rwrist
26:c1 rwrist
27:c2 rwrist
28:v lscaplae
29:c1 lshoulder
30:c2 lshoulder
31:v lshoulder
32:c lelbow
33:dummy lelbow
34:v lwrist
35:c1 lwrist
36:c2 lwrist
*/

int ActForceToTorqueAll(const double *joint_angle, const double* input, double* output){
	
	//vane + neck
	output[0] = input[0];//rhip
	output[6] = input[8];//lhip
	output[14] = 0.0;//input[18];//neck
	output[15] = input[19];//rscalpae
	output[18] = input[22];//rshoulder
	output[20] = input[25];//rwrist
	output[23] = input[28];//lscaplae
	output[26] = input[31];//lshoulder
	output[28] = input[34];//lwrist
	
	//input[5],input[13] : knee dummy
	//input[24],input[33] : elbow dummy

	//hand
	memcpy(&output[31], &input[37], sizeof(double)*10);

	//cylinder
	//leg 2 axis joint
	ActForceToTorque(joint_angle[2], joint_angle[1], input[1], input[2], &output[2], &output[1], p_2joint_array[0]);//rhip
	ActForceToTorque(joint_angle[4], joint_angle[5], input[6], input[7], &output[4], &output[5], p_2joint_array[1]);//rankle
	ActForceToTorque(joint_angle[8], joint_angle[7], input[9], input[10], &output[8], &output[7], p_2joint_array[2]);//lhip
	ActForceToTorque(joint_angle[10], joint_angle[11], input[14], input[15], &output[10], &output[11], p_2joint_array[3]);//lankle
	//waist joint
	ActForceToTorque(joint_angle[13], joint_angle[12], input[16], input[17], &output[13], &output[12], p_2joint_array[4]);//waist
	//arm 2 axis joint
	ActForceToTorque(joint_angle[17], joint_angle[16], input[20], input[21], &output[17], &output[16], p_2joint_array[5]);//rshoulder
	ActForceToTorque(joint_angle[21], joint_angle[22], input[26], input[27], &output[21], &output[22], p_2joint_array[6]);//rwrist
	ActForceToTorque(joint_angle[25], joint_angle[24], input[29], input[30], &output[25], &output[24], p_2joint_array[7]);//lshoulder
	ActForceToTorque(joint_angle[29], joint_angle[30], input[35], input[36], &output[29], &output[30], p_2joint_array[8]);//lwrist
	//1 axis joint
	ActForceToKneeTorque(joint_angle[3], input[4], input[3], &output[3], p_1joint_array[0]);//rknee
	ActForceToKneeTorque(joint_angle[9], input[11], input[12], &output[9], p_1joint_array[1]);//lknee
	ActForceToElbowTorque(joint_angle[19], input[23], &output[19], p_1joint_array[2]);//relbow
	ActForceToElbowTorque(joint_angle[27], input[32], &output[27], p_1joint_array[3]);//lelbow

	return 0;
}


int TorqueToActForceAll(const double* joint_angle, const double* input, double* output){

	//vane + neck
	output[0] = input[0];//rhip
	output[8] = input[6];//lhip
	output[18] = 0.0;//input[14];//neck
	output[19] = input[15];//rscalpae
	output[22] = input[18];//rshoulder
	output[25] = input[20];//rwrist
	output[28] = input[23];//lscaplae
	output[31] = input[26];//lshoulder
	output[34] = input[28];//lwrist
	
	output[5] = 0.0;//rknee dummy
	output[13] = 0.0;//lknee dummy
	output[24] = 0.0;//relbow dummy
	output[33] = 0.0;//lelbow dummy

	//hand
	memcpy(&output[37], &input[31], sizeof(double)*10);
	//cylinder
	//leg 2 axis joint
	TorqueToActForce(joint_angle[2], joint_angle[1], input[2], input[1], &output[1], &output[2], p_2joint_array[0]);//rhip
	TorqueToActForce(joint_angle[4], joint_angle[5], input[4], input[5], &output[6], &output[7], p_2joint_array[1]);//rankle
	TorqueToActForce(joint_angle[8], joint_angle[7], input[8], input[7], &output[9], &output[10], p_2joint_array[2]);//lhip
	TorqueToActForce(joint_angle[10], joint_angle[11], input[10], input[11], &output[14], &output[15], p_2joint_array[3]);//lankle
	//waist joint
	TorqueToActForce(joint_angle[13], joint_angle[12], input[13], input[12], &output[16], &output[17], p_2joint_array[4]);//waist
	//arm 2 axis joint
	TorqueToActForce(joint_angle[17], joint_angle[16], input[17], input[16], &output[20], &output[21], p_2joint_array[5]);//rshoulder
	TorqueToActForce(joint_angle[21], joint_angle[22], input[21], input[22], &output[26], &output[27], p_2joint_array[6]);//rwrist
	TorqueToActForce(joint_angle[25], joint_angle[24], input[25], input[24], &output[29], &output[30], p_2joint_array[7]);//lshoulder
	TorqueToActForce(joint_angle[29], joint_angle[30], input[29], input[30], &output[35], &output[36], p_2joint_array[8]);//rwrist
	//1 axis joint
	KneeTorqueToActForce(joint_angle[3], input[3], &output[4], &output[3], p_1joint_array[0]);//rknee
	KneeTorqueToActForce(joint_angle[9], input[9], &output[11], &output[12], p_1joint_array[1]);//lknee
	ElbowTorqueToActForce(joint_angle[19], input[19], &output[23], p_1joint_array[2]);//relbow
	ElbowTorqueToActForce(joint_angle[27], input[27], &output[32], p_1joint_array[3]);//lelbow

	return 0;
}

