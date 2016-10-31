/*-----------------------------------------------------------------------------
 * DCDemo.cpp
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master demo application
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include <AtEthercat.h>

#include <sys/stat.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>

#include "DCDemo.h"
#include "Logging.h"
#include "trqsrv.h"
#include "hydra.h"
#include "CalcParallelJoint.h"
#include "CalcForceTorque.h"
#include "robot_hydra_id.h"

#include "ihydraipc.h"
#include "chydrashm.h"
#include "chydrashmserver.h"

#ifdef ATEMRAS_SERVER 
#include <AtEmRasSrv.h>
#endif


#include <sstream>
#include <fstream>

using namespace std;
/*-DEFINES-------------------------------------------------------------------*/

#define DCM_ENABLE_LOGFILE
#define LogMsg      S_poLog->LogMsg
#define LogError    S_poLog->LogError
#define LogDcm      S_poLog->LogDcm

/* master internal debug level */
#define ERR_MSG_TO_LINK_LAYER                   0   /* 0=off, 1=on */

#if (defined LINUX)
#define NSEC_PER_SEC (1000000000)
#endif /* LINUX */

#define PARAM_OUTPUT
//#define ONLY_UPPER_BODY
#define LOWERBODY_3M_NUM 5

//#define DBG_KNEE

/*-TYPEDEFS------------------------------------------------------------------*/

/*-GLOBAL VARIABLES-----------------------------------------------------------*/

extern EC_T_BOOL HydraOnlyUpperbody;

/*-LOCAL VARIABLES-----------------------------------------------------------*/
EC_T_DWORD          dwClntId = 0;
static CAtEmLogging*       S_poLog    = EC_NULL;
static T_THREAD_PARAM 	   S_ThreadParam[THRD_IDX_MAX];
static EC_T_PVOID          S_pvtThread[THRD_IDX_MAX];

#ifdef ATEMRAS_SERVER 
static EC_T_BOOL           S_bRasSrvStarted  = EC_FALSE;
static EC_T_PVOID          S_pvRemoteApiSrvH = EC_NULL;
#endif
static EC_T_BOOL           S_bNoDcm          = EC_FALSE;
static EC_T_BOOL           S_bEnaPerfJobs    = EC_FALSE;
static EC_T_TSC_MEAS_DESC  S_TscMeasDesc;


static EC_T_CHAR*          S_aszMeasInfo[MAX_JOB_NUM] =
{
    (EC_T_CHAR*)"JOB_ProcessAllRxFrames",
    (EC_T_CHAR*)"JOB_SendAllCycFrames  ",
    (EC_T_CHAR*)"JOB_MasterTimer       ",
    (EC_T_CHAR*)"JOB_SendAcycFrames    ",
    (EC_T_CHAR*)"Cycle Time            ",
    (EC_T_CHAR*)"myAppWorkPd           ",
    (EC_T_CHAR*)"Write DCM logfile     "
};

static T_ALL_SLAVE_INFO   	tAllSlv;
extern EC_T_BOOL			flg_NewFile;
extern EC_T_BOOL			flg_NewLabel;
extern EC_T_BOOL			flg_FileUpdate;

// 共有メモリ用
IHydraIPC       *pIPC;
CHydraShmServer *pShmServer;

joint_state_t  joint_state[HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX];
joint_cmd_t    joint_cmd[HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX];
eha_state_t    eha_state[EHA_MAX];
eha_cmd_t      eha_cmd[EHA_MAX];
sensor_state_t sensor_state[1];
sensor_cmd_t   sensor_cmd[1];


/*-FORWARD DECLARATIONS------------------------------------------------------*/
static EC_T_DWORD atemDemoInit(
        EC_T_CNF_TYPE       eCnfType                /* [in]  Enum type of configuration data provided */
        ,EC_T_PBYTE          pbyCnfData              /* [in]  Configuration data */
        ,EC_T_DWORD          dwCnfDataLen            /* [in]  Length of configuration data in byte */
        ,EC_T_DWORD          dwBusCycleTimeUsec      /* [in]  bus cycle time in usec */
        ,CEmNotification*    pNotification           /* [in]  notification object */
        ,EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam/* [in]  pointer to link parameter */
        ,EC_T_DWORD          dwCpuIndex              /* [in]  SMP only: CPU index */
        #ifdef ATEMRAS_SERVER
        ,EC_T_WORD           wServerPort             /* [in]   Remote API Server Port */
        #endif
        ,EC_T_BOOL           bRedundancyOn           /* [in]  Redundancy active */
        ,EC_T_VOID*          pvSecondaryLinkParam    /* [in]  Redundancy Link Layer Parameter */
        );

static EC_T_DWORD atemDemoDeInit(EC_T_VOID);
static EC_T_DWORD ecatNotifyCallback(EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
#if (defined ATEMRAS_SERVER)
static EC_T_DWORD RasNotifyWrapper(EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
#endif
static EC_T_VOID  tEcJobTask(EC_T_VOID* pvThreadParamDesc);
static EC_T_VOID  tLogSaveTask(EC_T_VOID* pvThreadParamDesc);
//static EC_T_VOID  tLogSaveTask_jnt_tau_calib(EC_T_VOID* pvThreadParamDesc);
static EC_T_VOID  tLogSaveTask_dbg(EC_T_VOID* pvThreadParamDesc);


/*-MYAPP---------------------------------------------------------------------*/
static EC_T_DWORD myAppInit     (CAtEmLogging *poLog, EC_T_INT nVerbose);
static EC_T_DWORD myAppPrepare  (CAtEmLogging *poLog, EC_T_INT nVerbose);
static EC_T_DWORD myAppSetup    (CAtEmLogging *poLog, EC_T_INT nVerbose, 
                                 EC_T_DWORD dwClntId, EC_T_CHAR *pParamFile);
static EC_T_DWORD myAppWorkpd   (CAtEmLogging *poLog, EC_T_INT nVerbose);
static EC_T_DWORD myAppDiagnosis(CAtEmLogging *poLog, EC_T_INT nVerbose);
static EC_T_DWORD myAppNotify   (EC_T_DWORD dwCode, EC_T_NOTIFYPARMS* pParms);
/* Demo code: End */

static EC_T_DWORD myAppPdoInput(T_ALL_SLAVE_INFO* pAllSlv);
static EC_T_DWORD myAppPdoOutput(T_ALL_SLAVE_INFO* pAllSlv);
static EC_T_DWORD myAppSdoTfer (T_ALL_SLAVE_INFO* pAllSlv);

static EC_T_CHAR* TaAlarmText( EC_T_BYTE bAlarmCode);

static EC_T_VOID ClipEHAPos(eha_state_t *state, EC_T_DWORD offset, const unsigned int llim, const unsigned int ulim, const double phys_conv);

/*-FUNCTION DEFINITIONS------------------------------------------------------*/

/********************************************************************************/
/** \brief  EtherCAT Master DC demo Application
*
* This is a EtherCAT Master DC demo application.
*
* \return  Status value.
*/
EC_T_DWORD DCDemo(
        CAtEmLogging*      poLog
        ,EC_T_CNF_TYPE       eCnfType                /* [in] Enum type of configuration data provided */
        ,EC_T_PBYTE          pbyCnfData              /* [in] Configuration data */
        ,EC_T_DWORD          dwCnfDataLen            /* [in] Length of configuration data in byte */
        ,EC_T_DWORD          dwBusCycleTimeUsec      /* [in]  bus cycle time in usec */
        ,EC_T_INT            nVerbose                /* [in]  verbosity level */
        ,EC_T_DWORD          dwDuration              /* [in]  test duration in msec (0 = forever) */
        ,EC_T_CHAR		    *pParamFile				/* [in]  parameter file  */
        ,EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam /* [in]  pointer to link parameter */
        ,EC_T_VOID*          pvTimingEvent           /* [in]  Timing event handle */
        ,EC_T_DWORD          dwCpuIndex              /* [in]  SMP only: CPU index */
        ,EC_T_BOOL           bEnaPerfJobs            /* [in]  Performance measurement */
        #ifdef ATEMRAS_SERVER
        ,EC_T_WORD           wServerPort             /* [in]   Remote API Server Port */
        #endif
        ,EC_T_BOOL           bRedundancyOn           /* [in]  Redundancy active */
        ,EC_T_VOID*          pvSecondaryLinkParam    /* [in]  Redundancy Link Layer Parameter */
        ,EC_T_BOOL           bNoDcm                  /* [in]  Disables / Enables the Master Sync Controller */
        )
{
    EC_T_DWORD                  dwRetVal            = EC_E_NOERROR;
    EC_T_DWORD                  dwRes               = EC_E_NOERROR;
    CTimeoutHelper              oTimeout;
    EC_T_DWORD                  dwScanBustimeout    = 5000;
    EC_T_LINK_DEV_OPEN_PARAM    oLinkDevOpenParam;
    EC_T_DWORD                  dwStartTimeout      = ETHERCAT_START_TIMEOUT;
    CEmNotification*            pNotification       = EC_NULL;
    int							cnt, nfds;

    S_poLog = poLog;
    S_bEnaPerfJobs = bEnaPerfJobs;  /* Enable/Disable Performance measurement */
    S_bNoDcm       = bNoDcm;        /* Disables / Enables the Master Sync Controller */
    PERF_MEASURE_JOBS_INIT(EC_NULL);

    pNotification = EC_NEW(CEmNotification(0, poLog));
    if (EC_NULL == pNotification){
        dwRetVal = EC_E_NOMEMORY;
        goto Exit;
    }
    pNotification->Verbose(nVerbose);

    S_ThreadParam[THRD_IDX_JOB].pvTimingEvent = pvTimingEvent; // JOBスレッド起動イベント

    /******************************************************/
    /* Demo code: Remove/change this in your application  */
    /* Initialize application                             */
    /******************************************************/
    dwRes = myAppInit(poLog, nVerbose);
    if (EC_E_NOERROR != dwRes){
        LogError( (EC_T_CHAR*)"myAppInit failed, error code: 0x%x", dwRes );
        dwRetVal = dwRes;
        goto Exit;
    }

    if (0 != nVerbose) LogMsg( "\n==========================" );
    if (0 != nVerbose) LogMsg( "Initialize EtherCAT Master" );
    if (0 != nVerbose) LogMsg( "==========================" );

    /******************************************************/
    /* Setup the Link Layer Parameters                    */
    /******************************************************/
    {
        /* WinPcap adapter */
        /* EC_T_BYTE arrIpAddress[] = {172,6,6,100}; */

        /* get parameters for WinPcap*/
        /* CreateLinkParaWinPcap(&oLinkDevOpenParam, arrIpAddress, EcLinkMode_POLLING); */

        /* get parameters for Intel Pro 1000 series */
        /* CreateLinkParaI8254x(&oLinkDevOpenParam, 1, EcLinkMode_POLLING, 0); */

        /* get parameters for Intel Pro 100 series */
        /* CreateLinkParaI8255x(&oLinkDevOpenParam, 1, EcLinkMode_POLLING, 0); */

        /* get parameters for Realtek Gigabit series */
        /* CreateLinkParaRTL8169(&oLinkDevOpenParam, 1, EcLinkMode_POLLING, 0); */

        /* get parameters for Realtek 100Mbit series */
        /* CreateLinkParaRTL8139(&oLinkDevOpenParam, 1, EcLinkMode_POLLING, 0); */
    }

    /* get link layer parameters from command line */
    OsMemcpy(&oLinkDevOpenParam, poLinkDevOpenParam, sizeof(oLinkDevOpenParam));

    /* Initialize EtherCAT master */
    /******************************/
    dwRes = atemDemoInit(
                eCnfType, pbyCnfData, dwCnfDataLen,
                dwBusCycleTimeUsec, pNotification, &oLinkDevOpenParam, dwCpuIndex
            #ifdef ATEMRAS_SERVER
                ,wServerPort
            #endif
                ,bRedundancyOn, pvSecondaryLinkParam
                );

    if (dwRes != EC_E_NOERROR) {
        LogError((EC_T_CHAR*)"atemDemoInit failed, error code: 0x%x", dwRes);
        dwRetVal = dwRes;
        goto Exit;
    }

    /**********************/
    /* print found slaves */
    /**********************/
    if (nVerbose >= 2) {
        EC_T_SB_STATUS_NTFY_DESC oBusScanStatus = {0};

        DoBusScan(0, poLog, &oBusScanStatus, EC_TRUE, EC_FALSE, dwScanBustimeout);

        PrintSlaveInfos(0, poLog);
    }

    /**********************************************************************************/
    /* Start the EtherCAT bus --> switch to state OPERATIONAL if ENI file is provided */
    /**********************************************************************************/
    if (0 != nVerbose) LogMsg( "\n=====================" );
    if (0 != nVerbose) LogMsg( "Start EtherCAT Master" );
    if (0 != nVerbose) LogMsg( "=====================" );

    /* set master and bus state to INIT */
    dwRes = ecatSetMasterState(dwStartTimeout+dwScanBustimeout, eEcatState_INIT);
    pNotification->ProcessNotificationJobs();
    if (dwRes != EC_E_NOERROR)
    {
        LogError("Cannot start set master state to INIT (Result = %s (0x%lx))", EcErrorText(dwRes), dwRes);
        dwRetVal = dwRes;
        goto Exit;
    }

    /******************************************************/
    /* Demo code: Remove/change this in your application  */
    /* Search for some well known slaves                  */
    /******************************************************/
    dwRes = myAppPrepare(poLog, nVerbose);
    if (EC_E_NOERROR != dwRes) {
        LogError((EC_T_CHAR*)"myAppPrepare failed, error code: 0x%x", dwRes);
        dwRetVal = dwRes;
        goto Exit;
    }
    /* set master and bus state to PREOP */
    dwRes = ecatSetMasterState(dwStartTimeout+dwScanBustimeout, eEcatState_PREOP);
    pNotification->ProcessNotificationJobs();
    if (dwRes != EC_E_NOERROR) {
        LogError("Cannot start set master state to PREOP (Result = %s (0x%lx))", EcErrorText(dwRes), dwRes);
        dwRetVal = dwRes;
        goto Exit;
    }

    /* skip this step if demo started without ENI */
    if (pbyCnfData != EC_NULL) {
        /******************************************************/
        /* Demo code: Remove/change this in your application  */
        /* Setup the found slaves                             */
        /* After reaching the PREOP state CANOpen SDO uploads */
        /* and downloads are possible                         */
        /******************************************************/
        dwRes = myAppSetup(poLog, nVerbose, dwClntId, pParamFile);
        if (EC_E_NOERROR != dwRes) {
            LogError((EC_T_CHAR*)"myAppSetup failed, error code: 0x%x", dwRes);
            dwRetVal = dwRes;
            goto Exit;
        }
        /* set master and bus state to SAFEOP */
        dwRes = ecatSetMasterState(dwStartTimeout+dwScanBustimeout, eEcatState_SAFEOP);
        pNotification->ProcessNotificationJobs();
        if (dwRes != EC_E_NOERROR) {
            LogError("Cannot start set master state to SAFEOP (Result = %s (0x%lx))", EcErrorText(dwRes), dwRes);

            /* most of the time SAFEOP is not reachable due to DCM */
            if (!S_bNoDcm) {
                EC_T_DWORD dwStatus = 0;
                EC_T_INT   nDiffCur = 0, nDiffAvg = 0, nDiffMax = 0;

                dwRes = ecatDcmGetStatus(&dwStatus, &nDiffCur, &nDiffAvg, &nDiffMax);
                if (dwRes == EC_E_NOERROR) {
                    if (dwStatus != EC_E_NOERROR) {
                        LogError("DCM Status: %s (0x%08X)\n", EcErrorText(dwStatus), dwStatus);
                    }
                } else {
                    LogError("Cannot get DCM status! %s (0x%08X)\n", EcErrorText(dwRes), dwRes);
                }
            }
            dwRetVal = dwRes;
            goto Exit;
        }
        /* set master and bus state to OP */
        dwRes = ecatSetMasterState(dwStartTimeout+dwScanBustimeout, eEcatState_OP);
        pNotification->ProcessNotificationJobs();
        if (dwRes != EC_E_NOERROR) {
            LogError("Cannot start set master state to OP (Result = %s (0x%lx))", EcErrorText(dwRes), dwRes);
            dwRetVal = dwRes;
            goto Exit;
        }
    }
    else
    {
        if (0 != nVerbose)
        {
            EC_T_INT				iCnt;
            EC_T_BOOL  				bStopReading  = EC_FALSE;      /* Flag to stop object dictionary reading */
            EC_T_WORD 				MaxSlaves;	// 二つめのSlaveを決め打ちで
            EC_T_GET_SLAVE_INFO 	MySlave;

            LogMsg("No ENI file provided. EC-Master started with generated ENI file.");

            MaxSlaves = ecatGetNumConnectedSlaves();
            // AddrNoは１始まり
            for(iCnt = 1; iCnt <= MaxSlaves; iCnt++)
            {
                if ((dwRes = ecatGetSlaveInfo(EC_TRUE, iCnt, &MySlave)) == EC_E_NOERROR)
                {
                    LogMsg( "*************************************************************" );
                    LogMsg( "****             READ OBJECT DICTIONARY [%02d/%02d]          ****", iCnt , MaxSlaves);
                    LogMsg( "*************************************************************" );

                    CoeReadObjectDictionary(0, poLog, nVerbose, &bStopReading,
                                            pNotification->GetClientID(), MySlave.dwSlaveId, EC_TRUE, MBX_TIMEOUT);
                }
                else
                {
                    LogError("ecatGetSlaveInfo error!", EcErrorText(dwRes), dwRes);
                }
            }
        }

        dwRetVal = dwRes;
        goto Exit;
    }

    if (bEnaPerfJobs) {
        LogMsg("");
        LogMsg("Job times during startup <INIT> to <%s>:", EcStateString(ecatGetMasterState()));
        PERF_MEASURE_JOBS_SHOW();       /* show job times */
        LogMsg("");
        ecatPerfMeasReset(&S_TscMeasDesc, 0xFFFFFFFF);        /* clear job times of startup phase */
    }

    // Console Task Start
    OsSetEvent( S_ThreadParam[THRD_IDX_CONSOLE].pvTimingEvent);

    /****************/
    /* run the demo */
    /****************/
    if (dwDuration != 0) {
        oTimeout.Start(dwDuration);
    }

    while (bRun
           && (S_ThreadParam[THRD_IDX_CONSOLE].bThreadRunning)
           && (!oTimeout.IsStarted() || !oTimeout.IsElapsed())) {
        if (nVerbose >= 2) {
            PERF_MEASURE_JOBS_SHOW();       /* show job times */
        }
        bRun = !OsTerminateAppRequest();/* check if demo shall terminate */

#if 0
        /* simple error recovery mechanism: set master into INIT and OP again if at least one slave becomes non-OP */
        if (!pNotification->AllDevsOperational() && (pbyCnfData != EC_NULL)) {
            EC_T_DWORD dwNumConnectedSlaves = ecatGetNumConnectedSlaves();

            if (ecatGetMasterState() == eEcatState_OP) {
                dwRes = ecatSetMasterState( dwStartTimeout+dwScanBustimeout, eEcatState_SAFEOP);
                dwRetVal = dwRes;
            }

            if (dwNumConnectedSlaves == ecatGetNumConfiguredSlaves()) {
                LogError("Not all slaves are operational, try to set master into INIT state.\n");
                dwRes = ecatSetMasterState(dwStartTimeout+dwScanBustimeout, eEcatState_INIT);
                if (dwRes == EC_E_NOERROR) {
                    LogError("Not all slaves are operational, try to set master into OP state.\n");
                    dwRes = ecatSetMasterState(dwStartTimeout+dwScanBustimeout, eEcatState_OP);
                    dwRetVal = dwRes;
                }
            }
        }
#endif
        /*****************************************************/
        /* Demo code: Remove/change this in your application */
        /* Do some diagnosis job here outside the job task   */
        /*****************************************************/
        myAppDiagnosis(poLog, nVerbose);

        if (!S_bNoDcm) {
            static CTimeoutHelper s_DcmStatusTimeout;
            static EC_T_BOOL s_bFirstStatus = EC_TRUE;
            EC_T_DWORD dwStatus = 0;
            EC_T_INT   nDiffCur = 0, nDiffAvg = 0, nDiffMax = 0;

            dwRes = ecatDcmGetStatus(&dwStatus, &nDiffCur, &nDiffAvg, &nDiffMax);
            if (dwRes == EC_E_NOERROR) {
                if (s_bFirstStatus) {
                    LogMsg("DCM during startup (<INIT> to <%s>)", EcStateString(ecatGetMasterState()));
                }
                if (dwStatus != EC_E_NOERROR) {
                    LogMsg("DCM Status: %s (0x%08X)\n", EcErrorText(dwStatus), dwStatus);
                }
                if (nVerbose >= 3) {
                    if (!s_DcmStatusTimeout.IsStarted() || s_DcmStatusTimeout.IsElapsed()) {
                        LogMsg("DCM Diff(ns): Cur=%7d, Avg=%7d, Max=%7d", nDiffCur, nDiffAvg, nDiffMax);
                        s_DcmStatusTimeout.Start(5000);
#if 0
                        dwRes = ecatDcmShowStatus();
                        if (dwRes != EC_E_NOERROR) {
                            LogError("Cannot get DCM show status! %s (0x%08X)\n", EcErrorText(dwRes), dwRes);
                        }
#endif
                    }
                }
                if (s_bFirstStatus) {
                    s_bFirstStatus = EC_FALSE;
#if (defined ATECAT_VERSION) && (ATECAT_VERSION >= 0x02040106)
                    ecatDcmResetStatus();
#endif
                }
            } else {
                LogError("Cannot get DCM status! %s (0x%08X)\n", EcErrorText(dwRes), dwRes);
            }
        }
        /* process notification jobs */
        pNotification->ProcessNotificationJobs();

        OsSleep(5);
    }

Exit:
    if (EC_NULL != pNotification) {
        if (0 != nVerbose) LogMsg( "\n========================" );
        if (0 != nVerbose) LogMsg( "Shutdown EtherCAT Master" );
        if (0 != nVerbose) LogMsg( "========================" );
    }
    
    /**************************************************/
    /* Stop the EtherCAT bus --> switch to state INIT */
    /**************************************************/
    dwRes = ecatStop(dwStartTimeout);
    if (EC_E_NOERROR != dwRes) {
        LogError("Cannot stop EtherCAT-Master! %s (0x%lx)", EcErrorText(dwRes), dwRes);
        if (dwRetVal == EC_E_NOERROR) {
            dwRetVal = dwRes;
        }
    }

    /* De-Initialize EtherCAT master */
    /*********************************/
    dwRes = atemDemoDeInit();
    if (dwRetVal == EC_E_NOERROR) {
        dwRetVal = dwRes;
    }

    SafeDelete(pNotification);

    return dwRetVal;
}


/********************************************************************************/
/** \brief  Trigger jobs to drive master, and update process data.
*
* \return N/A
*/
#define	COE_BUF_SIZE	4
static EC_T_VOID tEcJobTask(EC_T_VOID* pvThreadParamDesc)
{
    EC_T_DWORD          dwRes             	= EC_E_ERROR;
    T_THREAD_PARAM*  	pThreadParam  		= (T_THREAD_PARAM*)pvThreadParamDesc;
    EC_T_CPUSET     	CpuSet;
    EC_T_BOOL           bPrevCycProcessed 	= EC_FALSE;
    EC_T_INT            nOverloadCounter  	= 0;               /* counter to check if cycle time is to short */
    EC_T_BOOL           bOk;

    EC_CPUSET_ZERO(CpuSet);
    EC_CPUSET_SET(CpuSet, pThreadParam->dwCpuIndex);
    bOk = OsSetThreadAffinity(EC_NULL, CpuSet);
    if (!bOk) {
        LogError("Error: Set job task affinitiy, invalid CPU index %d\n", pThreadParam->dwCpuIndex);
        goto Exit;
    }

    /* demo loop */
    pThreadParam->bThreadRunning = EC_TRUE;

    do {
        /* wait for next cycle (event from scheduler task) */
#if (defined RTAI)
        OsSleepTillTick(); /* period is set after creating jobtask */
#else
        OsWaitForEvent(pThreadParam->pvTimingEvent, EC_WAITINFINITE);

#if 0   /* toggle DTR (pin 4) signal on COM 1 */
        __asm push edx;
        __asm mov  edx, 0x3FC;
        __asm mov  al,  1;
        __asm out  dx, al;
        __asm pop  edx
        #endif
        #endif /* !RTAI */

                PERF_JOB_END(PERF_CycleTime);
        PERF_JOB_START(PERF_CycleTime);

        /* process all received frames (read new input values) */
        PERF_JOB_START(JOB_ProcessAllRxFrames);
        dwRes = ecatExecJob( eUsrJob_ProcessAllRxFrames, &bPrevCycProcessed );
        if (EC_E_NOERROR != dwRes
                && EC_E_INVALIDSTATE != dwRes
                && EC_E_LINK_DISCONNECTED != dwRes) {
            LogError("ERROR: ecatExecJob( eUsrJob_ProcessAllRxFrames): %s (0x%lx)",
                     EcErrorText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_ProcessAllRxFrames);

        if (EC_E_NOERROR == dwRes) {
            if (!bPrevCycProcessed) {
                /* it is not reasonable, that more than 5 continuous frames are lost */
                nOverloadCounter += 10;
                if (nOverloadCounter >= 50) {
                    LogError( "Error: System overload: Cycle time too short or huge jitter!" );
                } else {
                    LogError( "eUsrJob_ProcessAllRxFrames - not all previously sent frames are received/processed (frame loss)!" );
                }
            } else {
                /* everything o.k.? If yes, decrement overload counter */
                if (nOverloadCounter > 0)    nOverloadCounter--;
            }
        }
        /*********************************************************/
        /* Handle DCM logging                                    */
        /*********************************************************/
#ifdef DCM_ENABLE_LOGFILE
        PERF_JOB_START(PERF_DCM_Logfile);
        {
            EC_T_CHAR* pszLog = EC_NULL;

            ecatDcmGetLog(&pszLog);
            if (EC_NULL != pszLog){
                LogDcm(pszLog);
            }
        }
        PERF_JOB_END(PERF_DCM_Logfile);
#endif

        // PDOデータの取り込み
        myAppPdoInput(&tAllSlv);

        // 共有メモリ経由のSDOコマンドを処理する
        dwRes = myAppSdoTfer( &tAllSlv );
        if (EC_E_ERROR == dwRes )
        {
            bRun = EC_FALSE;
            break;
        }

        /*****************************************************/
        /* Demo code: Remove/change this in your application */
        /* Working process data cyclic call                  */
        /*****************************************************/
        PERF_JOB_START(PERF_myAppWorkpd);
        myAppWorkpd(pThreadParam->pLogInst, pThreadParam->pNotInst->Verbose());
        PERF_JOB_END(PERF_myAppWorkpd);

        // PDOデータの出力
        myAppPdoOutput(&tAllSlv);

        /* write output values of current cycle, by sending all cyclic frames */
        PERF_JOB_START(JOB_SendAllCycFrames);
        dwRes = ecatExecJob( eUsrJob_SendAllCycFrames, EC_NULL );
        if (EC_E_NOERROR != dwRes
                && EC_E_INVALIDSTATE != dwRes
                && EC_E_LINK_DISCONNECTED != dwRes) {
            LogError("ecatExecJob( eUsrJob_SendAllCycFrames,    EC_NULL ): %s (0x%lx)", EcErrorText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_SendAllCycFrames);

        /* remove this code when not using the evaluation version */
        if (EC_E_EVAL_EXPIRED == dwRes ) {
            /* set shutdown flag */
            bRun = EC_FALSE;
        }

        /* Execute some administrative jobs. No bus traffic is performed by this function */
        PERF_JOB_START(JOB_MasterTimer);
        dwRes = ecatExecJob(eUsrJob_MasterTimer, EC_NULL);
        if (EC_E_NOERROR != dwRes && EC_E_INVALIDSTATE != dwRes) {
            LogError("ecatExecJob(eUsrJob_MasterTimer, EC_NULL): %s (0x%lx)", EcErrorText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_MasterTimer);

        /* send queued acyclic EtherCAT frames */
        PERF_JOB_START(JOB_SendAcycFrames);
        dwRes = ecatExecJob(eUsrJob_SendAcycFrames, EC_NULL);
        if (EC_E_NOERROR != dwRes
                && EC_E_INVALIDSTATE != dwRes
                && EC_E_LINK_DISCONNECTED != dwRes) {
            LogError("ecatExecJob(eUsrJob_SendAcycFrames, EC_NULL): %s (0x%lx)", EcErrorText(dwRes), dwRes);
        }
        PERF_JOB_END(JOB_SendAcycFrames);

#if 0   /* toggle DTR (pin 4) signal on COM 1 */
        {
            __asm push edx;
            __asm mov  edx, 0x3FC;
            __asm mov  al,  0;
            __asm out  dx, al;
            __asm pop  edx
        }
#endif


#if !(defined NO_OS)
    } while (!pThreadParam->bThreadShutdown);

    PERF_MEASURE_JOBS_SHOW();

    pThreadParam->bThreadRunning = EC_FALSE;
#else
        /* in case of NO_OS the job task function is called cyclically within the timer ISR */
    } while (EC_FALSE);
    pThreadParam->bThreadRunning = !pThreadParam->bThreadShutdown;
#endif

Exit:

    return;
}



/********************************************************************************/
/** \brief  Initialize EtherCAT master
*
*
* \return  Status value.
*/
static EC_T_DWORD atemDemoInit(
        EC_T_CNF_TYPE       eCnfType                    /* [in] Enum type of configuration data provided */
        ,EC_T_PBYTE          pbyCnfData                  /* [in] Configuration data */
        ,EC_T_DWORD          dwCnfDataLen                /* [in] Length of configuration data in byte */
        ,EC_T_DWORD          dwBusCycleTimeUsec          /* [in]  bus cycle time in usec */
        ,CEmNotification*    pNotification               /* [in]  notification object */
        ,EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam    /* [in]  pointer to link parameter */
        ,EC_T_DWORD          dwCpuIndex                  /* [in]  SMP only: CPU index */
        #ifdef ATEMRAS_SERVER
        ,EC_T_WORD           wServerPort                 /* [in]   Remote API Server Port */
        #endif
        ,EC_T_BOOL           bRedundancyOn               /* [in]  Redundancy active */
        ,EC_T_VOID*          pvSecondaryLinkParam        /* [in]  Redundancy Link Layer Parameter */
        )
{
    EC_T_DWORD           dwRetVal                    = EC_E_ERROR;
    EC_T_DWORD           dwRes                       = EC_E_ERROR;
    EC_T_INITMASTERPARMS oInitParms                  = {0};    /* EtherCAT master parameters */
    EC_T_REGISTERRESULTS oRegisterResults            = {0};    /* client registration results */
    CTimeoutHelper       oTimeout;
    EC_T_CPUSET          CpuSet;
    EC_T_BOOL            bOk;
    EC_T_INT             loop;
    ETHERNET_ADDRESS     oSrcMacAddress;

#if (defined ATEMRAS_SERVER)
    ATEMRAS_T_SRVPARMS   oRemoteApiConfig;
    OsMemset(&oRemoteApiConfig, 0, sizeof(ATEMRAS_T_SRVPARMS));
#endif

    EC_CPUSET_ZERO(CpuSet);
    EC_CPUSET_SET(CpuSet, dwCpuIndex);
    bOk = OsSetThreadAffinity(EC_NULL, CpuSet);
    if (!bOk) {
        dwRetVal = EC_E_INVALIDPARM;
        LogError("Error: Set thread affinitiy, invalid CPU index %d\n", dwCpuIndex);
        goto Exit;
    }

    /* check if interrupt mode is selected */
    if (poLinkDevOpenParam->oLinkDevParam.eLinkMode != EcLinkMode_POLLING) {
        dwRetVal = EC_E_INVALIDPARM;
        LogError("Error: Link layer in 'interrupt' mode is not supported by ECMasterDemoDc. Please select 'polling' mode.");
        goto Exit;
    }

    /****************************************/
    /* static EtherCAT master configuration */
    /****************************************/

    /* initialize master */
    oInitParms.dwValPattern                                     = ATECAT_VALIDATION_PATTERN;
    oInitParms.dwVersion                                        = ATECAT_VERSION;

    oInitParms.pvConfigParms                                    = EC_NULL;
    oInitParms.pvLinkParms                                      = (EC_T_PVOID)poLinkDevOpenParam;
    oInitParms.wMaxBusSlaves                                    = MASTER_CFG_ECAT_MAX_BUS_SLAVES;
    oInitParms.pMasterConfig                                    = &oInitParms.MasterConfig;
    oInitParms.MasterConfig.dwValPattern                        = ATECAT_VALIDATION_PATTERN;
    oInitParms.MasterConfig.nSlaveMultiplier                    = SLAVE_MULTIPLIER;
    oInitParms.MasterConfig.dwBusCycleTimeUsec                  = dwBusCycleTimeUsec;
    oInitParms.MasterConfig.dwEcatCmdMaxRetries                 = MASTER_CFG_ECAT_CMD_MAX_RETRIES;
    oInitParms.MasterConfig.dwEoeTimeout                        = MASTER_CFG_EOE_TIMEOUT;
    oInitParms.MasterConfig.dwFoeBusyTimeout                    = MASTER_CFG_FOE_BUSY_TIMEOUT;
    oInitParms.MasterConfig.dwMaxQueuedEthFrames                = MASTER_CFG_MAX_QUEUED_ETH_FRAMES;
    oInitParms.MasterConfig.dwMaxSlaveCmdPerFrame               = MASTER_CFG_MAX_SLAVECMD_PER_FRAME;
    oInitParms.MasterConfig.dwMaxSentQueuedFramesPerCyc         = MASTER_CFG_MAX_SENT_QUFRM_PER_CYC;
    oInitParms.MasterConfig.dwErrorMsgToLinkLayer               = ERR_MSG_TO_LINK_LAYER;
    oInitParms.MasterConfig.dwStateChangeDebug                  = 0;
    oInitParms.bUseRedundancy                                   = bRedundancyOn;
    oInitParms.pvLinkParmsRed                                   = pvSecondaryLinkParam;

#ifdef ATEMRAS_SERVER 
    if (0xFFFF != wServerPort) {
        LogMsg("Start Remote API Server now\n");
        oRemoteApiConfig.oAddr.dwAddr           = 0;                    /* INADDR_ANY */
        oRemoteApiConfig.wPort                  = wServerPort;
        oRemoteApiConfig.dwCycleTime            = REMOTE_CYCLE_TIME;    /* 10 msec for the begin */
        oRemoteApiConfig.dwWDTOLimit            = (REMOTE_WD_TO_LIMIT/REMOTE_CYCLE_TIME);
        /* WD Timeout after 100*100msec = 10secs */
        oRemoteApiConfig.dwReConTOLimit         = 6000;                 /* Reconnect Timeout after 600*100msec = 60secs + 10secs */
        oRemoteApiConfig.dwMasterPrio           = MAIN_THREAD_PRIO;
        oRemoteApiConfig.dwClientPrio           = MAIN_THREAD_PRIO;
        oRemoteApiConfig.pvNotifCtxt            = pNotification;        /* Notification context */
        oRemoteApiConfig.pfNotification         = RasNotifyWrapper;     /* Notification function for emras Layer */

        oRemoteApiConfig.dwConcNotifyAmount     = 100;                  /* for the first pre-allocate 50 Notification spaces */
        
        oRemoteApiConfig.dwMbxNotifyAmount      = 50;                   /* for the first pre-allocate 20 Notification spaces */
        oRemoteApiConfig.dwMbxUsrNotifySize     = 3000;                 /* 1,5K user space for Mailbox Notifications */
        
        oRemoteApiConfig.dwCycErrInterval       = 500;                  /* span between to consecutive cyclic notifications of same type */

        dwRes = emRasSrvStart(oRemoteApiConfig, &S_pvRemoteApiSrvH);
        if (EC_E_NOERROR != dwRes) {
            LogError("ERROR: Cannot spawn Remote API Server\n");
        }
        S_bRasSrvStarted = EC_TRUE;

#if (defined INCLUDE_RAS_TRACESUPPORT)
        if (pNotification->Verbose() > 2) {
            emRasSrvTraceEnable(EC_TRUE);
            LogMsg("Remote API Server: Tracing enabled\n");
        }
#endif
    }
#endif

    /////////////////////////////////////////////////////////////////
    // JOBスレッド以外のスレッド用Eventを作成する
    for(loop = 1; loop < THRD_IDX_MAX;loop++) {
        /* create timing event to trigger the thread */
        S_ThreadParam[loop].pvTimingEvent = OsCreateEvent();
        if( EC_NULL == S_ThreadParam[loop].pvTimingEvent) {
            OsDbgMsg("ERROR: insufficient memory to create timing event!\n");
            goto Exit;
        }

        /* create socket Thread */
        S_ThreadParam[loop].bThreadRunning		= EC_FALSE;
        S_ThreadParam[loop].bThreadShutdown		= EC_FALSE;
        S_ThreadParam[loop].pLogInst			= S_poLog;
        S_ThreadParam[loop].pNotInst			= pNotification;
        S_ThreadParam[loop].dwCpuIndex			= dwCpuIndex;
        S_ThreadParam[loop].dwBusCycleTimeUsec  = dwBusCycleTimeUsec;
    }

    /* 個別のパラメータを設定 */
    S_ThreadParam[THRD_IDX_CONSOLE].pvApplSpecParm1 = (EC_T_VOID*)&tAllSlv;

    /* 個別のパラメータを設定 */
    S_ThreadParam[THRD_IDX_LOGSAVE].pvApplSpecParm1 = (EC_T_VOID*)&tAllSlv;

    /* create Thread */
    S_pvtThread[THRD_IDX_CONSOLE] = OsCreateThread( (EC_T_CHAR*)"tConsoleTask", tConsoleTask,
                                                    CONSOLE_THREAD_PRIO, CONSOLE_THREAD_STACKSIZE,
                                                    (EC_T_VOID*)&S_ThreadParam[THRD_IDX_CONSOLE]);
    S_pvtThread[THRD_IDX_LOGSAVE] = OsCreateThread( (EC_T_CHAR*)"tLogSaveTask", tLogSaveTask_dbg,
                                                    LOGSAVE_THREAD_PRIO, LOG_THREAD_STACKSIZE,
                                                    (EC_T_VOID*)&S_ThreadParam[THRD_IDX_LOGSAVE]);


    pShmServer = new CHydraShmServer();
    pShmServer->SetShmInName(SHM_IN_NAME);
    pShmServer->SetShmOutName(SHM_OUT_NAME);
    pShmServer->SetSemName(SEM_ECATSYNC_NAME);

    pIPC = (IHydraIPC*)pShmServer;

    if(pIPC->Init()<0)
    {
        OsDbgMsg("ERROR: sfd open NG!\n");
        goto Exit;
    }
    //////////////////////////////////////////////////////////


    /*************************/
    /* initialize the master */
    /*************************/
    dwRes = ecatInitMaster(&oInitParms);
    if (dwRes != EC_E_NOERROR) {
        dwRetVal = dwRes;
        LogError("Cannot initialize EtherCAT-Master! (Result = %s 0x%x)", EcErrorText(dwRes), dwRes);
        goto Exit;
    }

    /* get and print MAC address */
    ecatGetSrcMacAddress(&oSrcMacAddress);
    LogMsg("EtherCAT network adapter MAC: %02X-%02X-%02X-%02X-%02X-%02X\n",
           oSrcMacAddress.b[0], oSrcMacAddress.b[1], oSrcMacAddress.b[2], oSrcMacAddress.b[3], oSrcMacAddress.b[4], oSrcMacAddress.b[5]);

    S_ThreadParam[THRD_IDX_JOB].bThreadRunning      = EC_FALSE;
    S_ThreadParam[THRD_IDX_JOB].bThreadShutdown     = EC_FALSE;
    S_ThreadParam[THRD_IDX_JOB].pLogInst	    = S_poLog;
    S_ThreadParam[THRD_IDX_JOB].pNotInst            = pNotification;
    S_ThreadParam[THRD_IDX_JOB].dwCpuIndex          = dwCpuIndex;
    S_ThreadParam[THRD_IDX_JOB].dwBusCycleTimeUsec  = dwBusCycleTimeUsec;
    S_pvtThread[THRD_IDX_JOB] = OsCreateThread((EC_T_CHAR*)"tEcJobTask",
                                               tEcJobTask, JOBS_THREAD_PRIO, JOBS_THREAD_STACKSIZE,
                                               (EC_T_VOID*)&S_ThreadParam[THRD_IDX_JOB]);

#ifdef RTAI
    OsMakeThreadPeriodic(S_pvtThread[THRD_IDX_JOB], dwBusCycleTimeUsec);
#endif

    /* wait until thread is running */
    oTimeout.Start(2000);
    while (!oTimeout.IsElapsed() && !S_ThreadParam[THRD_IDX_JOB].bThreadRunning) {
        OsSleep(10);
    }
    if (!S_ThreadParam[THRD_IDX_JOB].bThreadRunning) {
        dwRetVal = EC_E_TIMEOUT;
        LogError("Timeout starting JobTask");
        goto Exit;
    }
    oTimeout.Stop();

    /********************/
    /* Configure master */
    /********************/
    dwRes = ecatConfigureMaster(eCnfType, pbyCnfData, dwCnfDataLen);
    if (dwRes != EC_E_NOERROR) {
        dwRetVal = dwRes;
        LogError("Cannot configure EtherCAT-Master! %s (Result = 0x%x)", EcErrorText(dwRes), dwRes);
        goto Exit;
    }

    /*******************/
    /* register client */
    /*******************/
    dwRes = ecatRegisterClient(ecatNotifyCallback, pNotification, &oRegisterResults);
    if (dwRes != EC_E_NOERROR) {
        dwRetVal = dwRes;
        LogError("Cannot register client! (Result = 0x%x)", dwRes);
        goto Exit;
    }
    dwClntId     = oRegisterResults.dwClntId;
    pNotification->SetClientID(dwClntId);

    /***********************************************/
    /* configure DC/DCM master is started with ENI */
    /***********************************************/
    if (pbyCnfData != EC_NULL) {
        /* configure DC    */
        {
            EC_T_DC_CONFIGURE oDcConfigure;

            OsMemset(&oDcConfigure, 0, sizeof(EC_T_DC_CONFIGURE));
            oDcConfigure.dwTimeout          = ETHERCAT_DC_TIMEOUT;
            oDcConfigure.dwDevLimit         = ETHERCAT_DC_DEV_LIMIT;
            oDcConfigure.dwSettleTime       = ETHERCAT_DC_SETTLE_TIME;
            oDcConfigure.dwTotalBurstLength = ETHERCAT_DC_ARMW_BURSTCYCLES;
            oDcConfigure.dwBurstBulk        = ETHERCAT_DC_ARMW_BURSTSPP;
            if (dwBusCycleTimeUsec < 1000) {
                /* if the cycle time is below 1000 usec, we have to reduce the number of frames sent within one cycle */
                oDcConfigure.dwBurstBulk        = ETHERCAT_DC_ARMW_BURSTSPP/2;
            }

            dwRes = ecatDcConfigure(&oDcConfigure);
            if (dwRes != EC_E_NOERROR ) {
                dwRetVal = dwRes;
                LogError("Cannot configure DC! (Result = 0x%x)", dwRes);
                goto Exit;
            }
        }
        /* configure DCM bus shift */
        if (S_bNoDcm) {
            dwRes = ecatDcmConfigureBusShift(EC_NULL, 0);
        } else {
            EC_T_DCM_BUSSHIFT_CONFIG oDcmConfigure;

            OsMemset(&oDcmConfigure, 0, sizeof(EC_T_DCM_BUSSHIFT_CONFIG));
            oDcmConfigure.nCtlSetVal    = ((dwBusCycleTimeUsec*2)/3)*1000; /* set value in nanosec, 66% of the bus cycle */
            //oDcmConfigure.dwInSyncLimit = (dwBusCycleTimeUsec*1000)/10;  /* 10 % limit in nsec for InSync monitoring */
            oDcmConfigure.dwInSyncLimit = 200000;
            //oDcmConfigure.dwInSyncLimit = 300000;
            //oDcmConfigure.nCtlDriftErrorGain = 1;

            if (pNotification->Verbose() >= 3)
            {
#ifdef DCM_ENABLE_LOGFILE
                oDcmConfigure.bLogEnabled = EC_TRUE;
#endif
                LogSave_FwriteStart();
            }
            else
            {
                LogSave_FwriteStop();
            }

            dwRes = ecatDcmConfigureBusShift(&oDcmConfigure, 0);
            switch (dwRes) {
            case EC_E_NOERROR:
                break;
            case EC_E_FEATURE_DISABLED:
                LogError("DCM bus shift mode is disabled!");
                LogError("Start with -nodcm to run the DC demo without DCM, or activate DCM in bus shift mode in the ENI file. In ET9000 for example, select under ""Advanced settings\\Distributed clocks"" ""DC in use"" and ""Slave Mode"":");
                LogError("Please refer to the class A manual for further information\n");
                dwRetVal = dwRes;
                goto Exit;
            default:
                LogError("Cannot configure DCM bus shift! %s (Result = 0x%x)\n", EcErrorText(dwRes), dwRes);
                dwRetVal = dwRes;
                goto Exit;
            }
        }
    }
    /* no errors */
    dwRetVal = EC_E_NOERROR;


    

Exit:
    return dwRetVal;
}



/********************************************************************************/
/** \brief  De-Initialize EtherCAT master
*
*
* \return  Status value.
*/
static EC_T_DWORD atemDemoDeInit(
        EC_T_VOID
        )
{
    EC_T_DWORD      dwRetVal    = EC_E_NOERROR;
    EC_T_DWORD      dwRes       = EC_E_NOERROR;
    CTimeoutHelper  oTimeout;
    EC_T_INT					loop;

    /* release resources */
    if (dwClntId != 0) {
        /* unregister client */
        dwRes = ecatUnregisterClient(dwClntId);
        if (EC_E_MASTERCORE_INACCESSIBLE == dwRes) {
            LogError("Master not responding! %s (0x%lx)", EcErrorText(dwRes), dwRes);
        } else if (dwRes != EC_E_NOERROR) {
            LogError("Cannot unregister client! %s (0x%lx)", EcErrorText(dwRes), dwRes);
        }
        
        dwClntId = 0;
        
        if (dwRetVal == EC_E_NOERROR) {
            dwRetVal = dwRes;
        }
    }

    /* shutdown tEcJobTask */
    S_ThreadParam[THRD_IDX_JOB].bThreadShutdown = EC_TRUE;
    /* wait 2 seconds */
    oTimeout.Start(2000);
    while (S_ThreadParam[THRD_IDX_JOB].bThreadShutdown && !oTimeout.IsElapsed()) {
        OsSleep(10);
    }
    /* delete thread handle */
    if (S_pvtThread[THRD_IDX_JOB] != EC_NULL) {
        OsDeleteThreadHandle(S_pvtThread[THRD_IDX_JOB]);
        S_pvtThread[THRD_IDX_JOB]= EC_NULL;
    }

    dwRes = ecatDeinitMaster();

    if (EC_E_MASTERCORE_INACCESSIBLE == dwRes) {
        LogError("Master not responding! %s (0x%lx)", EcErrorText(dwRes), dwRes);
    } else if (dwRes != EC_E_NOERROR) {
        LogError("Cannot de-initialize EtherCAT-Master! %s (0x%lx)", EcErrorText(dwRes), dwRes);
    }
    
    if (dwRetVal == EC_E_NOERROR) {
        dwRetVal = dwRes;
    }


#ifdef ATEMRAS_SERVER 
    if (S_bRasSrvStarted) {
        LogMsg("Stop Remote Api Server\n");

        if (EC_E_NOERROR != emRasSrvStop(S_pvRemoteApiSrvH, 2000)) {
            LogError("Remote API Server shutdown failed\n");
        }
    }
#endif

    /* shutdown All Task */
    for(loop = 1; loop < THRD_IDX_MAX;loop++) {
        S_ThreadParam[loop].bThreadShutdown = EC_TRUE;
    }

    for(loop = 1; loop < THRD_IDX_MAX;loop++)
    {
        /* delete thread handle */
        if (S_pvtThread[loop] != EC_NULL) {
            OsDeleteThreadHandle(S_pvtThread[loop]);
            S_pvtThread[loop]= EC_NULL;
        }

        /* delete the timing event */
        if( EC_NULL != S_ThreadParam[loop].pvTimingEvent) {
            OsDeleteEvent( S_ThreadParam[loop].pvTimingEvent);
            S_ThreadParam[loop].pvTimingEvent = EC_NULL;
        }
    }

    delete(pShmServer);
    return dwRetVal;
}

/********************************************************************************/
/** \brief  Handler for master notifications
*
*
* \return  Status value.
*/
static EC_T_DWORD ecatNotifyCallback(
        EC_T_DWORD         dwCode,  /**< [in]   Notification code */
        EC_T_NOTIFYPARMS*  pParms   /**< [in]   Notification parameters */
        )
{
    EC_T_DWORD         dwRetVal                = EC_E_NOERROR;
    CEmNotification*   pNotifyInstance;

    if ((EC_NULL == pParms)||(EC_NULL==pParms->pCallerData)) {
        dwRetVal = EC_E_INVALIDPARM;
        goto Exit;
    }

    /* notification for application ? */
    if ((dwCode >= EC_NOTIFY_APP) && (dwCode <= EC_NOTIFY_APP+EC_NOTIFY_APP_MAX_CODE)) {
        /*****************************************************/
        /* Demo code: Remove/change this in your application */
        /* to get here the API ecatNotifyApp(dwCode, pParms) */
        /* has to be called                                  */
        /*****************************************************/
        dwRetVal = myAppNotify(dwCode-EC_NOTIFY_APP, pParms);
    } else {
        pNotifyInstance = (CEmNotification*)pParms->pCallerData;

        /* call the default handler */
        dwRetVal = pNotifyInstance->ecatNotify(dwCode, pParms);
    }

Exit:
    return dwRetVal;
}


/********************************************************************************/
/** \brief  Handler for master RAS notifications
*
*
* \return  Status value.
*/
#ifdef ATEMRAS_SERVER 
static EC_T_DWORD RasNotifyWrapper(
        EC_T_DWORD         dwCode,
        EC_T_NOTIFYPARMS*  pParms
        )
{
    EC_T_DWORD                      dwRetVal                = EC_E_NOERROR;
    CEmNotification*                pNotInst                = EC_NULL;
    
    if ((EC_NULL == pParms)||(EC_NULL==pParms->pCallerData)) {
        dwRetVal = EC_E_INVALIDPARM;
        goto Exit;
    }
    
    pNotInst = (CEmNotification*)(pParms->pCallerData);
    dwRetVal = pNotInst->emRasNotify(dwCode, pParms);
Exit:
    return dwRetVal;
}
#endif

/*-MYAPP---------------------------------------------------------------------*/

/***************************************************************************************************/
/**
\brief  Initialize Application

\return EC_E_NOERROR on success, error code otherwise.
*/
static EC_T_DWORD myAppInit(
        CAtEmLogging*       poLog,          /* [in]  Logging instance */
        EC_T_INT            nVerbose        /* [in]  Verbosity level */
        )
{
    EC_UNREFPARM(poLog);
    EC_UNREFPARM(nVerbose);

    // 全スレーブ情報初期化
    OsMemset(&tAllSlv, 	0, sizeof(tAllSlv));

    return EC_E_NOERROR;
}

/***************************************************************************************************/
/**
\brief  Initialize Slave Instance.

Find slave parameters.
\return EC_E_NOERROR on success, error code otherwise.
*/
static EC_T_DWORD myAppPrepare(
    CAtEmLogging*       poLog,          /* [in]  Logging instance */ 
    EC_T_INT            nVerbose        /* [in]  Verbosity level */
    )
{
    EC_T_INT	loop;
    EC_T_WORD 	wFixedAddress = 0;

    EC_UNREFPARM(nVerbose);

    /*******************************************************************************/
    /* Searching for: YNL MD4KW-3M slave-unit                                      */
    /*******************************************************************************/
    /* search for the first device at the bus and return its fixed (EtherCAT) address */
    for (loop = 0; loop < SLV_NUM_MD4KW_3M; loop++) {
        if(FindSlaveGetFixedAddr(0, poLog, loop, ecvendor_soft_servo, ecprodcode_YNL_MD4KW_3M, &wFixedAddress)) {
            tAllSlv.MD4KW_3MSlaves++;
            /* now get the offset of this device in the process data buffer and some other infos */
            if(ecatGetSlaveInfo(EC_TRUE, wFixedAddress, &tAllSlv.MD4KW_3M[loop].Info) != EC_E_NOERROR) {
                LogError("ERROR: emGetSlaveInfo() returns with error." );
	    }
	}
    }

    /*******************************************************************************/
    /* Searching for: YNL MD4KW-2MFS slave-unit                                      */
    /*******************************************************************************/
    /* search for the first device at the bus and return its fixed (EtherCAT) address */
    for (loop = 0; loop < SLV_NUM_MD4KW_2MFS; loop++) {
        if(FindSlaveGetFixedAddr(0, poLog, loop, ecvendor_soft_servo, ecprodcode_YNL_MD4KW_2MFS, &wFixedAddress)) {
            tAllSlv.MD4KW_2MFSSlaves++;
            /* now get the offset of this device in the process data buffer and some other infos */
            if(ecatGetSlaveInfo(EC_TRUE, wFixedAddress, &tAllSlv.MD4KW_2MFS[loop].Info) != EC_E_NOERROR) {
                LogError("ERROR: emGetSlaveInfo() returns with error." );
    	    }
    	}
    }
    /*******************************************************************************/
    /* Searching for: YNL MD4KW-Hand slave-unit                                      */
    /*******************************************************************************/
    /* search for the first device at the bus and return its fixed (EtherCAT) address */
    for (loop = 0; loop < SLV_NUM_MD4KW_HAND; loop++) {
        if(FindSlaveGetFixedAddr(0, poLog, loop, ecvendor_soft_servo, ecprodcode_YNL_MD4KW_Hand, &wFixedAddress)) {
            tAllSlv.MD4KW_HandSlaves++;
            /* now get the offset of this device in the process data buffer and some other infos */
            if(ecatGetSlaveInfo(EC_TRUE, wFixedAddress, &tAllSlv.MD4KW_Hand[loop].Info) != EC_E_NOERROR) {
                LogError("ERROR: emGetSlaveInfo() returns with error." );
    	    }
    	}
    }
    /*******************************************************************************/
    /* Searching for: YNL MD4KW-IMU slave-unit                                      */
    /*******************************************************************************/
    /* search for the first device at the bus and return its fixed (EtherCAT) address */
    for (loop = 0; loop < SLV_NUM_MD4KW_IMU; loop++) {
        if(FindSlaveGetFixedAddr(0, poLog, loop, ecvendor_soft_servo, ecprodcode_YNL_MD4KW_IMU, &wFixedAddress)) {
            tAllSlv.MD4KW_IMUSlaves++;
            /* now get the offset of this device in the process data buffer and some other infos */
            if(ecatGetSlaveInfo(EC_TRUE, wFixedAddress, &tAllSlv.MD4KW_IMU[loop].Info) != EC_E_NOERROR) {
                LogError("ERROR: emGetSlaveInfo() returns with error." );
    	    }
    	}
    }

    // LogSave Task Start
    OsSetEvent( S_ThreadParam[THRD_IDX_LOGSAVE].pvTimingEvent);

    return EC_E_NOERROR;
}

/***************************************************************************************************/
/**
\brief  Setup slave parameters (normally done in PREOP state

  - SDO up- and Downloads
  - Read Object Dictionary

\return EC_E_NOERROR on success, error code otherwise.
*/

static EC_T_DWORD myAppSetup(
    CAtEmLogging*      poLog,           /* [in]  Logging instance */     
    EC_T_INT           nVerbose,        /* [in]  verbosity level */
    EC_T_DWORD         dwClntId,        /* [in]  EtherCAT master client id */
	EC_T_CHAR*		   pParamFile)
{
	EC_T_DWORD              dwRes                       = EC_E_NOERROR;
	EC_T_DWORD				dwSlaveId;
	EC_T_WORD				wObIndex;
	EC_T_DWORD				dwDataLen;

	ifstream   ifs;
	string     csv_file_path (pParamFile);
	string     line;
	ifs.open(csv_file_path.c_str() );
	if (ifs.good())
	{
		getline(ifs, line);	// 一度だけ空読み
		while (getline(ifs, line) ) 
		{
			stringstream ss(line);
			string 		value;
			int			index;
			EC_T_DWORD	ibuf[SETPARAM_MAX];

			for ( index = 0;index < SETPARAM_MAX; index++)
			{
				if(getline(ss, value, ',') == 0) 
				{
					break;
				}

				ibuf[index] = strtol(value.c_str(), NULL, 0);
			}

			dwRes = ecatCoeSdoDownload( 
					ibuf[SETPARAM_SLVID], ibuf[SETPARAM_INDEX], 0x00, 
					(EC_T_BYTE*)&ibuf[SETPARAM_VALUE], ibuf[SETPARAM_SIZE], MBX_TIMEOUT, 0);
			if( dwRes != EC_E_NOERROR ) {
				LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
				break;
			}
			LogMsg("<SDO Setup> type:%d,no:%d,jo:%d,index:0x%04x,size:%d,value:%d(%x)", 
					ibuf[SETPARAM_BDTYPE], ibuf[SETPARAM_SLVNO], ibuf[SETPARAM_JOINT],
					ibuf[SETPARAM_INDEX], ibuf[SETPARAM_SIZE], ibuf[SETPARAM_VALUE], ibuf[SETPARAM_VALUE] );
		}
		ifs.close();
	}
	else
	{
		EC_T_DWORD				dwOutDataLen 	= 0;
		EC_T_INT				loop;
        EC_T_INT                loop_jnt;
		EC_T_INT                jnt;
		EC_T_BYTE               servo_dir;
		EC_T_DWORD              offset;
		EC_T_WORD				wReset;
		EC_T_WORD				wGain;

		EC_T_DWORD				dwSlaveId;
		EC_T_WORD				wObIndex;
		EC_T_DWORD				dwDataLen;
		EC_T_BYTE				*pbyData;

#ifdef	PARAM_OUTPUT
		FILE					*fpPO 				= EC_NULL;
		EC_T_CHAR           	fname_po[256];
		EC_T_CHAR				tmpBuf[DATA_BUF_SIZE];

		OsMemset(fname_po, 	0, 	sizeof(fname_po));
		snprintf(fname_po,	256, "%s%s", "param_out", 	FILE_EXT);
		fpPO	= OsFopen( fname_po, 	"w+" );
		if(fpPO == EC_NULL) {
			printf("\n!!param output file open NG!!\n");
			return EC_E_ERROR;
		}

		OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
		snprintf(tmpBuf, DATA_BUF_SIZE, "bdtype,slaveno,joint,slaveid,index,size,value\n");
		OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif 

		//setup 3M
		for(loop = 0; loop < tAllSlv.MD4KW_3MSlaves; loop++) {
        if(HydraOnlyUpperbody){
            loop_jnt = loop + LOWERBODY_3M_NUM;
            if(loop_jnt>10){
                LogError( "TOO MANY SLAVES: This version is for the upperbody only!!");
                return EC_E_ERROR;
            }
        }else{
            loop_jnt = loop;
        }
			for(jnt = 0; jnt < 3; jnt++) {
#if 1 //3M_1
				//servo direction
				servo_dir = 0x00;
                if( eha_phys_pos_hydra_MD4KW_3M[loop_jnt][jnt]<0)
					servo_dir |= 0x01;
                if( eha_pos_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]<0)
					servo_dir |= 0x10;
                if( eha_tau_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]<0)
                    servo_dir |= 0x20;

				/*********** Reset ************************************************/
				wReset = 0x8000;

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7000|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				dwSlaveId 	= tAllSlv.MD4KW_3M[loop].Info.dwSlaveId;
				wObIndex 	= (0x7000|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wReset;
				dwDataLen 	= sizeof(wReset);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) Reset:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wReset);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Reset\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wReset);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Servo Direction ************************************************/
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7046|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7046|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&servo_dir;
				dwDataLen 	= sizeof(servo_dir);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) ServoDir:%02x", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, servo_dir);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Servo Direction\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, servo_dir);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Servo P Gain ************************************************/
                wGain = (EC_T_WORD)((eha_pos_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]<0) ? -eha_pos_p_gain_hydra_MD4KW_3M[loop_jnt][jnt] : eha_pos_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7011|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7011|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) ServoPGain:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Servo P Gain\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Servo D Gain ************************************************/
				//wGain = (EC_T_WORD)((eha_vel_gain_hydra_MD4KW_3M[loop][jnt]<0) ? -eha_vel_gain_hydra_MD4KW_3M[loop][jnt] : eha_vel_gain_hydra_MD4KW_3M[loop][jnt]);
                wGain = (EC_T_WORD)(eha_pos_d_gain_hydra_MD4KW_3M[loop_jnt][jnt]);

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7013|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7013|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) ServoDGain:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Servo D Gain\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Servo I Gain ************************************************/
                wGain = (EC_T_WORD)((eha_pos_i_gain_hydra_MD4KW_3M[loop_jnt][jnt]<0) ? -eha_pos_i_gain_hydra_MD4KW_3M[loop_jnt][jnt] : eha_pos_i_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
					   tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7012|(jnt<<8)), 0x00);
				
				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7012|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) ServoDGain:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Servo I Gain\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT


				/*********** Vel P Gain ************************************************/
                int jnt_num = joint_num_hydra_MD4KW_3M[loop_jnt][jnt];
                switch(EHA_CtrlWd_ON[jnt_num]){
                case EHA_CTRLWD_POS:
                    wGain = (EC_T_WORD)((eha_vel_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]<0) ? -eha_vel_p_gain_hydra_MD4KW_3M[loop_jnt][jnt] : eha_vel_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_PRES:
                    wGain = (EC_T_WORD)((eha_tau_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]<0) ? -eha_tau_p_gain_hydra_MD4KW_3M[loop_jnt][jnt] : eha_tau_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_STRAIN:
                    wGain = (EC_T_WORD)((eha_tau_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]<0) ? -eha_tau_p_gain_hydra_MD4KW_3M[loop_jnt][jnt] : eha_tau_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_COMPLE:
                    wGain = (EC_T_WORD)((eha_tau_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]<0) ? -eha_tau_p_gain_hydra_MD4KW_3M[loop_jnt][jnt] : eha_tau_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                default:
                    wGain = (EC_T_WORD)((eha_vel_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]<0) ? -eha_vel_p_gain_hydra_MD4KW_3M[loop_jnt][jnt] : eha_vel_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                }

                //wGain = (EC_T_WORD)((eha_vel_p_gain_hydra_MD4KW_3M[loop][jnt]<0) ? -eha_vel_p_gain_hydra_MD4KW_3M[loop][jnt] : eha_vel_p_gain_hydra_MD4KW_3M[loop][jnt]);

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7021|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7021|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) VelPGain:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Vel P Gain\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Vel D Gain ************************************************/
                switch(EHA_CtrlWd_ON[jnt_num]){
                case EHA_CTRLWD_POS:
                    wGain = (EC_T_WORD)(eha_vel_d_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_PRES:
                    wGain = (EC_T_WORD)(eha_tau_d_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_STRAIN:
                    wGain = (EC_T_WORD)(eha_tau_d_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_COMPLE:
                    wGain = (EC_T_WORD)(eha_tau_d_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                default:
                    wGain = (EC_T_WORD)(eha_vel_d_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                }

                //wGain = (EC_T_WORD)(eha_vel_d_gain_hydra_MD4KW_3M[loop][jnt]);

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7023|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7023|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) VelDGain:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Vel D Gain\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Vel I Gain ************************************************/
                switch(EHA_CtrlWd_ON[jnt_num]){
                case EHA_CTRLWD_POS:
                    wGain = (EC_T_WORD)(eha_vel_i_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_PRES:
                    wGain = (EC_T_WORD)(eha_tau_i_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_STRAIN:
                    wGain = (EC_T_WORD)(eha_tau_i_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_COMPLE:
                    wGain = (EC_T_WORD)(eha_tau_i_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                default:
                    wGain = (EC_T_WORD)(eha_vel_i_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
                    break;
                }


                //wGain = (EC_T_WORD)((eha_vel_i_gain_hydra_MD4KW_3M[loop][jnt]<0) ? -eha_vel_i_gain_hydra_MD4KW_3M[loop][jnt] : eha_vel_i_gain_hydra_MD4KW_3M[loop][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
					   tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7022|(jnt<<8)), 0x00);
				
				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7022|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) VelIGain:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Vel I Gain\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Cur P Gain ************************************************/
                wGain = (EC_T_WORD)((eha_cur_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]<0) ? -eha_cur_p_gain_hydra_MD4KW_3M[loop_jnt][jnt] : eha_cur_p_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7031|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7031|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) CurPGain:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Cur P Gain\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Cur D Gain ************************************************/
				//wGain = (EC_T_WORD)((eha_vel_gain_hydra_MD4KW_3M[loop][jnt]<0) ? -eha_vel_gain_hydra_MD4KW_3M[loop][jnt] : eha_vel_gain_hydra_MD4KW_3M[loop][jnt]);
                wGain = (EC_T_WORD)(eha_cur_d_gain_hydra_MD4KW_3M[loop_jnt][jnt]);

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7033|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7033|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) CurDGain:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Cur D Gain\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Cur I Gain ************************************************/
                wGain = (EC_T_WORD)((eha_cur_i_gain_hydra_MD4KW_3M[loop_jnt][jnt]<0) ? -eha_cur_i_gain_hydra_MD4KW_3M[loop_jnt][jnt] : eha_cur_i_gain_hydra_MD4KW_3M[loop_jnt][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
					   tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7032|(jnt<<8)), 0x00);
				
				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7032|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) CurIGain:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Cur I Gain\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT




				/*********** Enc Offset ************************************************/
                offset = enc_offset_MD4KW_3M[loop_jnt][jnt];
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7050|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7050|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&offset;
				dwDataLen 	= sizeof(offset);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) Enc0Offset:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, offset);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Enc Offset\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, offset);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** MEnc Offset ************************************************/
                offset = menc_offset_MD4KW_3M[loop_jnt][jnt];
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7051|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7051|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&offset;
				dwDataLen 	= sizeof(offset);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) Enc1Offset:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, offset);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// MEnc Offset\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, offset);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** motor k_emf ************************************************/
                wGain = motor_KEMF_MD4KW_3M[loop_jnt][jnt];
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7045|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7045|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&offset;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) MotorKEMF:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Motor KEMF\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT


				/*********** Un-Reset ************************************************/
				wReset = 0x0000;

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, (0x7000|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7000|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wReset;
				dwDataLen 	= sizeof(wReset);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) Reset:%d", &tAllSlv.MD4KW_3M[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_3M[loop].Info.wPhysAddress, wReset);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
                snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Un-Reset\n", BDTYPE_MD4KW_3M, loop_jnt, jnt, dwSlaveId, wObIndex, dwDataLen, wReset);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

#endif  //3M_1
			}
		}
		//setup 2MFS
		for(loop = tAllSlv.MD4KW_2MFSSlaves-1; loop >=0 ; loop--) {
			for(jnt = 0; jnt < 2; jnt++) {
#if 1  //2MFS
				servo_dir = 0x00;
				if( eha_phys_pos_hydra_MD4KW_2MFS[loop][jnt]<0)
					servo_dir |= 0x01;
				if( eha_pos_p_gain_hydra_MD4KW_2MFS[loop][jnt]<0)
					servo_dir |= 0x10;
                if( eha_tau_p_gain_hydra_MD4KW_2MFS[loop][jnt]<0)
                    servo_dir |= 0x20;
				/*********** Reset ************************************************/
				wReset = 0x8000;

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7000|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				dwSlaveId 	= tAllSlv.MD4KW_2MFS[loop].Info.dwSlaveId;
				wObIndex 	= (0x7000|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wReset;
				dwDataLen 	= sizeof(wReset);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) Reset:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wReset);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Reset\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wReset);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Servo Direction ************************************************/
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7046|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7046|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&servo_dir;
				dwDataLen 	= sizeof(servo_dir);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) ServoDir:%02x", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, servo_dir);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Servo Direction\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, servo_dir);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Servo P Gain ************************************************/
				wGain = (EC_T_WORD)((eha_pos_p_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_pos_p_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_pos_p_gain_hydra_MD4KW_2MFS[loop][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7011|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7011|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) ServoPGain:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wGain);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Servo P Gain\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Servo D Gain ************************************************/
				wGain = (EC_T_WORD)((eha_pos_d_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_pos_d_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_pos_d_gain_hydra_MD4KW_2MFS[loop][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7013|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7013|(jnt<<8));
				//pbyData		= (EC_T_BYTE*)&wReset;
                pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) ServoDGain:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wGain);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Servo D Gain\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Servo I Gain ************************************************/
				wGain = (EC_T_WORD)((eha_pos_i_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_pos_i_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_pos_i_gain_hydra_MD4KW_2MFS[loop][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
					tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7012|(jnt<<8)), 0x00);
				
				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7012|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) ServoDGain:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wGain);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Servo I Gain\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Vel P Gain ************************************************/
                int jnt_num = joint_num_hydra_MD4KW_2MFS[loop][jnt];
                switch(EHA_CtrlWd_ON[jnt_num]){
                case EHA_CTRLWD_POS:
                    wGain = (EC_T_WORD)((eha_vel_p_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_vel_p_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_vel_p_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_PRES:
                    wGain = (EC_T_WORD)((eha_tau_p_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_tau_p_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_tau_p_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_STRAIN:
                    wGain = (EC_T_WORD)((eha_tau_p_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_tau_p_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_tau_p_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_COMPLE:
                    wGain = (EC_T_WORD)((eha_tau_p_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_tau_p_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_tau_p_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                default:
                    wGain = (EC_T_WORD)((eha_vel_p_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_vel_p_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_vel_p_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                }

                //wGain = (EC_T_WORD)((eha_vel_p_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_vel_p_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_vel_p_gain_hydra_MD4KW_2MFS[loop][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7021|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7021|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) VelPGain:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Vel P Gain\n", BDTYPE_MD4KW_3M, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Vel D Gain ************************************************/
                switch(EHA_CtrlWd_ON[jnt_num]){
                case EHA_CTRLWD_POS:
                    wGain = (EC_T_WORD)(eha_vel_d_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_PRES:
                    wGain = (EC_T_WORD)(eha_tau_d_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_STRAIN:
                    wGain = (EC_T_WORD)(eha_tau_d_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_COMPLE:
                    wGain = (EC_T_WORD)(eha_tau_d_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                default:
                    wGain = (EC_T_WORD)(eha_vel_d_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                }
                //wGain = (EC_T_WORD)(eha_vel_d_gain_hydra_MD4KW_2MFS[loop][jnt]);

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7023|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7023|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) VelDGain:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Vel D Gain\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Vel I Gain ************************************************/
                switch(EHA_CtrlWd_ON[jnt_num]){
                case EHA_CTRLWD_POS:
                    wGain = (EC_T_WORD)(eha_vel_i_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_PRES:
                    wGain = (EC_T_WORD)(eha_tau_i_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_STRAIN:
                    wGain = (EC_T_WORD)(eha_tau_i_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                case EHA_CTRLWD_FORCE_COMPLE:
                    wGain = (EC_T_WORD)(eha_tau_i_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                default:
                    wGain = (EC_T_WORD)(eha_vel_i_gain_hydra_MD4KW_2MFS[loop][jnt]);
                    break;
                }
                //wGain = (EC_T_WORD)((eha_vel_i_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_vel_i_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_vel_i_gain_hydra_MD4KW_2MFS[loop][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
					   tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7022|(jnt<<8)), 0x00);
				
				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7022|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) VelIGain:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Vel I Gain\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Cur P Gain ************************************************/
				wGain = (EC_T_WORD)((eha_cur_p_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_cur_p_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_cur_p_gain_hydra_MD4KW_2MFS[loop][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7031|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7031|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) CurPGain:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Cur P Gain\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Cur D Gain ************************************************/
				//wGain = (EC_T_WORD)((eha_vel_gain_hydra_MD4KW_3M[loop][jnt]<0) ? -eha_vel_gain_hydra_MD4KW_3M[loop][jnt] : eha_vel_gain_hydra_MD4KW_3M[loop][jnt]);
            	wGain = (EC_T_WORD)(eha_cur_d_gain_hydra_MD4KW_2MFS[loop][jnt]);

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7033|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7033|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) CurDGain:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Cur D Gain\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Cur I Gain ************************************************/
				wGain = (EC_T_WORD)((eha_cur_i_gain_hydra_MD4KW_2MFS[loop][jnt]<0) ? -eha_cur_i_gain_hydra_MD4KW_2MFS[loop][jnt] : eha_cur_i_gain_hydra_MD4KW_2MFS[loop][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
					   tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7032|(jnt<<8)), 0x00);
				
				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7032|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) CurIGain:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Cur I Gain\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT


				/*********** Enc Offset ************************************************/
				offset = enc_offset_MD4KW_2MFS[loop][jnt];
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7050|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7050|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&offset;
				dwDataLen 	= sizeof(offset);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) Enc0Offset:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, offset);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Enc Offset\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, offset);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** MEnc Offset ************************************************/
				offset = menc_offset_MD4KW_2MFS[loop][jnt];
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7051|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7051|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&offset;
				dwDataLen 	= sizeof(offset);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) Enc1Offset:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, offset);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// MEnc Offset\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, offset);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** motor k_emf ************************************************/
				wGain = motor_KEMF_MD4KW_2MFS[loop][jnt];
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7045|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7045|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&offset;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) MotorKEMF:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wGain);

#ifdef	PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Motor KEMF\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT


				/*********** Un-Reset ************************************************/
				wReset = 0x0000;

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, (0x7000|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7000|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wReset;
				dwDataLen 	= sizeof(wReset);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) Reset:%d", &tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress, wReset);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Un-Reset\n", BDTYPE_MD4KW_2MFS, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wReset);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT
#endif  // 2MFS
			}
		}
		for(loop = 0; loop < tAllSlv.MD4KW_HandSlaves; loop++) {
			EC_T_WORD fing_dir=0;
#if 1 //hand1
			for(jnt = 0; jnt < 5; jnt++) {
				servo_dir = 0x00;
				if( eha_phys_pos_hydra_MD4KW_Hand[loop][jnt]<0)
					servo_dir |= 0x01;
				if( eha_pos_p_gain_hydra_MD4KW_Hand[loop][jnt]<0)
					servo_dir |= 0x10;

				/*********** Reset ************************************************/
				wReset = 0x8000;

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName, tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, (0x7000|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				dwSlaveId 	= tAllSlv.MD4KW_Hand[loop].Info.dwSlaveId;
				wObIndex 	= (0x7000|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wReset;
				dwDataLen 	= sizeof(wReset);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) Reset:%d", &tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, wReset);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Reset\n", BDTYPE_MD4KW_HAND, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wReset);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

#if 1 //hand2
				/*********** Servo Direction ************************************************/
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName, tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, (0x7046|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7046|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&servo_dir;
				dwDataLen 	= sizeof(servo_dir);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) ServoDir:%02x", &tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, servo_dir);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Servo Direction\n", BDTYPE_MD4KW_HAND, loop, jnt, dwSlaveId, wObIndex, dwDataLen, servo_dir);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Servo P Gain ************************************************/
				wGain = (EC_T_WORD)((eha_pos_p_gain_hydra_MD4KW_Hand[loop][jnt]<0) ? -eha_pos_p_gain_hydra_MD4KW_Hand[loop][jnt] : eha_pos_p_gain_hydra_MD4KW_Hand[loop][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName, tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, (0x7011|(jnt<<8)), 0x00);

				wObIndex 	= (0x7011|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) ServoPGain:%d", 
						&tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, wGain);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Servo P Gain\n", BDTYPE_MD4KW_HAND, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Servo D Gain ************************************************/
				wGain = (EC_T_WORD)((eha_pos_d_gain_hydra_MD4KW_Hand[loop][jnt]<0) ? -eha_pos_d_gain_hydra_MD4KW_Hand[loop][jnt] : eha_pos_d_gain_hydra_MD4KW_Hand[loop][jnt]);
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName, tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, (0x7013|(jnt<<8)), 0x00);

				wObIndex 	= (0x7013|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wGain;
				dwDataLen 	= sizeof(wGain);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) ServoDGain:%d", 
						&tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, wGain);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Servo D Gain\n", BDTYPE_MD4KW_HAND, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wGain);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Enc Offset ************************************************/
				offset = enc_offset_MD4KW_hand[loop][jnt];
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName, tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, (0x7050|(jnt<<8)), 0x00);

				wObIndex 	= (0x7050|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&offset;
				dwDataLen 	= sizeof(offset);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);

				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) Enc0Offset:%d", 
						&tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, offset);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Enc Offset\n", BDTYPE_MD4KW_HAND, loop, jnt, dwSlaveId, wObIndex, dwDataLen, offset);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

#endif //hand2
				/*********** Un-Reset ************************************************/
				wReset = 0x0000;

				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName, tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, (0x7000|(jnt<<8)), 0x00);

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				wObIndex 	= (0x7000|(jnt<<8));
				pbyData		= (EC_T_BYTE*)&wReset;
				dwDataLen 	= sizeof(wReset);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) Reset:%d", &tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, wReset);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Un-Reset\n", BDTYPE_MD4KW_HAND, loop, jnt, dwSlaveId, wObIndex, dwDataLen, wReset);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

			}
			fing_dir=0;
			for(jnt=0; jnt<16; jnt++) {
				/*********** Finger Enc Offset ************************************************/
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
						tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName, tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, (0x7520|jnt), 0x00);

				wObIndex 	= (0x7520|jnt);
				pbyData		= (EC_T_BYTE*)&(enc_offset_MD4KW_fingers[loop][jnt]);
				dwDataLen 	= sizeof(EC_T_WORD);
				dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
				if( dwRes != EC_E_NOERROR ) {
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					break;
				}
				LogMsg("<CoE> Slave:%s(cfg-addr:%d) FingerEncOffset:%d", 
						&tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, enc_offset_MD4KW_fingers[loop][jnt]);

#ifdef PARAM_OUTPUT
				OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
				snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Finger Enc Offset\n", BDTYPE_MD4KW_HAND, loop, jnt, dwSlaveId, wObIndex, dwDataLen, enc_offset_MD4KW_fingers[loop][jnt]);
				OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT

				/*********** Calc Finger Dir Reg********************************************/
				if(eha_phys_pos_hydra_MD4KW_Hand[loop][jnt+5]<0)
					fing_dir |= (0x0001<<jnt);
			}

			/*********** Set Finger Dir Reg ********************************************/
			LogMsg("<CoE> Slave:%s(cfg-addr:%d) SDO Download Idx:%04xh.Sub:%xh", 
					tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName, tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, 0x7530, 0x00);

			wObIndex 	= 0x7530;
			pbyData		= (EC_T_BYTE*)&fing_dir;
			dwDataLen 	= sizeof(fing_dir);
			dwRes = ecatCoeSdoDownload( dwSlaveId, wObIndex, 0x00, pbyData, dwDataLen, MBX_TIMEOUT, 0);
			if( dwRes != EC_E_NOERROR ) {
				LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
				break;
			}

			LogMsg("<CoE> Slave:%s(cfg-addr:%d) FingerEncOffset:%d", 
					&tAllSlv.MD4KW_Hand[loop].Info.abyDeviceName[0], tAllSlv.MD4KW_Hand[loop].Info.wPhysAddress, fing_dir);

#ifdef PARAM_OUTPUT
			OsMemset(tmpBuf, 0, DATA_BUF_SIZE);
			snprintf(tmpBuf, DATA_BUF_SIZE, "%d,%d,%d,%d,0x%04x,%d,%d,\t\t// Set Finger Dir Reg\n", BDTYPE_MD4KW_HAND, loop, jnt, dwSlaveId, wObIndex, dwDataLen, fing_dir);
			OsFwrite(tmpBuf, OsStrlen(tmpBuf), 1, fpPO);
#endif  // PARAM_OUTPUT
#endif  //hand1

		}
#ifdef	PARAM_OUTPUT
		OsFflush(fpPO);
		OsFclose(fpPO);
#endif 
	}
	return EC_E_NOERROR;
}

/***************************************************************************************************/
/**
\brief  demo application working process data function.

  This function is called in every cycle after the the master stack is started.
  
*/
static EC_T_DWORD myAppWorkpd(
        CAtEmLogging*        poLog,        /* [in]  Logging instance */
        EC_T_INT             nVerbose	/* [in]  Verbosity level */
        )
{
    EC_T_INT	loop;

    // 共有メモリのIndexを更新し、イベント発行
    int cnt = pIPC->Sync();

    return EC_E_NOERROR;
}

/***************************************************************************************************/
/**
\brief  demo application doing some diagnostic tasks

  This function is called in sometimes from the main demo task


*/
static EC_T_DWORD myAppDiagnosis(
        CAtEmLogging*       poLog,          /* [in]  Logging instance */
        EC_T_INT            nVerbose        /* [in]  Verbosity level */
        )
{
    EC_UNREFPARM(poLog);
    EC_UNREFPARM(nVerbose);

    return EC_E_NOERROR;
}

/********************************************************************************/
/** \brief  Handler for application notifications
*
*  !!! No blocking API shall be called within this function!!! 
*  !!! Function is called by cylic task                    !!! 
*
* \return  Status value.
*/
static EC_T_DWORD myAppNotify(
        EC_T_DWORD              dwCode,     /* [in]  Application notification code */
        EC_T_NOTIFYPARMS*       pParms      /* [in]  Notification parameters */
        )
{
    EC_T_DWORD dwRes = EC_E_ERROR;
    
    EC_UNREFPARM(pParms);

    /* dispatch notification code */
    switch(dwCode) {
    case 1:
        LogMsg("Application notification code=%d received\n", dwCode);
        /* dwRes = EC_E_NOERROR; */
        break;
    case 2:
        break;
    default:
        break;
    }

    return dwRes;
}

/********************************************************************************/
/** \brief  Handler for application notifications
*
*  !!! No blocking API shall be called within this function!!! 
*  !!! Function is called by cylic task                    !!! 
*
* \return  Status value.
*/
static EC_T_DWORD myAppPdoInput(T_ALL_SLAVE_INFO* pAllSlv)
{
    static EC_T_BOOL initial = true;
    EC_T_DWORD 		dwRes 	= EC_E_ERROR;
    EC_T_BYTE*  	pbyPDIn = EC_NULL;
    EC_T_INT		loop;
    EC_T_INT        loop_jnt;

    static EC_T_REAL all_joint_pos_prev[HYDRA_JNT_MAX];
    static EC_T_REAL all_EHA_pos_prev[EHA_MAX];

    double       all_EHA_pos[EHA_MAX];
    unsigned int	  all_EHA_rawpos[EHA_MAX];
    double       all_EHA_vel[EHA_MAX];
    double       all_EHA_tau[EHA_MAX];    //EC_T_REAL
    double       all_EHA_tau2[EHA_MAX];
    double       all_EHA_tau3[EHA_MAX];

    double       all_joint_pos[HYDRA_JNT_MAX];
    double       all_joint_vel[HYDRA_JNT_MAX];
    double       all_joint_tau[HYDRA_JNT_MAX];
    double       all_joint_tau2[HYDRA_JNT_MAX];
    double       all_joint_tau3[HYDRA_JNT_MAX];

    EC_T_INT          ret;
    static EC_T_BYTE  cnt=0;
    BytedFloat_t      tmp_flt;
    EC_T_BYTE         eha_status;

    EC_T_BOOL        err_reported;

    EC_T_INT         ch_num;
    EC_T_INT         eha_num_hydra;

    pbyPDIn  = ecatGetProcessImageInputPtr(); /* pointer to process data input buffer */

    // プロセスデータバッファがまだ存在しない場合
    if(pbyPDIn == EC_NULL) {
        // 何も行わない
        return dwRes;
    }

    if(pShmServer == EC_NULL) {
        return -1;
    }

    // 格納時間を保存
    pShmServer->SetTimeInfo(OsQueryMsecCount());
//    SHM_MSEC(1) = OsQueryMsecCount();

    err_reported = false;
    // YNL MD4KW_3M
    for(loop = 0; loop < pAllSlv->MD4KW_3MSlaves; loop++) {
    if(HydraOnlyUpperbody)
        loop_jnt = loop + LOWERBODY_3M_NUM;
    else
        loop_jnt = loop;

        //送受信の最新データ
        // pAllSlv 	: ローカルテーブルへ
        // SHM_TM～	：共有メモリへ
        //SHM_MD4KW_3M_INP(1, loop) =
        pAllSlv->MD4KW_3M[loop].Pdo.tInp 	= *(T_MD4KW_3M_TXPDO_BASIC*)(pbyPDIn + pAllSlv->MD4KW_3M[loop].Info.dwPdOffsIn/8);

        eha_status = EHA_NO_ERR;
        ch_num = 0;
        eha_num_hydra = joint_num_hydra_MD4KW_3M[loop_jnt][ch_num];
        if(pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis0_PosActIn != 0) {
            eha_state[eha_num_hydra].DATA.rawpos_act
                    = (unsigned int)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis0_PosActIn;

            ClipEHAPos(&(eha_state[eha_num_hydra]),
                       enc_offset_MD4KW_3M[loop_jnt][ch_num],
                       EHA_llim_hydra_MD4KW_3M[loop_jnt][ch_num],
                       EHA_ulim_hydra_MD4KW_3M[loop_jnt][ch_num],
                       eha_phys_pos_hydra_MD4KW_3M[loop_jnt][ch_num]);

        } else {
            if(err_reported==false)
                LogMsg("<PDO> Slave:%s(cfg-addr:%d) Encoder Axis 0 Read Failiure",
                       tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress);
            err_reported = true;
            eha_status |= ENC_READ_ERR;
        }
        eha_state[eha_num_hydra].DATA.vel_act = pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis0_VelActIn*eha_phys_pos_hydra_MD4KW_3M[loop_jnt][ch_num]/2e-4;
        all_EHA_tau[eha_num_hydra]  = eha_state[eha_num_hydra].DATA.tau_act  = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis0_TauActIn*EHA_force_bit2phy[eha_num_hydra];
        all_EHA_tau2[eha_num_hydra] = eha_state[eha_num_hydra].DATA.tau2_act = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis0_Tau2ActIn*CURRENT_BIT2PHY;
        all_EHA_tau3[eha_num_hydra] = eha_state[eha_num_hydra].DATA.tau3_act = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis0_Tau3ActIn*CURRENT_BIT2PHY;
        //eha_state[eha_num_hydra].DATA.tau3_act = ((short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis0_Tau3ActIn*1.0f*ADC_BIT_TO_V - TEMP_V_OFFSET)*V_TO_TEMP;
        eha_state[eha_num_hydra].DATA.stsword = eha_status;
        all_EHA_pos[eha_num_hydra] = eha_state[eha_num_hydra].DATA.pos_act;




        eha_status = EHA_NO_ERR;
        ch_num = 1;
        eha_num_hydra = joint_num_hydra_MD4KW_3M[loop_jnt][ch_num];
        if(pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis1_PosActIn != 0) {
            eha_state[eha_num_hydra].DATA.rawpos_act =
                    (unsigned int)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis1_PosActIn;
            ClipEHAPos(&(eha_state[eha_num_hydra]),
                       enc_offset_MD4KW_3M[loop_jnt][ch_num],
                       EHA_llim_hydra_MD4KW_3M[loop_jnt][ch_num],
                       EHA_ulim_hydra_MD4KW_3M[loop_jnt][ch_num],
                       eha_phys_pos_hydra_MD4KW_3M[loop_jnt][ch_num]);
        } else {
            if(err_reported==false)
                LogMsg("<PDO> Slave:%s(cfg-addr:%d) Encoder Axis 1 Read Failiure",
                       tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress);
            err_reported = true;
            eha_status |= ENC_READ_ERR;
        }
        eha_state[eha_num_hydra].DATA.vel_act = pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis1_VelActIn*eha_phys_pos_hydra_MD4KW_3M[loop_jnt][1]/2e-4;

        all_EHA_tau[eha_num_hydra]  = eha_state[eha_num_hydra].DATA.tau_act  = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis1_TauActIn*EHA_force_bit2phy[eha_num_hydra];
        all_EHA_tau2[eha_num_hydra] = eha_state[eha_num_hydra].DATA.tau2_act = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis1_Tau2ActIn*CURRENT_BIT2PHY; //ref
        all_EHA_tau3[eha_num_hydra] = eha_state[eha_num_hydra].DATA.tau3_act = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis1_Tau3ActIn*CURRENT_BIT2PHY; //act

        /*
        eha_state[eha_num_hydra].DATA.tau_act = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis1_Tau2ActIn*1.0f*eha_phys_forcepres_hydra_MD4KW_3M[loop][1];
        eha_state[eha_num_hydra].DATA.tau2_act = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis1_TauActIn*1.0f;
        eha_state[eha_num_hydra].DATA.tau3_act = ((short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis1_Tau3ActIn*1.0f*ADC_BIT_TO_V - TEMP_V_OFFSET)*V_TO_TEMP;
        */
        eha_state[eha_num_hydra].DATA.stsword = eha_status;
        all_EHA_pos[eha_num_hydra] = eha_state[eha_num_hydra].DATA.pos_act;

        eha_status = EHA_NO_ERR;
        ch_num = 2;
        eha_num_hydra = joint_num_hydra_MD4KW_3M[loop_jnt][ch_num];

        if((pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis2_PosActIn != 0)
                || (eha_num_hydra == EHA_rknee_tandem2)
                || (eha_num_hydra == EHA_lknee_tandem2)
                || (eha_num_hydra == EHA_neck)
                || (eha_num_hydra == EHA_relbow_tandem2)
                || (eha_num_hydra == EHA_relbow_tandem2)
                ) {

            eha_state[eha_num_hydra].DATA.rawpos_act = (unsigned int)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis2_PosActIn;

            ClipEHAPos(&(eha_state[eha_num_hydra]),
                       enc_offset_MD4KW_3M[loop_jnt][ch_num],
                       EHA_llim_hydra_MD4KW_3M[loop_jnt][ch_num],
                       EHA_ulim_hydra_MD4KW_3M[loop_jnt][ch_num],
                       eha_phys_pos_hydra_MD4KW_3M[loop_jnt][ch_num]);
        } else {
            if(err_reported==false)
                LogMsg("<PDO> Slave:%s(cfg-addr:%d) Encoder Axis 2 Read Failiure",
                       tAllSlv.MD4KW_3M[loop].Info.abyDeviceName, tAllSlv.MD4KW_3M[loop].Info.wPhysAddress);
            err_reported = true;
            eha_status |= ENC_READ_ERR;
        }
        eha_state[eha_num_hydra].DATA.vel_act  = pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis2_VelActIn*eha_phys_pos_hydra_MD4KW_3M[loop_jnt][ch_num]/2e-4;

        all_EHA_tau[eha_num_hydra]  = eha_state[eha_num_hydra].DATA.tau_act  = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis2_TauActIn*EHA_force_bit2phy[eha_num_hydra];
        all_EHA_tau2[eha_num_hydra] = eha_state[eha_num_hydra].DATA.tau2_act = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis2_Tau2ActIn*CURRENT_BIT2PHY;
        all_EHA_tau3[eha_num_hydra] = eha_state[eha_num_hydra].DATA.tau3_act = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis2_Tau3ActIn*CURRENT_BIT2PHY;

        /*
        eha_state[eha_num_hydra].DATA.tau_act  = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis2_Tau2ActIn*1.0f*eha_phys_forcepres_hydra_MD4KW_3M[loop][ch_num];
        eha_state[eha_num_hydra].DATA.tau2_act = (short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis2_TauActIn*1.0f;
        eha_state[eha_num_hydra].DATA.tau3_act = ((short)pAllSlv->MD4KW_3M[loop].Pdo.tInp.Axis2_Tau3ActIn*1.0f*ADC_BIT_TO_V - TEMP_V_OFFSET)*V_TO_TEMP;
        */
        eha_state[eha_num_hydra].DATA.stsword  = eha_status;
        all_EHA_pos[eha_num_hydra] = eha_state[eha_num_hydra].DATA.pos_act;

    }
    // YNL MD4KW_2MFS
    for(loop = 0; loop < pAllSlv->MD4KW_2MFSSlaves; loop++) {
        //送受信の最新データ
        // pAllSlv 	: ローカルテーブルへ
        // SHM_TM～	：共有メモリへ
        //SHM_MD4KW_2MFS_INP(1, loop) =
        pAllSlv->MD4KW_2MFS[loop].Pdo.tInp 	= *(T_MD4KW_2MFS_TXPDO_BASIC*)(pbyPDIn + pAllSlv->MD4KW_2MFS[loop].Info.dwPdOffsIn/8);

        eha_status = EHA_NO_ERR;
        ch_num = 0;
        eha_num_hydra = joint_num_hydra_MD4KW_2MFS[loop][ch_num];
        if(pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis0_PosActIn != 0) {
            eha_state[eha_num_hydra].DATA.rawpos_act = (unsigned int)pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis0_PosActIn;
            ClipEHAPos(&(eha_state[eha_num_hydra]),
                       enc_offset_MD4KW_2MFS[loop][ch_num],
                       EHA_llim_hydra_MD4KW_2MFS[loop][ch_num],
                       EHA_ulim_hydra_MD4KW_2MFS[loop][ch_num],
                       eha_phys_pos_hydra_MD4KW_2MFS[loop][ch_num]);
        } else {
            if(err_reported==false)
                LogMsg("<PDO> Slave:%s(cfg-addr:%d) Encoder Axis 0 Read Failiure",
                       tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress);
            err_reported = true;
            eha_status |= ENC_READ_ERR;
        }
        eha_state[eha_num_hydra].DATA.vel_act = pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis0_VelActIn*eha_phys_pos_hydra_MD4KW_2MFS[loop][ch_num]/1e-4;
        all_EHA_tau[eha_num_hydra]  = eha_state[eha_num_hydra].DATA.tau_act  = (short)pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis0_TauActIn*EHA_force_bit2phy[eha_num_hydra];
        all_EHA_tau2[eha_num_hydra] = eha_state[eha_num_hydra].DATA.tau2_act = (short)pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis0_Tau2ActIn*CURRENT_BIT2PHY;
        all_EHA_tau3[eha_num_hydra] = eha_state[eha_num_hydra].DATA.tau3_act = (short)pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis0_Tau3ActIn*CURRENT_BIT2PHY;

        eha_state[eha_num_hydra].DATA.stsword = eha_status;
        all_EHA_pos[eha_num_hydra] = eha_state[eha_num_hydra].DATA.pos_act;

        eha_status = EHA_NO_ERR;
        ch_num = 1;
        eha_num_hydra = joint_num_hydra_MD4KW_2MFS[loop][ch_num];
        if(pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis1_PosActIn != 0) {
            eha_state[eha_num_hydra].DATA.rawpos_act = (unsigned int)pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis1_PosActIn;
            ClipEHAPos(&(eha_state[eha_num_hydra]),
                       enc_offset_MD4KW_2MFS[loop][ch_num],
                       EHA_llim_hydra_MD4KW_2MFS[loop][ch_num],
                       EHA_ulim_hydra_MD4KW_2MFS[loop][ch_num],
                       eha_phys_pos_hydra_MD4KW_2MFS[loop][ch_num]);
        } else {
            if(err_reported==false)
                LogMsg("<PDO> Slave:%s(cfg-addr:%d) Encoder Axis 1 Read Failiure",
                       tAllSlv.MD4KW_2MFS[loop].Info.abyDeviceName, tAllSlv.MD4KW_2MFS[loop].Info.wPhysAddress);
            err_reported = true;
            eha_status |= ENC_READ_ERR;
        }
        eha_state[eha_num_hydra].DATA.vel_act = pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis1_VelActIn*eha_phys_pos_hydra_MD4KW_2MFS[loop][ch_num]/1e-4;
        all_EHA_tau[eha_num_hydra]  = eha_state[eha_num_hydra].DATA.tau_act  = (short)pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis1_TauActIn*EHA_force_bit2phy[eha_num_hydra];
        all_EHA_tau2[eha_num_hydra] = eha_state[eha_num_hydra].DATA.tau2_act = (short)pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis1_Tau2ActIn*CURRENT_BIT2PHY;
        all_EHA_tau3[eha_num_hydra] = eha_state[eha_num_hydra].DATA.tau3_act = (short)pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.Axis1_Tau3ActIn*CURRENT_BIT2PHY;
        eha_state[eha_num_hydra].DATA.stsword = eha_status;
        all_EHA_pos[eha_num_hydra] = eha_state[eha_num_hydra].DATA.pos_act;

        sensor_state[0].DATA.ft_sensor[loop].wrench[0] = (pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_Fx - FS_phys_offset_MD4KW_2MFS[loop][0]) * FS_phys_MD4KW_2MFS[loop][0];
        sensor_state[0].DATA.ft_sensor[loop].wrench[1] = (pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_Fy - FS_phys_offset_MD4KW_2MFS[loop][1]) * FS_phys_MD4KW_2MFS[loop][1];
        sensor_state[0].DATA.ft_sensor[loop].wrench[2] = (pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_Fz - FS_phys_offset_MD4KW_2MFS[loop][2]) * FS_phys_MD4KW_2MFS[loop][2];
        sensor_state[0].DATA.ft_sensor[loop].wrench[3] = (pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_Mx - FS_phys_offset_MD4KW_2MFS[loop][3]) * FS_phys_MD4KW_2MFS[loop][3];
        sensor_state[0].DATA.ft_sensor[loop].wrench[4] = (pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_My - FS_phys_offset_MD4KW_2MFS[loop][4]) * FS_phys_MD4KW_2MFS[loop][4];
        sensor_state[0].DATA.ft_sensor[loop].wrench[5] = (pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_Mz - FS_phys_offset_MD4KW_2MFS[loop][5]) * FS_phys_MD4KW_2MFS[loop][5];

#if 0
        SHM_HYDRA_FS_FX_RAW_IN(1, loop) = pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_Fx;
        SHM_HYDRA_FS_FY_RAW_IN(1, loop) = pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_Fy;
        SHM_HYDRA_FS_FZ_RAW_IN(1, loop) = pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_Fz;
        SHM_HYDRA_FS_MX_RAW_IN(1, loop) = pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_Mx;
        SHM_HYDRA_FS_MY_RAW_IN(1, loop) = pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_My;
        SHM_HYDRA_FS_MZ_RAW_IN(1, loop) = pAllSlv->MD4KW_2MFS[loop].Pdo.tInp.FS_Mz;
#endif
    }
    // YNL MD4KW_Hand
    for(loop = 0; loop < pAllSlv->MD4KW_HandSlaves; loop++) {
        //送受信の最新データ
        // pAllSlv 	: ローカルテーブルへ
        // SHM_TM～	：共有メモリへ
        //SHM_MD4KW_HAND_INP(1, loop) =
        pAllSlv->MD4KW_Hand[loop].Pdo.tInp 	= *(T_MD4KW_HAND_TXPDO_BASIC*)(pbyPDIn + pAllSlv->MD4KW_Hand[loop].Info.dwPdOffsIn/8);

        ch_num = 0;
        eha_num_hydra = joint_num_hydra_MD4KW_HAND[loop][ch_num];
        eha_state[eha_num_hydra].DATA.rawpos_act =
                pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis0_PosActIn;
        eha_state[eha_num_hydra].DATA.pos_act =
                ((EC_T_INT)eha_state[eha_num_hydra].DATA.rawpos_act)*eha_phys_pos_hydra_MD4KW_Hand[loop][ch_num];
        eha_state[eha_num_hydra].DATA.vel_act = pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis0_Vel2ActIn;
        eha_state[eha_num_hydra].DATA.tau_act = pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis0_TauActIn;
        all_EHA_pos[eha_num_hydra] = eha_state[eha_num_hydra].DATA.pos_act;


        ch_num = 1;
        eha_num_hydra = joint_num_hydra_MD4KW_HAND[loop][ch_num];
        eha_state[eha_num_hydra].DATA.rawpos_act =
                pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis1_PosActIn;
        eha_state[eha_num_hydra].DATA.pos_act =
                ((EC_T_INT)eha_state[eha_num_hydra].DATA.rawpos_act)*eha_phys_pos_hydra_MD4KW_Hand[loop][ch_num];
        eha_state[eha_num_hydra].DATA.vel_act = pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis1_Vel2ActIn;
        eha_state[eha_num_hydra].DATA.tau_act = pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis1_TauActIn;
        all_EHA_pos[eha_num_hydra] = eha_state[eha_num_hydra].DATA.pos_act;

        ch_num = 2;
        eha_num_hydra = joint_num_hydra_MD4KW_HAND[loop][ch_num];
        eha_state[eha_num_hydra].DATA.rawpos_act =
                pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis2_PosActIn;
        eha_state[eha_num_hydra].DATA.pos_act =
                ((EC_T_INT)eha_state[eha_num_hydra].DATA.rawpos_act)*eha_phys_pos_hydra_MD4KW_Hand[loop][ch_num];
        eha_state[eha_num_hydra].DATA.vel_act = pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis2_Vel2ActIn;
        eha_state[eha_num_hydra].DATA.tau_act = pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis2_TauActIn;
        all_EHA_pos[eha_num_hydra] = eha_state[eha_num_hydra].DATA.pos_act;

        ch_num = 3;
        eha_num_hydra = joint_num_hydra_MD4KW_HAND[loop][ch_num];
        eha_state[eha_num_hydra].DATA.rawpos_act =
                pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis3_PosActIn;
        eha_state[eha_num_hydra].DATA.pos_act =
                ((EC_T_INT)eha_state[eha_num_hydra].DATA.rawpos_act)*eha_phys_pos_hydra_MD4KW_Hand[loop][ch_num];
        eha_state[eha_num_hydra].DATA.vel_act = pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis3_Vel2ActIn;
        eha_state[eha_num_hydra].DATA.tau_act = pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis3_TauActIn;
        all_EHA_pos[eha_num_hydra] = eha_state[eha_num_hydra].DATA.pos_act;

        ch_num = 4;
        eha_num_hydra = joint_num_hydra_MD4KW_HAND[loop][ch_num];
        eha_state[eha_num_hydra].DATA.rawpos_act =
                pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis4_PosActIn;
        eha_state[eha_num_hydra].DATA.pos_act =
                ((EC_T_INT)eha_state[eha_num_hydra].DATA.rawpos_act)*eha_phys_pos_hydra_MD4KW_Hand[loop][ch_num];
        eha_state[eha_num_hydra].DATA.vel_act = pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis4_Vel2ActIn;
        eha_state[eha_num_hydra].DATA.tau_act = pAllSlv->MD4KW_Hand[loop].Pdo.tInp.Axis4_TauActIn;
        all_EHA_pos[eha_num_hydra] = eha_state[eha_num_hydra].DATA.pos_act;

    }
    // YNL MD4KW_IMU
    for(loop = 0; loop < pAllSlv->MD4KW_IMUSlaves; loop++) {
        //送受信の最新データ
        // pAllSlv 	: ローカルテーブルへ
        // SHM_TM～	：共有メモリへ
        //SHM_MD4KW_IMU_INP(1, loop) =
        pAllSlv->MD4KW_IMU[loop].Pdo.tInp 	= *(T_MD4KW_IMU_TXPDO_BASIC*)(pbyPDIn + pAllSlv->MD4KW_IMU[loop].Info.dwPdOffsIn/8);
        sensor_state[0].DATA.imu[0].gyro[0] = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.GyroXIn * IMU_phys_MD4KW_IMU[0];
        sensor_state[0].DATA.imu[0].gyro[1] = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.GyroYIn * IMU_phys_MD4KW_IMU[1];
        sensor_state[0].DATA.imu[0].gyro[2] = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.GyroZIn * IMU_phys_MD4KW_IMU[2];
        sensor_state[0].DATA.imu[0].acc[0]  = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.AccXIn  * IMU_phys_MD4KW_IMU[3];
        sensor_state[0].DATA.imu[0].acc[1]  = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.AccYIn  * IMU_phys_MD4KW_IMU[4];
        sensor_state[0].DATA.imu[0].acc[2]  = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.AccZIn  * IMU_phys_MD4KW_IMU[5];

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.Quarternion0In;
        sensor_state[0].DATA.imu[0].Quarternion[0] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.Quarternion1In;
        sensor_state[0].DATA.imu[0].Quarternion[1] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.Quarternion2In;
        sensor_state[0].DATA.imu[0].Quarternion[2] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.Quarternion3In;
        sensor_state[0].DATA.imu[0].Quarternion[3] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.Omega_c0In;
        sensor_state[0].DATA.imu[0].Omega_c[0] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.Omega_c1In;
        sensor_state[0].DATA.imu[0].Omega_c[1] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.Omega_c2In;
        sensor_state[0].DATA.imu[0].Omega_c[2] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.Accel0In;
        sensor_state[0].DATA.imu[0].Accel[0] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.Accel1In;
        sensor_state[0].DATA.imu[0].Accel[1] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.Accel2In;
        sensor_state[0].DATA.imu[0].Accel[2] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.VelWorld0In;
        sensor_state[0].DATA.imu[0].VelWorld[0] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.VelWorld1In;
        sensor_state[0].DATA.imu[0].VelWorld[1] = tmp_flt.SHRT_REAL;

        tmp_flt.DWORD = pAllSlv->MD4KW_IMU[loop].Pdo.tInp.VelWorld2In;
        sensor_state[0].DATA.imu[0].VelWorld[2] = tmp_flt.SHRT_REAL;
    }

    if(initial) {
        memset(all_joint_pos_prev, 0, sizeof(EC_T_REAL)*HYDRA_JNT_MAX);
        all_joint_pos_prev[JOINT_HYDRA_RSHOULDER_ROLL] = -M_PI/4;
        all_joint_pos_prev[JOINT_HYDRA_LSHOULDER_ROLL] =  M_PI/4;
        all_joint_pos_prev[JOINT_HYDRA_RELBOW_PITCH]   = -M_PI/2;
        all_joint_pos_prev[JOINT_HYDRA_LELBOW_PITCH]   = -M_PI/2;
        JointToCylinderAll(all_joint_pos_prev, all_EHA_pos_prev);
        initial = false;
    }

    if(	CylinderToJointAll(all_EHA_pos, all_joint_pos, all_joint_pos_prev) < 0)
        printf("Parallel Joint IK Failed");
    else {
        if(cnt==0) {

#ifdef DBG_KNEE
            LogMsg("RKnee: %08x, %08x, %f, %f, %f",
                   pAllSlv->MD4KW_3M[1].Pdo.tInp.Axis0_PosActIn,
                    pAllSlv->MD4KW_3M[1].Pdo.tInp.Axis1_PosActIn,
                    all_EHA_pos[3],
                    all_EHA_pos[4],
                    all_joint_pos[joint_rknee_pitch]);
            LogMsg("LKnee: %08x, %08x, %f, %f, %f",
                   pAllSlv->MD4KW_3M[3].Pdo.tInp.Axis0_PosActIn,
                    pAllSlv->MD4KW_3M[3].Pdo.tInp.Axis1_PosActIn,
                    all_EHA_pos[11],
                    all_EHA_pos[12],
                    all_joint_pos[joint_lknee_pitch]);
#endif
#ifdef DBG_HIP
            LogMsg("RHip: %08x, %08x, %08x, %f, %f, %f, %f, %f, %f",
                   pAllSlv->MD4KW_3M[0].Pdo.tInp.Axis0_PosActIn,
                    pAllSlv->MD4KW_3M[0].Pdo.tInp.Axis1_PosActIn,
                    pAllSlv->MD4KW_3M[0].Pdo.tInp.Axis2_PosActIn,
                    all_EHA_pos[0],
                    all_EHA_pos[1],
                    all_EHA_pos[2],
                    all_joint_pos[joint_rhip_yaw],
                    all_joint_pos[joint_rhip_pitch],
                    all_joint_pos[joint_rhip_roll]);
            LogMsg("LHip: %08x, %08x, %08x, %f, %f, %f, %f, %f, %f",
                   pAllSlv->MD4KW_3M[2].Pdo.tInp.Axis0_PosActIn,
                    pAllSlv->MD4KW_3M[2].Pdo.tInp.Axis1_PosActIn,
                    pAllSlv->MD4KW_3M[2].Pdo.tInp.Axis2_PosActIn,
                    all_EHA_pos[8],
                    all_EHA_pos[9],
                    all_EHA_pos[10],
                    all_joint_pos[joint_lhip_yaw],
                    all_joint_pos[joint_lhip_pitch],
                    all_joint_pos[joint_lhip_roll]);
#endif
#ifdef DBG_ANKLE
            LogMsg("RAnkle: %08x, %08x, %f, %f, %f, %f",
                   pAllSlv->MD4KW_2MFS[0].Pdo.tInp.Axis0_PosActIn,
                    pAllSlv->MD4KW_2MFS[0].Pdo.tInp.Axis1_PosActIn,
                    all_EHA_pos[6],
                    all_EHA_pos[7],
                    all_joint_pos[4],
                    all_joint_pos[5]);
            LogMsg("LAnkle: %08x, %08x, %f, %f, %f, %f",
                   pAllSlv->MD4KW_2MFS[1].Pdo.tInp.Axis0_PosActIn,
                    pAllSlv->MD4KW_2MFS[1].Pdo.tInp.Axis1_PosActIn,
                    all_EHA_pos[14],
                    all_EHA_pos[15],
                    all_joint_pos[10],
                    all_joint_pos[11]);
#endif
        }
    }
    cnt++;
    ActForceToTorqueAll(all_joint_pos, all_EHA_tau, all_joint_tau);
    //ActForceToTorqueAll(all_joint_pos, all_EHA_tau2, all_joint_tau2);
    //ActForceToTorqueAll(all_joint_pos, all_EHA_tau3, all_joint_tau3);

    for(loop = 0; loop<HYDRA_JNT_MAX; loop++) {
        joint_state[loop].DATA.pos_act = all_joint_pos[loop];
        joint_state[loop].DATA.vel_act = all_joint_vel[loop];
        joint_state[loop].DATA.tau_act = all_joint_tau[loop];
        joint_state[loop].DATA.tau2_act = all_joint_tau2[loop];
        joint_state[loop].DATA.tau3_act = all_joint_tau3[loop];
    }

    pIPC->WriteStatus(joint_state, eha_state, sensor_state);

    memcpy(all_joint_pos_prev, all_joint_pos, sizeof(EC_T_REAL)*EHA_MAX);
    return dwRes;
}

EC_T_REAL       all_EHA_pos_out[EHA_MAX];
EC_T_REAL       all_EHA_vel_out[EHA_MAX];
EC_T_REAL       all_EHA_tau_out[EHA_MAX];

EC_T_REAL       all_joint_pos_out[HYDRA_JNT_MAX];
EC_T_REAL       all_joint_vel_out[HYDRA_JNT_MAX];
EC_T_REAL       all_joint_tau_out[HYDRA_JNT_MAX];
/********************************************************************************/
/** \brief  Handler for application notifications
*
*  !!! No blocking API shall be called within this function!!! 
*  !!! Function is called by cylic task                    !!! 
*
* \return  Status value.
*/
static EC_T_DWORD myAppPdoOutput(T_ALL_SLAVE_INFO* pAllSlv)
{
    EC_T_DWORD 		dwRes 	= EC_E_ERROR;
    EC_T_BYTE*  	pbyPDOut= EC_NULL;
    EC_T_INT		loop;
    EC_T_INT        loop_jnt;
    static int cnt=0;

    pbyPDOut = ecatGetProcessImageOutputPtr();/* pointer to process data output buffer */

    // プロセスデータバッファがまだ存在しない場合
    if(pbyPDOut == EC_NULL) {
        // 何も行わない
        return dwRes;
    }

    if(pShmServer == EC_NULL) {
        return -1;
    }

    memset(all_EHA_pos_out, 0, sizeof(double)*EHA_MAX);

    pIPC->ReadCommand(joint_cmd, eha_cmd, sensor_cmd);

    for(loop = 0; loop<HYDRA_JNT_MAX; loop++) {
        all_joint_pos_out[loop] = joint_cmd[loop].DATA.pos_ref;
        all_joint_vel_out[loop] = joint_cmd[loop].DATA.vel_ref;
        all_joint_tau_out[loop] = joint_cmd[loop].DATA.tau_ref;
    }

    //JointToCylinderAll(all_joint_pos_out, all_EHA_pos_out); //moved to servo_ui
#if 1
    for(loop=0;loop<EHA_MAX;loop++){
        all_EHA_pos_out[loop] = eha_cmd[loop].DATA.pos_ref;
    }
#endif
#if 0
    if(cnt>=1000){
        LogMsg("J1 Joint Ref: %G EHA Ref %G\n", all_joint_pos_out[0], all_EHA_pos_out[0]);
        LogMsg("RH1 Joint Ref: %G EHA Ref %G\n", all_joint_pos_out[31], all_EHA_pos_out[37]);
        LogMsg("RH5 Joint Ref: %G EHA Ref %G\n", all_joint_pos_out[35], all_EHA_pos_out[41]);
        cnt=0;
    }
#endif
    cnt++;
    // YNL MD4KW_3M
    for(loop = 0; loop < pAllSlv->MD4KW_3MSlaves; loop++) {
    if(HydraOnlyUpperbody)
        loop_jnt = loop + LOWERBODY_3M_NUM;
    else
        loop_jnt = loop;

        T_MD4KW_3M_RXPDO_BASIC *pTmp = (T_MD4KW_3M_RXPDO_BASIC*)(pbyPDOut + pAllSlv->MD4KW_3M[loop].Info.dwPdOffsOut/8);
        T_MD4KW_3M_RXPDO_BASIC TmpRecv;
        //送受信の最新データ

        memset(&TmpRecv, 0, sizeof(TmpRecv));
        // OUTPUT用の共有メモリから指令値等を書き込む場合
        TmpRecv.Axis0_PosRefOut = (EC_T_DWORD) (all_EHA_pos_out[joint_num_hydra_MD4KW_3M[loop_jnt][0]]/eha_phys_pos_hydra_MD4KW_3M[loop_jnt][0]) + enc_offset_MD4KW_3M[loop_jnt][0];
        TmpRecv.Axis1_PosRefOut = (EC_T_DWORD) (all_EHA_pos_out[joint_num_hydra_MD4KW_3M[loop_jnt][1]]/eha_phys_pos_hydra_MD4KW_3M[loop_jnt][1]) + enc_offset_MD4KW_3M[loop_jnt][1];
        TmpRecv.Axis2_PosRefOut = (EC_T_DWORD) (all_EHA_pos_out[joint_num_hydra_MD4KW_3M[loop_jnt][2]]/eha_phys_pos_hydra_MD4KW_3M[loop_jnt][2]) + enc_offset_MD4KW_3M[loop_jnt][2];
        // TmpRecv.Axis0_VelRefOut = all_EHA_vel_out[joint_num_hydra_MD4KW_3M[loop_jnt][0]];
        // TmpRecv.Axis1_VelRefOut = all_EHA_vel_out[joint_num_hydra_MD4KW_3M[loop_jnt][1]];
        // TmpRecv.Axis2_VelRefOut = all_EHA_vel_out[joint_num_hydra_MD4KW_3M[loop_jnt][2]];
         TmpRecv.Axis0_TauRefOut = all_EHA_tau_out[joint_num_hydra_MD4KW_3M[loop_jnt][0]];
         TmpRecv.Axis1_TauRefOut = all_EHA_tau_out[joint_num_hydra_MD4KW_3M[loop_jnt][1]];
         TmpRecv.Axis2_TauRefOut = all_EHA_tau_out[joint_num_hydra_MD4KW_3M[loop_jnt][2]];

        TmpRecv.Axis0_CtrlWordOut = eha_cmd[joint_num_hydra_MD4KW_3M[loop_jnt][0]].DATA.ctlword;
        TmpRecv.Axis1_CtrlWordOut = eha_cmd[joint_num_hydra_MD4KW_3M[loop_jnt][1]].DATA.ctlword;
        TmpRecv.Axis2_CtrlWordOut = eha_cmd[joint_num_hydra_MD4KW_3M[loop_jnt][2]].DATA.ctlword;

        //	  TmpRecv.Axis0_CtrlWordOut = 0x0101;
        //	  TmpRecv.Axis1_CtrlWordOut = 0x0101;
        //	  TmpRecv.Axis2_CtrlWordOut = 0x0101;

        //LogMsg("Node: %d PosRef0 %p, PosRef1 %p, PosRef2 %p, CW0 %p, CW1 %p, CW2 %p",
        //	 loop, &TmpRecv.Axis0_PosRefOut, &TmpRecv.Axis1_PosRefOut, &TmpRecv.Axis2_PosRefOut,
        //	 &TmpRecv.Axis0_CtrlWordOut, &TmpRecv.Axis1_CtrlWordOut, &TmpRecv.Axis2_CtrlWordOut);

        // some calculations here

        *pTmp = TmpRecv;
    }
    // YNL MD4KW_2MFS
    for(loop = 0; loop < pAllSlv->MD4KW_2MFSSlaves; loop++) {
        T_MD4KW_2MFS_RXPDO_BASIC *pTmp = (T_MD4KW_2MFS_RXPDO_BASIC*)(pbyPDOut + pAllSlv->MD4KW_2MFS[loop].Info.dwPdOffsOut/8);
        T_MD4KW_2MFS_RXPDO_BASIC TmpRecv;
        //送受信の最新データ

        memset(&TmpRecv, 0, sizeof(TmpRecv));

        // OUTPUT用の共有メモリから指令値等を書き込む場合
        TmpRecv.Axis0_PosRefOut = (EC_T_DWORD) (all_EHA_pos_out[joint_num_hydra_MD4KW_2MFS[loop][0]]/eha_phys_pos_hydra_MD4KW_2MFS[loop][0]) + enc_offset_MD4KW_2MFS[loop][0];
        TmpRecv.Axis1_PosRefOut = (EC_T_DWORD) (all_EHA_pos_out[joint_num_hydra_MD4KW_2MFS[loop][1]]/eha_phys_pos_hydra_MD4KW_2MFS[loop][1]) + enc_offset_MD4KW_2MFS[loop][1];

        // TmpRecv.Axis0_VelRefOut = all_EHA_vel_out[joint_num_hydra_MD4KW_2MFS[loop][0]];
        // TmpRecv.Axis1_VelRefOut = all_EHA_vel_out[joint_num_hydra_MD4KW_2MFS[loop][1]];
        // TmpRecv.Axis0_TauRefOut = all_EHA_tau_out[joint_num_hydra_MD4KW_2MFS[loop][0]];
        // TmpRecv.Axis1_TauRefOut = all_EHA_tau_out[joint_num_hydra_MD4KW_2MFS[loop][1]];

        TmpRecv.Axis0_CtrlWordOut = eha_cmd[joint_num_hydra_MD4KW_2MFS[loop][0]].DATA.ctlword;
        TmpRecv.Axis1_CtrlWordOut = eha_cmd[joint_num_hydra_MD4KW_2MFS[loop][1]].DATA.ctlword;

        //	memset(&TmpRecv, 0, sizeof(TmpRecv));
        *pTmp = TmpRecv;
    }
    // YNL MD4KW_Hand
    for(loop = 0; loop < pAllSlv->MD4KW_HandSlaves; loop++)
    {
        T_MD4KW_HAND_RXPDO_BASIC *pTmp = (T_MD4KW_HAND_RXPDO_BASIC*)(pbyPDOut + pAllSlv->MD4KW_Hand[loop].Info.dwPdOffsOut/8);
        T_MD4KW_HAND_RXPDO_BASIC TmpRecv;
        //送受信の最新データ

        memset(&TmpRecv, 0, sizeof(TmpRecv));

        // OUTPUT用の共有メモリから指令値等を書き込む場合
        TmpRecv.Axis0_PosRefOut = (EC_T_DWORD)(all_EHA_pos_out[joint_num_hydra_MD4KW_HAND[loop][0]]/eha_phys_pos_hydra_MD4KW_Hand[loop][0]);
        TmpRecv.Axis1_PosRefOut = (EC_T_DWORD)(all_EHA_pos_out[joint_num_hydra_MD4KW_HAND[loop][1]]/eha_phys_pos_hydra_MD4KW_Hand[loop][1]);
        TmpRecv.Axis2_PosRefOut = (EC_T_DWORD)(all_EHA_pos_out[joint_num_hydra_MD4KW_HAND[loop][2]]/eha_phys_pos_hydra_MD4KW_Hand[loop][2]);
        TmpRecv.Axis3_PosRefOut = (EC_T_DWORD)(all_EHA_pos_out[joint_num_hydra_MD4KW_HAND[loop][3]]/eha_phys_pos_hydra_MD4KW_Hand[loop][3]);
        TmpRecv.Axis4_PosRefOut = (EC_T_DWORD)(all_EHA_pos_out[joint_num_hydra_MD4KW_HAND[loop][4]]/eha_phys_pos_hydra_MD4KW_Hand[loop][4]);
        // TmpRecv.Axis0_PosRefOut = 1023;
        // TmpRecv.Axis1_PosRefOut = 1024;
        // TmpRecv.Axis2_PosRefOut = 1025;
        // TmpRecv.Axis3_PosRefOut = 21026;
        // TmpRecv.Axis4_PosRefOut = 21027;

        TmpRecv.Axis0_CtrlWordOut = eha_cmd[joint_num_hydra_MD4KW_HAND[loop][0]].DATA.ctlword;
        TmpRecv.Axis1_CtrlWordOut = eha_cmd[joint_num_hydra_MD4KW_HAND[loop][1]].DATA.ctlword;
        TmpRecv.Axis2_CtrlWordOut = eha_cmd[joint_num_hydra_MD4KW_HAND[loop][2]].DATA.ctlword;
        TmpRecv.Axis3_CtrlWordOut = eha_cmd[joint_num_hydra_MD4KW_HAND[loop][3]].DATA.ctlword;
        TmpRecv.Axis4_CtrlWordOut = eha_cmd[joint_num_hydra_MD4KW_HAND[loop][4]].DATA.ctlword;

        *pTmp = TmpRecv;
    }
    // YNL MD4KW_IMU
    for(loop = 0; loop < pAllSlv->MD4KW_IMUSlaves; loop++) {
        T_MD4KW_IMU_RXPDO_BASIC *pTmp = (T_MD4KW_IMU_RXPDO_BASIC*)(pbyPDOut + pAllSlv->MD4KW_IMU[loop].Info.dwPdOffsOut/8);
        T_MD4KW_IMU_RXPDO_BASIC TmpRecv;
        //送受信の最新データ

        memset(&TmpRecv, 0, sizeof(TmpRecv));

        // OUTPUT用の共有メモリから指令値等を書き込む場合
        TmpRecv.CtrlWordOut = sensor_cmd[0].DATA.imu[loop].ctlword;

        // some calculations here
        
        *pTmp = TmpRecv;
    }
    //LogMsg("1:%04x", SHM_HYDRA_EHA_CTRLWORD_OUT(0, 7));
#if 0
    LogMsg("RHip(0:%04x, 1:%04x, 2:%04x), RKnee(3:%04x, 4:%04x, 5:%04x), RAnkle(6:%04x, 7:%04x), LHip(8:%04x, 9:%04x, 10:%04x), LKnee(11:%04x, 12:%04x, 13:%04x), LAnkle(14:%04x, 15:%04x), Body(16:%04x, 17:%04x, 18:%04x), RShoulder(19:%04x, 20:%04x, 21:%04x), RElbow(22:%04x, 23:%04x, 24:%04x), RWrist(25:%04x, 26:%04x, 27:%04x), LShoulder(28:%04x, 29:%04x, 30:%04x), LElbow(31:%04x, 32:%04x, 33:%04x), LWrist(34:%04x, 35:%04x, 36:%04x), RHand(37:%04x, 38:%04x, 39:%04x, 40:%04x, 41:%04x), LHand(42:%04x, 43:%04x, 44:%04x, 45:%04x, 46:%04x)",
           SHM_HYDRA_EHA_CTRLWORD_OUT(0, 0), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 1), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 2)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 3), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 4), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 5)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 6), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 7)
           ,SHM_HYDRA_EHA_CTRLWORD_OUT(0, 8), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 9), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 10)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 11), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 12), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 13)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 14), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 15)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 16), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 17), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 18)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 19), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 20), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 21)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 22), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 23), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 24)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 25), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 26), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 27)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 28), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 29), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 30)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 31), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 32), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 33)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 34), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 35), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 36)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 37), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 38), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 39), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 40), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 41)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 42), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 43), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 44), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 45), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 46)
           );

    LogMsg("RHip(0:%04x, 1:%04x, 2:%04x), RKnee(3:%04x, 4:%04x, 5:%04x), RAnkle(6:%04x, 7:%04x), LHip(8:%04x, 9:%04x, 10:%04x), LKnee(11:%04x, 12:%04x, 13:%04x), LAnkle(14:%04x, 15:%04x)",
           SHM_HYDRA_EHA_CTRLWORD_OUT(0, 0), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 1), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 2)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 3), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 4), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 5)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 6), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 7)
           ,SHM_HYDRA_EHA_CTRLWORD_OUT(0, 8), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 9), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 10)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 11), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 12), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 13)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 14), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 15)
           );

    LogMsg("RHip(0:%04x, 1:%04x, 2:%04x), RKnee(3:%04x, 4:%04x, 5:%04x), RAnkle(6:%04x, 7:%04x)",
           SHM_HYDRA_EHA_CTRLWORD_OUT(0, 0), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 1), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 2)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 3), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 4), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 5)
           , SHM_HYDRA_EHA_CTRLWORD_OUT(0, 6), SHM_HYDRA_EHA_CTRLWORD_OUT(0, 7)
           );
#endif
    return dwRes;
}


/********************************************************************************/
/** \brief  Handler for application notifications
*
*  !!! No blocking API shall be called within this function!!! 
*  !!! Function is called by cylic task                    !!! 
*
* \return  Status value.
*/
static EC_T_DWORD myAppSdoTfer (T_ALL_SLAVE_INFO* pAllSlv)
{
    static EC_T_MBXTFER_DESC	MbxTferDesc		= {0};
    static EC_T_MBXTFER*		pMbxCoETfer		= EC_NULL;
    static EC_T_BYTE*			pbyCoEBuffer	= EC_NULL;
    EC_T_DWORD					tmp_dwSlaveId;

    if(pShmServer  == EC_NULL)
    {
        return EC_E_NOERROR;
    }

    // まずは現在実行中であるかを確認する。
    // mbxTferを複数作成すれば並列で実施可能だが、
    // 現状としては高負荷を避けるため、１つずつ実行を行う。

    // オブジェクトがNULLの場合、未実行と判断する
    if( EC_NULL == pMbxCoETfer)
    {
        // SDO要求が存在する場合
        if(pShmServer->GetSDOReqNo() != pShmServer->GetSDOResNo())
        {
            //
            // SDO要求用のオブジェクトを作成する
            //

            /* create memory */
            pbyCoEBuffer = (EC_T_BYTE*)OsMalloc(COE_BUF_SIZE);
            if(pbyCoEBuffer == EC_NULL)
            {
                LogError("Error: NO MEMORY!!");
                return EC_E_ERROR;
            }
            OsMemset(pbyCoEBuffer, 0, COE_BUF_SIZE);

            /* create required MBX Transfer Objects */
            /* mailbox transfer object for OD list upload */
            MbxTferDesc.dwMaxDataLen        = COE_BUF_SIZE;
            MbxTferDesc.pbyMbxTferDescData  = pbyCoEBuffer;

            pMbxCoETfer = ecatMbxTferCreate( &MbxTferDesc );
            if( EC_NULL == pMbxCoETfer)
            {
                LogError("Error: ecatMbxTferCreate NG!!");
                return EC_E_ERROR;
            }
            OsMemset(pMbxCoETfer->pbyMbxTferData, 0, COE_BUF_SIZE);

            pMbxCoETfer->dwClntId = dwClntId;
            pMbxCoETfer->dwTferId = 1;

            //
            // SDO用のデータを共有メモリから取得する
            //
            switch(pShmServer->GetSDOParamPtr()->Md4kwType)
            {
            case BDTYPE_MD4KW_3M:
                tmp_dwSlaveId = pAllSlv->MD4KW_3M[pShmServer->GetSDOParamPtr()->SlaveNo].Info.dwSlaveId;
                break;

            case BDTYPE_MD4KW_2MFS:
                tmp_dwSlaveId = pAllSlv->MD4KW_2MFS[pShmServer->GetSDOParamPtr()->SlaveNo].Info.dwSlaveId;
                break;

            case BDTYPE_MD4KW_HAND:
                tmp_dwSlaveId = pAllSlv->MD4KW_Hand[pShmServer->GetSDOParamPtr()->SlaveNo].Info.dwSlaveId;
                break;

            case BDTYPE_MD4KW_IMU:
                tmp_dwSlaveId = pAllSlv->MD4KW_IMU[pShmServer->GetSDOParamPtr()->SlaveNo].Info.dwSlaveId;
                break;
            default:
                LogError("Error: Md4kwType NG!!");
                return EC_E_ERROR;
            }


            // SDOがSETコマンドの場合
            if( pShmServer->GetSDOParamPtr()->SetCmdFlg == true )
            {
                // データを設定
                OsMemset(pMbxCoETfer->pbyMbxTferData, 0, COE_BUF_SIZE);
                OsMemcpy(pMbxCoETfer->pbyMbxTferData, pShmServer->GetSDOParamPtr()->Data.byte, pShmServer->GetSDOParamPtr()->DataSize);
                pMbxCoETfer->dwDataLen  = pShmServer->GetSDOParamPtr()->DataSize;

                // EC-MasterにSDOの設定を要求
                ecatCoeSdoDownloadReq(
                            pMbxCoETfer,
                            tmp_dwSlaveId,  // dwSlaveId    : EtherCAT slave ID. To determine the slave ID the function ecatGetSlaveId has to be used.
                            pShmServer->GetSDOParamPtr()->ObIndex,    // wObIndex     : Object index.
                            pShmServer->GetSDOParamPtr()->ObSubIndex, // byObSubIndex : Object sub index.
                            MBX_TIMEOUT,    // dwTimeout    : Timeout in milliseconds. The function will block at most for this time.
                            0);             // dwFlags      : Mailbox Flags. Bit 0: if 1 Complete Access (EC_MAILBOX_FLAG_SDO_COMPLETE).
            }
            // SDOがGETコマンドの場合
            else
            {
                // EC-MasterにSDOの取得を要求
                ecatCoeSdoUploadReq(
                            pMbxCoETfer,
                            tmp_dwSlaveId, 					// dwSlaveId    : EtherCAT slave ID. To determine the slave ID the function ecatGetSlaveId has to be used.
                            pShmServer->GetSDOParamPtr()->ObIndex,		// wObIndex     : Object index.
                            pShmServer->GetSDOParamPtr()->ObSubIndex,	// byObSubIndex : Object sub index.
                            MBX_TIMEOUT,					// dwTimeout    : Timeout in milliseconds. The function will block at most for this time.
                            0);								// dwFlags      : Mailbox Flags. Bit 0: if 1 Complete Access (EC_MAILBOX_FLAG_SDO_COMPLETE).
            }
        }
    }
    else
    {
        // 処理状態がIdle(Idle->Pend->Done->Idleと遷移する)になるまでポーリングする
        if(pMbxCoETfer->eTferStatus == eMbxTferStatus_Idle)
        {
            if(pMbxCoETfer->eMbxTferType== eMbxTferType_COE_SDO_UPLOAD)
            {
                // SDO Resultへデータを設定する
                pShmServer->GetSDOParamPtr()->DataSize = pMbxCoETfer->dwDataLen;
                OsMemcpy(pShmServer->GetSDOParamPtr()->Data.byte, pMbxCoETfer->pbyMbxTferData, pMbxCoETfer->dwDataLen);
            }

            // SDO Resultの戻り値を設定する
            pShmServer->SetSDORetVal(pMbxCoETfer->dwErrorCode);
//            SDORES_RET(SDORES_NO) = pMbxCoETfer->dwErrorCode;

            // SDO Resultカウンターを更新する
            pShmServer->UpdateSDONo();
//            SDONO_UPDATE(SDORES_NO);

            /* Delete MBX Transfer objects */
            if( EC_NULL != pMbxCoETfer) {
                ecatMbxTferDelete( pMbxCoETfer );
                pMbxCoETfer = EC_NULL;
            }
            if( EC_NULL != pbyCoEBuffer) {
                OsFree(pbyCoEBuffer);
                pbyCoEBuffer = EC_NULL;
            }
        }
    }
}

static EC_T_CHAR* TaAlarmText( EC_T_BYTE bAlarmCode)
{
    switch(bAlarmCode) {
    case 0x11: return (char*)"過電流異常";
    case 0x12: return (char*)"過熱異常";
    case 0x21: return (char*)"過負荷異常";
    case 0x31: return (char*)"過速度異常";
    case 0x41: return (char*)"偏差異常";
    case 0x74: return (char*)"過電圧異常";
    case 0x75: return (char*)"低電圧異常";
    case 0x93: return (char*)"EEPROMデータ照合異常";
    case 0x94: return (char*)"EEPROM異常";
    case 0x95: return (char*)"EEPROM通信異常";
    case 0x9f: return (char*)"WDT異常";
    case 0xa1: return (char*)"トルクセンサ異常";

        //上記以外
    default:
        if((bAlarmCode >= 0x61) && (bAlarmCode <= 0x6f)) return (char*)"エンコーダセンサ異常";
        
        return (char*)"不明エラーコード";
    }
}


/********************************************************************************/
/** \brief  LogSave Task
*
* \return N/A
*/

static EC_T_VOID tLogSaveTask( EC_T_VOID* pvThreadParamDesc )
{
    T_THREAD_PARAM* 		pThreadParam  		= (T_THREAD_PARAM*)pvThreadParamDesc;
    EC_T_DWORD				Remain;
    EC_T_CPUSET         	CpuSet;
    EC_T_INT				loop;

    T_SHM_INPUT_ACC_MD4KW	*pAccIn 			= EC_NULL;
    T_SHM_OUTPUT_ACC_MD4KW	*pAccOut 			= EC_NULL;

    static EC_T_INT			sPutIdx 			= 0;
    FILE					*fpPdo 				= EC_NULL;
    EC_T_CHAR				*pTmpBuf			= EC_NULL;
    EC_T_CHAR				*pTmpBufAll			= EC_NULL;
    T_ALL_SLAVE_INFO*		pAllSlv				= EC_NULL;

    struct tm           	*local;
    time_t              	timer;
    EC_T_CHAR           	datetime[256];
    EC_T_CHAR           	fname_pdo[256];

    joint_state_t  jnt_state_log[HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX];
    joint_cmd_t    jnt_cmd_log[HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX];
    eha_state_t    eha_state_log[EHA_MAX];
    eha_cmd_t      eha_cmd_log[EHA_MAX];
    sensor_state_t sensor_state_log[1];
    sensor_cmd_t   sensor_cmd_log[1];

    EC_CPUSET_ZERO( CpuSet );
    EC_CPUSET_SET( CpuSet, (pThreadParam->dwCpuIndex +2));
    OsSetThreadAffinity( EC_NULL, CpuSet );

    pAllSlv  = (T_ALL_SLAVE_INFO*)pThreadParam->pvApplSpecParm1;

    OsMemset(fname_pdo, 	0, 	sizeof(fname_pdo));
    OsMemset(datetime, 		0, 	sizeof(datetime));

    // 最初にOPENするファイルだけ、従来どおりのファイル名とする(gnuplotで使用する関係上)
    snprintf(fname_pdo,	256, "%s%s", "pdo", 	FILE_EXT);
    fpPdo	= OsFopen( fname_pdo, 	"w+" );

    // いずれかでもOPENに失敗した場合
    if(fpPdo == EC_NULL) {
        printf("\n\n!!file open NG!!\n\n");
        pThreadParam->bThreadRunning = EC_FALSE;
        return;
    }

    // root権限でfopenしているため、アクセス権限の設定をしなおす
    chmod(fname_pdo, 	S_IRUSR|S_IWUSR| S_IRGRP|S_IWGRP| S_IROTH|S_IWOTH);

    pTmpBuf 	= (EC_T_CHAR*)OsMalloc(DATA_BUF_SIZE);
    pTmpBufAll  = (EC_T_CHAR*)OsMalloc(DATA_BUF_ALLSIZE);
    if((pTmpBuf == EC_NULL) || (pTmpBufAll == EC_NULL))
    {
        printf("\n\n!! Malloc NG!!\n\n");
        pThreadParam->bThreadRunning = EC_FALSE;
        return;
    }

    // 各Slaveの状態取得が完了するまで待つ
    OsWaitForEvent(pThreadParam->pvTimingEvent, EC_WAITINFINITE);
    pThreadParam->bThreadRunning = EC_TRUE;

    do {
        // もし、新規ファイル指定がされている場合
        if(flg_NewFile == EC_TRUE)
        {

            //現ファイルをクローズ
            if (OsFclose(fpPdo) == EOF)  {
                printf("\n\n!!file close NG!!\n\n");
                pThreadParam->bThreadRunning = EC_FALSE;
                break;
            }

            OsMemset(fname_pdo, 	0, 	sizeof(fname_pdo));
            OsMemset(datetime, 		0, 	sizeof(datetime));

            //現在の日付、時刻を取得
            time(&timer);
            local = localtime(&timer);
            snprintf(datetime, 256, "_%04d%02d%02d_%02d%02d%02d",
                     local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

            //ファイルを再オープン
            snprintf(fname_pdo,	256, "%s%s%s", "pdo",		datetime,	FILE_EXT);
            fpPdo	= OsFopen( fname_pdo, 	"w+" );
            if(fpPdo == EC_NULL) {
                printf("\n\n!!file open NG!!\n\n");
                pThreadParam->bThreadRunning = EC_FALSE;
                break;
            }

            // root権限でfopenしているため、アクセス権限の設定をしなおす
            chmod(fname_pdo, 	S_IRUSR|S_IWUSR| S_IRGRP|S_IWGRP| S_IROTH|S_IWOTH);

            //初期化
            sPutIdx 	   	= pShmServer->GetShmInIdx();
            flg_NewFile 	= EC_FALSE;
            flg_NewLabel   	= EC_TRUE;
            flg_FileUpdate 	= EC_TRUE;
        }

        // もし書き込み停止が指定されている場合
        if(flg_FileUpdate == EC_FALSE)
        {
            sPutIdx = pShmServer->GetShmInIdx();
        }

        //
        // PDOデータの保存処理
        //
        //
        if(sPutIdx != pShmServer->GetShmInIdx())
        {
            do {

                // 共有メモリの参照Indexから、対象面をまるごとポインタで参照する
//                pAccIn  = &pShmIn_MD4KW->Acc[sPutIdx];
//                pAccOut = &pShmOut_MD4KW->Acc[sPutIdx];

                OsMemset(pTmpBuf, 		0, DATA_BUF_SIZE);
                OsMemset(pTmpBufAll, 	0, DATA_BUF_ALLSIZE);

                // msec set
                if(flg_NewLabel == EC_TRUE)
                    snprintf(pTmpBufAll, DATA_BUF_SIZE, "time,");
                else
                    snprintf(pTmpBufAll, DATA_BUF_SIZE, "%d,", pShmServer->GetTimeInfo(sPutIdx));


                pShmServer->ReadStatus(sPutIdx,jnt_state_log,eha_state_log,sensor_state_log);
                pShmServer->ReadCommand(sPutIdx,jnt_cmd_log,eha_cmd_log,sensor_cmd_log);

                for(loop=0;loop<(HYDRA_JNT_MAX);loop++)
                {
                    Remain = DATA_BUF_ALLSIZE - OsStrlen(pTmpBufAll);
                    if(Remain <= 1) break;

                    if(flg_NewLabel==EC_TRUE)
                    {
                        snprintf(pTmpBuf,DATA_BUF_SIZE,
                                 "jnt_pos_ref[%d],jnt_pos_act[%d],jnt_vel_ref[%d],jnt_vel_act[%d],jnt_tau_ref[%d],jnt_tau_act[%d],jnt_enabled[%d],",
                                 loop,loop,loop,loop,loop,loop,loop);
                        strcat(pTmpBufAll, pTmpBuf);
                    }
                    else
                    {
                        snprintf(pTmpBuf,DATA_BUF_SIZE,
                                 "%lf,%lf,%lf,%lf,%lf,%lf,%d,",
                                 jnt_cmd_log[loop].DATA.pos_ref,
                                 jnt_state_log[loop].DATA.pos_act,
                                 jnt_cmd_log[loop].DATA.vel_ref,
                                 jnt_state_log[loop].DATA.vel_act,
                                 jnt_cmd_log[loop].DATA.tau_ref,
                                 jnt_state_log[loop].DATA.tau_act,
                                 jnt_state_log[loop].DATA.enabled);
                        OsStrncpy((pTmpBufAll + OsStrlen(pTmpBufAll)), pTmpBuf,
                                  ((DATA_BUF_ALLSIZE - 1) - OsStrlen(pTmpBufAll)));
                    }
                }
                for(loop=0;loop<(HYDRA_HAND_JNT_MAX);loop++)
                {
                    if(flg_NewLabel==EC_TRUE)
                    {
                        snprintf(pTmpBuf,DATA_BUF_SIZE,
                                 "jnt_pos_act[%d],jnt_vel_act[%d],",loop+HYDRA_JNT_MAX,loop+HYDRA_JNT_MAX);
                        strcat(pTmpBufAll, pTmpBuf);
                    }
                    else
                    {
                        snprintf(pTmpBuf,DATA_BUF_SIZE,
                                 "%lf,%lf,",
                                 jnt_state_log[loop].DATA.pos_act,
                                 jnt_state_log[loop].DATA.vel_act);
                        OsStrncpy((pTmpBufAll + OsStrlen(pTmpBufAll)), pTmpBuf,
                                  ((DATA_BUF_ALLSIZE - 1) - OsStrlen(pTmpBufAll)));
                    }
                }
                for(loop=0;loop<EHA_MAX;loop++)
                {
                    if(flg_NewLabel==EC_TRUE)
                    {
                        snprintf(pTmpBuf,DATA_BUF_SIZE,
                                 "eha_pos_ref[%d],eha_pos_act[%d],eha_vel_ref[%d],eha_vel_act[%d],eha_rawpos_act[%d],eha_tau_ref[%d],eha_tau_act[%d],eha_tau2_act[%d],eha_tau3_act[%d],eha_ctrlwd[%d],eha_stswd[%d],",
                                 loop,loop,loop,loop,loop,loop,loop,loop,loop,loop,loop);
                        strcat(pTmpBufAll, pTmpBuf);
                    }
                    else
                    {
                        snprintf(pTmpBuf,DATA_BUF_SIZE,
                                 "%lf,%lf,%lf,%lf,%d,%lf,%lf,%lf,%lf,%d,%d,",
                                 eha_cmd_log[loop].DATA.pos_ref,
                                 eha_state_log[loop].DATA.pos_act,
                                 eha_cmd_log[loop].DATA.vel_ref,
                                 eha_state_log[loop].DATA.vel_act,
                                 eha_state_log[loop].DATA.rawpos_act,
                                 eha_cmd_log[loop].DATA.tau_ref,
                                 eha_state_log[loop].DATA.tau_act,
                                 eha_state_log[loop].DATA.tau2_act,
                                 eha_state_log[loop].DATA.tau3_act,
                                 eha_cmd_log[loop].DATA.ctlword,
                                 eha_state_log[loop].DATA.stsword);
                        OsStrncpy((pTmpBufAll + OsStrlen(pTmpBufAll)), pTmpBuf,
                                  ((DATA_BUF_ALLSIZE - 1) - OsStrlen(pTmpBufAll)));
                    }
                }

                // 改行 add
                strcat(pTmpBufAll, "\n");
                OsFwrite( pTmpBufAll, OsStrlen(pTmpBufAll), 1, fpPdo);

                if(flg_NewLabel == EC_TRUE)
                {
                    // labelflg off
                    flg_NewLabel = EC_FALSE;
                }
                else
                {
                    // indexを更新
                    sPutIdx++;
                    sPutIdx %= SHM_ACCNUM_MAX;
                }
            } while(sPutIdx != pShmServer->GetShmInIdx());

            // IO負荷の多いフラッシュはまとめて一気に行う
            OsFflush(fpPdo);
        }

        OsSleep(20);

    } while( !pThreadParam->bThreadShutdown );

    OsFclose(fpPdo);

    if( EC_NULL != pTmpBuf) {
        OsFree(pTmpBuf);
        pTmpBuf = EC_NULL;
    }

    if( EC_NULL != pTmpBufAll) {
        OsFree(pTmpBufAll);
        pTmpBufAll = EC_NULL;
    }
    pThreadParam->bThreadRunning = EC_FALSE;

    return;
}

/*
static EC_T_VOID tLogSaveTask_jnt_tau_calib( EC_T_VOID* pvThreadParamDesc )
{
    T_THREAD_PARAM* 		pThreadParam  		= (T_THREAD_PARAM*)pvThreadParamDesc;
    EC_T_DWORD				Remain;
    EC_T_CPUSET         	CpuSet;
    EC_T_INT				loop;

    //T_SHM_INPUT_ACC_MD4KW	*pAccIn 			= EC_NULL;
    //T_SHM_OUTPUT_ACC_MD4KW	*pAccOut 			= EC_NULL;

    static EC_T_INT			sPutIdx 			= 0;
    FILE					*fpPdo 				= EC_NULL;
    EC_T_CHAR				*pTmpBuf			= EC_NULL;
    EC_T_CHAR				*pTmpBufAll			= EC_NULL;
    T_ALL_SLAVE_INFO*		pAllSlv				= EC_NULL;

    struct tm           	*local;
    time_t              	timer;
    EC_T_CHAR           	datetime[256];
    EC_T_CHAR           	fname_pdo[256];
    static int kotemp;

    joint_state_t  jnt_state_log[HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX];
    joint_cmd_t    jnt_cmd_log[HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX];
    eha_state_t    eha_state_log[EHA_MAX];
    eha_cmd_t      eha_cmd_log[EHA_MAX];
    sensor_state_t sensor_state_log[1];
    sensor_cmd_t   sensor_cmd_log[1];

    EC_CPUSET_ZERO( CpuSet );
    EC_CPUSET_SET( CpuSet, (pThreadParam->dwCpuIndex +2));
    OsSetThreadAffinity( EC_NULL, CpuSet );

    pAllSlv  = (T_ALL_SLAVE_INFO*)pThreadParam->pvApplSpecParm1;

    OsMemset(fname_pdo, 	0, 	sizeof(fname_pdo));
    OsMemset(datetime, 		0, 	sizeof(datetime));

    // 最初にOPENするファイルだけ、従来どおりのファイル名とする(gnuplotで使用する関係上)
    snprintf(fname_pdo,	256, "%s%s", "pdo", 	FILE_EXT);
    fpPdo	= OsFopen( fname_pdo, 	"w+" );

    // いずれかでもOPENに失敗した場合
    if(fpPdo == EC_NULL) {
        printf("\n\n!!file open NG!!\n\n");
        pThreadParam->bThreadRunning = EC_FALSE;
        return;
    }

    // root権限でfopenしているため、アクセス権限の設定をしなおす
    chmod(fname_pdo, 	S_IRUSR|S_IWUSR| S_IRGRP|S_IWGRP| S_IROTH|S_IWOTH);

    pTmpBuf 	= (EC_T_CHAR*)OsMalloc(DATA_BUF_SIZE);
    pTmpBufAll  = (EC_T_CHAR*)OsMalloc(DATA_BUF_ALLSIZE);
    if((pTmpBuf == EC_NULL) || (pTmpBufAll == EC_NULL))
    {
        printf("\n\n!! Malloc NG!!\n\n");
        pThreadParam->bThreadRunning = EC_FALSE;
        return;
    }

    // 各Slaveの状態取得が完了するまで待つ
    OsWaitForEvent(pThreadParam->pvTimingEvent, EC_WAITINFINITE);
    pThreadParam->bThreadRunning = EC_TRUE;

    do {
        // もし、新規ファイル指定がされている場合
        if(flg_NewFile == EC_TRUE)
        {

            //現ファイルをクローズ
            if (OsFclose(fpPdo) == EOF)  {
                printf("\n\n!!file close NG!!\n\n");
                pThreadParam->bThreadRunning = EC_FALSE;
                break;
            }

            OsMemset(fname_pdo, 	0, 	sizeof(fname_pdo));
            OsMemset(datetime, 		0, 	sizeof(datetime));

            //現在の日付、時刻を取得
            time(&timer);
            local = localtime(&timer);
            snprintf(datetime, 256, "_%04d%02d%02d_%02d%02d%02d",
                     local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

            //ファイルを再オープン
            snprintf(fname_pdo,	256, "%s%s%s", "pdo",		datetime,	FILE_EXT);
            fpPdo	= OsFopen( fname_pdo, 	"w+" );
            if(fpPdo == EC_NULL) {
                printf("\n\n!!file open NG!!\n\n");
                pThreadParam->bThreadRunning = EC_FALSE;
                break;
            }

            // root権限でfopenしているため、アクセス権限の設定をしなおす
            chmod(fname_pdo, 	S_IRUSR|S_IWUSR| S_IRGRP|S_IWGRP| S_IROTH|S_IWOTH);

            //初期化
            sPutIdx 	   	= pShmServer->GetShmInIdx();
            flg_NewFile 	= EC_FALSE;
            flg_NewLabel   	= EC_TRUE;
            flg_FileUpdate 	= EC_TRUE;
        }

        // もし書き込み停止が指定されている場合
        if(flg_FileUpdate == EC_FALSE)
        {
            sPutIdx = pShmServer->GetShmInIdx();
        }

        //
        // PDOデータの保存処理
        //
        //
        if(sPutIdx != pShmServer->GetShmInIdx())
        {
            do {

                // 共有メモリの参照Indexから、対象面をまるごとポインタで参照する
//                pAccIn  = &pShmIn_MD4KW->Acc[sPutIdx];
//                pAccOut = &pShmOut_MD4KW->Acc[sPutIdx];

                OsMemset(pTmpBuf, 		0, DATA_BUF_SIZE);
                OsMemset(pTmpBufAll, 	0, DATA_BUF_ALLSIZE);

                // msec set
                if(flg_NewLabel == EC_TRUE)
                    snprintf(pTmpBufAll, DATA_BUF_SIZE, "time,");
                else
                    snprintf(pTmpBufAll, DATA_BUF_SIZE, "%d,", pShmServer->GetTimeInfo(sPutIdx));


                pShmServer->ReadStatus(sPutIdx,jnt_state_log,eha_state_log,sensor_state_log);
                pShmServer->ReadCommand(sPutIdx,jnt_cmd_log,eha_cmd_log,sensor_cmd_log);

                for(loop=20;loop<=22;loop++) // rwrist
                {
                    Remain = DATA_BUF_ALLSIZE - OsStrlen(pTmpBufAll);
                    if(Remain <= 1) break;

                    if(flg_NewLabel==EC_TRUE)
                    {
                        snprintf(pTmpBuf,DATA_BUF_SIZE,
                                 "jnt_pos[%d],jnt_tau1[%d],jnt_tau2[%d],jnt_tau3[%d],",
                                 loop,loop,loop,loop);
                        strcat(pTmpBufAll, pTmpBuf);
                    }
                    else
                    {
                        snprintf(pTmpBuf,DATA_BUF_SIZE,
                                 "%d,%lf,%lf,%lf,",clock_kang,
                                 //jnt_state_log[loop].DATA.pos_act,
                                 jnt_state_log[loop].DATA.tau_act, //from pressure sensor
                                 jnt_state_log[loop].DATA.tau2_act, //from strain gauge
                                 jnt_state_log[loop].DATA.tau3_act); //from complementary filter
                        OsStrncpy((pTmpBufAll + OsStrlen(pTmpBufAll)), pTmpBuf,
                                  ((DATA_BUF_ALLSIZE - 1) - OsStrlen(pTmpBufAll)));
                    }
                }

                for(loop=25;loop<=27;loop++)
                {
                    if(flg_NewLabel==EC_TRUE)
                    {
                        snprintf(pTmpBuf,DATA_BUF_SIZE,
                                 "eha_pos[%d],eha_tau1[%d],eha_tau2[%d],eha_tau3[%d],",
                                 loop,loop,loop,loop);
                        strcat(pTmpBufAll, pTmpBuf);
                    }
                    else
                    {
                        snprintf(pTmpBuf,DATA_BUF_SIZE,
                                 "%lf,%lf,%lf,%lf,",
                                 eha_state_log[loop].DATA.pos_act,
                                 eha_state_log[loop].DATA.tau_act,
                                 eha_state_log[loop].DATA.tau2_act,
                                 eha_state_log[loop].DATA.tau3_act
                                 );

                        OsStrncpy((pTmpBufAll + OsStrlen(pTmpBufAll)), pTmpBuf,
                                  ((DATA_BUF_ALLSIZE - 1) - OsStrlen(pTmpBufAll)));
                    }
                }
                kotemp++;


                // 改行 add
                strcat(pTmpBufAll, "\n");
                OsFwrite( pTmpBufAll, OsStrlen(pTmpBufAll), 1, fpPdo);

                if(flg_NewLabel == EC_TRUE)
                {
                    // labelflg off
                    flg_NewLabel = EC_FALSE;
                }
                else
                {
                    // indexを更新
                    sPutIdx++;
                    sPutIdx %= SHM_ACCNUM_MAX;
                }
            } while(sPutIdx != pShmServer->GetShmInIdx());

            // IO負荷の多いフラッシュはまとめて一気に行う
            OsFflush(fpPdo);
        }

        OsSleep(20);

    } while( !pThreadParam->bThreadShutdown );

    OsFclose(fpPdo);

    if( EC_NULL != pTmpBuf) {
        OsFree(pTmpBuf);
        pTmpBuf = EC_NULL;
    }

    if( EC_NULL != pTmpBufAll) {
        OsFree(pTmpBufAll);
        pTmpBufAll = EC_NULL;
    }
    pThreadParam->bThreadRunning = EC_FALSE;

    return;
}
*/

static EC_T_VOID tLogSaveTask_dbg( EC_T_VOID* pvThreadParamDesc )
{
    T_THREAD_PARAM* 		pThreadParam  		= (T_THREAD_PARAM*)pvThreadParamDesc;
    EC_T_DWORD				Remain;
    EC_T_CPUSET         	CpuSet;
    EC_T_INT				loop;

    //T_SHM_INPUT_ACC_MD4KW	*pAccIn 			= EC_NULL;
    //T_SHM_OUTPUT_ACC_MD4KW	*pAccOut 			= EC_NULL;

    static EC_T_INT			sPutIdx 			= 0;
    //static EC_T_INT			sPutIdx2 			= 0;
    FILE					*fpPdo 				= EC_NULL;
    EC_T_CHAR				*pTmpBuf			= EC_NULL;
    EC_T_CHAR				*pTmpBufAll			= EC_NULL;
    T_ALL_SLAVE_INFO*		pAllSlv				= EC_NULL;

    struct tm           	*local;
    time_t              	timer;
    EC_T_CHAR           	datetime[256];
    EC_T_CHAR           	fname_pdo[256];
    static int kotemp;

    joint_state_t  jnt_state_log[HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX];
    joint_cmd_t    jnt_cmd_log[HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX];
    eha_state_t    eha_state_log[EHA_MAX];
    eha_cmd_t      eha_cmd_log[EHA_MAX];
    sensor_state_t sensor_state_log[1];
    sensor_cmd_t   sensor_cmd_log[1];

    EC_CPUSET_ZERO( CpuSet );
    EC_CPUSET_SET( CpuSet, (pThreadParam->dwCpuIndex +2));
    OsSetThreadAffinity( EC_NULL, CpuSet );

    pAllSlv  = (T_ALL_SLAVE_INFO*)pThreadParam->pvApplSpecParm1;

    OsMemset(fname_pdo, 	0, 	sizeof(fname_pdo));
    OsMemset(datetime, 		0, 	sizeof(datetime));

    // 最初にOPENするファイルだけ、従来どおりのファイル名とする(gnuplotで使用する関係上)
    snprintf(fname_pdo,	256, "%s%s", "pdo", 	FILE_EXT);
    fpPdo	= OsFopen( fname_pdo, 	"w+" );

    // いずれかでもOPENに失敗した場合
    if(fpPdo == EC_NULL) {
        printf("\n\n!!file open NG!!\n\n");
        pThreadParam->bThreadRunning = EC_FALSE;
        return;
    }

    // root権限でfopenしているため、アクセス権限の設定をしなおす
    chmod(fname_pdo, 	S_IRUSR|S_IWUSR| S_IRGRP|S_IWGRP| S_IROTH|S_IWOTH);

    pTmpBuf 	= (EC_T_CHAR*)OsMalloc(DATA_BUF_SIZE);
    pTmpBufAll  = (EC_T_CHAR*)OsMalloc(DATA_BUF_ALLSIZE);
    if((pTmpBuf == EC_NULL) || (pTmpBufAll == EC_NULL))
    {
        printf("\n\n!! Malloc NG!!\n\n");
        pThreadParam->bThreadRunning = EC_FALSE;
        return;
    }

    // 各Slaveの状態取得が完了するまで待つ
    OsWaitForEvent(pThreadParam->pvTimingEvent, EC_WAITINFINITE);
    pThreadParam->bThreadRunning = EC_TRUE;

    do {
        // もし、新規ファイル指定がされている場合
        if(flg_NewFile == EC_TRUE)
        {

            //現ファイルをクローズ
            if (OsFclose(fpPdo) == EOF)  {
                printf("\n\n!!file close NG!!\n\n");
                pThreadParam->bThreadRunning = EC_FALSE;
                break;
            }

            OsMemset(fname_pdo, 	0, 	sizeof(fname_pdo));
            OsMemset(datetime, 		0, 	sizeof(datetime));

            //現在の日付、時刻を取得
            time(&timer);
            local = localtime(&timer);
            snprintf(datetime, 256, "_%04d%02d%02d_%02d%02d%02d",
                     local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);

            //ファイルを再オープン
            snprintf(fname_pdo,	256, "%s%s%s", "pdo",		datetime,	FILE_EXT);
            fpPdo	= OsFopen( fname_pdo, 	"w+" );
            if(fpPdo == EC_NULL) {
                printf("\n\n!!file open NG!!\n\n");
                pThreadParam->bThreadRunning = EC_FALSE;
                break;
            }

            // root権限でfopenしているため、アクセス権限の設定をしなおす
            chmod(fname_pdo, 	S_IRUSR|S_IWUSR| S_IRGRP|S_IWGRP| S_IROTH|S_IWOTH);

            //初期化
            sPutIdx 	   	= pShmServer->GetShmInIdx();
            flg_NewFile 	= EC_FALSE;
            flg_NewLabel   	= EC_TRUE;
            flg_FileUpdate 	= EC_TRUE;
        }

        // もし書き込み停止が指定されている場合
        if(flg_FileUpdate == EC_FALSE)
        {
            sPutIdx = pShmServer->GetShmInIdx();
        }

        //
        // PDOデータの保存処理
        //
        //
        if(sPutIdx != pShmServer->GetShmInIdx())
        {
            do {

                // 共有メモリの参照Indexから、対象面をまるごとポインタで参照する
//                pAccIn  = &pShmIn_MD4KW->Acc[sPutIdx];
//                pAccOut = &pShmOut_MD4KW->Acc[sPutIdx];

                OsMemset(pTmpBuf, 		0, DATA_BUF_SIZE);
                OsMemset(pTmpBufAll, 	0, DATA_BUF_ALLSIZE);

                // msec set
                if(flg_NewLabel == EC_TRUE)
                    snprintf(pTmpBufAll, DATA_BUF_SIZE, "time,");
                else
                    snprintf(pTmpBufAll, DATA_BUF_SIZE, "%d,", pShmServer->GetTimeInfo(sPutIdx));
                    //snprintf(pTmpBufAll, DATA_BUF_SIZE, "%d,", pShmServer->GetTimeInfo());


                pShmServer->ReadStatus(sPutIdx,jnt_state_log,eha_state_log,sensor_state_log);
                pShmServer->ReadCommand(sPutIdx,jnt_cmd_log,eha_cmd_log,sensor_cmd_log);

                if(flg_NewLabel == EC_TRUE)
                    snprintf(pTmpBuf,DATA_BUF_SIZE,"ctrl_wd,tau11,tau12,tau13,tau21,tau22,tau23,vel2,vel3,pos1,pos2,");
                else
                    snprintf(pTmpBuf,DATA_BUF_SIZE,
                            "%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%d,%d,",
                            eha_state_log[32].DATA.stsword,
                            eha_state_log[32].DATA.tau_act,
                            eha_state_log[32].DATA.tau2_act,
                            eha_state_log[32].DATA.tau3_act,
                            eha_state_log[33].DATA.tau_act,
                            eha_state_log[33].DATA.tau2_act,
                            eha_state_log[33].DATA.tau3_act,
                            eha_state_log[32].DATA.vel_act,
                            eha_state_log[33].DATA.vel_act,
                            eha_state_log[32].DATA.rawpos_act,
                            //eha_cmd_log[32].DATA.rawpos_ref
                            eha_state_log[33].DATA.rawpos_act
                         );
                OsStrncpy((pTmpBufAll + OsStrlen(pTmpBufAll)), pTmpBuf,
                          ((DATA_BUF_ALLSIZE - 1) - OsStrlen(pTmpBufAll)));


                // 改行 add
                strcat(pTmpBufAll, "\n");
                OsFwrite( pTmpBufAll, OsStrlen(pTmpBufAll), 1, fpPdo);

                if(flg_NewLabel == EC_TRUE)
                {
                    // labelflg off
                    flg_NewLabel = EC_FALSE;
                }
                else
                {
                    // indexを更新
                    sPutIdx++;
                    sPutIdx %= SHM_ACCNUM_MAX;
                }
            } while(sPutIdx != pShmServer->GetShmInIdx());

            // IO負荷の多いフラッシュはまとめて一気に行う
            OsFflush(fpPdo);
        }
        OsSleep(20);

    } while( !pThreadParam->bThreadShutdown );

    OsFclose(fpPdo);

    if( EC_NULL != pTmpBuf) {
        OsFree(pTmpBuf);
        pTmpBuf = EC_NULL;
    }

    if( EC_NULL != pTmpBufAll) {
        OsFree(pTmpBufAll);
        pTmpBufAll = EC_NULL;
    }
    pThreadParam->bThreadRunning = EC_FALSE;

    return;
}


EC_T_VOID ClipEHAPos(eha_state_t *state, EC_T_DWORD offset, const unsigned int llim, const unsigned int ulim, const double phys_conv)
{
    if(state->DATA.rawpos_act > ulim){
        state->DATA.pos_act = ((EC_T_INT)ulim-(EC_T_INT)offset)*phys_conv;
    }else if(state->DATA.rawpos_act < llim){
        state->DATA.pos_act = ((EC_T_INT)llim-(EC_T_INT)offset)*phys_conv;
    }else{
        state->DATA.pos_act = ((EC_T_INT)state->DATA.rawpos_act-(EC_T_INT)offset)*phys_conv;
    }

}

/*-END OF SOURCE FILE--------------------------------------------------------*/

