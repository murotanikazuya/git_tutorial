/**
   @author Shin'ichiro Nakaoka
*/
#ifndef __HYDRAPOSCONTROL_H_
#define __HYDRAPOSCONTROL_H_

#include <cnoid/SimpleController>
#include <cnoid/ExecutablePath>
#include <vector>

#include <stdio.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <semaphore.h>

#include "hydra_types.h"
#include "ihydraipc.h"
#include "chydrashmserver.h"
#include "cbutter20.h"
#include "chydradatarec.h"
#include "chydraconfigloader.h"

#define USE_MATH_DEFINES
#include <math.h>

#ifndef RAD2DEG
#define RAD2DEG(x) ((x)*180.0/M_PI)
#endif

#ifndef DEG2RAD
#define DEG2RAD(x) ((x)*M_PI/180.0)
#endif

using namespace cnoid;
using namespace boost;

#define MAX_EVENTS     	10
#define STDIN_FD		0

// RAMDISK上の共有メモリ位置
#ifndef SHM_IN_NAME
#define SHM_IN_NAME             ("/tmp/shm/HydraIn")
#endif

#ifndef SHM_OUT_NAME
#define SHM_OUT_NAME            ("/tmp/shm/HydraOut")
#endif

#ifndef SEM_ECATSYNC_NAME
#define SEM_ECATSYNC_NAME ("/HydraSync")
#endif

class HydraPosControl : public SimpleController
{
    BodyPtr body;
    double dt;

private:
    std::vector<joint_state_t>  joint_state;
    std::vector<joint_cmd_t>    joint_cmd;
    std::vector<eha_state_t>    eha_state;
    std::vector<eha_cmd_t>      eha_cmd;
    std::vector<sensor_state_t> sensor_state;
    std::vector<sensor_cmd_t>   sensor_cmd;

    CButter20  butter[HYDRA_JNT_MAX];

    double joint_position[HYDRA_JNT_MAX];
    double EHA_position[EHA_MAX];
    double joint_position_prev[HYDRA_JNT_MAX];
    bool jlim;
    double t;

    filesystem::path log_filename;
    filesystem::path log_filepath;
    filesystem::path config_filename;
    filesystem::path config_filepath;

//    std::string log_file;

    CHydraShmServer *pShmServer;

    IHydraIPC *pIPC;

    CHydraDataRec recorder;

    CHydraConfigLoader config_loader;
public:
    HydraPosControl();
    ~HydraPosControl();
    virtual bool initialize(SimpleControllerIO* io);
    virtual bool control(void);

private:
    void measure_state(void);
    void output_command(void);
    void calc_parallel_constraint(void);

    bool EHA_is_vane(int i);
    bool EHA_is_hand(int i);
    bool joint_is_hand(int i);

	// 設定ファイル読み込み用
	void GetCurrentDllPath(filesystem::path& path);
    void ignore_space_and_tab(std::string& str);

	// controllerのdllの場所
	filesystem::path _abs_dll_path;
};

#endif //__HYDRAPOSCONTROL_H_

