/**
   @author Shin'ichiro Nakaoka
*/

#undef USE_SHM

#include <cnoid/SimpleController>
#include <vector>
#include <sstream>
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "HydraPosControl.h"
#include "CalcParallelJoint.h"
#include "robot_hydra_id.h"

// ReadParamFile()で判別するパラメータ種別
//#define PARAM_FILE  "config/HydraPosControl.dat"
#define SPACE_AND_TAB  (" \t")
enum{
	PARAM_TYPE_COMMENT=0,
	PARAM_TYPE_IMPORTFILE,
	PARAM_TYPE_POS_PDGAIN,
	PARAM_TYPE_MAX
};
std::string command[PARAM_TYPE_MAX] = 
{
	"//",
	"import_file",
	"pos_pd_gain"
};

using namespace cnoid;

int id_choreonoid_to_hydra[] = {
    0, 1, 2, 3, 4, 5, 
    6, 7, 8, 9, 10, 11,
    12, 13, 14, -1,
    15, 16, 17, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 27, 28, 29, 30,
    -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
int id_hydra_to_choreonoid[] = {
    0, 1, 2, 3, 4, 5,
    6, 7, 8, 9, 10, 11, //12
    12, 13, 14, //15
    16, 17, 18, 19, 20, 21, 22, 23, //23
    24, 25, 26, 27, 28, 29, 30, 31, //31
    32, 33, 36, 39, 42, 45, 46, 49, 52, 55, //41
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};
const int joint_to_EHA_power[][6] = {
	{1, 0, -1,-1,-1,-1},//rhip_yaw
	{2, 1,  2,-1,-1,-1},//rhip_pitch
	{2, 1,  2,-1,-1,-1},//rhip_roll
	{3, 3,  4, 5,-1,-1},//rknee
	{2, 6,  7,-1,-1,-1},//rankle_roll
	{2, 6,  7,-1,-1,-1},//rankle_pitch
	{1, 8, -1,-1,-1,-1},//lhip_yaw
	{2, 9, 10,-1,-1,-1},//lhip_pitch
	{2, 9, 10,-1,-1,-1},//lhip_roll
	{3, 11,12,13,-1,-1},//lknee
	{2, 14,15,-1,-1,-1},//lankle_roll
	{2, 14,15,-1,-1,-1},//lankle_pitch
	{2, 16,17,-1,-1,-1},//waist_pitch
	{2, 16,17,-1,-1,-1},//waist_roll
	{1, 18,-1,-1,-1,-1},//neck_pitch
	{1, 19,-1,-1,-1,-1},//rscaplae_yaw
	{2, 20,21,-1,-1,-1},//rshoulder_pitch
	{2, 20,21,-1,-1,-1},//rshoulder_roll
	{1, 22,-1,-1,-1,-1},//rshoulder_yaw
	{2, 23,24,-1,-1,-1},//relbow
	{1, 25,-1,-1,-1,-1},//rwrist_yaw
	{2, 26,27,-1,-1,-1},//rwrist_roll
	{2, 26,27,-1,-1,-1},//rwrist_pitch
	{1, 28,-1,-1,-1,-1},//lscaplae_yaw
	{2, 29,30,-1,-1,-1},//lshoulder_pitch
	{2, 29,30,-1,-1,-1},//lshoulder_roll
	{1, 31,-1,-1,-1,-1},//lshoulder_yaw
	{2, 32,33,-1,-1,-1},
	{1, 34,-1,-1,-1,-1},
	{2, 35,36,-1,-1,-1},
	{2, 35,36,-1,-1,-1},
	{1, 37,-1,-1,-1,-1},
	{1, 38,-1,-1,-1,-1},
	{1, 39,-1,-1,-1,-1},
	{1, 40,-1,-1,-1,-1},
	{1, 41,-1,-1,-1,-1},
	{1, 42,-1,-1,-1,-1},
	{1, 43,-1,-1,-1,-1},
	{1, 44,-1,-1,-1,-1},
	{1, 45,-1,-1,-1,-1},
	{1, 46,-1,-1,-1,-1}
};

// P$B%2%$%s=i4|CM!"8e$[$I%U%!%$%k$rFI$_9~$s$G>e=q$-$9$k(B
static double pgain[] = {
    8000.0, 16000.0, 16000.0, 16000.0, 16000.0, 16000.0,	// rleg
    8000.0, 16000.0, 16000.0, 16000.0, 16000.0, 16000.0,	// lleg
    32000.0, 32000.0, 8000.0, 8000.0,						// body
    //8000.0, 8000.0, 8000.0, 8000.0, 8000.0, 8000.0, 8000.0, 8000.0, //rarm
    8000.0, 4000.0, 4000.0, 4000.0, 1000.0, 1000.0, 1000.0, 1000.0, //rarm
    8000.0, 8000.0, 8000.0, 8000.0, 8000.0, 8000.0, 8000.0, 8000.0, //larm
    2.0, 2.0, 2.0, 2.0, 2.0,
    2.0, 2.0, 2.0, 2.0, 2.0,
//    8000.0, 8000.0, 8000.0,
//    8000.0, 8000.0, 8000.0, 8000.0, 8000.0, 8000.0, 8000.0,
//    8000.0, 8000.0, 8000.0, 8000.0, 8000.0, 8000.0,
//    8000.0,
};

static const double k_coupling = 1;
static const double d_coupling = 0.5;

// D$B%2%$%s=i4|CM!"8e$[$I%U%!%$%k$rFI$_9~$s$G>e=q$-$9$k(B
static double dgain[] = {
    100.0, 200.0, 200.0, 200.0, 200.0, 200.0, //1-6
    100.0, 200.0, 200.0, 200.0, 200.0, 200.0, //7-12
    400.0, 400.0, 100.0, 100.0, //13-16
    //100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0,//17-24 rarm
    100.0, 100.0, 100.0, 100.0, 300.0, 50.0, 50.0, 50.0,//17-24
    100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0,//25-32
    0.5, 0.5, 0.5, 0.5, 0.5, //33-37
    0.5, 0.5, 0.5, 0.5, 0.5, //48-42
//    100.0, 100.0, 100.0, //43-45
//    100.0, 100.0, 100.0, 100.0, 100.0, 100.0, 100.0,//46-52
//    100.0, 100.0, 100.0, 100.0, 100.0, 100.0, //53-58
//    100.0, //59
};

HydraPosControl::HydraPosControl()
{
	_abs_dll_path.clear();
}

HydraPosControl::~HydraPosControl()
{
    os() << "Write log file to " << log_filename.string() << std::endl;
    recorder.WriteData(log_filename.c_str());

    delete(pShmServer);
}

bool HydraPosControl::initialize(SimpleControllerIO* io)
{
    int ret;
    os() << "HydraPosControl::initialize()" << std::endl;

	GetCurrentDllPath(_abs_dll_path);
    os() << "HydraPosControl::location=" << _abs_dll_path.generic_string().c_str() << std::endl;

    body = io->body();
    dt   = io->timeStep();

    // parse option string
    std::string optstr = io->optionString();
    std::string buffer;
    std::stringstream ss(optstr);

    jlim = false;
    log_filepath = getenv("HOME");
    log_filename = log_filepath / std::string("/log.txt");
    config_filepath = std::string(getenv("HOME")) + std::string("/.choreonoid/config");
    config_filename = config_filepath / std::string("HydraPosControl.xml");

    while( std::getline(ss, buffer, ' ') )
    {
        if(buffer=="-j")
        {
            jlim = true;
            os() << "Parallel link joint limit enabled" << std::endl;
        }
        else if(buffer.find("-f")==0)
        {
            log_filename = log_filepath / buffer.substr(2);
        }
        else if(buffer.find("-c")==0)
        {
            config_filename = config_filepath / buffer.substr(2);
        }
        else
            os() << "Error: Option " << buffer << " not defined" << std::endl;
    }

    config_loader.Init(JOINT_HYDRA_MAX);
    config_loader.SetPGain(pgain);
    config_loader.SetDGain(dgain);
    if((ret = config_loader.LoadConfigXML(config_filename.c_str()))<0)
    {
        if(ret==CHydraConfigLoader::E_ERR_NOFILE)
            os() << "XML config file load error" << std::endl;
        else if(ret==CHydraConfigLoader::E_ERR_MODEL)
            os() << "XML config file invalid model" << std::endl;
        os() << "Using default config values" << std::endl;
    }

    config_loader.LoadPGain(pgain);
    config_loader.LoadDGain(dgain);

    t = 0;

    joint_state.resize(HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX);
    joint_cmd.resize(body->numJoints());
    eha_state.resize(EHA_MAX);
    eha_cmd.resize(EHA_MAX);
    sensor_state.resize(1);
    sensor_cmd.resize(1);

    os() << "Num Joints  " << body->numJoints() << std::endl;
    os() << "Log File    " << log_filename.string() << std::endl;
    os() << "Config File " << config_filename.string() << std::endl;

    for(int i=0; i < HYDRA_JNT_MAX; i++){
        if(id_hydra_to_choreonoid[i]!=-1) {
            joint_state[i].DATA.id      = i;
            joint_cmd[i].DATA.pos_ref   = body->joint(id_hydra_to_choreonoid[i])->q();
            joint_cmd[i].DATA.enable    = 1;
            joint_state[i].DATA.enabled = 1;
        }
    }

    for(int i=0; i < HYDRA_HAND_JNT_MAX; i++){
        joint_state[i+HYDRA_JNT_MAX].DATA.id = i+HYDRA_JNT_MAX;
    }

    for(int i=0; i<HYDRA_JNT_MAX; i++) {
            joint_position[i] = joint_cmd[i].DATA.pos_ref;
    }

    // calc eha positions from current joint angles
    JointToCylinderAll(joint_position, EHA_position);

    memcpy(joint_position_prev, joint_position, sizeof(double)*HYDRA_JNT_MAX);

    for(int i=0; i < EHA_MAX; ++i){
        eha_state[i].DATA.pos_act = EHA_position[i];

        eha_cmd[i].DATA.pos_ref   = eha_state[i].DATA.pos_act;

        eha_cmd[i].DATA.ctlword   = 0x0101;
        eha_state[i].DATA.stsword = 0x0101;
    }

    pShmServer = new CHydraShmServer();
    pShmServer->SetShmInName(SHM_IN_NAME);
    pShmServer->SetShmOutName(SHM_OUT_NAME);
    pShmServer->SetSemName(SEM_ECATSYNC_NAME);

    pIPC = (IHydraIPC*)pShmServer;

    if(pIPC->Init()<0)
    {
        fprintf(stderr, "IPC init failed\n");
        return false;
    }

    recorder.InitDataRec(3, 20000);

#if 0
    // Set LINK I/O
	io->setJointOutput(JOINT_TORQUE);
	io->setJointInput(JOINT_ANGLE | JOINT_VELOCITY | JOINT_ACCELERATION | JOINT_TORQUE);
#endif

  for (int i=0; i<body->numJoints(); i++){
	Link* joint = body->joint(i);
    io->setLinkOutput(body->joint(i), JOINT_TORQUE);
    //io->setLinkInput(body->joint(i), JOINT_ANGLE | JOINT_VELOCITY | JOINT_TORQUE);
    io->setLinkInput(body->joint(i), JOINT_ANGLE | JOINT_VELOCITY);
  }

    return true;
}

bool HydraPosControl::control()
{
    double qref;
    double q;
    double dq;
    double u;
    //static int cnt = 0;
    int cnt = 0;
    int enable;

    // measure
    measure_state();

    recorder.Record(0, joint_state[JOINT_HYDRA_RWRIST_PITCH].DATA.pos_act);
    recorder.Record(1, joint_state[JOINT_HYDRA_RWRIST_PITCH].DATA.pos_cmd);
    recorder.Record(2, joint_state[JOINT_HYDRA_RWRIST_PITCH].DATA.tau_act);
    recorder.Increment();

    //cnt++;
    for(int i=0; i<HYDRA_JNT_MAX; i++) {
        joint_position[i] = joint_state[i].DATA.pos_act;
    }

    // calc eha IK here
    JointToCylinderAll(joint_position, EHA_position);
    for(int i=0; i < EHA_MAX; ++i){
        eha_state[i].DATA.pos_act = EHA_position[i];
        if( (i==3)||(i==4)||(i==11)||(i==12) )
            eha_state[i].DATA.pos_act += 0.190;
    }

    pIPC->WriteStatus(&(joint_state[0]), &(eha_state[0]), &(sensor_state[0]));

	cnt = pIPC->Sync();
    cnt = pIPC->GetSyncStatus();
    usleep(2000);//ko
    if (cnt > 1 )
    //while(cnt > 1)
	{
        os() << "sem get_value [? > 1] " << cnt << std::endl;
        usleep(2000);//ko
        /*cnt = pIPC->GetSyncStatus(); //ko
        if(cnt>1000)
        {
            os() << "sem_wait_timeout" << std::endl;
            break;
        }*/
	}
#if 0 // Wait待ちに関わらず、セマフォを解放するように修正　20160413 okamoto
    int wdt = 0;
    switch(pIPC->Sync())
    {
    case CHydraShmServer::E_SEM_UNLK:
        os() << "sem unlock " << cnt << std::endl;
        cnt++;

#if 1   // $BBT$A9g$o$;HG(B 20160401 okamoto
        // to prevent semaphore being unlocked, this line waits
        // for the semaphore to be locked
        // while( pShmServer->PostSemaphore() == CHydraShmServer::E_SEM_UNLK);

        // timeout implemented to enable choreonoid to be shutdown after robot_console
        // was terminated
        while( wdt<1000 ) {
            if(pShmServer->PostSemaphore() == CHydraShmServer::E_SEM_UNLK) {
                wdt++;
            }
            else {
                break;
            }
        }
#endif
        break;
    case CHydraShmServer::E_SEM_INVLD_NUM:
        break;
    case CHydraShmServer::E_SHM_NOT_EXIST:
        break;
    default:
        cnt = 0;
        break;
    }
#endif

    pIPC->ReadCommand(&(joint_cmd[0]), &(eha_cmd[0]), &(sensor_cmd[0]));

    for(int i=0; i<HYDRA_JNT_MAX; i++) {
        joint_position[i] = joint_cmd[i].DATA.pos_ref;
    }

    if(jlim)
        calc_parallel_constraint();

    // decode power state
    for(int i=0; i<HYDRA_JNT_MAX; i++) {
        enable = 0;
        for(int j=0; j<joint_to_EHA_power[i][0]; j++) {
            if(eha_cmd[joint_to_EHA_power[i][j+1]].DATA.ctlword&0x1)
                enable = 1;
        }
        joint_cmd[i].DATA.enable = enable;
        //joint_cmd[i].DATA.enable = eha_cmd[joint_to_EHA_power[i][1]].DATA.ctlword&0x01;
    }

    // control
    for(int i=0; i < HYDRA_JNT_MAX; ++i){
        if( joint_is_hand(i) ) {
            qref = joint_cmd[i].DATA.pos_ref; // original joint reference position
            q    = joint_state[i].DATA.pos_act;
            dq   = joint_state[i].DATA.vel_act;
        } else {
            //qref = joint_cmd[i].DATA.pos_ref; // original joint reference position
            qref = joint_position[i];// original joint reference position with EHA limit
            q    = joint_state[i].DATA.pos_act;
            dq   = joint_state[i].DATA.vel_act;
        }

        if(joint_cmd[i].DATA.enable==1)
            u = (qref - q) * pgain[i]/1 + (0.0 - dq) * dgain[i]/1;
        else
            u = 0;

        joint_state[i].DATA.tau_cmd = u;
    }

    // output
    output_command();

    t += dt;
#ifdef USE_RTP
    session.Poll();
#endif
    return true;
}

void HydraPosControl::measure_state(void)
{
    for(int i=0; i < HYDRA_JNT_MAX; ++i){
        if(id_hydra_to_choreonoid[i]!=-1) {
            if( (i==JOINT_HYDRA_RHAND_THB_AA)||(i==JOINT_HYDRA_LHAND_THB_AA) ){
                joint_state[i].DATA.pos_act = body->joint(id_hydra_to_choreonoid[i])->q();
                joint_state[i].DATA.vel_act = body->joint(id_hydra_to_choreonoid[i])->dq();
                joint_state[i].DATA.tau_act = butter[i].Step(body->joint(id_hydra_to_choreonoid[i])->u());
//                joint_state[i].DATA.tau_act = body->joint(id_hydra_to_choreonoid[i])->u();
            } else if ( joint_is_hand(i) ) {
                joint_state[i].DATA.pos_act = body->joint(id_hydra_to_choreonoid[i])->q() +
                        body->joint(id_hydra_to_choreonoid[i]+1)->q() +
                        body->joint(id_hydra_to_choreonoid[i]+2)->q();
                joint_state[i].DATA.vel_act = body->joint(id_hydra_to_choreonoid[i])->dq() +
                        body->joint(id_hydra_to_choreonoid[i]+1)->dq() +
                        body->joint(id_hydra_to_choreonoid[i]+2)->dq();
                joint_state[i].DATA.tau_act = butter[i].Step(body->joint(id_hydra_to_choreonoid[i])->u());
//                joint_state[i].DATA.tau_act = body->joint(id_hydra_to_choreonoid[i])->u();
            } else {
                joint_state[i].DATA.pos_act = body->joint(id_hydra_to_choreonoid[i])->q();
                joint_state[i].DATA.vel_act = body->joint(id_hydra_to_choreonoid[i])->dq();
                joint_state[i].DATA.tau_act = butter[i].Step(body->joint(id_hydra_to_choreonoid[i])->u());
//                joint_state[i].DATA.tau_act = body->joint(id_hydra_to_choreonoid[i])->u();
            }
            joint_state[i].DATA.enabled = joint_cmd[i].DATA.enable;
            joint_state[i].DATA.pos_cmd = joint_cmd[i].DATA.pos_ref;
            joint_state[i].DATA.vel_cmd = 0;
        }
    }
    for(int i=0; i < HYDRA_HAND_JNT_MAX; ++i){
        joint_state[i+HYDRA_JNT_MAX].DATA.pos_act = body->joint(i+32)->q();
        joint_state[i+HYDRA_JNT_MAX].DATA.vel_act = body->joint(i+32)->dq();
    }
}

void HydraPosControl::output_command()
{
    double qref;
    double q;
    double dq;

    for(int i=0; i < HYDRA_JNT_MAX; ++i){
        if(id_hydra_to_choreonoid[i]!=-1) {
            if( (i==JOINT_HYDRA_RHAND_THB_AA)||(i==JOINT_HYDRA_LHAND_THB_AA) ) {
                body->joint(id_hydra_to_choreonoid[i])->u() = joint_state[i].DATA.tau_cmd;
            } else if ( joint_is_hand(i) ) {
                qref = joint_cmd[i].DATA.pos_ref/3;
                q    = body->joint(id_hydra_to_choreonoid[i])->q();
                dq   = body->joint(id_hydra_to_choreonoid[i])->dq();
                body->joint(id_hydra_to_choreonoid[i])->u()   = (qref - q) * k_coupling + (0.0 - dq) * d_coupling + joint_state[i].DATA.tau_cmd/3;
//                body->joint(id_hydra_to_choreonoid[i])->u()   = (qref - q) * pgain[i]/1 + (0.0 - dq) * dgain[i]/1 + joint_state[i].DATA.tau_cmd/3;

                q    = body->joint(id_hydra_to_choreonoid[i]+1)->q();
                dq   = body->joint(id_hydra_to_choreonoid[i]+1)->dq();
                body->joint(id_hydra_to_choreonoid[i]+1)->u() = (qref - q) * k_coupling + (0.0 - dq) * d_coupling + joint_state[i].DATA.tau_cmd/3;
//                body->joint(id_hydra_to_choreonoid[i]+1)->u() = (qref - q) * pgain[i]/1 + (0.0 - dq) * dgain[i]/1 + joint_state[i].DATA.tau_cmd/3;

                q    = body->joint(id_hydra_to_choreonoid[i]+2)->q();
                dq   = body->joint(id_hydra_to_choreonoid[i]+2)->dq();
                body->joint(id_hydra_to_choreonoid[i]+2)->u() = (qref - q) * k_coupling + (0.0 - dq) * d_coupling + joint_state[i].DATA.tau_cmd/3;
//                body->joint(id_hydra_to_choreonoid[i]+2)->u() = (qref - q) * pgain[i]/1 + (0.0 - dq) * dgain[i]/1 + joint_state[i].DATA.tau_cmd/3;
            } else {
                body->joint(id_hydra_to_choreonoid[i])->u() = joint_state[i].DATA.tau_cmd;
            }
        }
    }
}

void HydraPosControl::calc_parallel_constraint(void)
{
    // calc EHA reference position from joint reference position
    JointToCylinderAll(joint_position, EHA_position);
    for(int i=0; i < EHA_MAX; ++i){
        eha_cmd[i].DATA.pos_ref = EHA_position[i];

        //exception for knee actuators
        if( (i==EHA_rknee_single)     ||
                (i==EHA_rknee_tandem1)||
                (i==EHA_lknee_single) ||
                (i==EHA_lknee_tandem1) ) {
            eha_cmd[i].DATA.pos_ref += 0.190;
        }
    }

    // calc eha_cmd.DATA.pos_ref from joint_cmd.DATA.pos_ref
    // limit with EHA pos
    for(int i=0; i<EHA_MAX; i++)
    {
        if( (!EHA_is_vane(i))&&(!EHA_is_hand(i)) ) {
            if(eha_cmd[i].DATA.pos_ref > 0.025)
                eha_cmd[i].DATA.pos_ref = 0.025;
            else if(eha_cmd[i].DATA.pos_ref < -0.025)
                eha_cmd[i].DATA.pos_ref = -0.025;
        }
    }

    for(int i=0; i < EHA_MAX; ++i){
        EHA_position[i] = eha_cmd[i].DATA.pos_ref;
    }

    // calc joint reference position with EHA position limit consideration
    if(	CylinderToJointAll(EHA_position, joint_position, joint_position_prev) < 0)
        os() << "IK failed" << std::endl;

    memcpy(joint_position_prev, joint_position, sizeof(double)*HYDRA_JNT_MAX);
}

bool HydraPosControl::EHA_is_vane(int i)
{
    if(     (i==EHA_rhip_vane)      ||
            (i==EHA_lhip_vane)      ||
            (i==EHA_rshoulder_vane) ||
            (i==EHA_relbow_vane)    ||
            (i==EHA_rwrist_vane)    ||
            (i==EHA_lshoulder_vane) ||
            (i==EHA_lelbow_vane)    ||
            (i==EHA_lwrist_vane) ) {
        return true;
    } else {
        return false;
    }
}

bool HydraPosControl::EHA_is_hand(int i)
{
    if(     (i==EHA_rhand_thb_AA)   ||
            (i==EHA_rhand_thb_FE)   ||
            (i==EHA_rhand_idx)      ||
            (i==EHA_rhand_mid)      ||
            (i==EHA_rhand_rng)      ||
            (i==EHA_lhand_thb_AA)   ||
            (i==EHA_lhand_thb_FE)   ||
            (i==EHA_lhand_idx)      ||
            (i==EHA_lhand_mid)      ||
            (i==EHA_lhand_rng)
            ) {
        return true;
    } else {
        return false;
    }
}

bool HydraPosControl::joint_is_hand(int i)
{
    if(     (i==JOINT_HYDRA_RHAND_THB_AA)   ||
            (i==JOINT_HYDRA_RHAND_THB_FE)   ||
            (i==JOINT_HYDRA_RHAND_IDX)      ||
            (i==JOINT_HYDRA_RHAND_MID)      ||
            (i==JOINT_HYDRA_RHAND_RNG)      ||
            (i==JOINT_HYDRA_LHAND_THB_AA)   ||
            (i==JOINT_HYDRA_LHAND_THB_FE)   ||
            (i==JOINT_HYDRA_LHAND_IDX)      ||
            (i==JOINT_HYDRA_LHAND_MID)      ||
            (i==JOINT_HYDRA_LHAND_RNG)
            ) {
        return true;
    } else {
        return false;
    }
}

void HydraPosControl::GetCurrentDllPath(filesystem::path& path)
{
    path = filesystem::path(executableTopDirectory()) / CNOID_PLUGIN_SUBDIR / "simplecontroller";
}

void HydraPosControl::ignore_space_and_tab(std::string& str)
{
    int pos;

    // スペースとタブが含まれない先頭を探す
    pos = str.find_first_not_of(SPACE_AND_TAB);
    if (pos != std::string::npos){
        str.erase(0, pos);
    }
}

CNOID_IMPLEMENT_SIMPLE_CONTROLLER_FACTORY(HydraPosControl)
