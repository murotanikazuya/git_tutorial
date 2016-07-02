/*-----------------------------------------------------------------------------
 * trqsrv.h
 * Copyright                okamoto
 * Response                 okamoto
 * Description              --
 *---------------------------------------------------------------------------*/

#ifndef __TRQSRV_H__
#define __TRQSRV_H__ 1

/*-INCLUDES------------------------------------------------------------------*/
#ifndef INC_ECTYPE
#include "EcType.h"
#endif
#include "DCDemo.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "hydra_types.h"
/*-DEFINES-------------------------------------------------------------------*/
#define MBX_TIMEOUT 	5000
#define DEF_CTRL_TIC	5000

#define TIM_CTRL_CYCLE			0.001f

//#define M_PI					(3.141592654)
//#define M_PI2                 (M_PI*2)
//#define rad_to_deg(rad)       (rad*180.0f/M_PI)
//#define deg_to_rad(deg)       (deg*M_PI/180.0f)
//#define rpm_to_radsec(rpm)    (rpm*M_PI2/60.0f)
//#define radsec_to_rpm(radsec) (radsec*60.0f/M_PI2)


//PDOファイル格納領域
#define FILE_EXT				".csv"
#define EFD_SOCK_NAME			("/dev/shm/TS_EFD")
#define MAX_EVENTS     			10
#define STDIN_FD				0

#define DATA_BUF_ALLSIZE	16384
#define DATA_BUF_SIZE		1024

/*-TYPEDEFS------------------------------------------------------------------*/
// Thread Index
typedef enum  _T_eThread_Index {
    THRD_IDX_JOB	= 0,
    THRD_IDX_CONSOLE,
    THRD_IDX_LOGSAVE,

    //MAX
    THRD_IDX_MAX
} T_eThread_index;


/*-GLOBAL VARIABLES----------------------------------------------------------*/



/*-LOCAL VARIABLES-----------------------------------------------------------*/
/*-FUNCTIONS-----------------------------------------------------------------*/
extern EC_T_VOID  tConsoleTask(EC_T_VOID* pvThreadParamDesc);

extern EC_T_VOID LogSave_NewFile(EC_T_VOID);
extern EC_T_VOID LogSave_FwriteStart(EC_T_VOID);
extern EC_T_VOID LogSave_FwriteStop(EC_T_VOID);

#endif /* __TRQSRV_H__ */

/*-END OF SOURCE FILE--------------------------------------------------------*/
