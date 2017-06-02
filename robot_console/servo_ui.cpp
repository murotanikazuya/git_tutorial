
/* --Attention-- */
/* A usc tool generated it automatically. */
/* DO NOT EDIT - file is regenerated whenever the FSM changes */

#undef USE_EFD
//#define USE_RTP
//#define USE_UDP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <pthread.h>
#include <ncurses.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <semaphore.h>

#include <iostream>

#include "servo_ui.h"

#include "comm_common_def.h"

#include "hydra.h"
#include "CalcParallelJoint.h"
#include "CalcForceTorque.h"
#include "user_interface.h"
#include "hydra_types.h"

#include "chydrashmclient.h"
//#include "chydradatamanager.h"
#include "chydradata.h"
#include "cthreaddata.h"
#include "roshydra.h"
#include "chydradatalogger.h"

#include "cipcomm.h"

#include "walk.h"
CIPComm   *p_comm = NULL;

#define BUF_SIZE          8192
#define DEFAULT_TGT_ADDR  ("127.0.0.1")
#define DEFAULT_PORT_BASE 12300
#define DEFAULT_TGT_PORT  12400
//static unsigned char tx_buffer[BUF_SIZE];
//static unsigned char rx_buffer[BUF_SIZE];
static char tgt_addr[16];
static int  port_base;
static int  tgt_port;

#ifdef USE_RTP
#include "chydrajrtpcomm.h"
CHydraJrtpComm *p_rtp_comm = NULL;
#endif

#ifdef USE_UDP
#include "cudpcomm.h"
CUDPComm *p_udp_comm = NULL;
#endif

//#include "common_def.h"
#define LIM_FULL_VERSION
#include "robot_id.h"
#include "robot_hydra_id.h"
#include "7arm_id.h"

//#define EHA_ON 0x0101
//#define EHA_OFF 0x0100

// shared memory and semaphore
#ifndef SHM_IN_NAME
#define SHM_IN_NAME             ("/tmp/shm/HydraIn")
#endif

#ifndef SHM_OUT_NAME
#define SHM_OUT_NAME            ("/tmp/shm/HydraOut")
#endif

#ifndef SEM_ECATSYNC_NAME
#define SEM_ECATSYNC_NAME ("/HydraSync")
#endif

//#define POS_STEP DEG2RAD(0.004)
#define POS_STEP DEG2RAD(0.4)

CHydraShmClient *p_hydra_shm = NULL;

//CHydraDataManager hydra_data;
//CHydraDataManager* hydraData;
//CHydraData* hydraData;
static CthreadData* hydraData;
//joint_state_t  jnt_state[58];
//joint_cmd_t    jnt_cmd[HYDRA_JNT_MAX];
//eha_state_t    eha_state[EHA_MAX];
//eha_cmd_t      eha_cmd[EHA_MAX];
//int            buffer_index = 0;
double all_joint_tau_fk[HYDRA_JNT_MAX];
//double all_joint_pos_tgt[HYDRA_JNT_MAX];
//double all_joint_tau_tgt[HYDRA_JNT_MAX];

//double all_eha_pos_tgt[EHA_MAX];

double all_joint_initpos[HYDRA_JNT_MAX];
double all_joint_finalpos[HYDRA_JNT_MAX];
//double all_joint_finalpos_deg[HYDRA_JNT_MAX];

//double all_joint_ampl[HYDRA_JNT_MAX];
//double all_joint_freq_hz[HYDRA_JNT_MAX];
//double all_joint_phase[HYDRA_JNT_MAX];

//double all_joint_refpos_to_send[HYDRA_JNT_MAX]; //[rad]で管理する
//bool   all_joint_servo_switch[HYDRA_JNT_MAX]; //booooooooolllllll!!!!!!!!
//bool   arm_IK_switch[2]={false,false};  //0:right, 1:left

//double FootFS[2][6];

double JointAbsPos[31][3];
//double eef_to_print[6];
//double eef_to_update[6];
//double lhand_pos_err[6]={0,0,0,0,0,0};
//double larm_q_delta[8];
#define IK_GAIN_LARM   0.01;


//#include "kinematics/chydrakinematics.h" //murotani 2017/05/04
//CHydraKinematics hydra_kinematics;//2017/05/12
walk hydra_walk;//2017/05/12

#define INTERP_LEN 10000 // 1000[Hz] * 10[sec]
#define SMOOTH_SLOW


//#define LWRIST_FREE  /* free lwrist pitch joint */

/*
int interp_cnt;
inpt interp_length;
bool thread_data->interp_run;
bool thread_data->grasp_run;
bool thread_data->interp_ready;
bool thread_data->filemotion_en = false;
bool thread_data->filemotion_run;
bool thread_data->sot_en = false;
*/

//double hand_motion[INTERP_LEN][10];
//double interpolation_length = INTERP_LEN;

double **motion_data;
int      num_joints=0;
int      motion_length=0;
int      motion_line = 0;
char     motion_name[256];


// プロセス終了フラグ
int process_end_flg;
int thread_end_flg = 1;

//mutex
//thread_data_t thread_data;
//CthreadData thread_data;

FILE *fp_log=NULL;
FILE *fp_motion=NULL;
FILE *fp_debug=NULL;

bool debug_mode = false;

static int  initialize(CthreadData* thread_data);
static int  main_func(CthreadData* thread_data);
static void destroy(void);
static void sig_handler(int sig);

double slowmotion_rate;

// switch singleaxis, filetorque, walking...
int control_mode;

double** AllocateMotionData(int num_joints, int length)
{
    int i, j;
    double **data;
    data = (double**)malloc(sizeof(double*)*length);
    if(data==NULL) {
        perror("cannot allocate memory");
        return NULL;
    }
    for(i=0; i<length; i++) {
        data[i] = (double*) malloc(sizeof(double)*(num_joints+1));
        if(data[i]==NULL) {
            perror("cannot allocate memory");
            return NULL;
        } else {
            for(j=0; j<num_joints+1; j++) {
                data[i][j] = 0.0;
            }
        }
    }
    return data;
}

void FreeMotionData(double **data, int length)
{
    int i;
    for(i=0; i<length; i++) {
        free(data[i]);
        data[i] = NULL;
    }
    free(data);
    data = NULL;
}

int CheckMotionFileLength(FILE *fp) 
{
    int lines=0;
    char buf[1024];

    while( fgets(buf, sizeof(char)*512, fp)!=NULL ){
        lines ++;
    }
    rewind(fp);
    return lines;
}

int LoadMotionFile(FILE *fp, double** data, int num_joints, int length) 
{
    int i, j;
    char buf[1024];
    char *tok;

    i=0;
    j=0;
    for(i=0; i<length; i++) {
        if( fgets(buf, sizeof(buf), fp)==NULL )
            return i;

        tok = strtok( buf, " ,");
        data[i][j] = atof(tok);
        for(j=1; j<num_joints+1; j++) {
            tok = strtok( NULL, " ");
            data[i][j] = atof(tok);
        }
        j=0;
    }

    return i;
}
/*
void get_q_from_status(joint_state_t  jnt_state[], double q[])
{
    for(int i=0; i<32; i++)
    {
        q[i] = jnt_state[i].DATA.pos_act;
    }
    for(int i=0; i<HYDRA_HAND_JNT_MAX; i++)
    {
        q[i+32] = jnt_state[i+HYDRA_JNT_MAX].DATA.pos_act;
    }
}
*/

//double q[HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX];

int main(int argc,char *argv[])
{
//	int	extseq;
#ifdef USE_EFD
    eventfd_t tmp;
#endif
    int       rtn_main_func = 1;
    pthread_t thread_ui;
    pthread_t thread_ros;
    pthread_t thread_log;

    int getopt_result;
    struct timeval t;

    int  count = 0;
    char logfilename[256];
    char motionfilename[256];

    int gcomp = 0;

    //char sotNameIn[256];

    //hydraData = new CHydraData();
    hydraData = new CthreadData();

    CHydraDataLogger* logger = new CHydraDataLogger(hydraData);
    //CSotHydraBridge* sotHydraBridge;

//    CthreadData* thread_data = new CthreadData(hydraData);

    //thread_data->mutex = PTHREAD_MUTEX_INITIALIZER;
    hydraData->mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init( &(hydraData->mutex), 0);

#if 0
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
#endif

    process_end_flg = 1;
    hydraData->interp_cnt      = 0;
    //interp_length   = INTERP_LEN;
    hydraData->flags["interp_run"]      = false;
    hydraData->flags["interp_ready"]    = false;
    hydraData->flags["filemotion_run"]  = false;
    motion_line     = 0;

//    all_joint_phase[33] = 0;
//    all_joint_phase[34] = M_PI/3.0;
//    all_joint_phase[35] = 2*M_PI/3.0;

    /* defalt file names */
    strcpy(logfilename, "./log");
    strcpy(motionfilename, "./joint");
	slowmotion_rate = 1.0;

    tgt_port  = DEFAULT_TGT_PORT;
    port_base = DEFAULT_PORT_BASE;
    strcpy(tgt_addr, DEFAULT_TGT_ADDR);

    /* process command line options */
    while( (getopt_result=getopt(argc, argv, "r:t:a:f:l:s:m:dgh"))!=-1 ) {
        switch(getopt_result) {
        case 'f':
            strcpy(motionfilename, optarg);
            hydraData->flags["filemotion_en"] = true;
            break;
        case 'r':
            port_base = atoi(optarg);
            break;
        case 't':
            tgt_port = atoi(optarg);
            break;
        case 'm':
            control_mode = atoi(optarg);
            break;
        case 'a':
            strcpy(tgt_addr, optarg);
            break;
//        case 'f':
//            strcpy(motionfilename, optarg);
//            break;
        case 'l':
            strcpy(logfilename, optarg);
            break;
//        case 's':
//            strcpy(slowmotion, optarg);
//            slowmotion_rate = atof(slowmotion);
//            break;
//        case 's':
//            strcpy(sotNameIn,optarg);
//            hydraData->sot_en = true;
//            break;
        case 'd':
            //debug mode
            debug_mode = true;
            printf("Debug mode enabled\n");
            break;
        case'g':
            gcomp = 1;
            printf("Gravity torque compensation enabled\n");
            break;
        case 'h':
        default:
            fprintf(stderr, 
                    "servo_test [-r port base] [-t target port] [-a target IP address] [-f motionfilename] [-l logfilename][h]\n");
            return -1;
            break;
        }
    }

    printf("Communication Parameter ADDR:%s Port base:%d Target Port:%d\n", tgt_addr, port_base, tgt_port);
    printf("Log file:%s\n", logfilename);

    if(initialize(hydraData)<0)
        return -1;

    if( (fp_log = fopen(logfilename,"w"))==NULL ) {
        perror("Cannot open log file");
        return -1;
    }
    fprintf(fp_log, "application start\n");
    fflush(fp_log);

    if( (fp_debug = fopen("./debug.log","w"))==NULL ) {
        perror("Cannot open debug file");
        return -1;
    }

    if(hydraData->flags["filemotion_en"]){
        int ret;
        num_joints=HYDRA_JNT_MAX;
        if( (fp_motion = fopen(motionfilename,"r"))==NULL ) {
            perror("Cannot open motion file");
            return -1;
        }
        motion_length = CheckMotionFileLength(fp_motion);
        fprintf(fp_log, "motion %s length =%d\n", motionfilename, motion_length);
        fflush(fp_log);

        motion_data   = AllocateMotionData(num_joints, motion_length);
        if(motion_data==NULL) {
            fprintf(stderr, "Failed to allocate motion data\n");
            return -1;
        }

        ret = LoadMotionFile(fp_motion, motion_data, num_joints, motion_length);
        if(ret != motion_length) {
            fprintf(fp_log,
                    "motion length mismatch defined=%d actual=%d\n",
                    motion_length, ret);
            return -1;
        }
        strcpy(motion_name, motionfilename);

        fprintf(fp_log, "motion length = %d\n", motion_length);
        fflush(fp_log);
        for(int i=0; i<1; i++) {
            for(int j=0; j<num_joints+1; j++) {
                fprintf(fp_log, "%f ", motion_data[i][j]);
            }
            fprintf(fp_log, "\n");
        }
        memcpy(all_joint_finalpos, &(motion_data[0][1]), sizeof(double)*num_joints);

    }

    char tmp_char[256];
    p_hydra_shm->GetShmOutName(tmp_char);
    fprintf(fp_debug, "shout name %s", tmp_char);
    fflush(fp_debug);

    int ret;

//    hydraData->SetAllJointServoSwitch(false);


    if(pthread_create(&thread_ros, NULL, rosHydra_main, hydraData) != 0) {
        perror("pthread_create failed");
        destroy();
        return -1;
    }

    std::cout << "ros_init done" << std::endl;

    if(pthread_create(&thread_log, NULL, loggingTask_servoui, logger) != 0) {
        perror("pthread_create failed");
        destroy();
        return -1;
    }


    if(pthread_create(&thread_ui, NULL, servo_ui, hydraData) != 0) {
        perror("pthread_create failed");
        pthread_mutex_destroy( &(hydraData->mutex) );
        fclose(fp_log);
        fclose(fp_debug);
        destroy();
        return -1;
    }



    // realtime loop
    while(process_end_flg && rtn_main_func && thread_end_flg)
    {
        /* wait for semaphore to be released in realtime software */
//        ret = p_hydra_shm->WaitSemaphore();//comment out 2017/06/02 murotani
        ret = p_hydra_shm->PostSemaphore();//murotani 2017/06/02
        if (ret == -1)
        {
            perror("usc_ecat_work - sem_wait NG\n");
            continue;
        }

        p_hydra_shm->ReadStatus(hydraData);

        // cancel eha->joint conversion in single axis mode
        if(hydraData->flags["skip_jntcnv"]){
            for(int it=0;it<3;it++){
                hydraData->jnt.act.pos[it] = hydraData->eha.act.pos[it];
                hydraData->jnt.act.tau[it] = hydraData->eha.act.tau[it];
            }
        }



        //eef_to_print[0] = JointAbsPos[22][0]; // rwrist_pitch
        //eef_to_print[1] = JointAbsPos[22][1];
        //eef_to_print[2] = JointAbsPos[22][2];
        //eef_to_print[3] = JointAbsPos[30][0]; // lwrist_pitch
        //eef_to_print[4] = JointAbsPos[30][1];
        //eef_to_print[5] = JointAbsPos[30][2];

//        for(int i=0;i<3;i++){
//            lhand_pos_err[i]= (eef_to_update[i+3] - eef_to_print[i+3])*IK_GAIN_LARM;
//            larm_q_delta[i] = lhand_pos_err[i];
//        }


        //larm_q_delta = J_inv*lhand_pos_err;//////////////////////////////////////

//        if(gcomp==1)
//        {
//            for(int j=0; j<HYDRA_JNT_MAX; j++) {
//                hydraData->GetJointStatePtr(0)[j].DATA.tau_act -= all_joint_tau_fk[j];
//            }
//        }

        // ko //p_comm->WriteStatus(hydra_data.GetJointStatePtr(0), hydra_data.GetEHAStatePtr(0), hydra_data.GetSensorStatePtr(0));
        //p_comm->ReadCommand(hydra_data.GetJointCmdPtr(1), hydra_data.GetEHACmdPtr(1));
        // ko //extseq = p_comm->ReadCommand(hydra_data.GetJointCmdPtr(1), hydra_data.GetEHACmdPtr(1), hydra_data.GetSensorCmdPtr(1));

        rtn_main_func = main_func(hydraData);

        //hydraData->flags["joint_check_limit_en"] = true;
        hydraData->CheckJntRef();

        hydraData->UpdateEHAcmdFromJnt();
        //skip eha->joint conversion for single box
        if(hydraData->flags["skip_jntcnv"]){
            hydraData->eha.ref.pos[0] = hydraData->jnt.ref_raw.pos[0];
            hydraData->eha.ref.tau[0] = hydraData->jnt.ref_raw.tau[0];
        }

        p_hydra_shm->WriteCommand((CHydraData*) hydraData);

        // OUTPUTの共有メモリを反映させ、Indexを更新する
        p_hydra_shm->Sync();

        if(hydraData->flags["logging_en"])
            logger->pushToBuffer(hydraData);

        //hydraData->IncrementIndex();

#ifdef USE_RTP
//        p_rtp_comm->Poll();
#endif

    }

    pthread_join(thread_ui,NULL);
    pthread_join(thread_ros,NULL);
    pthread_join(thread_log,NULL);

    destroy();
    pthread_mutex_destroy( &(hydraData->mutex) );

    fclose(fp_debug);
    fclose(fp_log);
    return (0);
}

/**
   @fn     int main_func( )
   @brief  N/A
   @param  N/A
   @return 1:Process continuation 0:Process end
*/
//int main_func(void)
int main_func(CthreadData* hydraData)
{
    int loop;
    static int cnt = 0;
    static int t = 0;

    int rtn = 1;

    process_end_flg = !OsTerminateAppRequest();/* check if demo shall terminate */

    cnt++;

    hydra_walk.SetJointPosition(hydraData->jnt.act.pos.data());
    hydra_walk.SetJointVelocity(hydraData->jnt.act.vel.data());//2017/05/10 murotani
    hydra_walk.SetIMUacc(hydraData->imu.acc.data());//2017/05/10 murotani
    hydra_walk.SetIMUgyro(hydraData->imu.gyro.data());//2017/05/10 murotani    
    hydra_walk.SetForceSensor(0, hydraData->fs.right.wrench.data());
    hydra_walk.SetForceSensor(1, hydraData->fs.left.wrench.data());
//    hydra_walk.ForwardKinematics_imu();
    hydra_walk.ForwardKinematics_from_body();
//    hydra_walk.GetJointTorque(all_joint_tau_fk);//comment out 2017/05/12 murotani
//    hydra_walk.GetJointAbsPos(JointAbsPos);//comment out 2017/05/12 murotani

    // 指令値の設定
    if(hydraData->flags["interp_run"]) {
        motion_line=0;
        if(hydraData->interp_cnt <= hydraData->interp_length) {
            for(loop = 0; loop < HYDRA_JNT_MAX; loop++) {
                hydraData->jnt.ref_raw.pos[loop]
                        = all_joint_initpos[loop]
                        + (all_joint_finalpos[loop] - all_joint_initpos[loop])
                        *((double)hydraData->interp_cnt)/((double)hydraData->interp_length);

                /*
                all_joint_refpos_to_send[loop]
                    = all_joint_initpos[loop]
                    + (all_joint_finalpos[loop] - all_joint_initpos[loop])
                    *((double)hydraData->interp_cnt)/((double)hydraData->interp_length);
                    * */
            }
            if(hydraData->interp_cnt%100==0){
                fprintf(fp_log, 
                        "Interpolation running [%6d/%6d]\n", 
                        hydraData->interp_cnt, hydraData->interp_length);
            }


            hydraData->interp_cnt++;
        } else {
            hydraData->interp_cnt    = 0;
            hydraData->flags["interp_run"]    = false;
            hydraData->flags["interp_ready"]  = true;
            fprintf(fp_log, "Interpolation end\n");
        }
    }
    else if(hydraData->flags["filemotion_run"]) {
        if(motion_line < motion_length) {
            if(hydraData->flags["filetorque_en"]){
                for(loop = 0; loop < HYDRA_JNT_MAX; loop++) {
                    hydraData->jnt.ref_raw.tau[loop]
                        = motion_data[motion_line][loop+1];
                }
            }else{
                for(loop = 0; loop < HYDRA_JNT_MAX; loop++) {
                    hydraData->jnt.ref_raw.pos[loop]
                        = motion_data[motion_line][loop+1];
                }
            }
            //if(hydraData->interp_cnt%100==0)
            if(motion_line%100==0)
                fprintf(fp_log,
                        "filemotion running [%6d/%6d]\n",
                        motion_line, motion_length);
            motion_line++;
        } else {
            motion_line    = 0;
            hydraData->flags["filemotion_run"]    = false;
            hydraData->flags["interp_ready"]  = false;
            if(hydraData->flags["filetorque"]){
                for(loop = 0; loop < HYDRA_JNT_MAX; loop++) {
                    hydraData->jnt.ref_raw.tau[loop] = 0;
                }
            }

            fprintf(fp_log, "filemotion end\n");
            //ofs<<"Interpolation end"<<std::endl;
        }
    }else if(hydraData->flags["walking_run"]){//2017/05/12 murotani
        hydra_walk.t_update();
        hydra_walk.walkingcontroll();//2017/05/12 murotani
        hydra_walk.writeReference_pos(hydraData->jnt.ref_raw.pos.data());//2017/05/12 murotani
    }
    /*
    else if(hydraData->grasp_run) {
        if(hydraData->interp_cnt <= hydraData->interp_length) {
            for(loop = 31; loop < HYDRA_JNT_MAX; loop++) {
                all_joint_refpos_to_send[loop]
                    = all_joint_initpos[loop]
                    + (all_joint_finalpos[loop] - all_joint_initpos[loop])
                    *((double)hydraData->interp_cnt)/((double)hydraData->interp_length);
            }
            hydraData->interp_cnt++;
        } else {
            hydraData->interp_cnt    = 0;
            hydraData->grasp_run     = false;
            hydraData->interp_ready  = true;
            fprintf(fp_log, "Interpolation end\n");
            //ofs<<"Interpolation end"<<std::endl;
        }
    }
    */

    //for(loop=23;loop<31;loop++){
    /*
    if(arm_IK_switch[1]==true){
        for(loop=0;loop<8;loop++){
         all_joint_refpos_to_send[loop+23] += larm_q_delta[loop];
        }
    }
    */
/*
    for(loop = 0; loop < HYDRA_JNT_MAX; loop++) {
        all_joint_pos_tgt[loop] = CHK_LIM(all_joint_refpos_to_send[loop]
                                          +(all_joint_ampl[loop]*sin(cnt/1000.0*all_joint_freq_hz[loop]*2*M_PI+all_joint_phase[loop]))
                                          ,
                                          all_joint_angle_limit[loop][0],all_joint_angle_limit[loop][1]);
        //all_joint_pos_tgt[loop] = all_joint_refpos_to_send[loop];
        //all_joint_pos_tgt[loop] = DEG2RAD(30);

        if((hydraData->GetJointCmdPtr(0)[loop].DATA.pos_ref - all_joint_pos_tgt[loop])>POS_STEP)
            hydraData->GetJointCmdPtr(1)[loop].DATA.pos_ref = hydraData->GetJointCmdPtr(0)[loop].DATA.pos_ref - POS_STEP;
        else if((all_joint_pos_tgt[loop] - hydraData->GetJointCmdPtr(0)[loop].DATA.pos_ref)>POS_STEP)
            hydraData->GetJointCmdPtr(1)[loop].DATA.pos_ref = hydraData->GetJointCmdPtr(0)[loop].DATA.pos_ref + POS_STEP;
        else
            hydraData->GetJointCmdPtr(1)[loop].DATA.pos_ref = all_joint_pos_tgt[loop];
            //hydra_data.GetJointCmdPtr(1)[loop].DATA.pos_ref += (all_joint_ampl[loop]*
             //                          sin(cnt/1000.0*all_joint_freq_hz[loop]*2*M_PI+all_joint_phase[loop]));
#ifdef LWRIST_FREE
	hydra_data.GetJointCmdPtr(0)[JOINT_HYDRA_LWRIST_PITCH].DATA.pos_ref = hydra_data.GetJointStatePtr(0)[JOINT_HYDRA_LWRIST_PITCH].DATA.pos_act; 
#endif
        hydraData->GetJointCmdPtr(1)[loop].DATA.tau_ref = all_joint_tau_tgt[loop];

    }
    */

//    hydraData->setJointPosCmd(all_joint_pos_tgt);


    //hydraData->setJointTauCmd(all_joint_tau_tgt); //////////////////


   // JointToCylinderAll(all_joint_pos_tgt, all_eha_pos_tgt);

    /*
    for(loop=0;loop<EHA_MAX;loop++){
        hydra_data.GetEHACmdPtr(1)[loop].DATA.pos_ref = all_eha_pos_tgt[loop];
    }
    */
    t++;

    return (rtn);
}

//int initialize(void)
int initialize(CthreadData* hydraData)
{
    // pointer factory
    p_hydra_shm = new CHydraShmClient();
    if(p_hydra_shm==NULL)
        return -1;
    p_hydra_shm->SetShmInName(SHM_IN_NAME);
    p_hydra_shm->SetShmOutName(SHM_OUT_NAME);
    p_hydra_shm->SetSemName(SEM_ECATSYNC_NAME);
    if(p_hydra_shm->Init()<0)
        return -1;

    // initialize socket communications
#ifdef USE_RTP
    p_rtp_comm = new CHydraJrtpComm( );
    if(p_rtp_comm==NULL)
        return -1;
    p_comm = (CIPComm*)p_rtp_comm;
#elif defined USE_UDP
    p_udp_comm = new CUDPComm( );
    if(p_udp_comm==NULL)
        return -1;
    p_comm = (CIPComm*)p_udp_comm;
#endif
    /*
    p_comm->SetPortBase(port_base);
    p_comm->SetTargetAddress(tgt_addr);
    p_comm->SetTargetPort(tgt_port);
    if(p_comm->Init()<0)
        return -1;
        */

    //all_eha_switchの初期化
    /*
    for(int i=0;i<EHA_MAX;i++){
        hydraData->GetEHACmdPtr(0)[i].DATA.ctlword = EHA_CtrlWd_OFF[i];
    }
    */

    //initialize all_joint_servo_switch
    /*
    for(int i=0; i<HYDRA_JNT_MAX; i++){
        all_joint_servo_switch[i] = false;
    }
    */
    /*
    //initialize sine wave parameter
    for(int i=0; i<HYDRA_JNT_MAX; i++){
        all_joint_ampl[i] = 0.0;
        all_joint_freq_hz[i] = 0.1;
        all_joint_phase[i] = 0;
    }
    */

//    all_joint_phase[JOINT_HYDRA_RWRIST_ROLL] = M_PI/2.0;

    for(int i=0; i<HYDRA_JNT_MAX; i++){
        all_joint_finalpos[i] = 0;
    }
    //all_joint_finalpos[JOINT_HYDRA_RWRIST_YAW] = M_PI / 2.0;
    //all_joint_finalpos[JOINT_HYDRA_LWRIST_YAW] = -M_PI / 2.0;
    all_joint_finalpos[1] = -M_PI/12;//r_hip_pitch
    all_joint_finalpos[3] = M_PI/6;//r_knee
    all_joint_finalpos[5] = -M_PI/12;//r_ankle_pitch
    all_joint_finalpos[7] = -M_PI/12;//l_hip_pitch
    all_joint_finalpos[9] = M_PI/6;//l_knee
    all_joint_finalpos[11] = -M_PI/12;//l_ankle_pitch

    switch(control_mode){
    case 1:  // wholebody
        for(int i=0;i<HYDRA_JNT_MAX;i++)
            hydraData->log_en_jnt[i] = true;
        for(int i=0;i<EHA_MAX;i++)
            hydraData->log_en_eha[i] = true;
        break;
    case 2:  // lower body
        for(int i=0;i<14;i++)
            hydraData->log_en_jnt[i] = true;
        for(int i=0;i<18;i++)
            hydraData->log_en_eha[i] = true;
        break;
    case 3: // upper body
        for(int i=14;i<HYDRA_JNT_MAX;i++)
            hydraData->log_en_jnt[i] = true;
        for(int i=18;i<EHA_MAX;i++)
            hydraData->log_en_eha[i] = true;
        break;
    case 5:  // single axis, torque filemotion
        hydraData->flags["filetorque_en"] = true;
    case 4:  // single axis
        hydraData->log_en_eha[0] = true;
        hydraData->log_en_eha[1] = true;
        hydraData->log_en_eha[2] = true;
        hydraData->flags["skip_jntcnv"] = true;
        break;

    }

    return 0;
}


void destroy(void)
{
    if(p_hydra_shm != NULL)
    {
        p_hydra_shm->Close();
        delete p_hydra_shm;
    }
    p_hydra_shm = NULL;

    if(p_comm != NULL)
    {
        p_comm->Close();
#ifdef USE_RTP
        delete p_rtp_comm;
#elif defined USE_UDP
        delete p_udp_comm;
#endif
    }
    p_comm = NULL;

    return;
}
void sig_handler(int sig)
{
    if (sig == SIGINT)
    {
        thread_end_flg = 0;
        process_end_flg = 0;
        printf("Exit with SIGINT\n");
    }
}

int int_cmpr(const int *a,const int *b)
{
    if (*a < *b) return (-1);
    else if(*a > *b) return (1);
    else return (0);
}
