/*-----------------------------------------------------------------------------
 * DCDemo.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master demo header
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include "DCDemoConfig.h"
#include "ecatNotification.h"
#include "ecatDemoCommon.h"
#ifdef VXWORKS
#include "wvLib.h"
#endif
#include "trqsrv.h"

/*-MACROS--------------------------------------------------------------------*/

/*-DEFINES-------------------------------------------------------------------*/

#define MAX_LINKLAYER 5

#if (defined EC_SOCKET_SUPPORTED)
/* the RAS server is necessary to support the EC-Engineer or other remote applications */
  #define ATEMRAS_SERVER
#endif
#define REMOTE_WD_TO_LIMIT          10000
#define REMOTE_CYCLE_TIME           2
#define ATDS_CFG_DEFAULT_PORT       ((EC_T_WORD)6000)

#define CYC_CHK_OVERLOAD_FLG 	10000
#define CYC_CHK_OVERLOAD_MAX 	5

#define SLV_NUM_OMRON			1
#define SLV_NUM_MAXON			7
#define SLV_NUM_TAMAGAWA		14	
#define SLV_NUM_MD4KW_3M		16	
#define SLV_NUM_MD4KW_2MFS		2	
#define SLV_NUM_MD4KW_HAND		2	
#define SLV_NUM_MD4KW_IMU		1	
#define SLAVE_NOT_FOUND         0xFFFF

/*-TYPEDEFS------------------------------------------------------------------*/

/*-GLOBAL VARIABLES----------------------------------------------------------*/


/*-FORWARD DECLARATIONS------------------------------------------------------*/
EC_T_DWORD DCDemo(
     CAtEmLogging*       poLog
    ,EC_T_CNF_TYPE       eCnfType
    ,EC_T_PBYTE          pbyCnfData
    ,EC_T_DWORD          dwCnfDataLen
    ,EC_T_DWORD          dwBusCycleTimeUsec
    ,EC_T_INT            nVerbose
    ,EC_T_DWORD          dwDuration
    ,EC_T_CHAR			 *pParamFile
    ,EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam
    ,EC_T_VOID*          pvTimingEvent
    ,EC_T_DWORD          dwCpuIndex
    ,EC_T_BOOL           bEnaPerfJobs
#ifdef ATEMRAS_SERVER 
    ,EC_T_WORD           wServerPort
#endif
    ,EC_T_BOOL           bRedundancyOn
    ,EC_T_VOID*          pvSecondaryLinkParam
    ,EC_T_BOOL           bNoMasterSync
    );

/*--------------------------------------------------------------------------*/
/* Performance measurements of jobs                                         */
/* This is only available on CPUs with TSC support                          */
/*--------------------------------------------------------------------------*/

#define JOB_ProcessAllRxFrames  0
#define JOB_SendAllCycFrames    1
#define JOB_MasterTimer         2
#define JOB_SendAcycFrames      3
#define PERF_CycleTime          4
#define PERF_myAppWorkpd        5
#define PERF_DCM_Logfile        6
#define MAX_JOB_NUM             7

#define PERF_MEASURE_JOBS_INIT(msgcb)  if(S_bEnaPerfJobs) {ecatPerfMeasInit(&S_TscMeasDesc,0,MAX_JOB_NUM, msgcb); ecatPerfMeasEnable(&S_TscMeasDesc);}
#define PERF_MEASURE_JOBS_SHOW()  ecatPerfMeasShow(&S_TscMeasDesc,0xFFFFFFFF, S_aszMeasInfo)
#define PERF_JOB_START(nJobIndex) ecatPerfMeasStart(&S_TscMeasDesc,(EC_T_DWORD)(nJobIndex))
#define PERF_JOB_END(nJobIndex)   ecatPerfMeasEnd(&S_TscMeasDesc,(EC_T_DWORD)(nJobIndex))

/*-END OF SOURCE FILE--------------------------------------------------------*/
