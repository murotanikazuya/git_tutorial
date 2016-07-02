/*-----------------------------------------------------------------------------
 * ecatDemoCommon.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              header for demo common parts
 *---------------------------------------------------------------------------*/

#ifndef __ECATDEMOCOMMON_H__
#define __ECATDEMOCOMMON_H__  1

/*-INCLUDES------------------------------------------------------------------*/
#include <AtEthercat.h>
#include "SlaveInfo.h"
#include "Logging.h"
#include "ecatNotification.h"

/*-DEFINES-------------------------------------------------------------------*/
#define ETHERCAT_START_TIMEOUT      10000     /* default timeout waiting to start the master */
#define ETHERCAT_SCANBUS_TIMEOUT     5000     /* default timeout waiting for ScanBus */
#define ETHERCAT_STOP_TIMEOUT       10000     /* default timeout waiting to stop the master */

#define ETHERCAT_DC_TIMEOUT             12000   /* default init timeout 12 seconds */
#define ETHERCAT_DC_ARMW_BURSTCYCLES    10000   /* default burst length 10000 */
#if (defined __RCX__)
#define ETHERCAT_DC_ARMW_BURSTSPP       5      	/* default burst bulk 5 */     
#else
#define ETHERCAT_DC_ARMW_BURSTSPP       12      /* default burst bulk 12 */     
#endif
#define ETHERCAT_DC_DEV_LIMIT           13      /* default :13 -> 3^10 - 1 = 8191 nanosec */
#define ETHERCAT_DC_SETTLE_TIME         1500    /* default settle time : 1 second (1000msec) */

#define APP_ERROR       -1
#define SYNTAX_ERROR    -2
#define APP_NOERROR      0

//#define LINK_DEBUG
#if (defined LINK_DEBUG)
#define LINK_DBG_MSG(eth1,eth2,szMsg)   ecatEthDbgMsg((eth1),(eth2),(szMsg))
#else
#define LINK_DBG_MSG(eth1,eth2,szMsg)
#endif

/* tag names for DemoConfig.xml file */

#define DEMO_CFG_DEFAULT_FILENAME       (EC_T_CHAR*)"DemoConfig.xml"

#define DEMO_CFG_TAG_ENI_FILENAME       (EC_T_CHAR*)"Config\\Common\\ENIFileName"
#define DEMO_CFG_TAG_LOG_FILEPREFIX     (EC_T_CHAR*)"Config\\Common\\LogFilePrefix"
#define DEMO_CFG_TAG_LINK_LAYER         (EC_T_CHAR*)"Config\\Common\\LinkLayer"
#define DEMO_CFG_TAG_LINK_LAYER2        (EC_T_CHAR*)"Config\\Common\\LinkLayer2"
#define DEMO_CFG_TAG_DURATION           (EC_T_CHAR*)"Config\\Common\\DemoDuration"
#define DEMO_CFG_TAG_CPU_AFFINITY       (EC_T_CHAR*)"Config\\Common\\CpuAffinity"
#define DEMO_CFG_TAG_VERBOSITY_LVL      (EC_T_CHAR*)"Config\\Common\\VerbosityLevel"
#define DEMO_CFG_TAG_PERF_MEASURE       (EC_T_CHAR*)"Config\\Common\\PerfMeasurement"
#define DEMO_CFG_TAG_RAS_ENABLED        (EC_T_CHAR*)"Config\\Common\\RASEnabled"
#define DEMO_CFG_TAG_RAS_PORT           (EC_T_CHAR*)"Config\\Common\\RASPort"
#define DEMO_CFG_TAG_AUXCLK             (EC_T_CHAR*)"Config\\Common\\AuxClk"
#define DEMO_CFG_TAG_BUSCYCLETIME       (EC_T_CHAR*)"Config\\Common\\BusCycleTime"

#if (defined LINUX) && (defined __arm__)
  #ifndef INCLUDE_CPSW
    #define INCLUDE_CPSW
  #endif
  #ifndef INCLUDE_GEM
    #define INCLUDE_GEM
  #endif
#endif /* LINUX && __arm__ */

#if (defined STARTERWARE_NOOS)
  #ifndef INCLUDE_CPSW
    #define INCLUDE_CPSW
  #endif
#endif /* STARTERWARE_NOOS */

typedef struct _T_THREAD_PARAM
{
    EC_T_DWORD          dwMasterID;             /**< [in]   Master instance */
    EC_T_BOOL           bThreadRunning;         /**< [in]   Thread is running */
    EC_T_BOOL           bThreadShutdown;        /**< [in]   Thread Shutdown requested */
    EC_T_VOID*          pvTimingEvent;          /**< [in]   Event create for timing purposes */
    EC_T_DWORD          dwCpuIndex;             /**< [in]   SMP systems: CPU index */
    CAtEmLogging*       pLogInst;               /**< [in]   common base class pointer */
    CEmNotification*    pNotInst;               /**< [in]   Notification class pointer */
    EC_T_VOID*          pvApplSpecParm1;        /**< [in]   application specific parameters 1 */
    EC_T_VOID*          pvApplSpecParm2;        /**< [in]   application specific parameters 2 */
    EC_T_DWORD          dwDcmCtlCycleDivider;   /**< [in]   DCM controller cycle divider*/
    EC_T_BOOL           bDcmCtlInitialized;     /**< [in]   EC_TRUE if DCM controller is initialized */
    EC_T_DWORD          dwBusCycleTimeUsec;     /**< [in]   bus cycle time in usec */
} T_THREAD_PARAM;

/*-CLASS---------------------------------------------------------------------*/
class CEmDemoCommon
{
public:
                CEmDemoCommon(                  EC_T_DWORD              dwMasterID      =0          );
                ~CEmDemoCommon(                 EC_T_VOID                                           );

    EC_T_VOID   SetCycErrorNotifyMask(          EC_T_DWORD              dwCycErrorNotifyMask        );
    EC_T_VOID   LinkDbgMsg(                     EC_T_BYTE               byEthTypeByte0,
                                                EC_T_BYTE               byEthTypeByte1,
                                                EC_T_CHAR*              szMsg                       );

    EC_T_DWORD  SetFrameLoss(                   EC_T_DWORD              dwNumGoodFramesAfterStart,
                                                EC_T_DWORD              dwLikelihoodPpm,
                                                EC_T_DWORD              dwFixedLossNumGoodFrms,
                                                EC_T_DWORD              dwFixedLossNumBadFrms,
                                                EC_T_BOOL               bSuppressCyclicMessage     );

    EC_T_BOOL   PrintSlaveInfos(                EC_T_VOID);
    EC_T_VOID   PrintCfgSlavesInfo(             EC_T_VOID);
    EC_T_VOID   PrintBusSlavesInfo(             EC_T_VOID);

    EC_T_BOOL FindSlaveGetFixedAddr(            EC_T_DWORD  dwInstance,         /**< [in]   Slave instance (0 = first matching, 1 = second, ...) */
                                                EC_T_DWORD  dwVendorId,         /**< [in]   Vendor Id of slave to search */
                                                EC_T_DWORD  dwProductCode,      /**< [in]   Product Code of slave to search */
                                                EC_T_WORD*  pwPhysAddr);        /**< [out]  Station address of slave */

    EC_T_DWORD  DoBusScan(                      EC_T_SB_STATUS_NTFY_DESC*
                                                                        poStatus,
                                                EC_T_BOOL               bWait,
                                                EC_T_BOOL               bNtfyActive,
                                                EC_T_DWORD              dwScanBustimeout = ETHERCAT_SCANBUS_TIMEOUT );

    static
    EC_T_BOOL CreateLinkParaI8254x(             EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_DWORD              dwInstance,                      
                                                EC_T_LINKMODE           eLinkMode,                   
                                                EC_T_DWORD              dwRecvPriority);             
    
    static
    EC_T_BOOL CreateLinkParaFslFec(             EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_DWORD              dwInstance,                      
                                                EC_T_LINKMODE           eLinkMode,                   
                                                EC_T_DWORD              dwRecvPriority); 

#if (defined(LINUX) && defined(_ARCH_PPC)) || (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC32))
    static
    EC_T_BOOL CreateLinkParaFslEtsec(           EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_DWORD              dwInstance,                      
                                                EC_T_LINKMODE           eLinkMode,                   
                                                EC_T_DWORD              dwRecvPriority); 
#endif /* if (defined(LINUX) && defined(_ARCH_PPC)) || (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC32)) */
    
#if (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC440))
    static
    EC_T_BOOL CreateLinkParaEmac(               EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_DWORD              dwInstance,                      
                                                EC_T_LINKMODE           eLinkMode,                   
                                                EC_T_DWORD              dwRecvPriority); 
#endif /* (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC440)) */

    static
    EC_T_BOOL CreateLinkParaI8255x(   
                                                EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_DWORD              dwInstance,                  
                                                EC_T_LINKMODE           eLinkMode,                   
                                                EC_T_DWORD              dwRecvPriority             );             

    static
    EC_T_BOOL CreateLinkParaRTL8139(            EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_DWORD              dwInstance,                      
                                                EC_T_LINKMODE           eLinkMode,                   
                                                EC_T_DWORD              dwRecvPriority             );
    static
    EC_T_BOOL CreateLinkParaRTL8169(            EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_DWORD              dwInstance,                      
                                                EC_T_LINKMODE           eLinkMode,                   
                                                EC_T_DWORD              dwRecvPriority             );
   
    static
    EC_T_BOOL CreateLinkParaSnarf(              EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam, 
                                                EC_T_WCHAR*               aszAdapterName           );

    static
    EC_T_BOOL CreateLinkParaEtherlib(           EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam, 
                                                EC_T_WCHAR*               aszAdapterName           );

    static
    EC_T_BOOL CreateLinkParaWinPcap(            EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   
                                                EC_T_BYTE*                arrIpAddress,             
                                                EC_T_LINKMODE             eLinkMode                );     

    static
	EC_T_BOOL CreateLinkParaPxaMfu(				EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_DWORD                dwInstance,                      
                                                EC_T_LINKMODE             eLinkMode,                   
                                                EC_T_DWORD                dwRecvPriority           ); 

    static
    EC_T_BOOL CreateLinkParaCCAT(               EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_DWORD              dwInstance,
                                                EC_T_LINKMODE           eLinkMode,
                                                EC_T_DWORD              dwRecvPriority);

#if (defined INCLUDE_CPSW)
    static
    EC_T_BOOL CreateLinkParaCPSW(               EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_DWORD              dwPort,                      
                                                EC_T_LINKMODE           eLinkMode,                   
                                                EC_T_DWORD              dwRecvPriority,
                                                EC_T_DWORD              dwPortPriority,
                                                EC_T_BOOL               bMaster,
                                                EC_T_BOOL               bUseStarterwareDriver);

#endif

#if (defined INCLUDE_SMSC)
    static
    EC_T_BOOL CreateLinkParaSmscL9218i(         EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_LINKMODE   eLinkMode,
                                                EC_T_DWORD                dwRecvPriority); 
#endif 

#if (defined INCLUDE_GEM)
    static
    EC_T_BOOL CreateLinkParaGem(                EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                                EC_T_LINKMODE             eLinkMode,
                                                EC_T_DWORD                dwPort);
#endif

    static
    EC_T_CHAR*  SlaveVendorText(                T_eEtherCAT_Vendor      EV                          );
    static
    EC_T_CHAR*  SlaveProdCodeText(              T_eEtherCAT_Vendor      EV,
                                                T_eEtherCAT_ProductCode EPC                         );

    static
    EC_T_CHAR*  ESCTypeText(                    EC_T_BYTE               byESCType                   );

    EC_T_DWORD  MasterInstanceId(               EC_T_VOID                                           )
                    { return m_dwMasterID; }
    EC_T_VOID   MasterInstanceId(               EC_T_DWORD              dwVal                       )
                    { m_dwMasterID = dwVal; }
    CAtEmLogging*
                AtemLogging(                    EC_T_VOID                                           )
                    { return &m_oLogging; }

    EC_T_VOID   HandleMbxTferReqError(          EC_T_CHAR*              szErrMsg,
                                                EC_T_DWORD              dwErrorCode,
                                                EC_T_MBXTFER*           pMbxTfer                    );

protected:
    EC_T_DWORD                      m_dwMasterID;

    CAtEmLogging                    m_oLogging;
};

/*-FUNCTION DECLARATION------------------------------------------------------*/
EC_T_DWORD CoeReadObjectDictionary(EC_T_DWORD              dwInstanceID 
                                   ,CAtEmLogging*           poLog
                                   ,EC_T_INT                nVerbosePrinting   /**< [in]   Print messages */
                                   ,EC_T_BOOL*              pbStopReading   /**< [in]   Pointer to "Stop Reading" flag */
                                   ,EC_T_DWORD              dwClientId      /**< [in]   Current client ID */
                                   ,EC_T_DWORD              dwNodeId        /**< [in]   Slave Id to query ODL from  */
                                   ,EC_T_BOOL               bPerformUpload  /**< [in]   EC_TRUE: do SDO Upload */
                                   ,EC_T_DWORD              dwTimeout       /**< [in]   Individual call timeout */
                                  );


EC_T_VOID   SetCycErrorNotifyMask(          EC_T_DWORD              dwInstanceID,
                                            EC_T_DWORD              dwCycErrorNotifyMask        );
EC_T_VOID   LinkDbgMsg(                     EC_T_DWORD              dwInstanceID,
                                            EC_T_BYTE               byEthTypeByte0,
                                            EC_T_BYTE               byEthTypeByte1,
                                            EC_T_CHAR*              szMsg                       );

EC_T_DWORD  SetFrameLoss(                   EC_T_DWORD              dwInstanceID,
                                            CAtEmLogging*           poLog,
                                            EC_T_DWORD              dwNumGoodFramesAfterStart,
                                            EC_T_DWORD              dwLikelihoodPpm,
                                            EC_T_DWORD              dwFixedLossNumGoodFrms,
                                            EC_T_DWORD              dwFixedLossNumBadFrms,
                                            EC_T_BOOL               bSuppressCyclicMessage     );

EC_T_BOOL   PrintSlaveInfos(                EC_T_DWORD              dwInstanceID,
                                            CAtEmLogging*           poLog);
EC_T_VOID   PrintCfgSlavesInfo(             EC_T_DWORD              dwInstanceID,
                                            CAtEmLogging*           poLog);
EC_T_VOID   PrintBusSlavesInfo(             EC_T_DWORD              dwInstanceID,
                                            CAtEmLogging*           poLog);

EC_T_BOOL FindSlaveGetFixedAddr(            EC_T_DWORD              dwInstanceID,
                                            CAtEmLogging*           poLog,
                                            EC_T_DWORD              dwSlaveInstance,    /**< [in]   Slave instance (0 = first matching, 1 = second, ...) */
                                            EC_T_DWORD              dwVendorId,         /**< [in]   Vendor Id of slave to search */
                                            EC_T_DWORD              dwProductCode,      /**< [in]   Product Code of slave to search */
                                            EC_T_WORD*              pwPhysAddr);        /**< [out]  Station address of slave */

EC_T_DWORD  DoBusScan(                      EC_T_DWORD              dwInstanceID,
                                            CAtEmLogging*           poLog,
                                            EC_T_SB_STATUS_NTFY_DESC*
                                                                    poStatus,
                                            EC_T_BOOL               bWait,
                                            EC_T_BOOL               bNtfyActive,
                                            EC_T_DWORD              dwScanBustimeout = ETHERCAT_SCANBUS_TIMEOUT );

EC_T_BOOL CreateLinkParaI8254x(             EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_DWORD              dwInstance,                      
                                            EC_T_LINKMODE           eLinkMode,                   
                                            EC_T_DWORD              dwRecvPriority);             

EC_T_BOOL CreateLinkParaFslFec(             EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_DWORD              dwInstance,                      
                                            EC_T_LINKMODE           eLinkMode,                   
                                            EC_T_DWORD              dwRecvPriority); 

#if (defined(LINUX) && defined(_ARCH_PPC)) || (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC32))
EC_T_BOOL CreateLinkParaFslEtsec(           EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_DWORD              dwInstance,                      
                                            EC_T_LINKMODE           eLinkMode,                   
                                            EC_T_DWORD              dwRecvPriority); 
#endif /* if (defined(LINUX) && defined(_ARCH_PPC)) || (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC32)) */

#if (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC440))
EC_T_BOOL CreateLinkParaEmac(               EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_DWORD              dwInstance,                      
                                            EC_T_LINKMODE           eLinkMode,                   
                                            EC_T_DWORD              dwRecvPriority); 
#endif /* (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC440)) */

EC_T_BOOL CreateLinkParaI8255x(   
                                            EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_DWORD              dwInstance,                  
                                            EC_T_LINKMODE           eLinkMode,                   
                                            EC_T_DWORD              dwRecvPriority             );             

EC_T_BOOL CreateLinkParaRTL8139(            EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_DWORD              dwInstance,                      
                                            EC_T_LINKMODE           eLinkMode,                   
                                            EC_T_DWORD              dwRecvPriority             );
EC_T_BOOL CreateLinkParaRTL8169(            EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_DWORD              dwInstance,                      
                                            EC_T_LINKMODE           eLinkMode,                   
                                            EC_T_DWORD              dwRecvPriority             );

EC_T_BOOL CreateLinkParaSnarf(              EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam, 
                                            EC_T_WCHAR*               aszAdapterName           );

EC_T_BOOL CreateLinkParaEtherlib(           EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam, 
                                            EC_T_WCHAR*               aszAdapterName           );

EC_T_BOOL CreateLinkParaWinPcap(            EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   
                                            EC_T_BYTE*                arrIpAddress,             
                                            EC_T_LINKMODE             eLinkMode                );     

EC_T_BOOL CreateLinkParaPxaMfu(				EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_DWORD                dwInstance,                      
                                            EC_T_LINKMODE             eLinkMode,                   
                                            EC_T_DWORD                dwRecvPriority           ); 

EC_T_BOOL CreateLinkParaCCAT(               EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_DWORD              dwInstance,
                                            EC_T_LINKMODE           eLinkMode,
                                            EC_T_DWORD              dwRecvPriority);

EC_T_BOOL CreateLinkParaNdisuio(            EC_T_LINK_DEV_OPEN_PARAM*  poLinkDevOpenParam, 
                                            EC_T_CHAR*                  pszName, 
                                            EC_T_DWORD                  dwRecvPriority);
#if (defined INCLUDE_CPSW)
EC_T_BOOL CreateLinkParaCPSW(               EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_DWORD              dwPort,                      
                                            EC_T_LINKMODE           eLinkMode,                   
                                            EC_T_DWORD              dwRecvPriority,
                                            EC_T_DWORD              dwPortPriority,
                                            EC_T_BOOL               bMaster,
                                            EC_T_BOOL               bUseStarterwareDriver);

#endif

#if (defined INCLUDE_SMSC)
EC_T_BOOL CreateLinkParaSmscL9218i(         EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_LINKMODE   eLinkMode,
                                            EC_T_DWORD                dwRecvPriority); 
#endif 

#if (defined INCLUDE_GEM)
EC_T_BOOL CreateLinkParaGem(                EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                            EC_T_LINKMODE             eLinkMode,
                                            EC_T_DWORD                dwPort);
#endif

#if (defined EC_VERSION_NIOS)
EC_T_BOOL CreateLinkParaNios(               EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam);
#endif

EC_T_CHAR*  SlaveVendorText(                T_eEtherCAT_Vendor      EV                          );
EC_T_CHAR*  SlaveProdCodeText(              T_eEtherCAT_Vendor      EV,
                                            T_eEtherCAT_ProductCode EPC                         );

EC_T_CHAR*  ESCTypeText(                    EC_T_BYTE               byESCType                   );

EC_T_VOID   HandleMbxTferReqError(          CAtEmLogging*           poLog,
                                            EC_T_CHAR*              szErrMsg,
                                            EC_T_DWORD              dwErrorCode,
                                            EC_T_MBXTFER*           pMbxTfer                    );


/*-GLOBAL VARIABLES-----------------------------------------------------------*/
extern volatile EC_T_BOOL bRun;             /* global helper variable to shutdown the application */

#endif /*__ECATDEMOCOMMON_H__*/

/*-END OF SOURCE FILE--------------------------------------------------------*/
