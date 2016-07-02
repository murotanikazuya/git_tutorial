/*-----------------------------------------------------------------------------
 * DCDemoMain.cpp            
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master demo main entrypoint
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include <AtEthercat.h>
#include <EcCommon.h>

#include "DCDemo.h"
#include "selectLinkLayer.h"
#include "Logging.h"

#if (defined WIN32)
 #if (defined UNDER_CE)
  #include "pkfuncs.h"
  #include "nkintr.h"
  #if (_WIN32_WCE >= 0x600)
   #if (defined ECWIN_CE)
    #include <CeWin.h>
    #include <RtosLib.h>
   #else
    #include <AuxClk.h>
   #endif
   #include <VirtIO.h>
  #endif

 #elif (defined RTOS_32)
   #ifdef ECWIN_RTOS32
    #include <Rtvmf.h>
    #include <rtk32.h>
    /* STRICT and _CRT_SECURE_NO_WARNINGS will be set again by Rteos.h included by vmfInterface.h */
    #if !(defined _RTEOS_H)
      #undef STRICT
      #undef _CRT_SECURE_NO_WARNINGS
    #endif /* !_RTEOS_H */
    #include <vmfInterface.h>
    #include <RtosLib.h>

    #ifdef EC_SOCKET_IP_SUPPORTED
     /* Socket.h defines INCLUDE_RTIP when available */
     #if INCLUDE_RTIP
      #pragma comment(lib, "Rtip.lib")
      #pragma comment(lib, "Netvmf.lib")
     #else
      #include <RtipDummy.cpp>
     #endif /* INCLUDE_RTIP */
    #endif /* EC_SOCKET_IP_SUPPORTED */
  #else
      #include <rttarget.h>
      #include <rtk32.h>
      #include <clock.h>
	  #include <rttacpi.h>
	  #include <rttsmbios.h>
      #include <timer.h>
	  #include <rtfiles.h>
  #endif
  #if (defined ATEMRAS_SERVER) && (defined EC_SOCKET_IP_SUPPORTED) && (defined INCLUDE_RTIP)
    #include <NetRTOS32Init.cpp>
  #endif
 #else /* WINDOWS, RTX, ... */
  #include <warn_dis.h>
  #include <windows.h>
  #include <tchar.h>
  #define TCHAR_DEFINED
  #include <warn_ena.h>
  #endif
#endif /* WIN32 */

#if (defined VXWORKS)
 #include <vxWorks.h>
 #include <sysLib.h>
 #include <tickLib.h>
#if ( ((defined _WRS_VXWORKS_MAJOR) && (_WRS_VXWORKS_MAJOR >= 6)) && ((defined _WRS_VXWORKS_MINOR) && (_WRS_VXWORKS_MINOR >= 5)) )
#else
 #include <ifLib.h>
#endif
#endif /* VXWORKS */

#if (defined LINUX)
 #include <sys/mman.h>
 #include <sys/utsname.h>
 #include <signal.h>
#endif /* LINUX */

#if defined __INTEGRITY
 #include <unistd.h>
#endif

#if (defined __RCX__)
 #include "tlr_includes.h"     /* TLR includes */
 #include "AtemDemo_Functionlist.h"
#endif

#include <stdio.h>
#include <stdlib.h>

/*-DEFINES-------------------------------------------------------------------*/

#define LogMsg      oLogging.LogMsg
#define LogError    oLogging.LogError

#define COMMAND_LINE_BUFFER_LENGTH 512
#define LINK_LAYER_PARAMETERS_LENGTH 256

/* For operating systems don't supporting dynamic libraries, the link layers */
/* are linked statically to the application. Select the link layer(s) here   */
#if (defined RTAI)
  #define LINKLAYER_I8254X
#endif
#if (defined RTOS_32) || (defined __INTEGRITY) || (defined __TKERNEL)
  #define LINKLAYER_I8254X
//#define LINKLAYER_I8255X
//#define LINKLAYER_RTL8139
//#define LINKLAYER_RTL8169
#endif
#if (defined ECWIN_RTOS32)
/* all link layers are available */
  #define LINKLAYER_I8255X
  #define LINKLAYER_RTL8139
  #define LINKLAYER_RTL8169
#endif
#if (defined __RCX__)
  #define LINKLAYER_HNX
#endif

#if (defined LINUX)
#define NSEC_PER_SEC (1000000000)
#endif /* LINUX */
#if (defined RTAI)
extern "C" long int G_dwBusCycleTimeUsec;
/** created from MasterENI.xml with "objcopy -B i386 -I binary -O elf32-i386 MasterENI.xml MasterENI.o" */
extern EC_T_BYTE MasterENI_xml_data[]      asm("_binary_MasterENI_xml_start");
extern EC_T_BYTE MasterENI_xml_data_size[] asm("_binary_MasterENI_xml_size");
extern EC_T_BYTE MasterENI_xml_data_end[]  asm("_binary_MasterENI_xml_end");
#define STATIC_MASTERENI_XML_DATA          MasterENI_xml_data
#define STATIC_MASTERENI_XML_DATA_SIZE     (size_t)((EC_T_VOID*)MasterENI_xml_data_size)
#endif /* RTAI */

#if (defined EC_VERSION_WINDOWS) || (defined VXWORKS) || ((defined UNDER_CE) && (_WIN32_WCE >= 0x600)) || (defined UNDER_RTSS) || (defined STARTERWARE_NOOS)
#define AUXCLOCK_SUPPORTED
#endif

/*-TYPEDEFS------------------------------------------------------------------*/
#ifndef TCHAR_DEFINED
typedef char TCHAR;
#endif

typedef struct _EC_T_TIMING_DESC
{
    EC_T_VOID*          pvTimingEvent;      /* event handle */
    EC_T_DWORD          dwBusCycleTimeUsec; /* cycle time in usec */
    EC_T_BOOL           bShutdown;          /* EC_TRUE if aux thread shall shut down */
    EC_T_BOOL           bIsRunning;         /* EC_TRUE if the aux thread is running */
#if (defined AUXCLOCK_SUPPORTED)
    EC_T_BOOL           bUseAuxClock;       /* Either connect to IRQ or use sleep */
    EC_T_VOID*          pvAuxClkEvent;      /* event handle */
#endif
    EC_T_DWORD          dwCpuIndex;         /* SMP systems: CPU index */
#if ((defined UNDER_CE) && (_WIN32_WCE >= 0x600))
    HANDLE              hVirtualDrv;        /* virtual IO driver handle */
    VI_T_INTERRUPTDESC  oIrqDesc;           /* Irq descriptor for virtual IO driver */
#endif
} EC_T_TIMING_DESC;

/*-GLOBAL VARIABLES-----------------------------------------------------------*/
volatile EC_T_BOOL bRun = EC_TRUE;

/*-LOCAL FUNCTIONS-----------------------------------------------------------*/
#if (defined WIN32) && !(defined UNDER_CE)
/********************************************************************************/
/** \brief  CTRL-C handler.
*
* \return N/A
*/
static BOOL WINAPI win32CtrlCHandler(DWORD dwCtrlType)
{
    UNREFERENCED_PARAMETER( dwCtrlType );

    bRun = EC_FALSE;
    return TRUE;        // don't handle events
}
#endif /* WIN32 && !UNDER_CE */

#if (defined VXWORKS)
/********************************************************************************/
/** \brief  Change VxWorks task priority.
*
* \return  N/A.
*/
EC_T_VOID vxTaskPrioChange( EC_T_CHAR* szTaskName )
{
int nTid, nPrio;
    nTid = taskNameToId( szTaskName );
    if( nTid != ERROR )
    {
        taskPriorityGet( nTid, &nPrio );
        if( nPrio < 10 )
        {
            taskPrioritySet( nTid, nPrio + 10 );
        }
    }
}
/********************************************************************************/
/** \brief  VxWorks auxiliary clock ISR.
*
* \return  N/A.
*/
EC_T_VOID vxAuxClkIsr( EC_T_INT nEvent )
{
    EC_T_VOID* pvEvent = (EC_T_VOID*)nEvent;
    if( EC_NULL != pvEvent )
    {
        OsSetEvent(pvEvent);
    }
}
#endif /* VXWORKS */

#if (defined RTOS_32)
extern "C" int __rttMaxPCIBusses;
extern "C" {
    RTK32Config RTKConfig = {
            sizeof(RTK32Config),           // StructureSize
#if MP
            DF_IDLE_HALT |                 // DriverFlags (MP kernel)
#endif
            0,                             // DriverFlags
            0,                             // UserDriverFlags
            RF_PREEMPTIVE |                // preemptive
            RF_AUTOINIT | RF_NAMED_WIN32CS,// Flags
            16*1024,                       // DefaultTaskStackSize
#if MP
            1024,                          // DefaultIntStackSize
#else
            512,                           // DefaultIntStackSize
#endif
            5,                             // MainPriority
            0,                             // DefaultPriority
            0,                             // HookedInterrupts (none)
            256,                           // TaskStackOverhead
            0                              // TimeSlice (0 == off)
    } ;
#ifdef _MSC_VER
extern "C" __declspec(dllexport) void          Init(void)
#else
extern "C"                       void __export Init(void)
#endif
{
   __rttMaxPCIBusses = 256;
   RTSetFlags(RT_MM_VIRTUAL | RT_CLOSE_FIND_HANDLES, 1);
#ifdef ECWIN_RTOS32
   RTVmfInit();
   RTVmfExtendHeap();
#else
   RTCMOSExtendHeap();
#endif
}
}
#endif /* RTOS_32 */

#if (defined LINUX) && !(defined RTAI)
/********************************************************************************/
/** \brief  signal handler.
*
* \return N/A
*/
static void SignalHandler( int nSignal )
{
    bRun = EC_FALSE;
}
/********************************************************************************/
/** Enable real-time environment
*
* Return: EC_E_NOERROR in case of success, EC_E_ERROR in case of failure.
*/
EC_T_DWORD EnableRealtimeEnvironment( EC_T_VOID )
{
   struct utsname SystemName;
   int nMaj, nMin, nSub;
   struct timespec ts;
   int nRetval;
   EC_T_DWORD dwResult = EC_E_ERROR;
   EC_T_BOOL bHighResTimerAvail;
   struct sched_param schedParam;

   /* master only tested on >= 2.6 kernel */
   nRetval = uname( &SystemName );
   if( nRetval != 0)
   {
      OsDbgMsg( "ERROR calling uname(), required Linux kernel >= 2.6\n" );
      dwResult = EC_E_ERROR;
      goto Exit;
   }
   sscanf( SystemName.release, "%d.%d.%d", &nMaj, &nMin, &nSub );
   if(!
        (  ((nMaj == 2) && (nMin == 6))  ||  (nMaj == 3)  )
     )
   {
      OsDbgMsg( "ERROR - detected kernel = %d.%d.%d, required Linux kernel >= 2.6\n", nMaj, nMin, nSub );
      dwResult = EC_E_ERROR;
      goto Exit;
   }

   /* request realtime scheduling for the current process
    * This value is overwritten for each individual task
    */
   schedParam.sched_priority = MAIN_THREAD_PRIO; /* 1 lowest priority, 99 highest priority */
   nRetval = sched_setscheduler( 0, SCHED_FIFO, &schedParam );
   if( nRetval == -1 )
   {
      printf( "ERROR - cannot change scheduling policy!\n"
                "root privilege is required or realtime group has to be joined!\n" );
      goto Exit;
   }

   /* disable paging */
   nRetval = mlockall( MCL_CURRENT | MCL_FUTURE );
   if( nRetval == -1 )
   {
      OsDbgMsg( "ERROR - cannot disable paging!\n" );
      dwResult = EC_E_ERROR;
      goto Exit;
   }

   /* check if high resolution timers are available */
   if( clock_getres(CLOCK_MONOTONIC, &ts) )
   {
      bHighResTimerAvail = EC_FALSE;
   }
   else
   {
      bHighResTimerAvail = !(ts.tv_sec != 0 || ts.tv_nsec != 1);
   }
   if( !bHighResTimerAvail )
   {
      OsDbgMsg( "WARNING: High resolution timers not available\n" );
   }

   /* set type of OsSleep implementation  (eSLEEP_USLEEP, eSLEEP_NANOSLEEP or eSLEEP_CLOCK_NANOSLEEP) */
   OsSleepSetType( eSLEEP_CLOCK_NANOSLEEP );
   //OsSleepSetType( eSLEEP_USLEEP );
   //OsSleepSetType( eSLEEP_NANOSLEEP );

   dwResult = EC_E_NOERROR;

Exit:
    return dwResult;
}
#endif /* LINUX && !RTAI */

#if (defined UNDER_RTSS)
VOID RTFCNDCL RtxAuxClkIsr(PVOID pvEvent)
{
    if( EC_NULL != pvEvent )
    {
        OsSetEvent(pvEvent);
    }
}
#endif

#if (defined ECWIN_RTOS32)
/*******************************************************************************
*
* RdDbgPortChar - Read character from Debug PORT
*
* Return: TRUE if character was available, FALSE if not.
*/
BOOL RdDbgPortChar( UINT8* pbyData )
{
UINT32 dwNumRead;

    vmfVioRead( 0, FALSE, pbyData, 1, &dwNumRead);
    return (dwNumRead == 1);
}

extern volatile EC_T_BOOL bRun;             /* global helper variable to shutdown the application */
/*-----------------------------------*/
#define CTRL_C  3
#define ESC    27
void tTerminationReq(void)
{
UINT8 byData;

    /* flush input first */
    while( RdDbgPortChar(&byData) )
    {
        Sleep(1);
    }
    for(;;)
    {
        Sleep(100);
        if( RdDbgPortChar(&byData) )
        {
            if( (byData == CTRL_C) || (byData == ESC) )
            {
                bRun = 0;
                break;
            }
        }
    }
}
#endif /* ECWIN_RTOS32 */

#if (defined ECWIN)
/*-----------------------------------*/
BOOL OpenRtosLibShm(CHAR* szShmName, UINT8** ppbyShm, UINT32* pdwShmSize)
{
BOOL   bRetVal       = FALSE;
UINT32 dwRes         = RTE_SUCCESS;
UINT32 dwShmId       = 0;
UINT32 dwTotalSize   = 0;
UINT8* pbyShm        = EC_NULL;
UINT32 dwGrantedSize = 0;

    dwRes = RtosGetIdByNameA(szShmName, RTOS_ID_SHM, &dwShmId);
    if (RTE_SUCCESS != dwRes)
    {
        goto Exit;
    }
    dwRes = RtosShmTotalSizeGet(dwShmId, &dwTotalSize);
    if (RTE_SUCCESS != dwRes)
    {
        goto Exit;
    }
    dwRes = RtosShmAddrGet(dwShmId, dwTotalSize, &dwGrantedSize, (VOID**)&pbyShm);
    if (RTE_SUCCESS != dwRes)
    {
        goto Exit;
    }
    if (dwTotalSize != dwGrantedSize)
    {
        goto Exit;
    }
    /* no errors */
    *pdwShmSize = dwTotalSize;
    *ppbyShm    = pbyShm;
    bRetVal     = TRUE;

Exit:
    return bRetVal;
}
#endif /* ECWIN */

/********************************************************************************/
/** Show syntax
*
* Return: N/A
*/
static EC_T_VOID ShowSyntax(EC_T_VOID)
{
    OsDbgMsg("Syntax:\n");
    OsDbgMsg("ECMasterDemoDc [-f ENI-FileName] [-t time] [-b time] [-a affinity] [-v lvl] [-perf] [-log Prefix]");
#if (defined AUXCLOCK_SUPPORTED)
    OsDbgMsg(" [-auxclk period]");
#endif
#if (defined ATEMRAS_SERVER)
    OsDbgMsg(" [-sp [port]]");
#endif
    ShowLinkLayerSyntax1();
    OsDbgMsg("   -f                Use given ENI file\n");
    OsDbgMsg("     FileName        file name .xml\n");
    OsDbgMsg("   -s                Use given Param file\n");
    OsDbgMsg("     FileName        file name .xml\n");
    OsDbgMsg("   -t                Demo duration\n");
    OsDbgMsg("     time            Time in msec, 0 = forever (default = 120000)\n");
    OsDbgMsg("   -b                Bus cycle time\n");
    OsDbgMsg("     cycle time      Cycle time in usec\n");
    OsDbgMsg("   -a                CPU affinity\n");
    OsDbgMsg("     affinity        0 = first CPU, 1 = second, ...\n");
    OsDbgMsg("   -v                Set verbosity level\n");
    OsDbgMsg("     lvl             Level: 0=off, 1..n=more messages, 3 generate dcmlog file (default)\n");
    OsDbgMsg("   -perf             Enable job measurement\n");
    OsDbgMsg("   -log              Use given file name prefix for log files\n");
    OsDbgMsg("     Prefix          prefix\n");
#if (defined AUXCLOCK_SUPPORTED)
    OsDbgMsg("   -auxclk           use auxiliary clock\n");
    OsDbgMsg("     period          clock period in usec\n" );
#endif
#if (defined ATEMRAS_SERVER)
    OsDbgMsg("   -sp               Server port binding\n");
    OsDbgMsg("     port            port (default = %d)\n", ATDS_CFG_DEFAULT_PORT);
#endif
    OsDbgMsg("   -nodcm            Disables the Master Sync Controller\n");

    ShowLinkLayerSyntax2();

    return;
}

#if !(defined RTAI)
/********************************************************************************/
/* \brief Set event according to periodical sleep or aux clock
 * Cyclically sets an event for thread synchronization purposes.
 * Either use OsSleep() or use the aux clock by means of:
 * - Enable AUX clock if selected.
 * - Wait for IRQ, aknowledge IRQ, SetEvent in loop until shutdown
 * - Disable AUX clock
 * Return: N/A
 */
static EC_T_VOID tEcTimingTask( EC_T_VOID* pvThreadParamDesc )
{
EC_T_TIMING_DESC* pTimingDesc = (EC_T_TIMING_DESC*)pvThreadParamDesc;
EC_T_CPUSET       CpuSet;
#if (defined LINUX)
    struct timespec t;
#endif /* LINUX */
#if (defined QNX6)
    struct _clockperiod oClockPeriod = {0};
#endif /* QNX6 */

    EC_CPUSET_ZERO( CpuSet );
    EC_CPUSET_SET( CpuSet, pTimingDesc->dwCpuIndex );
    OsSetThreadAffinity( EC_NULL, CpuSet );

#if (defined QNX6)
    /* Set the clock period to bus cycle time */
	oClockPeriod.nsec = pTimingDesc->dwBusCycleTimeUsec * 1000;
    if(ClockPeriod(CLOCK_REALTIME, &oClockPeriod, EC_NULL, 0) == -1)
    {
        OsPrintf("tEcTimingTask:: Cannot set the clock period! Error %i", errno);
    }
#endif /* QNX6 */

#if ((defined UNDER_CE) && (_WIN32_WCE >= 0x600))
    /* enable auxilary clock */
    if (pTimingDesc->bUseAuxClock)
    {
    DWORD dwAuxClkFreq = 1000000 / pTimingDesc->dwBusCycleTimeUsec;

        if (!KernelIoControl((DWORD)IOCTL_AUXCLK_ENABLE, &dwAuxClkFreq, sizeof(DWORD), NULL, 0, NULL))
        {
            OsDbgMsg("Error calling KernelIoControl(IOCTL_AUXCLK_ENABLE) (0x%08X)!\n", GetLastError());
            goto Exit;
        }
    }
#elif (defined LINUX)
    /* get current time */
    clock_gettime(CLOCK_MONOTONIC, &t);

    /* start after one second */
    t.tv_sec += 1;
#endif /* LINUX */

    /* timing task started */
    pTimingDesc->bIsRunning = EC_TRUE;

    /* periodically generate events as long as the application runs */
    while (!pTimingDesc->bShutdown)
    {
        /* wait for the next cycle */
#if (defined LINUX)
        /* Use the Linux high resolution timer. This API offers resolution
         * below the systick (i.e. 50us cycle is possible) if the Linux
         * kernel is patched with the RT-PREEMPT patch.
         */

        /* wait until next shot */
        if (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL) != 0)
        {
           perror("clock_nanosleep failed");
           pTimingDesc->bShutdown = EC_TRUE;
        }

        /* calculate next shot. t.tv_nsec is always < 1000000000 */
        t.tv_nsec += pTimingDesc->dwBusCycleTimeUsec * 1000;

        /* norm time */
        while (t.tv_nsec >= NSEC_PER_SEC)
        {
           t.tv_nsec -= NSEC_PER_SEC;
           t.tv_sec++;
        }
 /* LINUX */
#elif ((defined UNDER_CE) && (_WIN32_WCE >= 0x600))

        if (pTimingDesc->bUseAuxClock)
        {
            /* wait for auxclk event */
            OsWaitForEvent(pTimingDesc->pvAuxClkEvent, EC_WAITINFINITE);

            if (pTimingDesc->bShutdown)
            {
                /* disable auxilary clock */
                if (!KernelIoControl((DWORD)IOCTL_AUXCLK_DISABLE, NULL, 0, NULL, 0, NULL))
                {
                    OsDbgMsg("Error calling KernelIoControl(IOCTL_AUXCLK_DISABLE) (0x%08X)!\n", GetLastError());
                }
            }
            /* Acknowledge the IRQ */
            if (pTimingDesc->hVirtualDrv != NULL)
            {
                DeviceIoControl(pTimingDesc->hVirtualDrv, (DWORD)IOCTL_VIRTDRV_INTERRUPT_DONE, &(pTimingDesc->oIrqDesc), sizeof(VI_T_INTERRUPTDESC), NULL, 0, NULL, NULL );
            }
        }
        else
        {
            /* wait for next cycle */
            OsSleep(EC_MAX(pTimingDesc->dwBusCycleTimeUsec / 1000, 1));
        }
 /* UNDER_CE, _WIN32_WCE < 0x600 */
#elif (defined QNX6)
        {
            /* wait for next cycle */
            OsSleep(1);
        }
 /* QNX6 */
#else
        {
            /* wait for next cycle */
            OsSleep(EC_MAX(pTimingDesc->dwBusCycleTimeUsec / 1000, 1));
        }
#endif
        /* trigger jobtask */
        OsSetEvent( pTimingDesc->pvTimingEvent );
    }

#if ((defined UNDER_CE) && (_WIN32_WCE >= 0x600))
Exit:
#endif
    pTimingDesc->bIsRunning = EC_FALSE;

    return;
}
#endif /* !RTAI */

/********************************************************************************/
/** \brief  Demo Application entry point.
*
* \return  Value 0 is returned.
*/
#if (defined VXWORKS)
extern "C" int dcDemo(char* lpCmdLine)
#elif (defined __TKERNEL)
EXPORT ER main( INT nArgc, UB *ppArgv[] )
#elif (defined RTOS_32)
int main(void)
#elif (defined __RCX__)
TLR_RESULT dcDemo(TLR_VOID FAR* pvInit)
#elif (defined STARTERWARE_NOOS)
int ECMasterDemoDc(int nArgc, char* ppArgv[])
#elif (defined RTAI)
extern "C" void ECMasterDemoDcMain(long int t)
#elif (defined UNDER_CE)
int _tmain(int nArgc, _TCHAR* ppArgv[])
#else
/* QNX6, LINUX, __INTEGRITY, ... */
int main(int nArgc, char* ppArgv[])
#endif
{
    int                     nRetVal            = APP_ERROR;
    EC_T_DWORD              dwRes              = EC_E_ERROR;
    EC_T_BOOL               bLogInitialized    = EC_FALSE;
    EC_T_CHAR               szCommandLine[COMMAND_LINE_BUFFER_LENGTH];
    EC_T_CHAR               szFullCommandLine[COMMAND_LINE_BUFFER_LENGTH];
    EC_T_BOOL               bGetNextWord       = EC_TRUE;
    EC_T_CHAR*              ptcWord            = EC_NULL;
    EC_T_CHAR               tcStorage          = '\0';

    EC_T_CHAR               szLogFileprefix[256] = {'\0'};
    EC_T_CHAR               szParamFileprefix[256] = {'\0'};
    EC_T_CNF_TYPE           eCnfType           = eCnfType_Unknown;
    EC_T_PBYTE              pbyCnfData         = 0;
    EC_T_DWORD              dwCnfDataLen       = 0;
    EC_T_CHAR               szENIFilename[256] = {'\0'};
    EC_T_DWORD              dwDuration         = 120000;
    EC_T_DWORD              dwNumLinkLayer     = 0;
    EC_T_LINK_DEV_OPEN_PARAM arrLinkDevLinkOpenParam[MAX_LINKLAYER];
#ifdef ATEMRAS_SERVER
    EC_T_WORD               wServerPort = 0xFFFF;
#endif
    CAtEmLogging            oLogging;
    EC_T_DWORD              dwCpuIndex         = 0;
    EC_T_CPUSET             CpuSet;
    EC_T_BOOL               bEnaPerfJobs       = EC_FALSE;  /* enable job measurements */
    EC_T_TIMING_DESC        TimingDesc;
    EC_T_BOOL               bStartTimingTask   = EC_FALSE;
    EC_T_INT                nVerbose           = 3;
#if (defined UNDER_CE) && (_WIN32_WCE >= 0x600)
    BOOL                    bRes               = FALSE;
    DWORD                   dwAuxClkSysIntr    = 0;
    DWORD                   dwWinRes           = EC_E_ERROR;
#endif
#if (defined UNDER_RTSS)
    HANDLE                  hTimer             = NULL;
    LARGE_INTEGER           liTimer;
#endif
    EC_T_BOOL               bNoDcm             = EC_FALSE;

    OsMemset(arrLinkDevLinkOpenParam, 0, sizeof(arrLinkDevLinkOpenParam));
    OsMemset(&TimingDesc, 0, sizeof(TimingDesc));

    szCommandLine[0] = '\0';

    /* OS specific initialization */
#if (defined WIN32) && !(defined UNDER_CE)
    SetConsoleCtrlHandler( (PHANDLER_ROUTINE)win32CtrlCHandler, TRUE );
#endif

#if (defined RTOS_32)
#ifdef ECWIN_RTOS32
    RTKernelInit(0);
#else
    RTKernelInit(4);
    RTCMOSSetSystemTime();

    RTMPSetAPICMode(0);
    {
        UINT CPUClock         = RTSMBIOSGetCPUSpeed();
        UINT BusClockBIOS     = RTSMBIOSGetBusSpeed();
        UINT BusClockMeasured = RTMPBusFromCPUClock(CPUClock, 40000000);    // 40M, that's 1/100 of a second on a 4GHz machine

        OsDbgMsg("CPU Clock         : %uMHz\n""Bus Clock         : %uMHz\n""Bus Clock measured: %uMHz\n\n",
                    CPUClock,BusClockBIOS,BusClockMeasured);
    }

    /* workaround for IDE driver */
    RTMPSetIOAPICVector(0, 5, 5, 1, 1);

    /* prevent warning message "IDE driver in PIO mode */
    for(int nDevIndex = 0; nDevIndex < 8; nDevIndex++)
    {
        if(RTFDeviceList[nDevIndex].DeviceType == RTF_DEVICE_FDISK)
        {
            RTFDeviceList[nDevIndex].DeviceFlags |= RTF_DEVICE_NO_DIAG_MSG;
        }
        else if(RTFDeviceList[nDevIndex].DeviceType == 0)
        {
            break;
        }
    }
#endif
    RTKPreemptionsON();
#if (defined ATEMRAS_SERVER) && (defined EC_SOCKET_IP_SUPPORTED) && (defined INCLUDE_RTIP)
    NetInitialize();
#endif
#endif /* RTOS_32 */
#if (defined ECWIN_RTOS32)
    {
    void* pvReg = NULL;

        /* protect memory area at address 0 (detect NULL pointer access) */
        pvReg = NULL;
        RTReserveVirtualAddress(&pvReg, 0x1000, RT_MAP_NO_RELOCATE);

        /* start thread waiting for termination request */
        RTKRTLCreateThread( (RTKThreadFunction)tTerminationReq, 64, 0x1000, TF_NO_MATH_CONTEXT, NULL, "tTerminationReq" );

        printf( "you can stop the demo by typing CTRL-C or ESCAPE in the debug console\n");
    }
#endif /* ECWIN_RTOS32 */
#if (defined ECWIN)
    /* must be call before any RtosLib API call */
    RtosLibInit();
#endif /* ECWIN */

#if (defined VXWORKS)
    /* change VxWorks standard task priorities to optimize EtherCAT performance */
#ifndef VXWORKS_NORMAL_PRIO
    vxTaskPrioChange( "tJobTask" );
    vxTaskPrioChange( "tLogTask" );
    vxTaskPrioChange( "tNbioLog" );
    vxTaskPrioChange( "tShell" );
    vxTaskPrioChange( "tShell0" );
    vxTaskPrioChange( "tShellRem1" );
    vxTaskPrioChange( "tShellRem2" );
    vxTaskPrioChange( "tWdbTask" );
    vxTaskPrioChange( "tTelnetd" );
#endif
#if ( ((defined _WRS_VXWORKS_MAJOR) && (_WRS_VXWORKS_MAJOR >= 6)) && ((defined _WRS_VXWORKS_MINOR) && (_WRS_VXWORKS_MINOR >= 5)) )
    taskPrioritySet( taskNameToId("tNet0"), RECV_THREAD_PRIO );
#else
    taskPrioritySet( taskNameToId("tNetTask"), RECV_THREAD_PRIO );
#endif
    /* redirect outputs to target shell console device */
    int nTidTargetShell = taskNameToId("tShellRem1");
    if (nTidTargetShell != -1)
    {
        /* redirect all outputs at the target shell */
        ioGlobalStdSet (STD_OUT, ioTaskStdGet(nTidTargetShell,STD_OUT));
        ioGlobalStdSet (STD_ERR, ioTaskStdGet(nTidTargetShell,STD_ERR));
    }
    /* sysclk always set to 1msec */
    sysClkRateSet(1000);
#endif /* VXWORKS */

#if (defined LINUX) && !(defined RTAI)
    dwRes = EnableRealtimeEnvironment( );
    if( dwRes != EC_E_NOERROR )
    {
        goto Exit;
    }
    {
        sigset_t SigSet;
        int nSigNum = SIGALRM;
        sigemptyset( &SigSet );
        sigaddset( &SigSet, nSigNum );
        sigprocmask( SIG_BLOCK, &SigSet, NULL );
        signal( SIGINT, SignalHandler );
        signal( SIGTERM, SignalHandler );
    }
#endif /* LINUX && !RTAI */

#if (defined __INTEGRITY)
    WaitForFileSystemInitialization();
#endif

#if (defined __RCX__)
    ATEMDEMO_STARTUPPARAMETER_T* pStartup = ((ATEMDEMO_STARTUPPARAMETER_T*)pvInit);

    TCHAR* lpCmdLine = (TCHAR*)(pStartup->szParamList);
    lpCmdLine = (lpCmdLine==EC_NULL)?(TCHAR*)" ":lpCmdLine;
    tcStorage = *lpCmdLine;
#endif

#if !(defined UNDER_RTSS) && !(defined RTAI)
    srand((unsigned)OsQueryMsecCount());
#endif
    /* set running flag */
    bRun = EC_TRUE;

    /* add hook to log all EtherCAT messages. Without hook no messages will be generated! */
    OsAddDbgMsgHook( CAtEmLogging::OsDbgMsgHookWrapper );

    /* Initialize Timing Event descriptor */
    TimingDesc.bShutdown          = EC_FALSE;
    TimingDesc.bIsRunning         = EC_FALSE;
#if (defined RTAI)
    TimingDesc.dwBusCycleTimeUsec = G_dwBusCycleTimeUsec;
#else
    TimingDesc.dwBusCycleTimeUsec = CYCLE_TIME * 1000;
#endif

    /* prepare command line */
#if (defined VXWORKS)
    OsStrncpy(szCommandLine, lpCmdLine, sizeof(szCommandLine) - 1);
#elif (defined RTOS_32)
#if (defined ECWIN_RTOS32)
    {
    VMF_HANDLE hEcatKey;
    VMF_CONFIG_ADDDATA AddData;
    UINT32 dwLength = 0;

        dwRes = vmfConfigRegKeyOpenA(VMF_CONFIGREG_HKEY_OS_CURRENT, "Ecat", &hEcatKey);
        if( dwRes == RTE_SUCCESS )
        {

            dwLength = sizeof(szCommandLine);
            vmfConfigRegValueQueryA(hEcatKey, "CommandLine", NULL, NULL, (UINT8*)&szCommandLine[0], &dwLength );
            vmfConfigRegKeyClose(hEcatKey);
        }
        if ('\0' == szCommandLine[0])
        {
            /* for compatibility */
            dwLength = sizeof(szCommandLine);
            dwRes = vmfConfigQueryValue( "Ecat", "CommandLine", VMF_CONFIG_SZ_TYPE, (UINT8*)&szCommandLine[0], &dwLength, &AddData);
            if( dwRes != RTE_SUCCESS )
            {
                printf( "Cannot read EtherCAT demo command line, (EcatShm.config)\n" );
                printf( "Please, enter command line (e.g. atemDemo -v 2 -i8255x 1 1):\n" );
            }
        }
    }
#else
    OsStrncpy(szCommandLine, GetCommandLine(), sizeof(szCommandLine) - 1);
#endif /* !ECWIN_RTOS32 */
#elif (defined RTAI)
    OsStrncpy(szCommandLine, "-i8254x 1 1 -v 1 -t 15000 ", sizeof(szCommandLine) - 1);
#else
    /* build szCommandLine from argument list */
    {
    EC_T_CHAR* pcStrCur      = szCommandLine;
    EC_T_INT   nStrRemain    = COMMAND_LINE_BUFFER_LENGTH;
    EC_T_BOOL  bIsFileName   = EC_FALSE;
#if (defined UNDER_CE)
    EC_T_CHAR  szStrFormat[] = "%S"; /* convert UNICODE to multibyte */
#else
    EC_T_CHAR  szStrFormat[] = "%s";
#endif
        for (; nArgc > 0; nArgc--, ppArgv++)
        {
            /* insert next argument */
            OsSnprintf(pcStrCur, nStrRemain - 1, szStrFormat, *ppArgv);
            if (0 == OsStrcmp(pcStrCur, "-f"))
            {
                bIsFileName = EC_TRUE;
            }
            else
            {
                bIsFileName = EC_FALSE;
            }
            /* adjust string cursor */
            nStrRemain -= (EC_T_INT)OsStrlen(pcStrCur);
            pcStrCur = pcStrCur + OsStrlen(pcStrCur);

            /* insert space */
            OsStrncpy(pcStrCur, " ", nStrRemain - 1); nStrRemain--; pcStrCur++;
               
            if (bIsFileName && (1 < nArgc))
            {
                /* move to next arg (ENI file name) */
                nArgc--; ppArgv++;

                /* insert quotation mark */
                OsStrncpy(pcStrCur, "\"", nStrRemain - 1); nStrRemain--; pcStrCur++;

                /* insert ENI file name */                
                OsSnprintf(pcStrCur, nStrRemain - 1, szStrFormat, *ppArgv); nStrRemain -= (EC_T_INT)OsStrlen(pcStrCur);
                pcStrCur = pcStrCur + OsStrlen(pcStrCur);

                /* insert quotation mark */                
                OsStrncpy(pcStrCur, "\" ", nStrRemain - 1); nStrRemain--; pcStrCur++;
            }
        }
    }
#endif
    OsStrncpy(szFullCommandLine, szCommandLine, sizeof(szFullCommandLine) - 1);

    /* parse command line */
#if (defined VXWORKS) || (defined RTOS_32) || (defined __RCX__) || (defined RTAI)
    ptcWord = OsStrtok(szCommandLine, " ");
#else
    /* skip executable name */
    OsStrtok(szCommandLine, " ");
    ptcWord = OsStrtok(EC_NULL, " ");
#endif
    while (ptcWord != EC_NULL)
    {

        if( 0 == OsStricmp( ptcWord, "-f") )
        {
            EC_T_INT nPtcWordIndex = 3;

            /* Search for the start of the config file name. The config file
               name may start with quotation marks because of blanks in the filename */
            while(ptcWord[nPtcWordIndex] != '\0')
            {
                if(ptcWord[nPtcWordIndex] == '\"' || ptcWord[nPtcWordIndex] != ' ')
                {
                    break;
                }
                nPtcWordIndex++;
            }

            /* Depending of a config file name within quotation marks (or without
               quotation marks) extract the filename */
            if(ptcWord[nPtcWordIndex] == '\"')
            {
                /* Check if the strtok position is already correct */
                if(nPtcWordIndex > 3)
                {
                    /* More than 1 blank before -f. Correct strtok position. */
                    OsStrtok(EC_NULL,"\"");
                }

                /* Now extract the config file name */
                ptcWord = OsStrtok(EC_NULL,"\"");
            }
            else
            {
                /* Extract the config file name if it was not set within quotation marks */
                ptcWord = GetNextWord((EC_T_CHAR**)&szCommandLine, &tcStorage );
            }

            if (
                (ptcWord == EC_NULL)
             || (OsStrncmp( ptcWord, "-", 1) == 0)
               )
            {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            OsSnprintf(szENIFilename, sizeof(szENIFilename) - 1, "%s", ptcWord);
        }
        if( 0 == OsStricmp( ptcWord, "-log") )
        {
            EC_T_INT nPtcWordIndex = 4;

            /* Search for the start of the config file name. The config file
               name may start with quotation marks because of blanks in the filename */
            while(ptcWord[nPtcWordIndex] != '\0')
            {
                if(ptcWord[nPtcWordIndex] == '\"' || ptcWord[nPtcWordIndex] != ' ')
                {
                    break;
                }
                nPtcWordIndex++;
            }

            /* Depending of a config file name within quotation marks (or without
               quotation marks) extract the filename */
            if(ptcWord[nPtcWordIndex] == '\"')
            {
                /* Check if the strtok position is already correct */
                if(nPtcWordIndex > 3)
                {
                    /* More than 1 blank before -f. Correct strtok position. */
                    OsStrtok(EC_NULL,"\"");
                }

                /* Now extract the config file name */
                ptcWord = OsStrtok(EC_NULL,"\"");
            }
            else
            {
                /* Extract the config file name if it was not set within quotation marks */
                ptcWord = GetNextWord((EC_T_CHAR**)&szCommandLine, &tcStorage );
            }

            if (
                (ptcWord == EC_NULL)
             || (OsStrncmp( ptcWord, "-", 1) == 0)
               )
            {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            OsSnprintf(szLogFileprefix, sizeof(szLogFileprefix) - 1, "%s", ptcWord);
        }
        if( 0 == OsStricmp( ptcWord, "-s") )
        {
            EC_T_INT nPtcWordIndex = 5;

            /* Search for the start of the config file name. The config file
               name may start with quotation marks because of blanks in the filename */
            while(ptcWord[nPtcWordIndex] != '\0')
            {
                if(ptcWord[nPtcWordIndex] == '\"' || ptcWord[nPtcWordIndex] != ' ')
                {
                    break;
                }
                nPtcWordIndex++;
            }

            /* Depending of a config file name within quotation marks (or without
               quotation marks) extract the filename */
            if(ptcWord[nPtcWordIndex] == '\"')
            {
                /* Check if the strtok position is already correct */
                if(nPtcWordIndex > 5)
                {
                    /* More than 1 blank before -f. Correct strtok position. */
                    OsStrtok(EC_NULL,"\"");
                }

                /* Now extract the config file name */
                ptcWord = OsStrtok(EC_NULL,"\"");
            }
            else
            {
                /* Extract the config file name if it was not set within quotation marks */
                ptcWord = GetNextWord((EC_T_CHAR**)&szCommandLine, &tcStorage );
            }

            if (
                (ptcWord == EC_NULL)
             || (OsStrncmp( ptcWord, "-", 1) == 0)
               )
            {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            OsSnprintf(szParamFileprefix, sizeof(szParamFileprefix) - 1, "%s", ptcWord);
        }
        else if (OsStricmp( ptcWord, "-t") == 0)
        {
            ptcWord = GetNextWord((EC_T_CHAR**)&szCommandLine, &tcStorage );
            if (
                (ptcWord == EC_NULL)
             || (OsStrncmp( ptcWord, "-", 1) == 0)
               )
            {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            dwDuration = OsStrtol(ptcWord, EC_NULL, 0);
        }
        else if( OsStricmp( ptcWord, "-auxclk") == 0)
        {
#if (defined AUXCLOCK_SUPPORTED)
            TimingDesc.bUseAuxClock = EC_TRUE;
            ptcWord = GetNextWord((EC_T_CHAR**)&szCommandLine, &tcStorage);
            if (
                (ptcWord == EC_NULL)
                || (OsStrncmp( ptcWord, "-", 1) == 0)
                )
            {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            TimingDesc.dwBusCycleTimeUsec = OsStrtol(ptcWord, EC_NULL, 0);
            if( TimingDesc.dwBusCycleTimeUsec < 10 )
            {
                TimingDesc.dwBusCycleTimeUsec = 10;
            }
#else
            OsDbgMsg( "Auxiliary clock not supported by this operating system!)\n" );
            goto Exit;
#endif
        }
        else if (OsStricmp( ptcWord, "-b") == 0)
        {
#if (defined AUXCLOCK_SUPPORTED)
            if( EC_TRUE == TimingDesc.bUseAuxClock )
            {
                OsDbgMsg( "Using bus cycle time %d usec from auxclock parameter\n", TimingDesc.dwBusCycleTimeUsec );
            }
            else
#endif
            {
                ptcWord = GetNextWord((EC_T_CHAR**)&szCommandLine, &tcStorage );
                if (
                    (ptcWord == EC_NULL)
                 || (OsStrncmp( ptcWord, "-", 1) == 0)
                   )
                {
                    nRetVal = SYNTAX_ERROR;
                    goto Exit;
                }
                TimingDesc.dwBusCycleTimeUsec = OsStrtol(ptcWord, EC_NULL, 0);
            }
        }
        else if (OsStricmp( ptcWord, "-a") == 0)
        {
            ptcWord = GetNextWord((EC_T_CHAR**)&szCommandLine, &tcStorage);
            if (
                (ptcWord == EC_NULL)
                || (OsStrncmp( ptcWord, "-", 1) == 0)
                )
            {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            dwCpuIndex = OsStrtol(ptcWord, EC_NULL, 0);
        }
        else if (OsStricmp( ptcWord, "-v") == 0)
        {
            ptcWord = GetNextWord((EC_T_CHAR**)&szCommandLine, &tcStorage);
            if (
                (ptcWord == EC_NULL)
                || (OsStrncmp( ptcWord, "-", 1) == 0)
                )
            {
                nRetVal = SYNTAX_ERROR;
                goto Exit;
            }
            nVerbose = OsStrtol(ptcWord, EC_NULL, 10);
        }
        else if (OsStricmp( ptcWord, "-perf") == 0)
        {
            bEnaPerfJobs = EC_TRUE;
        }
#if (defined ATEMRAS_SERVER)
        else if( OsStricmp(ptcWord, "-sp") == 0 )
        {
            ptcWord = GetNextWord((EC_T_CHAR**)&szCommandLine, &tcStorage);
            if ((ptcWord == NULL) || (OsStrncmp(ptcWord, "-", 1) == 0))
            {
                wServerPort  = ATDS_CFG_DEFAULT_PORT;
                
                /* optional sub parameter not found, use the current word for the next parameter */
                bGetNextWord = EC_FALSE;
            }
            else
            {
                wServerPort = (EC_T_WORD)OsStrtol(ptcWord, EC_NULL, 10);
            }
        }
#endif
        else if (OsStricmp( ptcWord, "-nodcm") == 0)
        {
            bNoDcm = EC_TRUE;
        }
        else
        {
           EC_CPUSET_ZERO( CpuSet );
           EC_CPUSET_SET( CpuSet, dwCpuIndex );

           if( DoSelectLinkLayer(
                    &ptcWord,
                    (EC_T_CHAR**)&szCommandLine,
                    &tcStorage,
                    (&arrLinkDevLinkOpenParam[dwNumLinkLayer]),
                    &nRetVal,
#ifdef LINUX
                    (CpuSet << 16) | RECV_THREAD_PRIO,
#else
                    RECV_THREAD_PRIO,
#endif
                    EC_NULL
                                  )
               )
           {
               if( SYNTAX_ERROR == nRetVal )
               {
                   goto Exit;
               }
               if (dwNumLinkLayer>2)
               {
                   nRetVal = SYNTAX_ERROR;
                   goto Exit;
               }
               else
               {
                   dwNumLinkLayer++;
               }
           }
        }
        /* get next word */
        if (bGetNextWord)
        {
            ptcWord = GetNextWord((EC_T_CHAR**)&szCommandLine, &tcStorage );
        }
        bGetNextWord = EC_TRUE;
    }
    /* initialize master logging */
    /* For EC-Win log messages are directed to shared memory, disabling file logging before calling InitLogging */
#if (defined ECWIN_RTOS32) || (defined RTAI)
    bLogFileEnb = EC_FALSE;
#elif (defined ECWIN_CE)
    if ('\0' != szLogFileprefix[0])
    {
        bLogFileEnb = EC_FALSE;
    }
    else
    {
        /* legacy log handling */
    }
#endif
    oLogging.InitLogging(0, 0, LOG_THREAD_PRIO, dwCpuIndex, szLogFileprefix, LOG_THREAD_STACKSIZE);
    bLogInitialized = EC_TRUE;
    oLogging.SetLogThreadAffinity(dwCpuIndex);
    OsAddDbgMsgHook(CAtEmLogging::OsDbgMsgHookWrapper);
    LogMsg("Full command line: %s\n", szFullCommandLine);
#if (defined ECWIN)
    if ('\0' != szLogFileprefix[0])
    {
    BYTE*  pbyShm    = NULL;
    UINT32 dwShmSize = 0;
    CHAR   szShmName[256];

        /* get a pointer to the master log */
        OsSnprintf(szShmName, sizeof(szShmName) - 1, "%s.log", szLogFileprefix);
        if (OpenRtosLibShm(szShmName, &pbyShm, &dwShmSize))
        {
            oLogging.SetLogMsgBuf(pbyShm, dwShmSize);
        }
        else
        {
#if (defined ECWIN_CE)
            LogMsg("Cannot open shared memory '%s', try to use file system\n", szShmName);
#else
            LogError("Error cannot open shared memory '%s'\n", szShmName);
            return 0;
#endif
        }
        /* get a pointer to the DCM log */
        OsSnprintf(szShmName, sizeof(szShmName) - 1, "%s_dcm.log", szLogFileprefix);
        if (OpenRtosLibShm(szShmName, &pbyShm, &dwShmSize))
        {
            oLogging.SetLogDcmBuf(pbyShm, dwShmSize);
        }
        else
        {
#if (defined ECWIN_CE)
            LogMsg("Cannot open shared memory '%s', try to use file system\n", szShmName);
#else
            LogError("Error cannot open shared memory '%s'\n", szShmName);
            return 0;
#endif
        }
    }
#endif /* ECWIN */

    /* determine master configuration type */
#if (defined ECWIN_RTOS32)
    if ('\0' == szENIFilename[0])
    {
        /* for compatibility */
        OsSnprintf(szENIFilename, sizeof(szENIFilename) - 1, "MasterENI");
    }
#endif
    if ('\0' != szENIFilename[0])
    {
#if (defined ECWIN)
        if ('0' == szENIFilename[0])
        {
            /* for compatibility */
            eCnfType = eCnfType_GenPreopENI;
        }
        else
        {
            /* get a pointer to the master log */
            if (OpenRtosLibShm(szENIFilename, &pbyCnfData, &dwCnfDataLen))
            {
                eCnfType = eCnfType_Data;
            }
            else
            {
#if (defined ECWIN_CE)
                eCnfType     = eCnfType_Filename;
                pbyCnfData   = (EC_T_BYTE*)&szENIFilename[0];
                dwCnfDataLen = 256;

                LogMsg("Cannot open shared memory '%s', try to use file system\n", szENIFilename);
#else
                if (OsStrncmp(szENIFilename, "MasterENI", 9) == 0)
                {
                    /* for compatibility */
                    eCnfType = eCnfType_GenPreopENI;

                    LogMsg("Cannot open shared memory '%s', use auto generated ENI until PreOP\n", szENIFilename);
                }
                else
                {
                    LogMsg("Cannot open shared memory '%s'\n", szENIFilename);

                    return 0;
                }
#endif
            }
        }
#elif (defined __RCX__)
        eCnfType     = eCnfType_Data;
        pbyCnfData   = pStartup->pBecFile;
        dwCnfDataLen = pStartup->dwBecFileLen;
#else
        eCnfType     = eCnfType_Filename;
        pbyCnfData   = (EC_T_BYTE*)&szENIFilename[0];
        dwCnfDataLen = 256;
#endif
    }
    else
    {
#if (defined STATIC_MASTERENI_XML_DATA)
        eCnfType = eCnfType_Data;
        pbyCnfData   = STATIC_MASTERENI_XML_DATA;
        dwCnfDataLen = STATIC_MASTERENI_XML_DATA_SIZE;
#else
        eCnfType     = eCnfType_GenPreopENI;
#endif
    }
    if (0 == dwNumLinkLayer)
    {
        OsDbgMsg("Syntax error: missing link layer command line parameter\n");
        nRetVal = SYNTAX_ERROR;
        goto Exit;
    }
#if !(defined RTAI)
    /* for multi core cpus: select cpu number for this thread */
    EC_CPUSET_ZERO( CpuSet );
    EC_CPUSET_SET( CpuSet, dwCpuIndex );
    if( ! OsSetThreadAffinity(EC_NULL, CpuSet) )
    {
       OsDbgMsg("ERROR: Set Affinity Failed!\n");
    }
    TimingDesc.dwCpuIndex = dwCpuIndex;

    /* create timing event to trigger the job task */
    TimingDesc.pvTimingEvent = OsCreateEvent();
    if( EC_NULL == TimingDesc.pvTimingEvent )
    {
        OsDbgMsg("ERROR: insufficient memory to create timing event!\n");
        goto Exit;
    }

#if (defined AUXCLOCK_SUPPORTED)
    /* initialize auxiliary clock */
    if( EC_TRUE == TimingDesc.bUseAuxClock )
    {
#if (defined VXWORKS)
        sysAuxClkDisable();
        if (OK != sysAuxClkRateSet(1000000 / TimingDesc.dwBusCycleTimeUsec))
        {
            OsDbgMsg("Error calling sysAuxClkRateSet!\n");
            goto Exit;
        }
        sysAuxClkConnect((FUNCPTR)vxAuxClkIsr, ((EC_T_INT)TimingDesc.pvTimingEvent));        
        sysAuxClkEnable( );
        OsSleep(2000);

#elif ((defined UNDER_CE) && (_WIN32_WCE >= 0x600))
        /* get auxilary clock sysintr */
        bRes = KernelIoControl((DWORD)IOCTL_AUXCLK_GET_SYSINTR, (DWORD)NULL, (DWORD)0, &dwAuxClkSysIntr, (DWORD)sizeof(DWORD), &dwWinRes);
        if (!bRes)
        {
            OsDbgMsg("Error calling KernelIoControl(IOCTL_AUXCLK_GET_SYSINTR) (0x%08X)!\n", GetLastError());
            goto Exit;
        }
        /* open VirtualDrv for interrupt management */
        TimingDesc.hVirtualDrv = CreateFile(TEXT("VIR1:"),
                                 GENERIC_READ | GENERIC_WRITE,
                                 FILE_SHARE_READ | FILE_SHARE_WRITE,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                 INVALID_HANDLE_VALUE);
        if ((TimingDesc.hVirtualDrv == NULL) || (TimingDesc.hVirtualDrv == INVALID_HANDLE_VALUE))
        {
            OsDbgMsg("Error calling CreateFile(""VIR1:"") (0x%08X)!\n", GetLastError());
            TimingDesc.hVirtualDrv = NULL;
            goto Exit;
        }
        /* connect auxilary clock interrupt */
        TimingDesc.oIrqDesc.dwSysIrq = dwAuxClkSysIntr;
        swprintf(TimingDesc.oIrqDesc.szEventName, TEXT("%s"), TEXT("AUXCLK"));
        bRes = DeviceIoControl(TimingDesc.hVirtualDrv, (DWORD)IOCTL_VIRTDRV_INTERRUPT_INIT, &(TimingDesc.oIrqDesc), sizeof(VI_T_INTERRUPTDESC), NULL, 0, NULL, NULL );
        if (!bRes)
        {
            OsDbgMsg("Error calling DeviceIoControl(IOCTL_VIRTDRV_INTERRUPT_INIT) (0x%08X)!\n", GetLastError());
            goto Exit;
        }
        /* create auxilary clock interrupt event */
        TimingDesc.pvAuxClkEvent = (VOID*)CreateEvent(NULL, FALSE, FALSE, TEXT("AUXCLK"));
        if ((TimingDesc.pvAuxClkEvent == NULL) || (TimingDesc.pvTimingEvent == INVALID_HANDLE_VALUE))
        {
            OsDbgMsg("Error creating AuxClk event (0x%08X)!\n", GetLastError());
            TimingDesc.pvAuxClkEvent = NULL;
            goto Exit;
        }
        /* auxiliary clock event handled within timing task */
        bStartTimingTask = EC_TRUE;

#elif (defined UNDER_RTSS)
        hTimer = RtCreateTimer(NULL, 0, RtxAuxClkIsr, (PVOID)TimingDesc.pvTimingEvent, RT_PRIORITY_MAX, CLOCK_2);
        liTimer.QuadPart = (LONGLONG)10*TimingDesc.dwBusCycleTimeUsec;
        RtSetTimerRelative(hTimer, &liTimer, &liTimer);
#else /* !UNDER_RTSS */
        dwRes = OsAuxClkInit( dwCpuIndex, 1000000 / TimingDesc.dwBusCycleTimeUsec, TimingDesc.pvTimingEvent );
        if( EC_E_NOERROR != dwRes )
        {
            OsDbgMsg( "ERROR at auxiliary clock initialization!\n" );
            goto Exit;
        }
#endif /* !UNDER_RTSS */
    } // if( EC_TRUE == TimingDesc.bUseAuxClock )
    else
#endif
    {
#if (defined RTOS_32)
        CLKSetTimerIntVal( TimingDesc.dwBusCycleTimeUsec );
        RTKDelay( 1 );
#endif /* RTOS_32 */
#if !(defined NO_OS)
        bStartTimingTask = EC_TRUE;
#endif /* !NO_OS */
    }
    /* create timing task if needed */
    if (bStartTimingTask)
    {
        OsCreateThread( (EC_T_CHAR*)"tEcTimingTask", (EC_PF_THREADENTRY)tEcTimingTask, TIMER_THREAD_PRIO, TIMER_THREAD_STACKSIZE, (EC_T_VOID*)&TimingDesc );
        while( !TimingDesc.bIsRunning )
        {
            OsSleep( 1 );
        }
    }
#endif /* !RTAI */
    OsDbgMsg( "Run demo now with cycle time %d usec\n", TimingDesc.dwBusCycleTimeUsec);
#if (defined AUXCLOCK_SUPPORTED)
    OsDbgMsg("Using %s\n",
          (TimingDesc.bUseAuxClock ? "AuxClock" : "Sleep"));
#endif
    dwRes = DCDemo(   &oLogging,
                      eCnfType, pbyCnfData, dwCnfDataLen,
                      TimingDesc.dwBusCycleTimeUsec, nVerbose, dwDuration,
					  &szParamFileprefix[0],
                      &arrLinkDevLinkOpenParam[0],
                      TimingDesc.pvTimingEvent, dwCpuIndex,
                      bEnaPerfJobs
#if (defined ATEMRAS_SERVER)
                      ,wServerPort
#endif
                      ,(2 == dwNumLinkLayer), &arrLinkDevLinkOpenParam[1]
                      , bNoDcm
                    );
    if (dwRes != EC_E_NOERROR)
    {
        goto Exit;
    }
    /* no errors */
    nRetVal = APP_NOERROR;

Exit:
    if (nRetVal == SYNTAX_ERROR)
    {
        ShowSyntax();
    }
    OsDbgMsg("EtherCAT Master demo application stop.\n");
#if !(defined VXWORKS)
    if (nRetVal != APP_NOERROR)
    {
        OsSleep(5000);
    }
#endif
    /* stop timing task if running */
    if (EC_TRUE == TimingDesc.bIsRunning)
    {
        TimingDesc.bShutdown = EC_TRUE;
        while( TimingDesc.bIsRunning )
        {
            OsSleep( 1 );
        }
    }
#if (defined AUXCLOCK_SUPPORTED)
    /* clean up auxclk */
    if( TimingDesc.bUseAuxClock )
    {
#if (defined EC_VERSION_WINDOWS)
        OsAuxClkDeinit(0);

#elif (defined VXWORKS)
        sysAuxClkDisable();

#elif ((defined UNDER_CE) && (_WIN32_WCE >= 0x600))
        if( NULL != TimingDesc.hVirtualDrv )
        {
            /* deinit the auxilary clock interrupt and close the handle to it */
            TimingDesc.oIrqDesc.dwSysIrq = dwAuxClkSysIntr;
            bRes = DeviceIoControl(TimingDesc.hVirtualDrv, (DWORD)IOCTL_VIRTDRV_INTERRUPT_DEINIT, &(TimingDesc.oIrqDesc), sizeof(VI_T_INTERRUPTDESC), NULL, 0, NULL, NULL );
            if (!bRes)
            {
                printf("Error calling DeviceIoControl(IOCTL_VIRTDRV_INTERRUPT_DEINIT) (0x%08X)!\n", GetLastError());
            }
            CloseHandle(TimingDesc.hVirtualDrv);
            TimingDesc.hVirtualDrv = NULL;
        }
        /* Close the AUXCLK-TimingTask synchronization handle */
        if( EC_NULL != TimingDesc.pvAuxClkEvent )
        {
            CloseHandle(TimingDesc.pvAuxClkEvent);
            TimingDesc.pvAuxClkEvent = EC_NULL;
        }
#elif (defined UNDER_RTSS)
        if (NULL != hTimer)
        {
             RtCancelTimer(hTimer, &liTimer);
             RtDeleteTimer(hTimer);
        }
#endif
    }
#endif
    /* delete the timing event */
    if( EC_NULL != TimingDesc.pvTimingEvent )
    {
        OsDeleteEvent( TimingDesc.pvTimingEvent );
        TimingDesc.pvTimingEvent = EC_NULL;
    }

    if (bLogInitialized)
    {
        /* de-initialize message logging */
        oLogging.DeinitLogging();
    }
    /* final OS layer cleanup */
    OsDeinit();

#if !(defined RTAI)
    return nRetVal;
#endif
}

/*-Handle static linked link layers -----------------------------------------*/
#if (defined RTOS_32) || (defined __INTEGRITY) || (defined __TKERNEL) || (defined __RCX__) || (defined RTAI)
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "EcLink.h"
extern EC_T_DWORD emllRegisterI8255x (EC_T_LINK_DRV_DESC* pLinkDrvDesc, EC_T_DWORD dwLinkDrvDescSize);
extern EC_T_DWORD emllRegisterI8254x (EC_T_LINK_DRV_DESC* pLinkDrvDesc, EC_T_DWORD dwLinkDrvDescSize);
extern EC_T_DWORD emllRegisterRTL8139(EC_T_LINK_DRV_DESC* pLinkDrvDesc, EC_T_DWORD dwLinkDrvDescSize);
extern EC_T_DWORD emllRegisterRTL8169(EC_T_LINK_DRV_DESC* pLinkDrvDesc, EC_T_DWORD dwLinkDrvDescSize);
extern EC_T_DWORD emllRegisterHnx    (EC_T_LINK_DRV_DESC* pLinkDrvDesc, EC_T_DWORD dwLinkDrvDescSize);

EC_PF_LLREGISTER OsGetLinkLayerRegFunc(EC_T_CHAR* szDriverIdent)
{
EC_PF_LLREGISTER pfLlRegister = EC_NULL;

#if (defined LINKLAYER_I8254X)
    if (0 == OsStrcmp("I8254x", szDriverIdent))
    {
        pfLlRegister = (EC_PF_LLREGISTER)emllRegisterI8254x;
    } else
#endif
#if (defined LINKLAYER_I8255X)
    if (0 == OsStrcmp("I8255x", szDriverIdent))
    {
        pfLlRegister = (EC_PF_LLREGISTER)emllRegisterI8255x;
    } else
#endif
#if (defined LINKLAYER_RTL8139)
    if (0 == OsStrcmp("RTL8139", szDriverIdent))
    {
        pfLlRegister = (EC_PF_LLREGISTER)emllRegisterRTL8139;
    } else
#endif
#if (defined LINKLAYER_RTL8169)
    if (0 == OsStrcmp("RTL8169", szDriverIdent))
    {
        pfLlRegister = (EC_PF_LLREGISTER)emllRegisterRTL8169;
    } else
#endif
#if (defined LINKLAYER_HNX)
    if (0 == OsStrcmp("Hnx", szDriverIdent))
    {
        pfLlRegister = (EC_PF_LLREGISTER)emllRegisterHnx;
    } else
#endif
    {
        pfLlRegister = EC_NULL;
    }
    return pfLlRegister;
}
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /* RTOS_32 ||  __INTEGRITY || __TKERNEL || __RCX__ || RTAI */

/*-END OF SOURCE FILE--------------------------------------------------------*/
