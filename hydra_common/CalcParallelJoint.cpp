#include "CalcParallelJoint.h"
#include "hydra_types.h"

#define IK_GAIN 0.9

int JointToCylinder(const double q_cmd1, const double q_cmd2, double* z_ans1, double* z_ans2, p_2joint_t p_joint){
	double o1 = q_cmd1;
	double o2 = q_cmd2;
	
	if(!strcmp(p_joint.name, "rhip") || !strcmp(p_joint.name,"lhip")){
		o1 = -o1;o2 = -o2;
		*z_ans1 =-sqrt(-pow(p_joint.y1-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1),2)-pow(p_joint.x1-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l)-p_joint.a*cos(o1)*sin(o2)+p_joint.h*cos(o1)*cos(o2)+p_joint.b*sin(o1);
		*z_ans2 =-sqrt(-pow(p_joint.y2-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1),2)-pow(p_joint.x2-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l)-p_joint.a*cos(o1)*sin(o2)+p_joint.h*cos(o1)*cos(o2)-p_joint.b*sin(o1);
		*z_ans1 = (*z_ans1 - p_joint.z1_0)/1000.0;
		*z_ans2 = (*z_ans2 - p_joint.z2_0)/1000.0;
	}else if(!strcmp(p_joint.name,"rshoulder")){
		o1 = -o1;
		*z_ans1 = -p_joint.a*sin(o2)+p_joint.h*cos(o1)*cos(o2)+p_joint.b*sin(o1)*cos(o2)+sqrt(pow(p_joint.a*sin(o2),2)-2*p_joint.a*cos(o2)*p_joint.h*cos(o1)*sin(o2)-2*p_joint.b*sin(o1)*cos(o2)*p_joint.a*sin(o2)+pow(p_joint.h*cos(o1)*cos(o2),2)+2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.b*sin(o1)+pow(p_joint.b*sin(o1)*cos(o2),2)-p_joint.x1*p_joint.x1-p_joint.y1*p_joint.y1+2*p_joint.y1*p_joint.b*cos(o1)+2*p_joint.x1*p_joint.b*sin(o1)*sin(o2)-2*p_joint.y1*p_joint.h*sin(o1)-p_joint.h*p_joint.h-p_joint.a*p_joint.a-p_joint.b*p_joint.b+p_joint.l*p_joint.l+2*p_joint.x1*p_joint.h*cos(o1)*sin(o2)+2*p_joint.x1*p_joint.a*cos(o2));
		*z_ans2 = p_joint.b*sin(o1)*cos(o2)+p_joint.h*cos(o1)*cos(o2)+p_joint.a*sin(o2)+sqrt(pow(p_joint.b*sin(o1)*cos(o2),2)+2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.b*sin(o1)+2*p_joint.b*sin(o1)*cos(o2)*p_joint.a*sin(o2)+pow(p_joint.h*cos(o1)*cos(o2),2)+2*p_joint.a*cos(o2)*p_joint.h*cos(o1)*sin(o2)+pow(p_joint.a*sin(o2),2)+2*p_joint.x2*p_joint.b*sin(o1)*sin(o2)+2*p_joint.y2*p_joint.b*cos(o1)-p_joint.a*p_joint.a-p_joint.b*p_joint.b+p_joint.l*p_joint.l-p_joint.h*p_joint.h-p_joint.y2*p_joint.y2-2*p_joint.x2*p_joint.a*cos(o2)+2*p_joint.x2*p_joint.h*cos(o1)*sin(o2)-p_joint.x2*p_joint.x2-2*p_joint.y2*p_joint.h*sin(o1));
		*z_ans1 = (*z_ans1 - p_joint.z1_0)/1000.0;
		*z_ans2 = (*z_ans2 - p_joint.z2_0)/1000.0;
	}else if(!strcmp(p_joint.name,"lshoulder")){
		*z_ans1 = -p_joint.a*sin(o2)+p_joint.h*cos(o1)*cos(o2)+p_joint.b*sin(o1)*cos(o2)+sqrt(pow(p_joint.a*sin(o2),2)-2*p_joint.a*cos(o2)*p_joint.h*cos(o1)*sin(o2)-2*p_joint.b*sin(o1)*cos(o2)*p_joint.a*sin(o2)+pow(p_joint.h*cos(o1)*cos(o2),2)+2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.b*sin(o1)+pow(p_joint.b*sin(o1)*cos(o2),2)-p_joint.x1*p_joint.x1-p_joint.y1*p_joint.y1+2*p_joint.y1*p_joint.b*cos(o1)+2*p_joint.x1*p_joint.b*sin(o1)*sin(o2)-2*p_joint.y1*p_joint.h*sin(o1)-p_joint.h*p_joint.h-p_joint.a*p_joint.a-p_joint.b*p_joint.b+p_joint.l*p_joint.l+2*p_joint.x1*p_joint.h*cos(o1)*sin(o2)+2*p_joint.x1*p_joint.a*cos(o2));
		*z_ans2 = p_joint.b*sin(o1)*cos(o2)+p_joint.h*cos(o1)*cos(o2)+p_joint.a*sin(o2)+sqrt(pow(p_joint.b*sin(o1)*cos(o2),2)+2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.b*sin(o1)+2*p_joint.b*sin(o1)*cos(o2)*p_joint.a*sin(o2)+pow(p_joint.h*cos(o1)*cos(o2),2)+2*p_joint.a*cos(o2)*p_joint.h*cos(o1)*sin(o2)+pow(p_joint.a*sin(o2),2)+2*p_joint.x2*p_joint.b*sin(o1)*sin(o2)+2*p_joint.y2*p_joint.b*cos(o1)-p_joint.a*p_joint.a-p_joint.b*p_joint.b+p_joint.l*p_joint.l-p_joint.h*p_joint.h-p_joint.y2*p_joint.y2-2*p_joint.x2*p_joint.a*cos(o2)+2*p_joint.x2*p_joint.h*cos(o1)*sin(o2)-p_joint.x2*p_joint.x2-2*p_joint.y2*p_joint.h*sin(o1));
		*z_ans1 = (*z_ans1 - p_joint.z1_0)/1000.0;
		*z_ans2 = (*z_ans2 - p_joint.z2_0)/1000.0;
	}else if(!strcmp(p_joint.name,"waist")){
		o1 = -o1;o2 = -o2;
		*z_ans1 = 1/2.0/(1+p_joint.s*p_joint.s)*(2*p_joint.a*cos(o1)*sin(o2)+2*p_joint.b*sin(o1)-2*p_joint.s*p_joint.t+2*p_joint.s*p_joint.h*sin(o2)+2*p_joint.h*cos(o1)*cos(o2)-2*p_joint.s*p_joint.a*cos(o2)+2*sqrt(pow(p_joint.h*cos(o1)*cos(o2),2)+pow(p_joint.a*cos(o1)*sin(o2),2)+2*p_joint.y1*p_joint.b*cos(o1)+2*p_joint.t*p_joint.h*sin(o2)-2*p_joint.t*p_joint.a*cos(o2)+p_joint.l*p_joint.l-2*p_joint.y1*p_joint.a*sin(o1)*sin(o2)-2*p_joint.y1*p_joint.h*sin(o1)*cos(o2)+pow(p_joint.b*sin(o1),2)-p_joint.h*p_joint.h-p_joint.a*p_joint.a-p_joint.y1*p_joint.y1+2*p_joint.a*pow(cos(o1),2)*sin(o2)*p_joint.h*cos(o2)-p_joint.b*p_joint.b+pow(p_joint.s*p_joint.l,2)-pow(p_joint.s*p_joint.b,2)-pow(p_joint.s*p_joint.h,2)-pow(p_joint.s*p_joint.y1,2)-pow(p_joint.s*p_joint.a,2)-2*p_joint.b*sin(o1)*p_joint.s*p_joint.t+2*p_joint.s*p_joint.s*p_joint.y1*p_joint.b*cos(o1)+2*p_joint.a*sin(o1)*sin(o2)*p_joint.b*cos(o1)+2*p_joint.b*cos(o1)*p_joint.h*sin(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.y1*p_joint.h*sin(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.y1*p_joint.a*sin(o1)*sin(o2)-2*p_joint.a*cos(o1)*sin(o2)*p_joint.s*p_joint.t+2*p_joint.a*cos(o1)*pow(sin(o2),2)*p_joint.s*p_joint.h-2*p_joint.a*p_joint.a*cos(o1)*sin(o2)*p_joint.s*cos(o2)+2*p_joint.b*sin(o1)*p_joint.s*p_joint.h*sin(o2)-2*p_joint.b*sin(o1)*p_joint.s*p_joint.a*cos(o2)-2*p_joint.s*p_joint.t*p_joint.h*cos(o1)*cos(o2)+2*p_joint.s*p_joint.h*p_joint.h*sin(o2)*cos(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.h*sin(o2)*p_joint.a*cos(o2)-2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.s*p_joint.a-p_joint.t*p_joint.t+pow(p_joint.s*p_joint.h*sin(o2),2)+pow(p_joint.s*p_joint.a*cos(o2),2)));
		*z_ans2 = 1/2.0/(1+p_joint.s*p_joint.s)*(2*p_joint.h*cos(o1)*cos(o2)+2*p_joint.a*cos(o1)*sin(o2)-2*p_joint.b*sin(o1)-2*p_joint.s*p_joint.t-2*p_joint.s*p_joint.a*cos(o2)+2*p_joint.s*p_joint.h*sin(o2)+2*sqrt(pow(p_joint.h*cos(o1)*cos(o2),2)+pow(p_joint.a*cos(o1)*sin(o2),2)+2*p_joint.t*p_joint.h*sin(o2)-2*p_joint.t*p_joint.a*cos(o2)-p_joint.y2*p_joint.y2-2*p_joint.y2*p_joint.b*cos(o1)+p_joint.l*p_joint.l+pow(p_joint.b*sin(o1),2)-p_joint.h*p_joint.h-p_joint.a*p_joint.a+2*p_joint.a*pow(cos(o1),2)*sin(o2)*p_joint.h*cos(o2)-p_joint.b*p_joint.b-2*p_joint.y2*p_joint.a*sin(o1)*sin(o2)-2*p_joint.y2*p_joint.h*sin(o1)*cos(o2)+pow(p_joint.s*p_joint.l,2)-pow(p_joint.s*p_joint.b,2)-pow(p_joint.s*p_joint.h,2)-pow(p_joint.s*p_joint.a,2)+2*p_joint.b*sin(o1)*p_joint.s*p_joint.t-pow(p_joint.s*p_joint.y2,2)-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.h*sin(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.b*cos(o1)-2*p_joint.s*p_joint.s*p_joint.y2*p_joint.a*sin(o1)*sin(o2)-2*p_joint.a*sin(o1)*sin(o2)*p_joint.b*cos(o1)-2*p_joint.b*cos(o1)*p_joint.h*sin(o1)*cos(o2)-2*p_joint.a*cos(o1)*sin(o2)*p_joint.s*p_joint.t+2*p_joint.a*cos(o1)*pow(sin(o2),2)*p_joint.s*p_joint.h-2*p_joint.a*p_joint.a*cos(o1)*sin(o2)*p_joint.s*cos(o2)-2*p_joint.b*sin(o1)*p_joint.s*p_joint.h*sin(o2)+2*p_joint.b*sin(o1)*p_joint.s*p_joint.a*cos(o2)-2*p_joint.s*p_joint.t*p_joint.h*cos(o1)*cos(o2)+2*p_joint.s*p_joint.h*p_joint.h*sin(o2)*cos(o1)*cos(o2)-2*p_joint.s*p_joint.s*p_joint.h*sin(o2)*p_joint.a*cos(o2)-2*p_joint.h*cos(o1)*pow(cos(o2),2)*p_joint.s*p_joint.a-p_joint.t*p_joint.t+pow(p_joint.s*p_joint.h*sin(o2),2)+pow(p_joint.s*p_joint.a*cos(o2),2)));
		*z_ans1 = (*z_ans1 - p_joint.z1_0)/1000.0*2.0/sqrt(3);
		*z_ans2 = (*z_ans2 - p_joint.z2_0)/1000.0*2.0/sqrt(3);
	}else if(!strcmp(p_joint.name,"rankle") || !strcmp(p_joint.name,"lankle") || !strcmp(p_joint.name,"rwrist") || !strcmp(p_joint.name,"lwrist")){
		*z_ans1 = sqrt(-pow(p_joint.y1-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)-p_joint.b*cos(o1),2)-pow(p_joint.x1-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l)-p_joint.a*cos(o1)*sin(o2)+p_joint.h*cos(o1)*cos(o2)+p_joint.b*sin(o1);
		*z_ans2 = sqrt(-pow(p_joint.y2-p_joint.a*sin(o1)*sin(o2)+p_joint.h*sin(o1)*cos(o2)+p_joint.b*cos(o1),2)-pow(p_joint.x2-p_joint.h*sin(o2)-p_joint.a*cos(o2),2)+p_joint.l*p_joint.l)-p_joint.a*cos(o1)*sin(o2)+p_joint.h*cos(o1)*cos(o2)-p_joint.b*sin(o1);
		*z_ans1 = (*z_ans1 - p_joint.z1_0)/1000.0;
		*z_ans2 = (*z_ans2 - p_joint.z2_0)/1000.0;
	}else{
		return -1;
	}
	return 0;
}

int CylinderToJoint(const double z_cmd1, const double z_cmd2, double *q_ans1, double *q_ans2, const double pre_q1, const double pre_q2, const int zd_max, p_2joint_t p_joint){

	double J[2][2],Jinv[2][2],detJ;	// zdot = J * qdot ※Jはヤコビアンのようなもの
	double z[2],zd[2];		//z:更新中のシリンダ変位,	zd:zの微分,
	                                //		z_cmd:シリンダ指令値or計測値
	const double z_0[2] = {p_joint.z1_0/1000.0, p_joint.z2_0/1000.0};//[mm]
	double q[2],qd[2];													//q:更新中の関節角,	qd:qの微分
	//const double q0[2] = {0.0,0.0};						//q0:関節各初期位置[rad]
	
	double z_cmd1_mm,z_cmd2_mm;
	double pre_z1, pre_z2;

	JointToCylinder( pre_q1, pre_q2, &pre_z1, &pre_z2, p_joint);

	*q_ans1 = pre_q1;
	*q_ans2 = pre_q2;

	z_cmd1_mm = z_cmd1*1000;
	z_cmd2_mm = z_cmd2*1000;
	if(!strcmp(p_joint.name,"waist")){
		z_cmd1_mm = z_cmd1_mm*sqrt(3)/2.0;
		z_cmd2_mm = z_cmd2_mm*sqrt(3)/2.0;
	}
	
	double z_cmd1_offset = z_cmd1_mm + p_joint.z1_0;
	double z_cmd2_offset = z_cmd2_mm + p_joint.z2_0;
	
	const double DELTA = 0.001;							//打ち切りのz誤差しきい値
	const int MAXLOOP = 10;					//強制的に計算を終了する最大ループ数
	double o1,o2;
	z[0] = pre_z1;//z_0[0];//[m]
	z[1] = pre_z2;//z_0[1];//[m]								//初期化
	
	q[0] = 0;q[1] = 0;					//初期化
	int loop =0;
	//struct timeval time_zero, time_now;
    //gettimeofday(&time_zero, NULL);
	while(1){
		o1 = q[0];o2 = q[1];
		if(!strcmp(p_joint.name,"lhip") || !strcmp(p_joint.name,"rhip")){
			CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
		}else if(!strcmp(p_joint.name,"rshoulder") || !strcmp(p_joint.name,"lshoulder")){
			CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
		}else if(!strcmp(p_joint.name, "waist")){
			CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
		}else if(!strcmp(p_joint.name,"rankle") || !strcmp(p_joint.name,"lankle") || !strcmp(p_joint.name,"rwrist") || !strcmp(p_joint.name,"lwrist")){
			CalcJacobian(o1,o2,&J[0][0],&J[0][1],&J[1][0],&J[1][1],p_joint);
		}else{
			return -1;
		}
		//ヤコビアンの逆行列計算
		detJ = J[0][0]*J[1][1] - J[0][1]*J[1][0];	//if(detJ==0){ hydra has no singular point}
		Jinv[0][0] = J[1][1]/detJ;
		Jinv[0][1] = -J[0][1]/detJ;
		Jinv[1][0] = -J[1][0]/detJ;
		Jinv[1][1] = J[0][0]/detJ;
		//zの微分，qの微分計算
		zd[0] = z_cmd1_mm - z[0]*1000;
		zd[1] = z_cmd2_mm - z[1]*1000;
		
		if(zd_max != 0){
			if(zd[0] > zd_max) zd[0] = zd_max;
			if(zd[0] < -1*zd_max) zd[0] = -1*zd_max;
			if(zd[1] > zd_max) zd[1] = zd_max;
			if(zd[1] < -1*zd_max) zd[1] = -1*zd_max;
		}
		
		qd[0] = Jinv[0][0]*zd[0] + Jinv[0][1]*zd[1];
		qd[1] = Jinv[1][0]*zd[0] + Jinv[1][1]*zd[1];
	
	
		//q積分して更新
		q[0] = q[0] + IK_GAIN*qd[0];
		q[1] = q[1] + IK_GAIN*qd[1];

		
		if(!strcmp(p_joint.name,"rhip") || !strcmp(p_joint.name,"lhip") || !strcmp(p_joint.name, "waist")){	
			o1 = -q[0];
			o2 = -q[1];
			*q_ans1 = -q[0];
		 	*q_ans2 = -q[1];
		}else if(!strcmp(p_joint.name, "rshoulder")){
			o1 = -q[0];
			o2 = q[1];
			*q_ans1 = -q[0];
			*q_ans2 = q[1];
		}else{
			o1 = q[0];
			o2 = q[1];
			*q_ans1 = q[0];
			*q_ans2 = q[1];
		}
	
		JointToCylinder(o1,o2,&z[0],&z[1],p_joint);
		//if(p_joint.name == "rhip")printf("z[0] = %lf\no2 = %lf\n",z[0],o2);
		//printf("J=%f,%f,%f,%f detJ=%f z=%f,%f q=%f,%f\n",J[0][0],J[0][1],J[1][0],J[1][1],detJ,z[0],z[1],q[0],q[1]);
		loop++;
		if(fabs(z_cmd1_mm-z[0]*1000)<DELTA && fabs(z_cmd2_mm-z[1]*1000)<DELTA){break;}
		if(loop>=MAXLOOP){
			//printf("Reached Max Loop Limit\n");
			return -1;
		}
	}
	
	//gettimeofday(&time_now, NULL);
	//time = (double)(time_now.tv_sec - time_zero.tv_sec) + (time_now.tv_usec - time_zero.tv_usec)/1000000.0;
	//printf("Calc finished in %d [roop] = %f [msec]\n",roop,time*1000);
	return loop;
}

int CylinderToKnee(const double z_cmd1,const double z_cmd2, double *q_ans, p_1joint_t p_joint){
	double z1_cmd_offset, z2_cmd_offset,q1,q2;
	
	if(!strcmp(p_joint.name,"lknee")){
		z1_cmd_offset = z_cmd1*1000 + p_joint.z2_0;
		z2_cmd_offset = z_cmd2*1000 + p_joint.z1_0;
		q1 = asin((p_joint.a*p_joint.a + z1_cmd_offset*z1_cmd_offset + p_joint.r*p_joint.r - p_joint.l*p_joint.l)/(2*p_joint.r*sqrt(p_joint.a*p_joint.a + z1_cmd_offset*z1_cmd_offset))) - atan(p_joint.a/z1_cmd_offset) + 55*M_PI/180;
		q2 = asin((p_joint.l*p_joint.l - p_joint.a*p_joint.a - z2_cmd_offset*z2_cmd_offset - p_joint.r*p_joint.r)/(2*p_joint.r*sqrt(p_joint.a*p_joint.a + z2_cmd_offset*z2_cmd_offset))) - atan(-p_joint.a/z2_cmd_offset) + 55*M_PI/180;	
        *q_ans = q1;
	}else if(!strcmp(p_joint.name,"rknee")){
		z1_cmd_offset = z_cmd1*1000 + p_joint.z1_0;
		z2_cmd_offset = z_cmd2*1000 + p_joint.z2_0;
		q1 = asin((p_joint.l*p_joint.l - p_joint.a*p_joint.a - z1_cmd_offset*z1_cmd_offset - p_joint.r*p_joint.r)/(2*p_joint.r*sqrt(p_joint.a*p_joint.a + z1_cmd_offset*z1_cmd_offset))) - atan(-p_joint.a/z1_cmd_offset) + 55*M_PI/180;
		q2 = asin((p_joint.a*p_joint.a + z2_cmd_offset*z2_cmd_offset + p_joint.r*p_joint.r - p_joint.l*p_joint.l)/(2*p_joint.r*sqrt(p_joint.a*p_joint.a + z2_cmd_offset*z2_cmd_offset))) - atan(p_joint.a/z2_cmd_offset) + 55*M_PI/180;	
		*q_ans = q2;//use single encoder only
		//        *q_ans = q1;//use tandem encoder only
	}else{
		return -1;
	}
	//*q_ans = (q1 + q2) / 2.0;


	return 0;
}

int KneeToCylinder(const double q_cmd, double *z_ans1, double *z_ans2, p_1joint_t p_joint){
	if(!strcmp(p_joint.name, "lknee")){
		*z_ans1 = p_joint.r*sin(q_cmd - 55*M_PI/180) + sqrt(p_joint.l*p_joint.l - pow((-p_joint.a + p_joint.r*cos(q_cmd - 55*M_PI/180)),2));
		*z_ans2 = -p_joint.r*sin(q_cmd - 55*M_PI/180) + sqrt(p_joint.l*p_joint.l - pow((p_joint.a - p_joint.r*cos(q_cmd - 55*M_PI/180)),2));
	}else if(!strcmp(p_joint.name, "rknee")){	
		*z_ans1 = -p_joint.r*sin(q_cmd - 55*M_PI/180) + sqrt(p_joint.l*p_joint.l - pow((p_joint.a - p_joint.r*cos(q_cmd - 55*M_PI/180)),2));
		*z_ans2 = p_joint.r*sin(q_cmd - 55*M_PI/180) + sqrt(p_joint.l*p_joint.l - pow((-p_joint.a + p_joint.r*cos(q_cmd - 55*M_PI/180)),2));
	}else{
		return -1;
	}
	*z_ans1 = (*z_ans1 - p_joint.z1_0)/1000.0;
	*z_ans2 = (*z_ans2 - p_joint.z2_0)/1000.0;
	return 0;
}

int CylinderToElbow(const double z_cmd, double *q_ans, p_1joint_t p_joint){
	if(!strcmp(p_joint.name,"relbow") || !strcmp(p_joint.name,"lelbow")){
		double z_cmd_offset = z_cmd*1000 + p_joint.z1_0;
		*q_ans = asin((p_joint.l*p_joint.l - p_joint.a*p_joint.a - p_joint.r*p_joint.r - z_cmd_offset*z_cmd_offset)/(2*p_joint.r*sqrt(p_joint.a*p_joint.a + z_cmd_offset*z_cmd_offset))) + atan(p_joint.a/z_cmd_offset) - 56*M_PI/180;
	}else{
		return -1;
	}
		return 0;
}
int ElbowToCylinder(const double q_cmd, double *z_ans, p_1joint_t p_joint){
	if(!strcmp(p_joint.name,"relbow") || !strcmp(p_joint.name,"lelbow")){
		*z_ans = - p_joint.r*sin(q_cmd + 56*M_PI/180) + sqrt(p_joint.l*p_joint.l - pow((-p_joint.a + p_joint.r*cos(q_cmd + 56*M_PI/180)),2));
	}else{
		return -1;
	}
	*z_ans = (*z_ans - p_joint.z1_0)/1000.0;
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
#define MAX_RENEWAL 5
int CylinderToJointAll(const double* input, double* output, const double* pre_input){
	
  memset(output, 0 , sizeof(double)*HYDRA_JNT_MAX);
	//vane + neck
	output[0] = input[0];//rhip
	output[6] = input[8];//lhip
	output[14] = input[18];//neck
	output[15] = input[19];//rscalpae
	output[18] = input[22];//rshoulder
	output[20] = input[25];//rwrist
	output[23] = input[28];//lscaplae
	output[26] = input[31];//lshoulder
	output[28] = input[34];//lwrist
	memcpy(&(output[31]), &(input[37]), sizeof(double)*10);
	//input[5],input[13] : knee dummy
	//input[24],input[33] : elbow dummy
	
	//cylinder
	//leg 2 axis joint
	CylinderToJoint(input[1], input[2], &output[2], &output[1], pre_input[1], pre_input[2], MAX_RENEWAL, p_2joint_array[0]);//rhip
	CylinderToJoint(input[6], input[7], &output[4], &output[5], pre_input[6], pre_input[7], MAX_RENEWAL, p_2joint_array[1]);//rankle
	CylinderToJoint(input[9], input[10], &output[8], &output[7], pre_input[9], pre_input[10], MAX_RENEWAL, p_2joint_array[2]);//lhip
	CylinderToJoint(input[14], input[15], &output[10], &output[11], pre_input[14], pre_input[15], MAX_RENEWAL, p_2joint_array[3]);//lankle
	//waist joint
	CylinderToJoint(input[16], input[17], &output[13], &output[12], pre_input[16], pre_input[17], MAX_RENEWAL, p_2joint_array[4]);//waist
	//arm 2 axis joint
	CylinderToJoint(input[20], input[21], &output[17], &output[16], pre_input[20], pre_input[21], MAX_RENEWAL, p_2joint_array[5]);//rshoulder
	CylinderToJoint(input[26], input[27], &output[21], &output[22], pre_input[26], pre_input[27], MAX_RENEWAL, p_2joint_array[6]);//rwrist
	CylinderToJoint(input[29], input[30], &output[25], &output[24], pre_input[29], pre_input[30], MAX_RENEWAL, p_2joint_array[7]);//lshoulder
	CylinderToJoint(input[35], input[36], &output[29], &output[30], pre_input[35], pre_input[36], MAX_RENEWAL, p_2joint_array[8]);//lwrist
	//1 axis joint
	CylinderToKnee(input[4], input[3], &output[3], p_1joint_array[0]);//rknee
	CylinderToKnee(input[11], input[12], &output[9], p_1joint_array[1]);//lknee
	CylinderToElbow(input[23], &output[19], p_1joint_array[2]);//relbow
	CylinderToElbow(input[32], &output[27], p_1joint_array[3]);//lelbow

	return 0;
}


int JointToCylinderAll(const double* input, double* output){

  memset(output, 0, sizeof(double)*EHA_MAX);

	//vane + neck
	output[0] = input[0];//rhip
	output[8] = input[6];//lhip
	output[18] = input[14];//neck
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

	//	memcpy( &(output[37]), &(input[31]), sizeof(double)*10);
	output[37]=input[31];
	output[38]=input[32];
	output[39]=input[33];
	output[40]=input[34];
	output[41]=input[35];
	output[42]=input[36];
	output[43]=input[37];
	output[44]=input[38];
	output[45]=input[39];
	output[46]=input[40];

	//cylinder
	//leg 2 axis joint
	JointToCylinder(input[2], input[1], &output[1], &output[2], p_2joint_array[0]);//rhip
	JointToCylinder(input[4], input[5], &output[6], &output[7], p_2joint_array[1]);//rankle
	JointToCylinder(input[8], input[7], &output[9], &output[10], p_2joint_array[2]);//lhip
	JointToCylinder(input[10], input[11], &output[14], &output[15], p_2joint_array[3]);//lankle
	//waist joint
	JointToCylinder(input[13], input[12], &output[16], &output[17], p_2joint_array[4]);//waist
	//arm 2 axis joint
	JointToCylinder(input[17], input[16], &output[20], &output[21], p_2joint_array[5]);//rshoulder
	JointToCylinder(input[21], input[22], &output[26], &output[27], p_2joint_array[6]);//rwrist
	JointToCylinder(input[25], input[24], &output[29], &output[30], p_2joint_array[7]);//lshoulder
	JointToCylinder(input[29], input[30], &output[35], &output[36], p_2joint_array[8]);//rwrist
	//1 axis joint
	KneeToCylinder(input[3], &output[4], &output[3], p_1joint_array[0]);//rknee
	KneeToCylinder(input[9], &output[11], &output[12], p_1joint_array[1]);//lknee
	ElbowToCylinder(input[19], &output[23], p_1joint_array[2]);//relbow
	ElbowToCylinder(input[27], &output[32], p_1joint_array[3]);//lelbow
	


	return 0;
}

