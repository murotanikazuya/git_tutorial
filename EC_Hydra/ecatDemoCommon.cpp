/*-----------------------------------------------------------------------------
 * ecatDemoCommon.cpp            
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master common part of the demos
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include <AtEthercat.h>
#ifndef INC_ECCOMMON
#include <EcCommon.h>
#endif
#ifndef INC_ECOS
#include <EcOs.h>
#endif
#include "ecatNotification.h"
#include "Logging.h"
#include "ecatDemoCommon.h"
#include "TimeoutHelper.h"

#ifdef VXWORKS
#  include <muxLib.h>
#endif /* ifdef VXWORKS */

#include <EcObjDef.h>

/*-DEFINES-------------------------------------------------------------------*/

#define LogErrorAdd poLog->LogErrorAdd
#define LogErr      poLog->LogError
#define LogMsgAdd   poLog->LogMsgAdd
#define LogMsg      poLog->LogMsg

/*-TYPEDEFS------------------------------------------------------------------*/

/*-GLOBAL VARIABLES----------------------------------------------------------*/

/*-LOCAL VARIABLES-----------------------------------------------------------*/

/*-FORWARD DECLARATIONS------------------------------------------------------*/

/*-FUNCTIONS-----------------------------------------------------------------*/

CEmDemoCommon::CEmDemoCommon(EC_T_DWORD  dwMasterInstance)
{
    m_dwMasterID                                        = dwMasterInstance;
}

CEmDemoCommon::~CEmDemoCommon(EC_T_VOID)
{
}

#ifndef EXCLUDE_ATEM
/********************************************************************************/

/********************************************************************************/
/** \brief  Set bit mask to enable/disable error notifications for cyclic commands.
*
* \return  State.
*/
EC_T_VOID CEmDemoCommon::SetCycErrorNotifyMask
    (EC_T_DWORD          dwCycErrorNotifyMask )
{
    ::SetCycErrorNotifyMask(m_dwMasterID, dwCycErrorNotifyMask); 
}

EC_T_VOID SetCycErrorNotifyMask
    (EC_T_DWORD          dwInstanceID     
    ,EC_T_DWORD          dwCycErrorNotifyMask )
{
    EC_T_IOCTLPARMS oIoCtlParms = {0};    

    /* get current operational state */
    oIoCtlParms.dwInBufSize     = sizeof(EC_T_DWORD);
    oIoCtlParms.pbyInBuf        = (EC_T_PBYTE)&dwCycErrorNotifyMask;
    oIoCtlParms.dwOutBufSize    = 0;
    oIoCtlParms.pbyOutBuf       = EC_NULL;
    oIoCtlParms.pdwNumOutData   = EC_NULL;
    emIoControl(dwInstanceID, EC_IOCTL_SET_CYC_ERROR_NOTIFY_MASK, &oIoCtlParms);
}



/********************************************************************************/
/** \brief  Send a debug message to the EtherCAT link layer.
*
* \return  N/A.
*/
EC_T_VOID CEmDemoCommon::LinkDbgMsg
    (EC_T_BYTE      byEthTypeByte0  /*< in  Ethernet type byte 0 */
    ,EC_T_BYTE      byEthTypeByte1  /*< in  Ethernet type byte 0 */
    ,EC_T_CHAR*     szMsg           /*< in  message to send to link layer */
)
{
    ecatEthDbgMsg(byEthTypeByte0, byEthTypeByte1, szMsg);
}

EC_T_VOID LinkDbgMsg
    (EC_T_DWORD     dwInstanceID
    ,EC_T_BYTE      byEthTypeByte0  /*< in  Ethernet type byte 0 */
    ,EC_T_BYTE      byEthTypeByte1  /*< in  Ethernet type byte 0 */
    ,EC_T_CHAR*     szMsg           /*< in  message to send to link layer */
)
{
    emEthDbgMsg(dwInstanceID, byEthTypeByte0, byEthTypeByte1, szMsg);
}


/********************************************************************************/
/** \brief  Handle mailbox transfer request errors.
*
* \return N/A
*/
EC_T_VOID CEmDemoCommon::HandleMbxTferReqError
(   EC_T_CHAR*    szErrMsg,     /**< [in] error message */
    EC_T_DWORD    dwErrorCode,  /**< [in] basic error code */
    EC_T_MBXTFER* pMbxTfer )    /**< [in] mbx transfer object */
{

    ::HandleMbxTferReqError(&m_oLogging, szErrMsg, dwErrorCode, pMbxTfer);
}

EC_T_VOID HandleMbxTferReqError
    (CAtEmLogging* poLog
    ,EC_T_CHAR*    szErrMsg      /**< [in] error message */
    ,EC_T_DWORD    dwErrorCode   /**< [in] basic error code */
    ,EC_T_MBXTFER* pMbxTfer )    /**< [in] mbx transfer object */
{
    if( dwErrorCode != EC_E_NOERROR )
    {
        if( dwErrorCode == EC_E_TIMEOUT )
        {
            EC_T_DWORD dwWorstCaseTimeout = 10;

            LogErr( "%s: timeout waiting for mailbox transfer response", szErrMsg );
            OsSleep( 2000 );
            /* 
             * The mailbox transfer is still running and the transfer object owned by the master.
             * We cannot re-use the transfer object until the master finally returns the transfer object.
             * Here we just wait until the master terminates the transfer with a internal timeout
             * and thus returns the ownership back to the client (by calling ecatNotify).
             */
            while( pMbxTfer->eTferStatus != eMbxTferStatus_Idle )
            {
                LogErr( "HandleMbxTferReqError: Waiting for master to return the mailbox transfer object...\n" );
                OsSleep( 2000 );

                /* to let application finish, in case master never returns MBX Object, which may happen,
                 * while using Remote API */
                if( 0 == (dwWorstCaseTimeout--) )
                {
                    break;
                }
            }
        }
        else
        {
            if( dwErrorCode == EC_E_INVALID_SLAVE_STATE )
            {
                LogErrorAdd( "%s: slave not in a state for mailbox transfer", szErrMsg );
            }
            else if( dwErrorCode == EC_E_SLAVE_NOT_ADDRESSABLE )
            {
                LogErrorAdd( "%s: slave cannot be addressed for mailbox transfer", szErrMsg );
            }
            else
            {
                LogErrorAdd( "%s: dwErrorCode=%s (0x%lx)", szErrMsg, EcErrorText(dwErrorCode), dwErrorCode );
            }
            if( pMbxTfer->eTferStatus == eMbxTferStatus_TferReqError )
            {
                /* 
                 * The mailbox transfer could not be initiated. 
                 * The transfer object is owned by the client now, thus set transfer object state into idle 
                 */
                LogErr( ", TferReqError=%s (0x%lx)", EcErrorText(pMbxTfer->dwErrorCode), pMbxTfer->dwErrorCode );
                pMbxTfer->eTferStatus = eMbxTferStatus_Idle;
            }
            else
            {
                LogErr( "" );
            }
        }
    }
}

/********************************************************************************/
/** \brief  Set frame loss simulation parameters.
*
* This IO Control is introduced for testing and debugging purposes. It enables 
* an application to simulate the loss of EtherCAT frames on both transmit and 
* receive bus direction. 
* Attention: Do not activate this on shipped releases. Frameloss has significant 
* influence on performance and reliablility of your application !!
*
* Note: this is not a official API function and may be not available in future
*       releases.
*
* \return N/A
*/
EC_T_DWORD CEmDemoCommon::SetFrameLoss
    (EC_T_DWORD dwNumGoodFramesAfterStart   /**< [in] number of good frames before frame loss simulation starts                     */
    ,EC_T_DWORD dwLikelihoodPpm             /**< [in] random loss simulation: frame loss likelihood (ppm)                           */
    ,EC_T_DWORD dwFixedLossNumGoodFrms      /**< [in] fixed loss simulation: number of good frames before frame loss                */
    ,EC_T_DWORD dwFixedLossNumBadFrms       /**< [in] fixed loss simulation: number of lost frames after processing the good ones   */
    ,EC_T_BOOL  bSuppressCyclicMessage      /**< [in] suppress cyclic error messages                                                */
)
{
    return ::SetFrameLoss(m_dwMasterID, &m_oLogging, 
        dwNumGoodFramesAfterStart, dwLikelihoodPpm, dwFixedLossNumGoodFrms, dwFixedLossNumBadFrms, bSuppressCyclicMessage); 
}

EC_T_DWORD SetFrameLoss
    (EC_T_DWORD     dwInstanceID
    ,CAtEmLogging*  poLog
    ,EC_T_DWORD dwNumGoodFramesAfterStart   /**< [in] number of good frames before frame loss simulation starts                     */
    ,EC_T_DWORD dwLikelihoodPpm             /**< [in] random loss simulation: frame loss likelihood (ppm)                           */
    ,EC_T_DWORD dwFixedLossNumGoodFrms      /**< [in] fixed loss simulation: number of good frames before frame loss                */
    ,EC_T_DWORD dwFixedLossNumBadFrms       /**< [in] fixed loss simulation: number of lost frames after processing the good ones   */
    ,EC_T_BOOL  bSuppressCyclicMessage      /**< [in] suppress cyclic error messages                                                */
)
{
EC_T_IOCTLPARMS      oIoCtlParms      = {0};    
EC_T_DWORD adwFrameLossParams[4];
EC_T_DWORD dwResult = EC_E_NOERROR;

    adwFrameLossParams[0] = dwNumGoodFramesAfterStart;
    adwFrameLossParams[1] = dwLikelihoodPpm;
    adwFrameLossParams[2] = dwFixedLossNumGoodFrms;
    adwFrameLossParams[3] = dwFixedLossNumBadFrms;
    oIoCtlParms.dwInBufSize     = 4 * sizeof(EC_T_DWORD);
    oIoCtlParms.pbyInBuf        = (EC_T_PBYTE)adwFrameLossParams;
    oIoCtlParms.dwOutBufSize    = 0;
    oIoCtlParms.pbyOutBuf       = EC_NULL;
    oIoCtlParms.pdwNumOutData   = EC_NULL;
    dwResult = emIoControl(dwInstanceID, EC_IOCTL_SET_FRAME_LOSS_SIMULATION, &oIoCtlParms);
    if( dwResult != EC_E_NOERROR)
    {
        LogErr( "Frame Loss Simulation parameters could not be set! (Result = 0x%x)", dwResult);
        goto Exit;
    }

    if( bSuppressCyclicMessage )
    {
        /* suppress cyclic error messages */
        ::SetCycErrorNotifyMask(dwInstanceID, EC_CYC_ERR_MASK_ALL & ~EC_CYC_ERR_MASK_NO_FRAME_RESPONSE_ERROR & ~EC_CYC_ERR_MASK_UNEXPECTED_FRAME_RESPONSE);
    }
    else
    {
        /* get cyclic error messages */
        ::SetCycErrorNotifyMask(dwInstanceID, EC_CYC_ERR_MASK_ALL);
    }

Exit:
    return dwResult;
}

/***************************************************************************************************/
/**
\brief  Find a specific slaves offset (or print all slave information).
\return EC_TRUE on success, EC_FALSE otherwise.
*/
EC_T_BOOL CEmDemoCommon::PrintSlaveInfos(EC_T_VOID)
{
    return ::PrintSlaveInfos(m_dwMasterID, &m_oLogging);
}

EC_T_BOOL PrintSlaveInfos
    (EC_T_DWORD     dwInstanceID
    ,CAtEmLogging*  poLog)
{
    EC_T_IOCTLPARMS             oIoctlParms = {0};
    EC_T_SB_STATUS_NTFY_DESC    oStatus     = {0};
    EC_T_DWORD                  dwRes       = EC_E_ERROR;
    EC_T_DWORD                  dwScanSpan  = 0;
    EC_T_DWORD                  dwScan      = 0;
    EC_T_WORD                   wAIAddr     = 0;
    EC_T_WORD                   wValue      = 0;
    EC_T_GET_SLAVE_INFO         oSlaveInfo = {0};
    EC_T_BUS_SLAVE_INFO         oBusSlaveInfo = {0};


    /* determine slave amount */
    oIoctlParms.dwInBufSize     = 0;
    oIoctlParms.pbyInBuf        = EC_NULL;
    oIoctlParms.dwOutBufSize    = sizeof(EC_T_SB_STATUS_NTFY_DESC);
    oIoctlParms.pbyOutBuf       = (EC_T_PBYTE)&oStatus;
    oIoctlParms.pdwNumOutData   = EC_NULL;
    dwRes = emIoControl(dwInstanceID, EC_IOCTL_SB_STATUS_GET, &oIoctlParms);
    if( EC_E_NOERROR != dwRes )
    {
        LogErr("Cannot get Scan Bus Status");
        goto Exit;
    }

    dwScanSpan = oStatus.dwSlaveCount;

    /* do all slaves */    
    for(dwScan = 0; dwScan < dwScanSpan; dwScan++)
    {
        wAIAddr = (EC_T_WORD)(0-dwScan);

        /* get all information about the slave */
        if((dwRes = emGetSlaveInfo(dwInstanceID, EC_FALSE, wAIAddr, &oSlaveInfo)) != EC_E_NOERROR)
        {
            LogErr("CEmDemoCommon::FindSlave() emGetSlaveInfo return with error 0x%x", dwRes);
        }
        if((dwRes = emGetBusSlaveInfo(dwInstanceID, EC_FALSE, wAIAddr, &oBusSlaveInfo)) != EC_E_NOERROR)
        {
            LogErr("CEmDemoCommon::FindSlave() emGetBusSlaveInfo return with error 0x%x", dwRes);
        }

        LogMsg( "");
        LogMsg( "******************************************************************************" );
        LogMsg( "Number      : %d", dwScan);
        LogMsg( "Vendor ID   : 0x%08X = %s", oBusSlaveInfo.dwVendorId, SlaveVendorText((T_eEtherCAT_Vendor)oBusSlaveInfo.dwVendorId) );
        LogMsg( "Product Code: 0x%08X = %s", oBusSlaveInfo.dwProductCode, SlaveProdCodeText((T_eEtherCAT_Vendor)oBusSlaveInfo.dwVendorId, (T_eEtherCAT_ProductCode)oBusSlaveInfo.dwProductCode) );
        LogMsg( "Revision    : 0x%08X   Serial Number: %d", oBusSlaveInfo.dwRevisionNumber, oBusSlaveInfo.dwSerialNumber );
        LogMsg( "ESC Type    : %s (0x%x)  Revision: %d  Build: %d", ESCTypeText(oBusSlaveInfo.byESCType), oBusSlaveInfo.byESCType, 
            oBusSlaveInfo.byESCRevision, oBusSlaveInfo.wESCBuild );

        LogMsg( "Bus    AutoInc Address: %4d (0x%x)", (EC_T_INT)(0-dwScan), wAIAddr );
        LogMsg( "Bus    Station Address: %04d (0x%x)", oBusSlaveInfo.wStationAddress, oBusSlaveInfo.wStationAddress );
        LogMsg( "Bus    Alias Address  : %04d (0x%x)", oBusSlaveInfo.wAliasAddress, oBusSlaveInfo.wAliasAddress );
        LogMsg( "Config Station Address: %04d (0x%x)", oSlaveInfo.wCfgPhyAddress, oSlaveInfo.wCfgPhyAddress );
        
        if( 0 != oSlaveInfo.dwPdSizeIn )
        {
            LogMsg( "PD IN    Byte.Bit offset: %d.%d   Size: %d bits", oSlaveInfo.dwPdOffsIn/8,  oSlaveInfo.dwPdOffsIn%8,  oSlaveInfo.dwPdSizeIn );
        }
        if( 0 != oSlaveInfo.dwPdSizeOut )            
        {
            LogMsg( "PD OUT   Byte.Bit offset: %d.%d   Size: %d bits", oSlaveInfo.dwPdOffsOut/8, oSlaveInfo.dwPdOffsOut%8, oSlaveInfo.dwPdSizeOut );
        }

        if( 0 != oSlaveInfo.dwPdSizeIn2 )
        {
            LogMsg( "PD IN  2 Byte.Bit offset: %d.%d   Size: %d bits", oSlaveInfo.dwPdOffsIn2/8,  oSlaveInfo.dwPdOffsIn2%8,  oSlaveInfo.dwPdSizeIn2 );
        }
        if( 0 != oSlaveInfo.dwPdSizeOut2 )            
        {
            LogMsg( "PD OUT 2 Byte.Bit offset: %d.%d   Size: %d bits", oSlaveInfo.dwPdOffsOut2/8, oSlaveInfo.dwPdOffsOut2%8, oSlaveInfo.dwPdSizeOut2 );
        }

        if( 0 != oSlaveInfo.dwPdSizeIn3 )
        {
            LogMsg( "PD IN  3 Byte.Bit offset: %d.%d   Size: %d bits", oSlaveInfo.dwPdOffsIn3/8,  oSlaveInfo.dwPdOffsIn3%8,  oSlaveInfo.dwPdSizeIn3 );
        }
        if( 0 != oSlaveInfo.dwPdSizeOut3 )            
        {
            LogMsg( "PD OUT 3 Byte.Bit offset: %d.%d   Size: %d bits", oSlaveInfo.dwPdOffsOut3/8, oSlaveInfo.dwPdOffsOut3%8, oSlaveInfo.dwPdSizeOut3 );
        }

        if( 0 != oSlaveInfo.dwPdSizeIn4 )
        {
            LogMsg( "PD IN  4 Byte.Bit offset: %d.%d   Size: %d bits", oSlaveInfo.dwPdOffsIn4/8,  oSlaveInfo.dwPdOffsIn4%8,  oSlaveInfo.dwPdSizeIn4 );
        }
        if( 0 != oSlaveInfo.dwPdSizeOut4 )            
        {
            LogMsg( "PD OUT 4 Byte.Bit offset: %d.%d   Size: %d bits", oSlaveInfo.dwPdOffsOut4/8, oSlaveInfo.dwPdOffsOut4%8, oSlaveInfo.dwPdSizeOut4 );
        }

        /* read and output link Port settings */
        wValue = oBusSlaveInfo.wPortState;

        LogMsg("Port 0: %s   Port 1: %s   Port 2: %s   Port 3: %s", 
            ((0x0001<<0)==(wValue&(0x0001<<0)))? "Connected": "Not_Conn.",
            ((0x0001<<1)==(wValue&(0x0001<<1)))? "Connected": "Not_Conn.",
            ((0x0001<<2)==(wValue&(0x0001<<2)))? "Connected": "Not_Conn.",
            ((0x0001<<3)==(wValue&(0x0001<<3)))? "Connected": "Not_Conn.");
    }; /* for(dwScan = 0; dwScan < dwScanSpan; dwScan++) */

Exit:
    return EC_TRUE;
}

/***************************************************************************************************/
/**
\brief  Find a specific slaves offset (or print all slave information).
\return EC_TRUE on success, EC_FALSE otherwise.
*/
EC_T_VOID CEmDemoCommon::PrintCfgSlavesInfo()
{
    ::PrintCfgSlavesInfo(m_dwMasterID, &m_oLogging);
}

EC_T_VOID PrintCfgSlavesInfo
    (EC_T_DWORD     dwInstanceID
    ,CAtEmLogging*  poLog)
{
EC_T_DWORD          dwRes           = EC_E_ERROR;
EC_T_WORD           wAutoIncAddress = 0;
EC_T_CFG_SLAVE_INFO oCfgSlaveInfo   = {0};

    LogMsg( "==================================================" );
    LogMsg( "=== Config slaves ================================" );
    LogMsg( "==================================================" );
    for (wAutoIncAddress = 0; wAutoIncAddress != 1; wAutoIncAddress--)
    {
        /* get config slave information */
        dwRes = emGetCfgSlaveInfo(dwInstanceID, EC_FALSE, wAutoIncAddress, &oCfgSlaveInfo);
        if (EC_E_NOERROR != dwRes)
        {
            break;
        }
        LogMsg( "");
        LogMsg( "Slave #%d - (SlaveId %d):", EC_LOWORD((0 - wAutoIncAddress)), oCfgSlaveInfo.dwSlaveId );
        LogMsg( "**************************************************" );

        LogMsg( "Vendor ID   : 0x%08X = %s", oCfgSlaveInfo.dwVendorId, SlaveVendorText((T_eEtherCAT_Vendor)oCfgSlaveInfo.dwVendorId) );
        LogMsg( "Product Code: 0x%08X = %s", oCfgSlaveInfo.dwProductCode, SlaveProdCodeText((T_eEtherCAT_Vendor)oCfgSlaveInfo.dwVendorId, (T_eEtherCAT_ProductCode)oCfgSlaveInfo.dwProductCode) );
        LogMsg( "Revision    : 0x%08X", oCfgSlaveInfo.dwRevisionNumber);
        LogMsg( "Serial      : 0x%08X", oCfgSlaveInfo.dwSerialNumber );

        LogMsg( "Station address (0x10) : %04d (0x%04X)", oCfgSlaveInfo.wStationAddress, oCfgSlaveInfo.wStationAddress );

        if (0 != oCfgSlaveInfo.dwPdSizeIn)
        {
            LogMsg( "PD IN   Byte.Bit offset: %d.%d   Size: %d bits", oCfgSlaveInfo.dwPdOffsIn/8,  oCfgSlaveInfo.dwPdOffsIn%8,  oCfgSlaveInfo.dwPdSizeIn );
        }
        if (0 != oCfgSlaveInfo.dwPdSizeOut)
        {
            LogMsg( "PD OUT  Byte.Bit offset: %d.%d   Size: %d bits", oCfgSlaveInfo.dwPdOffsOut/8, oCfgSlaveInfo.dwPdOffsOut%8, oCfgSlaveInfo.dwPdSizeOut );
        }
        if (0 != oCfgSlaveInfo.dwPdSizeIn2)
        {
            LogMsg( "PD IN2  Byte.Bit offset: %d.%d   Size: %d bits", oCfgSlaveInfo.dwPdOffsIn2/8,  oCfgSlaveInfo.dwPdOffsIn2%8,  oCfgSlaveInfo.dwPdSizeIn2 );
        }
        if (0 != oCfgSlaveInfo.dwPdSizeOut2)
        {
            LogMsg( "PD OUT2 Byte.Bit offset: %d.%d   Size: %d bits", oCfgSlaveInfo.dwPdOffsOut2/8, oCfgSlaveInfo.dwPdOffsOut2%8, oCfgSlaveInfo.dwPdSizeOut2 );
        }
        if( 0 != oCfgSlaveInfo.dwPdSizeIn3 )
        {
            LogMsg( "PD IN3  Byte.Bit offset: %d.%d   Size: %d bits", oCfgSlaveInfo.dwPdOffsIn3/8,  oCfgSlaveInfo.dwPdOffsIn3%8,  oCfgSlaveInfo.dwPdSizeIn3 );
        }
        if (0 != oCfgSlaveInfo.dwPdSizeOut3)
        {
            LogMsg( "PD OUT3 Byte.Bit offset: %d.%d   Size: %d bits", oCfgSlaveInfo.dwPdOffsOut3/8, oCfgSlaveInfo.dwPdOffsOut3%8, oCfgSlaveInfo.dwPdSizeOut3 );
        }
        if (0 != oCfgSlaveInfo.dwPdSizeIn4)
        {
            LogMsg( "PD IN4  Byte.Bit offset: %d.%d   Size: %d bits", oCfgSlaveInfo.dwPdOffsIn4/8,  oCfgSlaveInfo.dwPdOffsIn4%8,  oCfgSlaveInfo.dwPdSizeIn4 );
        }
        if (0 != oCfgSlaveInfo.dwPdSizeOut4)
        {
            LogMsg( "PD OUT4 Byte.Bit offset: %d.%d   Size: %d bits", oCfgSlaveInfo.dwPdOffsOut4/8, oCfgSlaveInfo.dwPdOffsOut4%8, oCfgSlaveInfo.dwPdSizeOut4 );
        }
        if (0 != oCfgSlaveInfo.dwMbxSupportedProtocols)
        {
            LogMsg( "Protocol supported: %s%s%s%s%s%s", (oCfgSlaveInfo.dwMbxSupportedProtocols & EC_MBX_PROTOCOL_AOE)?"AoE ":"",
                                                        (oCfgSlaveInfo.dwMbxSupportedProtocols & EC_MBX_PROTOCOL_EOE)?"EoE ":"",
                                                        (oCfgSlaveInfo.dwMbxSupportedProtocols & EC_MBX_PROTOCOL_COE)?"CoE ":"",
                                                        (oCfgSlaveInfo.dwMbxSupportedProtocols & EC_MBX_PROTOCOL_FOE)?"FoE ":"",
                                                        (oCfgSlaveInfo.dwMbxSupportedProtocols & EC_MBX_PROTOCOL_SOE)?"SoE ":"",
                                                        (oCfgSlaveInfo.dwMbxSupportedProtocols & EC_MBX_PROTOCOL_VOE)?"VoE ":"" );
            LogMsg( "Mailbox size   out: %06d / in: %06d", oCfgSlaveInfo.dwMbxOutSize, oCfgSlaveInfo.dwMbxInSize  );
            if ((oCfgSlaveInfo.dwMbxOutSize2 != 0) || (oCfgSlaveInfo.dwMbxInSize2 != 0))
            {
                LogMsg( "BootMbx size   out: %06d / in: %06d", oCfgSlaveInfo.dwMbxOutSize2, oCfgSlaveInfo.dwMbxInSize2 );
            }
        }
        else
        {
            LogMsg( "Mailbox supported  : No" );
        }
#if (defined INCLUDE_HOTCONNECT)
        LogMsg("Slave is optional  : %s", ((oCfgSlaveInfo.dwHCGroupIdx == 0) ? "Yes":"No"));
        LogMsg("Slave is present   : %s", (oCfgSlaveInfo.bIsPresent ? "Yes":"No"));
        LogMsg("HC group is present: %s", (oCfgSlaveInfo.bIsPresent ? "Yes":"No"));
#endif
    }
    LogMsg( "==================================================" );
    return;
}

/***************************************************************************************************/
/**
\brief  Print bus slaves information
\return EC_TRUE on success, EC_FALSE otherwise.
*/
EC_T_VOID CEmDemoCommon::PrintBusSlavesInfo(EC_T_VOID)
{
    ::PrintBusSlavesInfo(m_dwMasterID, &m_oLogging);
}

EC_T_VOID PrintBusSlavesInfo
    (EC_T_DWORD     dwInstanceID
    ,CAtEmLogging*  poLog)
{
EC_T_DWORD          dwRes           = EC_E_ERROR;
EC_T_WORD           wAutoIncAddress = 0;
EC_T_WORD           wValue          = 0;
EC_T_INT            nLoop           = 0;
EC_T_BUS_SLAVE_INFO oBusSlaveInfo   = {0};

    LogMsg( "==================================================" );
    LogMsg( "=== Bus slaves ===================================" );
    LogMsg( "==================================================" );
    for (wAutoIncAddress = 0; wAutoIncAddress != 1; wAutoIncAddress--)
    {
        /* get bus slave information */
        dwRes = emGetBusSlaveInfo(dwInstanceID, EC_FALSE, wAutoIncAddress, &oBusSlaveInfo);
        if (EC_E_NOERROR != dwRes)
        {
            break;
        }
        LogMsg( "");
        LogMsg( "Slave #%d - (SlaveId %d):", EC_LOWORD((0 - wAutoIncAddress)), oBusSlaveInfo.dwSlaveId );
        LogMsg( "**************************************************" );

        LogMsg( "Vendor ID   : 0x%08X = %s", oBusSlaveInfo.dwVendorId, SlaveVendorText((T_eEtherCAT_Vendor)oBusSlaveInfo.dwVendorId) );
        LogMsg( "Product Code: 0x%08X = %s", oBusSlaveInfo.dwProductCode, SlaveProdCodeText((T_eEtherCAT_Vendor)oBusSlaveInfo.dwVendorId, (T_eEtherCAT_ProductCode)oBusSlaveInfo.dwProductCode) );
        LogMsg( "Revision    : 0x%08X", oBusSlaveInfo.dwRevisionNumber);
        LogMsg( "Serial      : 0x%08X", oBusSlaveInfo.dwSerialNumber );

        LogMsg( "ESC Type: %s (0x%04X)", ESCTypeText(oBusSlaveInfo.byESCType), oBusSlaveInfo.byESCType);

        LogMsg( "Bus Station Address (0x10): %04d (0x%04X)", oBusSlaveInfo.wStationAddress, oBusSlaveInfo.wStationAddress );
        LogMsg( "Bus Alias Address   (0x12): %04d (0x%04X)", oBusSlaveInfo.wAliasAddress, oBusSlaveInfo.wAliasAddress );
        if(oBusSlaveInfo.bDc64Support)
        {
            LogMsg( "DC Supported 64-Bit: %s", (oBusSlaveInfo.bDc64Support?"Yes":"No") );
        }
        else
        {
            LogMsg( "DC Supported 32-Bit: %s", (oBusSlaveInfo.bDcSupport?"Yes":"No") );
        }
        wValue = oBusSlaveInfo.wPortState;
        LogMsg("PortState: Connection: 0x%01X  Link: 0x%01X  Loop: 0x%01X  Signal: 0x%01X", ((wValue>>0x0)&0xf), ((wValue>>0x4)&0xf), ((wValue>>0x8)&0xf), ((wValue>>0xc)&0xf));
        for (nLoop=0; nLoop < 4; nLoop++)
        {
            LogMsg("Port%d    : <%s>", nLoop, (((0x0001<<nLoop)==(wValue&(0x0001<<nLoop)))?"Connected":"Not connected"));
        }
    };
    LogMsg( "==================================================" );
    return;
}

/***************************************************************************************************/
/**
\brief  Find a specific slave and return its fixed (ethercat) address
\return EC_TRUE on success, EC_FALSE otherwise.
*/
EC_T_BOOL CEmDemoCommon::FindSlaveGetFixedAddr
    (EC_T_DWORD     dwSlaveInstance     /**< [in]   Slave instance (0 = first matching, 1 = second, ...) */
    ,EC_T_DWORD     dwVendorId          /**< [in]   Vendor Id of slave to search */
    ,EC_T_DWORD     dwProductCode       /**< [in]   Product Code of slave to search */
    ,EC_T_WORD*     pwPhysAddr)         /**< [out]  Physical Address of slave */
{
    return ::FindSlaveGetFixedAddr(m_dwMasterID, &m_oLogging, dwSlaveInstance, dwVendorId, dwProductCode, pwPhysAddr);
}
EC_T_BOOL FindSlaveGetFixedAddr
    (EC_T_DWORD     dwInstanceID
    ,CAtEmLogging*  poLog
    ,EC_T_DWORD     dwSlaveInstance     /**< [in]   Slave instance (0 = first matching, 1 = second, ...) */
    ,EC_T_DWORD     dwVendorId          /**< [in]   Vendor Id of slave to search */
    ,EC_T_DWORD     dwProductCode       /**< [in]   Product Code of slave to search */
    ,EC_T_WORD*     pwPhysAddr)         /**< [out]  Physical Address of slave */
{
    EC_T_BOOL                   bRet        = EC_FALSE;
    EC_T_IOCTLPARMS             oIoctlParms = {0};
    EC_T_SB_STATUS_NTFY_DESC    oStatus     = {0};
    EC_T_DWORD                  dwRes       = EC_E_ERROR;
    EC_T_DWORD                  dwScanSpan  = 0;
    EC_T_DWORD                  dwScan      = 0;
    EC_T_WORD                   wAIAddr     = 0;
    EC_T_SB_SLAVEINFO_DESC      oSlaveInfo  = {0};
    EC_T_DWORD                  dwInstanceFound = 0;
    EC_T_BOOL                   bFound       = EC_FALSE;
    EC_T_GET_SLAVE_INFO         oSlaveInfoAll = {0};

    /* determine slave amount */
    oIoctlParms.dwInBufSize     = 0;
    oIoctlParms.pbyInBuf        = EC_NULL;
    oIoctlParms.dwOutBufSize    = sizeof(EC_T_SB_STATUS_NTFY_DESC);
    oIoctlParms.pbyOutBuf       = (EC_T_PBYTE)&oStatus;
    oIoctlParms.pdwNumOutData   = EC_NULL;
    dwRes = emIoControl(dwInstanceID, EC_IOCTL_SB_STATUS_GET, &oIoctlParms);
    if( EC_E_NOERROR != dwRes )
    {
        LogErr("Cannot get Scan Bus Status");
        goto Exit;
    }

    dwScanSpan = oStatus.dwSlaveCount;

    if( 0 == dwScanSpan )
    {                 
        goto Exit;
    }

    /* search for slave */    
    for(dwScan = 0; dwScan < dwScanSpan; dwScan++)
    {
        bFound = EC_FALSE;

        wAIAddr = (EC_T_WORD)(0-dwScan);
        
        OsMemset(&oIoctlParms, 0, sizeof(EC_T_IOCTLPARMS));
        OsMemset(&oSlaveInfo,  0, sizeof(EC_T_SB_SLAVEINFO_DESC));

        oIoctlParms.dwOutBufSize = sizeof(EC_T_SB_SLAVEINFO_DESC);
        oIoctlParms.pbyOutBuf    = (EC_T_PBYTE)&oSlaveInfo;
        oIoctlParms.pdwNumOutData= EC_NULL;
        oIoctlParms.pbyInBuf     = (EC_T_PBYTE)&wAIAddr;
        oIoctlParms.dwInBufSize  = sizeof(EC_T_WORD);
        
        dwRes = emIoControl(dwInstanceID, EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO, &oIoctlParms);
        if( (EC_E_NOTREADY == dwRes) || (EC_E_BUSY == dwRes) )
        {
            continue;
        }
        else if( EC_E_NOERROR != dwRes )
        {
            LogErr("Cannot get SlaveInfo 0x%x", dwRes);
            continue;
        }
        if( EC_E_NOERROR != oSlaveInfo.dwScanBusStatus && EC_E_NOTREADY != oSlaveInfo.dwScanBusStatus )
        {
            goto Exit;
        }

        if( (dwVendorId != oSlaveInfo.dwVendorId) || (dwProductCode != oSlaveInfo.dwProductCode) )
        {
            continue;
        }
        bFound = EC_TRUE;

        if( dwInstanceFound == dwSlaveInstance )
        {
            if( EC_NULL != pwPhysAddr )
            {
                if((dwRes = emGetSlaveInfo(dwInstanceID, EC_FALSE, wAIAddr, &oSlaveInfoAll)) != EC_E_NOERROR)
                {
                    LogErr("FindSlaveGetFixedAddr() emGetSlaveInfo return with error 0x%x", dwRes);
                }
                else
                {
                    (*pwPhysAddr) = oSlaveInfoAll.wCfgPhyAddress;
                }
            }

            break;
        } /* if( dwInstanceFound == dwInstance ) */
        dwInstanceFound++;
        bFound = EC_FALSE;
    }; /* for(dwScan = 0; dwScan < dwScanSpan; dwScan++) */
        
    if( (dwInstanceFound == dwSlaveInstance) && bFound)
    {
        bRet = EC_TRUE;
    }

Exit:
    return bRet;
}



/***************************************************************************************************/
/**
\brief  Perform Scanbus / Topology Scan.

May be called in any state.
\return EC_E_NOERROR on success, error code otherwise.
*/
EC_T_DWORD CEmDemoCommon::DoBusScan
    (EC_T_SB_STATUS_NTFY_DESC*   poStatus           /**< [out]  bus scan status */
    ,EC_T_BOOL                   bWait              /**< [in]   EC_TRUE if call shall block until the scan is finish */
    ,EC_T_BOOL                   bNtfyActive        /**< [in]   EC_TRUE if notifications are active */
    ,EC_T_DWORD                  dwScanBustimeout)  /**< [in]   Timeout value in msec */
{
    return ::DoBusScan(m_dwMasterID, &m_oLogging, poStatus, bWait, bNtfyActive, dwScanBustimeout);
}

EC_T_DWORD DoBusScan
    (EC_T_DWORD                 dwInstanceID
    ,CAtEmLogging*              poLog
    ,EC_T_SB_STATUS_NTFY_DESC*  poStatus           /**< [out]  bus scan status */
    ,EC_T_BOOL                  bWait              /**< [in]   EC_TRUE if call shall block until the scan is finish */
    ,EC_T_BOOL                  bNtfyActive        /**< [in]   EC_TRUE if notifications are active */
    ,EC_T_DWORD                 dwScanBustimeout)  /**< [in]   Timeout value in msec */
{
    EC_T_DWORD              dwRes = EC_E_NOERROR;
    EC_T_IOCTLPARMS         oIoCtlParms = {0};    
    CTimeoutHelper          oTimeout;
    EC_T_BOOL               bReady = EC_FALSE;
    
    EC_UNREFPARM(bNtfyActive);

    /* set timeout */
    oIoCtlParms.dwInBufSize     = sizeof(EC_T_DWORD);
    oIoCtlParms.pbyInBuf        = (EC_T_PBYTE)&dwScanBustimeout;
    oIoCtlParms.dwOutBufSize    = 0;
    oIoCtlParms.pbyOutBuf       = EC_NULL;
    oIoCtlParms.pdwNumOutData   = EC_NULL;
    dwRes = emIoControl(dwInstanceID, EC_IOCTL_SB_ENABLE, &oIoCtlParms);
    if( dwRes != EC_E_NOERROR)
    {
        LogErr( "Scan Bus feature could not be enabled! (Result = 0x%x)", dwRes);
        goto Exit;
    }

    oTimeout.Start(dwScanBustimeout);
    
    for( ;!oTimeout.IsElapsed() && !bReady; )
    {
        dwRes = emIoControl(dwInstanceID, EC_IOCTL_SB_RESTART, EC_NULL);
        if( dwRes == EC_E_BUSY )
        {
            //LogMsg( "DoBusScan(): master is busy when trying to start bus scan" );
            OsSleep(10);
        }
        else if( dwRes != EC_E_NOERROR )
        {
            LogErr( "DoBusScan(): Error 0x%x when restarting bus scan", dwRes );
            break;
        }
        else
        {
            oIoCtlParms.dwInBufSize     = 0;
            oIoCtlParms.pbyInBuf        = EC_NULL;
            oIoCtlParms.dwOutBufSize    = sizeof(EC_T_SB_STATUS_NTFY_DESC);
            oIoCtlParms.pbyOutBuf       = (EC_T_PBYTE)poStatus;
            oIoCtlParms.pdwNumOutData   = EC_NULL;
            for(;!oTimeout.IsElapsed() ;)
            {
                OsSleep(10);
                dwRes = emIoControl(dwInstanceID, EC_IOCTL_SB_STATUS_GET, &oIoCtlParms);
                if( dwRes != EC_E_NOERROR )
                {
                    LogErr( "DoBusScan(): Error 0x%x getting the Bus scan status", dwRes );
                    break;
                }
                dwRes = poStatus->dwResultCode;
                if(    !bWait || (   (dwRes != EC_E_BUSY) && (dwRes != EC_E_NOTREADY) ) )
                {
                    bReady = EC_TRUE;
                    break;
                }
            }
        }
    }

    if( oTimeout.IsElapsed() )
    {
        dwRes = EC_E_TIMEOUT;
    }
Exit:
    return dwRes;
}


#define VENDOR_TEXT(id, text)\
    case (id): pRet = (EC_T_CHAR*)(text);break;

/***************************************************************************************************/
/**
\brief  Slave Vendor Text.

\return Vendor Text.
*/
EC_T_CHAR* CEmDemoCommon::SlaveVendorText
    (T_eEtherCAT_Vendor EV)       /**< [in]   Vendor ID */
{
    return ::SlaveVendorText(EV);
}
EC_T_CHAR* SlaveVendorText
    (T_eEtherCAT_Vendor EV)       /**< [in]   Vendor ID */
{
    EC_T_CHAR* pRet = EC_NULL;

    switch( EV )
    {
    VENDOR_TEXT( ecvendor_etg,                              "EtherCAT Technology Group");
    VENDOR_TEXT( ecvendor_beckhoff,                         "Beckhoff Automation GmbH");
    VENDOR_TEXT( ecvendor_scuola_superiore_s_anna,          "Scuola Superiore S. Anna (PERCRO)");
    VENDOR_TEXT( ecvendor_ixxat,                            "IXXAT Automation GmbH");
    VENDOR_TEXT( ecvendor_vector_informatik,                "Vector Informatik GmbH");
    VENDOR_TEXT( ecvendor_knestel,                          "KNESTEL Technologie & Elektronik GmbH (Hardware/Product Development)");
    VENDOR_TEXT( ecvendor_cmz_sistemi,                      "CMZ Sistemi Elettronici");
    VENDOR_TEXT( ecvendor_softing,                          "Softing AG");
    VENDOR_TEXT( ecvendor_microcontrol,                     "MicroControl GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_pollmeier,                        "ESR Pollmeier");
    VENDOR_TEXT( ecvendor_lust,                             "LTI Lust");
    VENDOR_TEXT( ecvendor_kuebler,                          "Fritz Kuebler GmbH");
    VENDOR_TEXT( ecvendor_keb,                              "KEB, Karl E. Brinkmann GmbH");
    VENDOR_TEXT( ecvendor_lti,                              "LTi DRiVES GmbH");
    VENDOR_TEXT( ecvendor_esd_electronic_design,            "esd electronic system design gmbh");
    VENDOR_TEXT( ecvendor_hms_industrial_networks,          "HMS Industrial Networks AB");
    VENDOR_TEXT( ecvendor_epis_automation,                  "epis Automation GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_festo,                            "Festo AG & Co, KG (TV-N)");
    VENDOR_TEXT( ecvendor_wago,                             "WAGO Kontakttechnik GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_boschrexroth,                     "Bosch Rexroth AG");
    VENDOR_TEXT( ecvendor_moog,                             "Moog GmbH");
    VENDOR_TEXT( ecvendor_port,                             "port GmbH");
    VENDOR_TEXT( ecvendor_buerkert_werke,                   "Buerkert Werke GmbH & Co. KG (Development)");
    VENDOR_TEXT( ecvendor_lenze,                            "Lenze Drive Systems GmbH (Lenze AG)");
    VENDOR_TEXT( ecvendor_tigris_electronic,                "Tigris Electronic GmbH");
    VENDOR_TEXT( ecvendor_hilscher,                         "Hilscher GmbH");
    VENDOR_TEXT( ecvendor_murrelektronik,                   "Murrelektronik GmbH");
    VENDOR_TEXT( ecvendor_bombardier_transportation,        "Bombardier Transportation (Power Converter Solutions) Germany GmbH (PCS Germany)");          
    VENDOR_TEXT( ecvendor_komax,                            "Komax AG");
    VENDOR_TEXT( ecvendor_sew_eurodrive,                    "SEW-EURODRIVE GmbH & Co. (EETB)");
    VENDOR_TEXT( ecvendor_bachmann_electronic,              "Bachmann electronic GmbH");
    VENDOR_TEXT( ecvendor_danaher,                          "Danaher Motion GmbH");
    VENDOR_TEXT( ecvendor_kollmorgen,                       "Kollmorgen Corporation");
    VENDOR_TEXT( ecvendor_woodward_seg,                     "Woodward SEG GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_bernecker_rainer_ie,              "Bernecker + Rainer Industrie-Elektronik Ges.m.b.H");
    VENDOR_TEXT( ecvendor_infranor_electronics,             "Infranor Electronics Sas");
    VENDOR_TEXT( ecvendor_omron,                            "OMRON Corporation");
    VENDOR_TEXT( ecvendor_gefran,                           "Gefran S.P.A.");
    VENDOR_TEXT( ecvendor_elmo_motion,                      "Elmo Motion Control Ltd.");
    VENDOR_TEXT( ecvendor_sontheim_industrie_elektronik,    "Sontheim Industrie Elektronik GmbH");
    VENDOR_TEXT( ecvendor_hirschmann_automation,            "Hirschmann Automation and Control GmbH (INET)");
    VENDOR_TEXT( ecvendor_copley,                           "Copley Controls, a Division of Analogic Corporation");
    VENDOR_TEXT( ecvendor_pepperl_fuchs,                    "Pepperl+Fuchs GmbH");
    VENDOR_TEXT( ecvendor_johannes_huebner,                 "Johannes Huebner Fabrik elektrischer Maschinen GmbH");
    VENDOR_TEXT( ecvendor_abb_oy_drives,                    "ABB Oy Drives");
    VENDOR_TEXT( ecvendor_stoeber,                          "Stoeber Antriebstechnik GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_messung_systems,                  "MESSUNG SYSTEMS");
    VENDOR_TEXT( ecvendor_bonfiglioli_vectron,              "BONFIGLIOLI VECTRON GmbH");
    VENDOR_TEXT( ecvendor_phase_motion_control,             "Phase Motion Control SpA");
    VENDOR_TEXT( ecvendor_metronix,                         "Metronix Messgeraete und Elektronik GmbH");
    VENDOR_TEXT( ecvendor_ascon,                            "Ascon S.p.A. (R&D)");
    VENDOR_TEXT( ecvendor_controltechniques,                "Control Techniques");
    VENDOR_TEXT( ecvendor_maxon_motor,                      "Maxon motor");
    VENDOR_TEXT( ecvendor_keba,                             "KEBA AG (Product Development - Control)");
    VENDOR_TEXT( ecvendor_wittenstein,                      "WITTENSTEIN motion control GmbH");
    VENDOR_TEXT( ecvendor_twk_elektronik,                   "TWK-Elektronik GmbH");
    VENDOR_TEXT( ecvendor_psa_elettronica,                  "PSA Elettronica di F. Grifa");
    VENDOR_TEXT( ecvendor_smc,                              "SMC Corporation");
    VENDOR_TEXT( ecvendor_jvl_industri_elektronik,          "JVL Industri Elektronik A/S");
    VENDOR_TEXT( ecvendor_hottinger_baldwin,                "Hottinger Baldwin Messtechnik GmbH");
    VENDOR_TEXT( ecvendor_leuze_electronic,                 "Leuze electronic GmbH + Co. KG");
    VENDOR_TEXT( ecvendor_jumo,                             "Juchheim Fulda");
    VENDOR_TEXT( ecvendor_hsd,                              "HSD S.p.A");
    VENDOR_TEXT( ecvendor_lika_electronic,                  "Lika Electronic SncCSM GmbH");
    VENDOR_TEXT( ecvendor_csm,                              "CSM GmbH");
    VENDOR_TEXT( ecvendor_lpkf_motion_control,              "LPKF Motion & Control GmbH");
    VENDOR_TEXT( ecvendor_imc_messysteme,                   "imc Messysteme GmbH");
    VENDOR_TEXT( ecvendor_baumueller,                       "Baumueller Nuernberg GmbH");
    VENDOR_TEXT( ecvendor_pneumax,                          "Pneumax S.p.A.");
    VENDOR_TEXT( ecvendor_promess,                          "Promess Incorporated");
    VENDOR_TEXT( ecvendor_deutschmann,                      "Deutschmann Automation GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_golden,                           "Golden A/S (Research And Development)");
    VENDOR_TEXT( ecvendor_brunner_elektronik,               "Brunner Elektronik AG");
    VENDOR_TEXT( ecvendor_technosoft,                       "TECHNOSOFT S.A.");
    VENDOR_TEXT( ecvendor_peyer_engineering,                "Peyer Engineering");
    VENDOR_TEXT( ecvendor_robox,                            "Robox S.P.A.");
    VENDOR_TEXT( ecvendor_parker,                           "Parker?");
    VENDOR_TEXT( ecvendor_sanyo_denki,                      "Sanyo Denki Co., Ltd. (2nd Design Dept. Servo System Div.)");
    VENDOR_TEXT( ecvendor_delta_electronics,                "Delta Electronics, Inc.");
    VENDOR_TEXT( ecvendor_amk,                              "AMK GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_national_instruments,             "National Instruments (PortableDAQ)");
    VENDOR_TEXT( ecvendor_fernsteuergeraete_kurt_oelsch,    "Fernsteuergeraete Kurt Oelsch GmbH");
    VENDOR_TEXT( ecvendor_idam,                             "INA - Drives & Mechatronics GmbH & Co.oHG (Research & Development)");
    VENDOR_TEXT( ecvendor_prueftechnik_ndt,                 "PRUEFTECHNIK NDT GmbH");
    VENDOR_TEXT( ecvendor_baumer_th,                        "Baumer Thalheim?");
    VENDOR_TEXT( ecvendor_esitron_electronic,               "esitron-electronic GmbH");
    VENDOR_TEXT( ecvendor_systeme_helmholz,                 "Systeme Helmholz GmbH (Development)");
    VENDOR_TEXT( ecvendor_pantec,                           "Pantec Engineering AG");
    VENDOR_TEXT( ecvendor_abb_stotz_kontakt,                "ABB STOTZ-KONTAKT GmbH (STO-CPD)");
    VENDOR_TEXT( ecvendor_berghof_automationstechnik,       "Berghof Automationstechnik GmbH");
    VENDOR_TEXT( ecvendor_stotz_feinmesstechnik,            "Stotz Feinmesstechnik GmbH");
    VENDOR_TEXT( ecvendor_dunkermotoren,                    "Dunkermotoren GmbH");
    VENDOR_TEXT( ecvendor_roche_diagnostics,                "Roche Diagnostics AG");
    VENDOR_TEXT( ecvendor_toshiba_schneider,                "Toshiba Schneider Inverter Corporation");
    VENDOR_TEXT( ecvendor_bihl_wiedemann,                   "Bihl-Wiedemann GmbH");
    VENDOR_TEXT( ecvendor_trinamic_motion_control,          "TRINAMIC Motion Control GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_performance_motion_devices,       "Performance Motion Devices, Inc.");
    VENDOR_TEXT( ecvendor_ingenia_cat,                      "INGENIA-CAT, S.L.");
    VENDOR_TEXT( ecvendor_crevis,                           "CREVIS Co., Ltd.");
    VENDOR_TEXT( ecvendor_thk,                              "THK Co., Ltd.");
    VENDOR_TEXT( ecvendor_digitronic,                       "Digitronic Automationsanlagen GmbH");
    VENDOR_TEXT( ecvendor_hanyang,                          "HanYang System (Korea)");
    VENDOR_TEXT( ecvendor_baldor_uk,                        "Baldor UK Ltd");
    VENDOR_TEXT( ecvendor_beck_ipc,                         "Beck IPC GmbH");
    VENDOR_TEXT( ecvendor_etas,                             "ETAS GmbH");
    VENDOR_TEXT( ecvendor_phytec_messtechnik,               "PHYTEC Messtechnik GmbH");
    VENDOR_TEXT( ecvendor_anca_motion,                      "ANCA Motion Pty. Ltd");
    VENDOR_TEXT( ecvendor_fh_koeln,                         "Fachhochschule Koeln (IME-RT)");
    VENDOR_TEXT( ecvendor_nuvation_research,                "Nuvation Research Corporation");
    VENDOR_TEXT( ecvendor_tr,                               "TR-Electronic");
    VENDOR_TEXT( ecvendor_gantner,                          "Gantner Instruments GmbH");
    VENDOR_TEXT( ecvendor_mks_systems,                      "MKS Instruments (CIT Controls)");
    VENDOR_TEXT( ecvendor_abb_robotics,                     "ABB AB (Robotics)");
    VENDOR_TEXT( ecvendor_unitro_fleischmann,               "Unitro-Fleischmann");
    VENDOR_TEXT( ecvendor_zub_machine_control,              "zub machine control AG");
    VENDOR_TEXT( ecvendor_dspace,                           "dSPACE GmbH");
    VENDOR_TEXT( ecvendor_samsung,                          "Samsung Heavy Industries (Mechatronics Center)");
    VENDOR_TEXT( ecvendor_bce,                              "BCE Elektronik GmbH");
    VENDOR_TEXT( ecvendor_jaeger_messtechnik,               "Jaeger Computergesteuerte Messtechnik GmbH");
    VENDOR_TEXT( ecvendor_tetra,                            "TETRA Gesellschaft fuer Sensorik, Robotik und Automation mbH");
    VENDOR_TEXT( ecvendor_justek,                           "Justek Inc. (Research Lab.)");
    VENDOR_TEXT( ecvendor_baumer_thalheim,                  "Baumer Thalheim GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_elin_ebg_traction,                "Elin EBG Traction GmbH");
    VENDOR_TEXT( ecvendor_meka_robotics,                    "Meka Robotics");
    VENDOR_TEXT( ecvendor_altera_japan,                     "Altera Japan Ltd.");
    VENDOR_TEXT( ecvendor_ebv_elektronik,                   "EBV Elektronik GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_igh,                              "Ingenieurgemeinschaft IgH");
    VENDOR_TEXT( ecvendor_iav,                              "IAV GmbH (MD-E4)");
    VENDOR_TEXT( ecvendor_hitachi,                          "Hitachi Industrial Equipment Systems");
    VENDOR_TEXT( ecvendor_tenasys,                          "TenAsys Corp. (INtime)");
    VENDOR_TEXT( ecvendor_pondis,                           "PONDis AG");
    VENDOR_TEXT( ecvendor_moog_italiana,                    "Moog Italiana S.r.l. (Casella)");
    VENDOR_TEXT( ecvendor_wallner_automation,               "Wallner Automation");
    VENDOR_TEXT( ecvendor_avl_list,                         "AVL List GmbH (PECE)");
    VENDOR_TEXT( ecvendor_ritter_elektronik,                "RITTER-Elektronik GmbH");
    VENDOR_TEXT( ecvendor_zwick,                            "Zwick GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_dresdenelektronik,                "dresden elektronik ingenieurtechnik gmbh");
    VENDOR_TEXT( ecvendor_philips_healthcare,               "Philips Healthcare (CT Division)");
    VENDOR_TEXT( ecvendor_chess,                            "Chess B.V.");
    VENDOR_TEXT( ecvendor_nct,                              "NCT kft");
    VENDOR_TEXT( ecvendor_anywire,                          "Anywire Corporation");
    VENDOR_TEXT( ecvendor_shadow_robot,                     "Shadow Robot Company Ltd.");
    VENDOR_TEXT( ecvendor_fecon,                            "FeCon GmbH");
    VENDOR_TEXT( ecvendor_fh_suedwestfahlen,                "FH Suedwestfalen (LaborEV)");
    VENDOR_TEXT( ecvendor_add2,                             "add2 Ldt");
    VENDOR_TEXT( ecvendor_arm_automation,                   "ARM Automation, Inc.");
    VENDOR_TEXT( ecvendor_knapp_logistik,                   "KNAPP Logistik Automation GmbH (KNAPP AG)");
    VENDOR_TEXT( ecvendor_getriebebau_nord,                 "Getriebebau NORD GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_yaskawa,                          "Yaskawa Electric Corporation (Japan)");
    VENDOR_TEXT( ecvendor_oki,                              "Oki Information Systems Co., Ltd. (Japan)");
    VENDOR_TEXT( ecvendor_takasaki_kyoudou,                 "Takasaki Kyoudou Computing Center Co. (Multimedia and Audio Visual)");
    VENDOR_TEXT( ecvendor_nittetsu_elex,                    "NITTETSU ELEX Co., Ltd. (FA-System)");
    VENDOR_TEXT( ecvendor_unjo,                             "Unjo AB");
    VENDOR_TEXT( ecvendor_eads_deutschland,                 "EADS Deutschland GmbH (Military Air Systems)");
    VENDOR_TEXT( ecvendor_acs_motion_control,               "ACS Motion Control Ltd.");
    VENDOR_TEXT( ecvendor_keyence,                          "KEYENCE Corporation");
    VENDOR_TEXT( ecvendor_mefi,                             "MEFI s.r.o.");
    VENDOR_TEXT( ecvendor_mut,                              "m-u-t AG Messgeraete fuer Medizin- und Umwelttechnik");
    VENDOR_TEXT( ecvendor_isw_uni_stuttgart,                "ISW - Universitaet Stuttgart (Fachbereich C)");
    VENDOR_TEXT( ecvendor_elsena,                           "ELSENA, Inc. (TOKYO)");
    VENDOR_TEXT( ecvendor_be_semiconductor,                 "BE Semiconductor Industries N.V. (Datacon Technology GmbH)");
    VENDOR_TEXT( ecvendor_hauni_lni,                        "Hauni LNI Electronics S.A. (R&D)");
    VENDOR_TEXT( ecvendor_etel,                             "ETEL S.A. (Electronics R&D)");
    VENDOR_TEXT( ecvendor_vat_vakuumventile,                "VAT Vakuumventile AG");
    VENDOR_TEXT( ecvendor_laytec,                           "LayTec GmbH (Research&Development)");
    VENDOR_TEXT( ecvendor_num,                              "NUM AG");
    VENDOR_TEXT( ecvendor_hauni_maschinenbau,               "Hauni Maschinenbau AG");
    VENDOR_TEXT( ecvendor_exatronic,                        "Exatronic, Engenharia Electronica, Lda");
    VENDOR_TEXT( ecvendor_iim_chinese_aos,                  "Institute of Intelligent Machines, Chinese Academy of Sciences");
    VENDOR_TEXT( ecvendor_tu_eindhoven,                     "Eindhoven University of Technology (Mechanical Engineering)");
    VENDOR_TEXT( ecvendor_scansonic,                        "Scansonic MI GmbH");
    VENDOR_TEXT( ecvendor_shanghai_sodick_sw,               "Shanghai Sodick Software Co., Ltd.");
    VENDOR_TEXT( ecvendor_chuo_electronics,                 "CHUO ELECTRONICS CO., LTD");
    VENDOR_TEXT( ecvendor_agie,                             "AGIE SA");
    VENDOR_TEXT( ecvendor_hei_canton_de_vaud,               "Haute Ecoled'Ingenierie et de Gestion du Canton de Vaud (IAI - Institut d'Automatisation Industrielle de la HEIG-VD)");
    VENDOR_TEXT( ecvendor_jenny_science,                    "Jenny Science AG");
    VENDOR_TEXT( ecvendor_industrial_control_communications,"Industrial Control Communications, Inc.");  
    VENDOR_TEXT( ecvendor_ckd_elektrotechnika,              "CKD ELEKTROTECHNIKA, a.s. (Electrotechnics)");
    VENDOR_TEXT( ecvendor_qem,                              "QEM S.r.l.");
    VENDOR_TEXT( ecvendor_simatex,                          "Simatex AG (Software)");
    VENDOR_TEXT( ecvendor_kithara,                          "Kithara Software GmbH");
    VENDOR_TEXT( ecvendor_converteam,                       "Converteam GmbH");
    VENDOR_TEXT( ecvendor_ara,                              "Ara apparatenfabriek B.V.");
    VENDOR_TEXT( ecvendor_tata_consultancy,                 "Tata Consultancy Services Ltd. (Abhilash Embedded)");
    VENDOR_TEXT( ecvendor_tiab,                             "Tiab Limited");
    VENDOR_TEXT( ecvendor_rkc_instrument,                   "RKC INSTRUMENT INC.");
    VENDOR_TEXT( ecvendor_switched_reluctance,              "Switched Reluctance Drives Ltd. (Development)");
    VENDOR_TEXT( ecvendor_avnet_electronics,                "Avnet Electronics Marketing");
    VENDOR_TEXT( ecvendor_abb_force_measurement,            "ABB AB (Force Measurement)");
    VENDOR_TEXT( ecvendor_kunbus,                           "KUNBUS GmbH");
    VENDOR_TEXT( ecvendor_acd_antriebstechnik,              "ACD Antriebstechnik GmbH");
    VENDOR_TEXT( ecvendor_bronkhorst,                       "Bronkhorst High-Tech B.V.");
    VENDOR_TEXT( ecvendor_k_mecs,                           "K.MECS Co., Ltd. (Engineering Dept.)");
    VENDOR_TEXT( ecvendor_thomson_broadcast,                "Thomson Broadcast & Multimedia AG");
    VENDOR_TEXT( ecvendor_ufg_elettronica,                  "UFG Elettronica s.r.l. (Manufacturing)");
    VENDOR_TEXT( ecvendor_xilinx,                           "Xilinx Ireland (Xilinx Design Services)");
    VENDOR_TEXT( ecvendor_abb_power_systems,                "ABB AB (Power Systems)");
    VENDOR_TEXT( ecvendor_servoland,                        "Servoland Corporation");
    VENDOR_TEXT( ecvendor_hivertec,                         "Hivertec, Inc.");
    VENDOR_TEXT( ecvendor_fike_europe,                      "Fike Europe B.v.b.a.");
    VENDOR_TEXT( ecvendor_ropex,                            "ROPEX Industrie-Elektronik GmbH");
    VENDOR_TEXT( ecvendor_tlu,                              "TLU - Thueringer Leistungselektronik Union GmbH");
    VENDOR_TEXT( ecvendor_prodrive,                         "Prodrive B.V.");
    VENDOR_TEXT( ecvendor_miho_inspektionssysteme,          "miho Inspektionssysteme GmbH");
    VENDOR_TEXT( ecvendor_tokyo_electron,                   "Tokyo Electron Device Limited (PLD)");
    VENDOR_TEXT( ecvendor_lintec,                           "LINTEC CO., LTD. (Design Division2)");
    VENDOR_TEXT( ecvendor_simplex_vision,                   "Symplex Vision Systems GmbH");
    VENDOR_TEXT( ecvendor_sus,                              "SUS Corporation");
    VENDOR_TEXT( ecvendor_trsystems,                        "TRsystems GmbH (Systembereich Unidor)");
    VENDOR_TEXT( ecvendor_harmonic_drive,                   "Harmonic Drive AG");
    VENDOR_TEXT( ecvendor_staeubli_faverges,                "Staeubli Faverges SCA (Robotics)");
    VENDOR_TEXT( ecvendor_scienlab_electronic,              "ScienLab electronic systems GmbH");
    VENDOR_TEXT( ecvendor_fujisoft,                         "FUJISOFT Incorporated (Atsugi Office)");
    VENDOR_TEXT( ecvendor_iai_corporation,                  "IAI Corporation");
    VENDOR_TEXT( ecvendor_promavtomatika,                   "PromAvtomatika");
    VENDOR_TEXT( ecvendor_kistler_instrumente,              "Kistler Instrumente AG");
    VENDOR_TEXT( ecvendor_lauda_wobser,                     "LAUDA DR. R. WOBSER GmbH & Co. KG (Development / Construction)");
    VENDOR_TEXT( ecvendor_schweitzer_engineering_labs,      "Schweitzer Engineering Laboratories, Inc.");
    VENDOR_TEXT( ecvendor_mutracx,                          "Mutracx B.V.");
    VENDOR_TEXT( ecvendor_algo,                             "Algo System Co., Ltd.");
    VENDOR_TEXT( ecvendor_muehlbauer,                       "Muehlbauer AG (Construction of special-purpose machines)");
    VENDOR_TEXT( ecvendor_sealevel_systems,                 "Sealevel Systems, Inc.");
    VENDOR_TEXT( ecvendor_igm_robotersysteme,               "igm Robotersysteme AG");
    VENDOR_TEXT( ecvendor_zbe,                              "ZBE Inc.");
    VENDOR_TEXT( ecvendor_schneider_electric,               "Schneider Electric Power Drives GmbH");
    VENDOR_TEXT( ecvendor_fraunhofer_iosb_ina,              "Fraunhofer IOSB-INA Kompetenzzentrum Industrial Automation");
    VENDOR_TEXT( ecvendor_skf_magnetic_bearings,            "SKF Magnetic Bearings");
    VENDOR_TEXT( ecevndor_galil_motion_control,             "Galil Motion Control Inc.");
    VENDOR_TEXT( ecvendor_ihi,                              "IHI Corporation (Yokohama Engineering Center)");
    VENDOR_TEXT( ecvendor_wenglor_sensoric,                 "wenglor sensoric gmbh");
    VENDOR_TEXT( ecvendor_ingeteam,                         "Ingeteam Technology S.A.");
    VENDOR_TEXT( ecvendor_micro_vu,                         "Micro-Vu Corporation");
    VENDOR_TEXT( ecvendor_oehri_electronic,                 "oehri electronic ag");
    VENDOR_TEXT( ecvendor_nagano_oki,                       "Nagano Oki Electric Co., Ltd.");
    VENDOR_TEXT( ecvendor_condalo,                          "Condalo GmbH");
    VENDOR_TEXT( ecvendor_tg_drives,                        "TG Drives s.r.o.");
    VENDOR_TEXT( ecvendor_schleuniger,                      "Schleuniger AG");
    VENDOR_TEXT( ecvendor_koenig,                           "Koenig Prozessautomatisierungs gmbH");
    VENDOR_TEXT( ecvendor_shanghai_cnc,                     "Shanghai Capital Numerical Control Co., Ltd. (Research and Developement)");
    VENDOR_TEXT( ecvendor_mitsubishi,                       "Mitsubishi Electric Corporation (Nagoya Works Marketing dept. Servo system section)");
    VENDOR_TEXT( ecvendor_john_deere,                       "John Deere Werke Mannheim");
    VENDOR_TEXT( ecvendor_cantops,                          "CanTops");
    VENDOR_TEXT( ecvendor_ids,                              "IDS GmbH (Germany)");
    VENDOR_TEXT( ecvendor_adlink,                           "ADLINK TECHNOLOGY INC.");
    VENDOR_TEXT( ecvendor_eubus,                            "eubus GmbH");
    VENDOR_TEXT( ecvendor_unico,                            "Unico Inc.");
    VENDOR_TEXT( ecvendor_dlr,                              "DLR Deutsches Zentrum fuer Luft- und Raumfahrt e.V. (Institut fuer Robotik und Mechatronik)");
    VENDOR_TEXT( ecvendor_hei_canton_de_vaud_reds,          "Haute Ecole d'Ingenierie et de Gestion du Canton de Vaud du Canton de Vaud (REDS)");
    VENDOR_TEXT( ecvendor_bystorm,                          "BySTORM & CO. srl");
    VENDOR_TEXT( ecvendor_ipetronik,                        "IPETRONIK GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_sennheiser,                       "Sennheiser electronic GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_danieli_automation,               "DANIELI AUTOMATION SPA (HiPAC Platform / Flat Products)");
    VENDOR_TEXT( ecvendor_acontis,                          "acontis technologies GmbH");
    VENDOR_TEXT( ecvendor_kuka,                             "KUKA Roboter GmbH (Purchasing)");
    VENDOR_TEXT( ecvendor_nat,                              "N.A.T. GmbH");
    VENDOR_TEXT( ecvendor_mecapion,                         "Mecapion Co., Ltd. (R&D Center)");
    VENDOR_TEXT( ecvendor_ontec,                            "ONTEC CO.,LTD (R&D Center)");
    VENDOR_TEXT( ecvendor_foxnum,                           "Foxnum Technology Co., Ltd. (Production Department)");
    VENDOR_TEXT( ecvendor_kyoei,                            "Kyoei Electronics Co., Ltd. (Systems Technical Sales Department)");
    VENDOR_TEXT( ecvendor_brother,                          "Brother Industries Ltd. (Machinery & Solution Company)");
    VENDOR_TEXT( ecvendor_shenyang_machine_tool,            "Shenyang Machine Tool (Group) Design Institute Co., Ltd. (Hardware Design Department)");
    VENDOR_TEXT( ecvendor_soft_servo,                       "Soft Servo Systems, Inc");
    VENDOR_TEXT( ecvendor_vipa,                             "VIPA GmbH");
    VENDOR_TEXT( ecvendor_gd,                               "G.D SpA (R&D GD3)");
    VENDOR_TEXT( ecvendor_keba_at,                          "KEBA AG (AT)");
    VENDOR_TEXT( ecvendor_willow_garage,                    "Willow Garage, Inc.");
    VENDOR_TEXT( ecvendor_interroll,                        "Interroll Trommelmotoren GmbH (Drummotor)");
    VENDOR_TEXT( ecvendor_silica_avnet,                     "Silica, Avnet EMG GmbH");
    VENDOR_TEXT( ecvendor_altima,                           "ALTIMA Corp.");
    VENDOR_TEXT( ecvendor_kuhnke,                           "Kuhnke Automation GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_jat,                              "Jenaer Antriebstechnik GmbH");
    VENDOR_TEXT( ecvendor_heidelberger,                     "Heidelberger Druckmaschinen AG");
    VENDOR_TEXT( ecvendor_mecalc,                           "Mecalc PTY Limited");
    VENDOR_TEXT( ecvendor_samsung_sec,                      "Samsung Electronics Co. Ltd. (Mechatronics & Manufacturing Technology Center)");
    VENDOR_TEXT( ecvendor_aixcon,                           "aixcon PowerSystems GmbH");
    VENDOR_TEXT( ecvendor_kk_electronic,                    "KK-electronic a/s");
    VENDOR_TEXT( ecvendor_sick,                             "SICK AG (CD R&D)");
    VENDOR_TEXT( ecvendor_parker_hannifin,                  "Parker Hannifin SpA");
    VENDOR_TEXT( ecvendor_balluf,                           "Balluff GmbH (PC3)");
    VENDOR_TEXT( ecvendor_ma_vi,                            "Ma.Vi. srl");
    VENDOR_TEXT( ecvendor_kraeutner_software,               "Kraeutner Software Solutions (Development)");
    VENDOR_TEXT( ecvendor_parker_hannifin_eme,              "Parker Hannifin GmbH - EME (EMD Hauser)");
    VENDOR_TEXT( ecvendor_danfoss_drives,                   "Danfoss Drives A/S");
    VENDOR_TEXT( ecvendor_parker_hannifin_eme_630,          "Parker Hannifin GmbH - EME (Automation Group-SSD Drives Europe 630)");
    VENDOR_TEXT( ecvendor_parker_hannifin_ssd,              "Parker Hannifin Ltd. (SSD Drives)");
    VENDOR_TEXT( ecvendor_schneider_motion_control,         "Schneider Electric Motion Deutschland GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_fas,                              "FAS Technology Co., Ltd. (network)");
    VENDOR_TEXT( ecvendor_beckhoff_hardware,                "Beckhoff Automation GmbH (Hardware Development)");
    VENDOR_TEXT( ecvendor_hengstler,                        "Hengstler GmbH (Germany)");
    VENDOR_TEXT( ecvendor_lenord_bauer,                     "Lenord Bauer & Co.GmbH");
    VENDOR_TEXT( ecvendor_ibv,                              "IBV - Echtzeit- und Embedded GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_red_one,                          "Red one technologies (Institute of Robot research)");
    VENDOR_TEXT( ecvendor_shf_communication,                "SHF Communication Technologies AG");
    VENDOR_TEXT( ecvendor_grossenbacher,                    "Grossenbacher Systeme AG");
    VENDOR_TEXT( ecvendor_nti_linmot,                       "NTI AG / LinMot (LinMot)");
    VENDOR_TEXT( ecvendor_eltromat,                         "Eltromat GmbH (Leopoldshoehe)");
    VENDOR_TEXT( ecvendor_arte_motion,                      "Arte Motion S.p.A.");
    VENDOR_TEXT( ecvendor_paul_maschinenfabrik,             "PAUL Maschinenfabrik GmbH & Co.KG (Elektronik)");
    VENDOR_TEXT( ecvendor_handtmann,                        "Albert Handtmann Maschinenfabrik GmbH & Co. KG");
    VENDOR_TEXT( ecvendor_tamagawa_seiki,                   "TAMAGAWA SEIKI CO.,LTD."); // add

    default:                    
        if( 0xE0000000 == (((EC_T_DWORD)EV)&0xE0000000) )
        {
            pRet = SlaveVendorText( ((T_eEtherCAT_Vendor)(((EC_T_DWORD)EV)&(~0xE0000000))));
        }
        else
        {
            pRet  = (EC_T_CHAR*)"----"; break;
        }
    }

    return pRet;
}

/***************************************************************************************************/
/**
\brief  Slave Product Code Text.

\return PC Text.
*/
EC_T_CHAR* CEmDemoCommon::SlaveProdCodeText(
    T_eEtherCAT_Vendor      EV,     /**< [in]   Vendor ID */
    T_eEtherCAT_ProductCode EPC     /**< [in]   Product Code */
)
{
    return ::SlaveProdCodeText(EV, EPC);
}

EC_T_CHAR* SlaveProdCodeText(
    T_eEtherCAT_Vendor      EV,     /**< [in]   Vendor ID */
    T_eEtherCAT_ProductCode EPC     /**< [in]   Product Code */
)
{
    EC_T_CHAR*  pRet = EC_NULL;

    switch( ((T_eEtherCAT_Vendor) (((EC_T_DWORD)EV)&(~0xe0000000))) )
    {
    case ecvendor_beckhoff:
        {
            switch( EPC )
            {
            case ecprodcode_beck_AX2000_B110:   pRet  = (EC_T_CHAR*)"AX2000";  break;
                /*case ecprodcode_beck_AX2000_B110:   pRet  = (EC_T_CHAR*)"AX2000 B110";  break;*/
                /*case ecprodcode_beck_AX2000_B120:   pRet  = (EC_T_CHAR*)"AX2000 B120";  break;*/
            case ecprodcode_beck_BK1120:        pRet  = (EC_T_CHAR*)"BK1120";  break;
            case ecprodcode_beck_CX1100_0004:   pRet  = (EC_T_CHAR*)"CX1100 0004";  break;
            case ecprodcode_beck_EK1100:        pRet  = (EC_T_CHAR*)"EK1100";  break;
            case ecprodcode_beck_EK1101:        pRet  = (EC_T_CHAR*)"EK1101";  break;
            case ecprodcode_beck_EK1122:        pRet  = (EC_T_CHAR*)"EK1122";  break;
            case ecprodcode_beck_EL1002:        pRet  = (EC_T_CHAR*)"EL1002";  break;
            case ecprodcode_beck_EL1004:        pRet  = (EC_T_CHAR*)"EL1004";  break;
                /*case ecprodcode_beck_EL1004_0010:   pRet  = (EC_T_CHAR*)"EL1004 0010";  break;*/
            case ecprodcode_beck_EL1012:        pRet  = (EC_T_CHAR*)"EL1012";  break;
            case ecprodcode_beck_EL1014:        pRet  = (EC_T_CHAR*)"EL1014";  break;
                /*case ecprodcode_beck_EL1014_0010:   pRet  = (EC_T_CHAR*)"EL1014 0010";  break;*/
            case ecprodcode_beck_EL1018:        pRet  = (EC_T_CHAR*)"EL1018";  break;
            case ecprodcode_beck_EL1094:        pRet  = (EC_T_CHAR*)"EL1094";  break;
            case ecprodcode_beck_EL1904:        pRet  = (EC_T_CHAR*)"EL1904";  break;

            case ecprodcode_beck_EL2002:        pRet  = (EC_T_CHAR*)"EL2002";  break;
            case ecprodcode_beck_EL2004:        pRet  = (EC_T_CHAR*)"EL2004";  break;
            case ecprodcode_beck_EL2008:        pRet  = (EC_T_CHAR*)"EL2008";  break;
            case ecprodcode_beck_EL2032:        pRet  = (EC_T_CHAR*)"EL2032";  break;
            case ecprodcode_beck_EL2502:        pRet  = (EC_T_CHAR*)"EL2502";  break;
            case ecprodcode_beck_EL2521:        pRet  = (EC_T_CHAR*)"EL2521";  break;
                /*case ecprodcode_beck_EL2521_1001:   pRet  = (EC_T_CHAR*)"EL2521 0001";  break;*/
            case ecprodcode_beck_EL2904:        pRet  = (EC_T_CHAR*)"EL2904";  break;
            case ecprodcode_beck_EL3102:        pRet  = (EC_T_CHAR*)"EL3102";  break;
            case ecprodcode_beck_EL3112:        pRet  = (EC_T_CHAR*)"EL3112";  break;
            case ecprodcode_beck_EL3122:        pRet  = (EC_T_CHAR*)"EL3122";  break;
            case ecprodcode_beck_EL3142:        pRet  = (EC_T_CHAR*)"EL3142";  break;
                /*case ecprodcode_beck_EL3142_0010:   pRet  = (EC_T_CHAR*)"EL3142 0010";  break;*/
            case ecprodcode_beck_EL3152:        pRet  = (EC_T_CHAR*)"EL3152";  break;
            case ecprodcode_beck_EL3162:        pRet  = (EC_T_CHAR*)"EL3162";  break;
            case ecprodcode_beck_EL3202:        pRet  = (EC_T_CHAR*)"EL3202";  break;
            case ecprodcode_beck_EL3312:        pRet  = (EC_T_CHAR*)"EL3312";  break;
            case ecprodcode_beck_EL4002:        pRet  = (EC_T_CHAR*)"EL4002";  break;
            case ecprodcode_beck_EL4102:        pRet  = (EC_T_CHAR*)"EL4102";  break;
            case ecprodcode_beck_EL4112:        pRet  = (EC_T_CHAR*)"EL4112";  break;
                /*case ecprodcode_beck_EL4112_0010:   pRet  = (EC_T_CHAR*)"EL4112 0010";  break;*/
            case ecprodcode_beck_EL4122:        pRet  = (EC_T_CHAR*)"EL4122";  break;
            case ecprodcode_beck_EL4132:        pRet  = (EC_T_CHAR*)"EL4132";  break;
            case ecprodcode_beck_EL5001:        pRet  = (EC_T_CHAR*)"EL5001";  break;
            case ecprodcode_beck_EL5101:        pRet  = (EC_T_CHAR*)"EL5101";  break;
            case ecprodcode_beck_EL5151:        pRet  = (EC_T_CHAR*)"EL5151";  break;
            case ecprodcode_beck_EL6001:        pRet  = (EC_T_CHAR*)"EL6001";  break;
            case ecprodcode_beck_EL6021:        pRet  = (EC_T_CHAR*)"EL6021";  break;
            case ecprodcode_beck_EL6601:        pRet  = (EC_T_CHAR*)"EL6601";  break;
            case ecprodcode_beck_EL6690:        pRet  = (EC_T_CHAR*)"EL6690";  break;
            case ecprodcode_beck_EL6731:        pRet  = (EC_T_CHAR*)"EL6731";  break;
                /*case ecprodcode_beck_EL6731_0010:   pRet  = (EC_T_CHAR*)"EL6731 0010";  break;*/
            case ecprodcode_beck_EL6751:        pRet  = (EC_T_CHAR*)"EL6751";  break;
            case ecprodcode_beck_EL6752:        pRet  = (EC_T_CHAR*)"EL6752";  break;

            case ecprodcode_beck_EL6900:        pRet  = (EC_T_CHAR*)"EL6900";  break;
            case ecprodcode_beck_EL9800:        pRet  = (EC_T_CHAR*)"EL9800";  break;
            case ecprodcode_beck_EL9820:        pRet  = (EC_T_CHAR*)"EL9820";  break;
            case ecprodcode_beck_FM5001:        pRet  = (EC_T_CHAR*)"FM5001";  break;
                
            case ecprodcode_beck_EK1110:        pRet  = (EC_T_CHAR*)"EK1110";  break;
            case ecprodcode_beck_EL2252:        pRet  = (EC_T_CHAR*)"EL2252";  break;
            case ecprodcode_beck_EL1252:        pRet  = (EC_T_CHAR*)"EL1252";  break;
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_scuola_superiore_s_anna:
        {
            pRet  = (EC_T_CHAR*)"Unknown";
        } break;
    case ecvendor_ixxat:
        {
            switch( EPC )
            {
            case ecprodcode_ixx_iem:            pRet  = (EC_T_CHAR*)"IEM";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_pollmeier:
        {
            switch( EPC )
            {
            case ecprodcode_esr_Trio_1:
            case ecprodcode_esr_Trio:           pRet  = (EC_T_CHAR*)"TrioDrive";  break;              
            case ecprodcode_esr_Midi:           pRet  = (EC_T_CHAR*)"MidiDrive";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_lust:
        {
            pRet  = (EC_T_CHAR*)"Unknown";
        } break;
    case ecvendor_kuebler:
        {
            switch( EPC )
            {
            case ecprodcode_kuebler_Multiturn5868:
                pRet  = (EC_T_CHAR*)"Multiturn 5868";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_keb:
        {
            switch( EPC )
            {
            case ecprodcode_keb_KEB1736F5_3d:   pRet  = (EC_T_CHAR*)"KEB1736F5 3.d";  break;
            case ecprodcode_keb_EcatGateway:    pRet  = (EC_T_CHAR*)"KEB ECAT Gateway"; break;
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_moog:
        {
            switch( EPC )
            {             
            case ecprodcode_moog_anin:          pRet  = (EC_T_CHAR*)"Analogue In"; break;
            case ecprodcode_moog_ServoValveD671:pRet  = (EC_T_CHAR*)"Servo Valve D671";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_lenze:
        {
            switch( EPC )
            {
            case ecprodcode_ldc_el9400:         pRet  = (EC_T_CHAR*)"EL9400 L-Force Servo Drive"; break;
            case ecprodcode_ldc_e94aycet:       pRet  = (EC_T_CHAR*)"E94AYCET L-Force Servo Drive"; break;
            case ecprodcode_ldc_servogun2:      pRet  = (EC_T_CHAR*)"2 Axis Servo Gun"; break;
            case ecprodcode_ldc_servogun3:      pRet  = (EC_T_CHAR*)"3 Axis Servo Gun"; break;
            case ecprodcode_ldc_epms130:        pRet  = (EC_T_CHAR*)"EPM-S130"; break;
            case ecprodcode_ldc_el8400:         pRet  = (EC_T_CHAR*)"EL8400 L-Force Servo Drive"; break;
            case ecprodcode_ldc_stateline:      pRet  = (EC_T_CHAR*)"Stateline L-Force Servo Drive"; break;
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_hilscher:
        {
            switch( EPC )
            {
            case ecprodcode_hil_NXSB100:        pRet  = (EC_T_CHAR*)"NX SB100";  break;              
            case ecprodcode_hil_NXSB100DC:      pRet  = (EC_T_CHAR*)"NX SB100 DC";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_komax:
        {
            switch( EPC )
            {
            case ecprodcode_sh_ACSE:            pRet  = (EC_T_CHAR*)"ACSE";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };;
        } break;
    case ecvendor_sew_eurodrive:
        {
            switch( EPC )
            {
            case ecprodcode_sew_movidrive:      pRet  = (EC_T_CHAR*)"Movi Drive";  break;              
            case ecprodcode_sew_moviaxis:       pRet  = (EC_T_CHAR*)"Movi Axis";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_danaher:
        {
            switch( EPC )
            {
            case ecprodcode_dan_servostar300:   pRet  = (EC_T_CHAR*)"ServoStar 300";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_controltechniques:
        {
            switch( EPC )
            {
            case ecprodcode_ct_drive:           pRet  = (EC_T_CHAR*)"Emerson Drive";  break;              
            case ecprodcode_ct_drive_sp:        pRet  = (EC_T_CHAR*)"Emerson Drive SP";  break; 
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_smc:
        {
            switch( EPC )
            {
            case ecprodcode_smc_serifcunit:     pRet  = (EC_T_CHAR*)"Serif C Unit";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_jumo:
        {
            switch( EPC )
            {
            case ecprodcode_jumo_Basis:         pRet  = (EC_T_CHAR*)"Basismodul";  break;              
            case ecprodcode_jumo_Busskoppler:   pRet  = (EC_T_CHAR*)"Busskoppler";  break;              
            case ecprodcode_jumo_HMI:           pRet  = (EC_T_CHAR*)"HMI";  break;              
            case ecprodcode_jumo_Router:        pRet  = (EC_T_CHAR*)"Router";  break;              
            case ecprodcode_jumo_BIO:           pRet  = (EC_T_CHAR*)"Basic I/O";  break;              
            case ecprodcode_jumo_RELAIS:        pRet  = (EC_T_CHAR*)"Relais";  break;              
            case ecprodcode_jumo_REGLER:        pRet  = (EC_T_CHAR*)"Regler";  break;              
            case ecprodcode_jumo_Analog_4_Ein:  pRet  = (EC_T_CHAR*)"4 x Analog Eingang";  break;              
            case ecprodcode_jumo_Analog_8_Ein:  pRet  = (EC_T_CHAR*)"8 x Analog Eingang";  break;              
            case ecprodcode_jumo_Analog_4_Aus:  pRet  = (EC_T_CHAR*)"4 x Analog Ausgang";  break;              
            case ecprodcode_jumo_Analog_8_Aus:  pRet  = (EC_T_CHAR*)"8 x Analog Ausgang";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_baumueller:
        {
            switch( EPC )
            {
            case ecprodcode_baumueller_BM3000:  pRet  = (EC_T_CHAR*)"BM3000";  break;              
            case ecprodcode_baumueller_BM4000:  pRet  = (EC_T_CHAR*)"BM4000";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_deutschmann:
        {
            switch( EPC )
            {
            case ecprodcode_dm_rs232gw:         pRet  = (EC_T_CHAR*)"RS232 Gateway";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_parker:
        {
            switch( EPC )
            {
            case ecprodcode_par_drive:          pRet  = (EC_T_CHAR*)"Drive";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_national_instruments:
        {
            switch( EPC )
            {
            case ecprodcode_ni_digio:           pRet  = (EC_T_CHAR*)"Digital I/O";  break;              
            case ecprodcode_ni_anaio:           pRet  = (EC_T_CHAR*)"Analog I/O";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_idam:
        {
            switch( EPC )
            {
            case ecprodcode_idam_DSMRW:         pRet  = (EC_T_CHAR*)"DSMRW";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_baumer_th:
        {
            pRet  = (EC_T_CHAR*)"Unknown";
        } break;
    case ecvendor_tr:
        {
            switch( EPC )
            {
            case ecprodcode_tr_linencoder2:     pRet  = (EC_T_CHAR*)"Line Encoder 2";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_bce:
        {
            switch( EPC )
            {
            case ecprodcode_bce_AMAT_Handbox:   pRet  = (EC_T_CHAR*)"AMAT Handbox";  break;              
            case ecprodcode_bce_AMAT_HB_digin:  pRet  = (EC_T_CHAR*)"AMAT Handbox Digital In";  break;              
            case ecprodcode_bce_AMAT_HB_digout: pRet  = (EC_T_CHAR*)"AMAT Handbox Digital Out";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_koenig:
        {
            switch( EPC )
            {
            case ecprodcode_kng_pc104:          pRet  = (EC_T_CHAR*)"PC104";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_acontis:
        {
            switch( EPC )
            {
            case ecprodcode_at_atem:            pRet  = (EC_T_CHAR*)"EtherCAT Master"; break;
            case ecprodcode_at_atemTestSlave:   pRet  = (EC_T_CHAR*)"EtherCAT TestSlave"; break;
            default:                            pRet  = (EC_T_CHAR*)"Unknown"; break;
            }
        } break;
    case ecvendor_kuka:
        {
            switch( EPC )
            {
            case ecprodcode_kr_cib:             pRet  = (EC_T_CHAR*)"CIB"; break;
            case ecprodcode_kr_rdc3:            pRet  = (EC_T_CHAR*)"RDC 3 angle resolver"; break;
            case ecprodcode_kr_cibsion:         pRet  = (EC_T_CHAR*)"CIB-SION"; break;
            case ecprodcode_kr_sionkpp:         pRet  = (EC_T_CHAR*)"SION-KPP"; break;
            case ecprodcode_kr_sionksp:         pRet  = (EC_T_CHAR*)"SION-KSP"; break;
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_kuhnke:
        {
            switch( EPC )
            {
            case ecprodcode_kuh_VFIO_BK:        pRet  = (EC_T_CHAR*)"BK";  break;              
            case ecprodcode_kuh_VFIO_DIO:       pRet  = (EC_T_CHAR*)"Digital I/O";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_jat:
        {
            switch( EPC )
            {
            case ecprodcode_jat_drive1:         pRet  = (EC_T_CHAR*)"Drive"; break;
            default:                            pRet  = (EC_T_CHAR*)"Unknown"; break;
            }
        } break;
    case ecvendor_festo:
        {
            switch( EPC )
            {
            case ecprodcode_fst_cpx:            pRet  = (EC_T_CHAR*)"CPX"; break;
            default:                            pRet  = (EC_T_CHAR*)"Unknown"; break;
            }
        } break;
    case ecvendor_copley:
        {
            switch( EPC )
            {
            case ecprodcode_cpl_accelnet:       pRet  = (EC_T_CHAR*)"Accelnet Servo Drive";  break;
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_robox:
        {
            switch( EPC )
            {
            case ecprodcode_rx_coedrivegw:      pRet  = (EC_T_CHAR*)"CoE Drive Gateway";  break;
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            }
        } break;
    case ecvendor_dresdenelektronik:
        {
            switch( EPC )
            {
            case ecprodcode_de_sdac3100:        pRet  = (EC_T_CHAR*)"de axis controller 3100";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_yaskawa:
        {
            switch( EPC )
            {
            case ecprodcode_yas_sgdv_e1:        pRet  = (EC_T_CHAR*)"SGDV-E1 CoE Drive";  break;              
            default:                            pRet  = (EC_T_CHAR*)"Unknown";  break;
            };
        } break;
    case ecvendor_hitachi: /* add */
		{
            switch( EPC )
            {
            case ecprodcode_hitachi_Slave:      pRet  = (EC_T_CHAR*)"Hitachi eval slave-unit";  break; /* add */
            default:                            pRet  = (EC_T_CHAR*)"Unknown Product Code";  break;
            };
        } break;
    case ecvendor_omron: /* add */
		{
            switch( EPC )
            {
            case ecprodcode_omron_Slave:        pRet  = (EC_T_CHAR*)"R88D-KNA5L-ECT";  break; /* add */
            default:                            pRet  = (EC_T_CHAR*)"Unknown Product Code";  break;
            };
        } break;
    case ecvendor_maxon_motor: /* add */
		{
            switch( EPC )
            {
            case ecprodcode_maxon_EPOS3:		pRet  = (EC_T_CHAR*)"EPOS3 70/10 EtherCAT";  break; /* add */
            default:                            pRet  = (EC_T_CHAR*)"Unknown Product Code";  break;
            };
        } break;
    case ecvendor_tamagawa_seiki: /* add */
		{
			switch( EPC )
			{
			case ecprodcode_tamagawa_TA8499: 	pRet  = (EC_T_CHAR*)"TA8499";  break; /* add */
			default:                            pRet  = (EC_T_CHAR*)"Unknown Product Code";  break;
			};
		} break;
    case ecvendor_soft_servo: /* add */
		{
			switch( EPC )
			{
			case ecprodcode_YNL_MD4KW_3M:
			  pRet  = (EC_T_CHAR*)"MD4KW_3M";
			  break; /* add */
			case ecprodcode_YNL_MD4KW_2MFS:
			  pRet  = (EC_T_CHAR*)"MD4KW_2MFS";
			  break; /* add */
			case ecprodcode_YNL_MD4KW_Hand:
			  pRet  = (EC_T_CHAR*)"MD4KW_Hand";
			  break; /* add */
			case ecprodcode_YNL_MD4KW_IMU:
			  pRet  = (EC_T_CHAR*)"MD4KW_IMU";
			  break; /* add */
			default:
			  pRet  = (EC_T_CHAR*)"Unknown Product Code";  break;
			};
		} break;
    default:									pRet  = (EC_T_CHAR*)"Unknown Product Code";  break;
    }

    return pRet;
};

/***************************************************************************************************/
/**
\brief  Create ESC Type string from Type.

\return Name of ESC type.
*/
EC_T_CHAR* CEmDemoCommon::ESCTypeText(
    EC_T_BYTE   byESCType   /**< [in]   ESC Type value from Slave Register Offset 0x00 */
                                     )
{
    return ::ESCTypeText(byESCType);
}

EC_T_CHAR* ESCTypeText(
    EC_T_BYTE   byESCType   /**< [in]   ESC Type value from Slave Register Offset 0x00 */
                                     )
{
    EC_T_CHAR*      pszRet = EC_NULL;

    switch( byESCType )
    {
    case ESCTYPE_BKHF_ELOLD:            pszRet  = (EC_T_CHAR*)"Beckhoff ELs (Old)"; break;
    case ESCTYPE_ESC10|ESCTYPE_ESC20:   pszRet  = (EC_T_CHAR*)"Beckhoff FPGA ESC 10/20"; break;
    case ESCTYPE_IPCORE:                pszRet  = (EC_T_CHAR*)"IPCORE"; break;
    case ESCTYPE_ET1100:                pszRet  = (EC_T_CHAR*)"Beckhoff ET1100"; break;
    case ESCTYPE_ET1200:                pszRet  = (EC_T_CHAR*)"Beckhoff ET1200"; break;
    case ESCTYPE_NETX100_500:           pszRet  = (EC_T_CHAR*)"Hilscher NetX 100/500"; break;
    case ESCTYPE_NETX50:                pszRet  = (EC_T_CHAR*)"Hilscher NetX 50"; break;
    case ESCTYPE_NETX5:                 pszRet  = (EC_T_CHAR*)"Hilscher NetX 5"; break;
    case ESCTYPE_NETX51_52:             pszRet  = (EC_T_CHAR*)"Hilscher NetX 51/52"; break;
    case ESCTYPE_TI_SITARA:             pszRet  = (EC_T_CHAR*)"TI Sitara"; break;
    case ESCTYPE_RENESAS_IN32M3:        pszRet  = (EC_T_CHAR*)"Renesas IN 32 M3"; break;
    case ESCTYPE_HMS_RES:               pszRet  = (EC_T_CHAR*)"HMS"; break;
    default:                            pszRet  = (EC_T_CHAR*)"Unknown Slave Controller"; break;
    }

    return pszRet;
}




#define CRODLError   poLog->LogError
#undef  LogMsg
#undef  LogMsgAdd
#define CRODLMsg     poLog->LogMsg
#define CRODLMsgAdd  poLog->LogMsgAdd

#if (defined INCLUDE_MASTER_OBD)
/***************************************************************************************************/
/**
\brief  Parse DIAG Message.
*/
EC_T_VOID ParseDiagMsg(
    CAtEmLogging*           poLog,      /**< [in]   Logging Instance */
    EC_T_VOID*              pvDiag)     /**< [in]   pointer to DIAG Message Struct */
{
    static
    EC_T_CHAR               szOutPut[0x200];
    EC_T_CHAR*              pszFormat       = EC_NULL;
    EC_T_CHAR*              pszWork         = EC_NULL;
    EC_T_OBJ10F3_DIAGMSG*   pDiag           = (EC_T_OBJ10F3_DIAGMSG*)pvDiag;
    EC_T_DWORD              dwParse         = 0;
    EC_T_DWORD              dwParseLimit    = 0;
    EC_T_BYTE*              pbyParamPtr     = EC_NULL;
    EC_T_WORD               wParmFlags      = 0;
    EC_T_WORD               wParmSize       = 0;
    EC_T_CHAR*              pszSeverity     = EC_NULL;

    if( EC_NULL == pDiag )
    {
        goto Exit;
    }

    OsMemset(szOutPut, 0, sizeof(szOutPut));
    
    pszFormat = ecatGetText(pDiag->wTextId);

    if( EC_NULL == pszFormat )
    {
        goto Exit;
    }

    switch( pDiag->wFlags )
    {
    case DIAGFLAGINFO:  pszSeverity  = (EC_T_CHAR*)"INFO"; break;
    case DIAGFLAGWARN:  pszSeverity  = (EC_T_CHAR*)"WARN"; break;
    case DIAGFLAGERROR: pszSeverity  = (EC_T_CHAR*)" ERR"; break;
    default:            pszSeverity  = (EC_T_CHAR*)" UNK"; break;
    }

    dwParseLimit = (EC_T_DWORD) OsStrlen(pszFormat);
    pszWork = szOutPut;

    pbyParamPtr = pDiag->byParams;

    for( dwParse = 0; dwParse < dwParseLimit; )
    {
        switch(pszFormat[0])
        {
        case '%':
            {
                /* param */
                pszFormat++;
                dwParse++;
                if( pszFormat[0] == 'l' || pszFormat[0] == 'L' )
                {
                    pszFormat++;
                    dwParse++;
                }

                switch( pszFormat[0] )
                {
                case '%':
                    {
                        pszWork[0] = pszFormat[0];
                        pszWork++; 
                        pszFormat++;
                        dwParse++;
                    } break;
                case 's':
                case 'S':
                    {
                        wParmFlags = EC_GETWORD(pbyParamPtr);

                        switch( (wParmFlags)&(0xF<<12) )
                        {
                        case DIAGPARMTYPEASCIISTRG:
                        case DIAGPARMTYPEBYTEARRAY:
                            {
                                wParmSize = (EC_T_WORD)(wParmFlags&0xFFF);
                                pbyParamPtr += sizeof(EC_T_WORD);

                                OsMemcpy(pszWork, pbyParamPtr, (wParmSize*sizeof(EC_T_BYTE)));
                                pszWork += (wParmSize*sizeof(EC_T_BYTE));
                                pbyParamPtr += wParmSize;
                                pszFormat++;
                                dwParse++;
                            } break;
                        case DIAGPARMTYPEUNICODESTRG:
                            {
                                wParmSize = (EC_T_WORD)(wParmFlags&0xFFF);
                                pbyParamPtr += sizeof(EC_T_WORD);
                                
                                OsMemcpy(pszWork, pbyParamPtr, (wParmSize*sizeof(EC_T_WORD)));
                                pszWork += (wParmSize*sizeof(EC_T_WORD));
                                pbyParamPtr += wParmSize;
                                pszFormat++;
                                dwParse++;
                            } break;
                        case DIAGPARMTYPETEXTID:
                            {
                                EC_T_CHAR*  pszTextFromId   = EC_NULL;

                                pbyParamPtr += sizeof(EC_T_WORD);
                                pszTextFromId = ecatGetText(EC_GETWORD(pbyParamPtr));
                                if( EC_NULL == pszTextFromId )
                                {
                                    pszWork[0] = '%';
                                    pszWork[1] = pszFormat[0];
                                    pszWork +=2;
                                    pszFormat++;
                                    dwParse++;
                                    break;
                                }
                                OsMemcpy(pszWork, pszTextFromId, OsStrlen(pszTextFromId));
                                pszWork += OsStrlen(pszTextFromId);
                                pbyParamPtr += sizeof(EC_T_WORD);
                                pszFormat++;
                                dwParse++;
                            } break;
                        default:
                            {
                                pszWork[0] = '%';
                                pszWork[1] = pszFormat[0];
                                pszWork +=2;
                                pszFormat++;
                                dwParse++;
                            } break;
                        }
                    } break;
                case 'd':
                    {
                        wParmFlags = EC_GETWORD(pbyParamPtr);
                        
                        switch( (wParmFlags)&(0xF<<12) )
                        {
                        case DIAGPARMTYPEDATATYPE:
                            {
                                switch(wParmFlags&0xFFF)
                                {
                                case DEFTYPE_INTEGER8:
                                case DEFTYPE_UNSIGNED8:
                                    {
                                        wParmSize = sizeof(EC_T_BYTE);
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 4, "%d", pbyParamPtr[0]);
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                case DEFTYPE_INTEGER16:
                                case DEFTYPE_UNSIGNED16:
                                    {
                                        wParmSize = sizeof(EC_T_WORD);
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 7, "%d", EC_GETWORD(pbyParamPtr));
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                case DEFTYPE_BOOLEAN:
                                case DEFTYPE_INTEGER32:
                                case DEFTYPE_UNSIGNED32:
                                    {
                                        wParmSize = sizeof(EC_T_DWORD);
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 11, "%ld", EC_GETDWORD(pbyParamPtr));
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                case DEFTYPE_UNSIGNED24:
                                    {
                                        wParmSize = (3*sizeof(EC_T_BYTE));
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 11, "%ld", EC_GETDWORD(pbyParamPtr));
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                default:
                                    {
                                        pszWork[0] = '%';
                                        pszWork[1] = pszFormat[0];
                                        pszWork +=2;
                                        pszFormat++;
                                        dwParse++;
                                    };
                                }
                            } break;
                        default:
                            {
                                pszWork[0] = '%';
                                pszWork[1] = pszFormat[0];
                                pszWork +=2;
                                pszFormat++;
                                dwParse++;
                            } break;
                        }
                    } break;
                case 'x':
                    {
                        wParmFlags = EC_GETWORD(pbyParamPtr);
                        
                        switch( (wParmFlags)&(0xF<<12) )
                        {
                        case DIAGPARMTYPEDATATYPE:
                            {
                                switch(wParmFlags&0xFFF)
                                {
                                case DEFTYPE_INTEGER8:
                                case DEFTYPE_UNSIGNED8:
                                    {
                                        wParmSize = sizeof(EC_T_BYTE);
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 3, "%x", pbyParamPtr[0]);
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                case DEFTYPE_INTEGER16:
                                case DEFTYPE_UNSIGNED16:
                                    {
                                        wParmSize = sizeof(EC_T_WORD);
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 5, "%x", EC_GETWORD(pbyParamPtr));
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                case DEFTYPE_BOOLEAN:
                                case DEFTYPE_INTEGER32:
                                case DEFTYPE_UNSIGNED32:
                                    {
                                        wParmSize = sizeof(EC_T_DWORD);
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 9, "%lx", EC_GETDWORD(pbyParamPtr));
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                case DEFTYPE_UNSIGNED24:
                                    {
                                        wParmSize = (3*sizeof(EC_T_BYTE));
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 7, "%lx", EC_GETDWORD(pbyParamPtr));
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                default:
                                    {
                                        pszWork[0] = '%';
                                        pszWork[1] = pszFormat[0];
                                        pszWork +=2;
                                        pszFormat++;
                                        dwParse++;
                                    };
                                }
                            } break;
                        default:
                            {
                                pszWork[0] = '%';
                                pszWork[1] = pszFormat[0];
                                pszWork +=2;
                                pszFormat++;
                                dwParse++;
                            } break;
                        }
                    } break;
                case 'X':
                    {
                        wParmFlags = EC_GETWORD(pbyParamPtr);
                        
                        switch( (wParmFlags)&(0xF<<12) )
                        {
                        case DIAGPARMTYPEDATATYPE:
                            {
                                switch(wParmFlags&0xFFF)
                                {
                                case DEFTYPE_INTEGER8:
                                case DEFTYPE_UNSIGNED8:
                                    {
                                        wParmSize = sizeof(EC_T_BYTE);
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 3, "%X", pbyParamPtr[0]);
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                case DEFTYPE_INTEGER16:
                                case DEFTYPE_UNSIGNED16:
                                    {
                                        wParmSize = sizeof(EC_T_WORD);
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 5, "%X", EC_GETWORD(pbyParamPtr));
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                case DEFTYPE_BOOLEAN:
                                case DEFTYPE_INTEGER32:
                                case DEFTYPE_UNSIGNED32:
                                    {
                                        wParmSize = sizeof(EC_T_DWORD);
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 9, "%lX", EC_GETDWORD(pbyParamPtr));
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                case DEFTYPE_UNSIGNED24:
                                    {
                                        wParmSize = (3*sizeof(EC_T_BYTE));
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        OsSnprintf(pszWork, 7, "%lX", EC_GETDWORD(pbyParamPtr));
                                        pszWork += OsStrlen(pszWork);
                                        pbyParamPtr += wParmSize;
                                        pszFormat++;
                                        dwParse++;
                                    } break;
                                default:
                                    {
                                        pszWork[0] = '%';
                                        pszWork[1] = pszFormat[0];
                                        pszWork +=2;
                                        pszFormat++;
                                        dwParse++;
                                    };
                                }
                            } break;
                        default:
                            {
                                pszWork[0] = '%';
                                pszWork[1] = pszFormat[0];
                                pszWork +=2;
                                pszFormat++;
                                dwParse++;
                            } break;
                        }
                    } break;
                case 'C':
                case 'c':
                    {
                        wParmFlags = EC_GETWORD(pbyParamPtr);

                        switch( (wParmFlags)&(0xF<<12) )
                        {
                        case DIAGPARMTYPEDATATYPE:
                            {
                                switch(wParmFlags&0xFFF)
                                {
                                case DEFTYPE_INTEGER8:
                                case DEFTYPE_UNSIGNED8:
                                    {
                                        wParmSize = sizeof(EC_T_BYTE);
                                        pbyParamPtr += sizeof(EC_T_WORD);
                                        pszWork[0] = pbyParamPtr[0];
                                        pszWork ++;
                                        pbyParamPtr ++;
                                        pszFormat ++;
                                        dwParse++;
                                    } break;
                                default:
                                    {
                                        pszWork[0] = '%';
                                        pszWork[1] = pszFormat[0];
                                        pszWork +=2;
                                        pszFormat++;
                                        dwParse++;
                                    };
                                }
                            } break;
                        default:
                            {
                                pszWork[0] = '%';
                                pszWork[1] = pszFormat[0];
                                pszWork +=2;
                                pszFormat++;
                                dwParse++;
                            } break;
                        }
                    } break;
                default:
                    {
                        pszFormat++;
                        dwParse++;
                    } break;
                }
            } break;
        default:
            {
                /* normal char */
                pszWork[0] = pszFormat[0];
                pszWork++; 
                pszFormat++;
                dwParse++;
            } break;
        }
    }

    CRODLMsg("DIAG(%s): %s", pszSeverity, szOutPut);

Exit:
    return;
}

#endif

/********************************************************************************/
/** \brief  Read object dictionary.
*
* This function reads the CoE object dictionary.
* 
* \return EC_E_NOERROR on success, error code otherwise.
*/
EC_T_DWORD CoeReadObjectDictionary
   (EC_T_DWORD              dwInstanceID 
   ,CAtEmLogging*           poLog               /**< [in]   Logging Instance */
   ,EC_T_INT                nVerbosePrinting   /**< [in]   Print messages */
   ,EC_T_BOOL*              pbStopReading   /**< [in]   Pointer to shutdwon flag */
   ,EC_T_DWORD              dwClientId      /**< [in]   Current client ID */
   ,EC_T_DWORD              dwNodeId        /**< [in]   Slave Id to query ODL from  */
   ,EC_T_BOOL               bPerformUpload  /**< [in]   EC_TRUE: do SDO Upload */
   ,EC_T_DWORD              dwTimeout       /**< [in]   Individual call timeout */
                                  )
{
    /* buffer sizes */
#define CROD_ODLTFER_SIZE       ((EC_T_DWORD)0x1100)    /* 0x4000 ? */
#define CROD_OBDESC_SIZE        ((EC_T_DWORD)100)
#define CROD_ENTRYDESC_SIZE     ((EC_T_DWORD)100)
#define CROD_MAXSISDO_SIZE      ((EC_T_DWORD)0x200)
#define MAX_OBNAME_LEN          ((EC_T_DWORD)100)

    /* variables */
    EC_T_DWORD          dwRetVal                = EC_E_ERROR;   /* return value */
    EC_T_DWORD          dwRes                   = EC_E_ERROR;   /* tmp return value for API calls */
    EC_T_BYTE*          pbyODLTferBuffer        = EC_NULL;      /* OD List */
    EC_T_BYTE*          pbyOBDescTferBuffer     = EC_NULL;      /* OB Desc */
    EC_T_BYTE*          pbyGetEntryTferBuffer   = EC_NULL;      /* Entry Desc */
    EC_T_MBXTFER_DESC   MbxTferDesc             = {0};          /* mailbox transfer descriptor */
    EC_T_MBXTFER*       pMbxGetODLTfer          = EC_NULL;      /* mailbox transfer object for OD list upload */
    EC_T_MBXTFER*       pMbxGetObDescTfer       = EC_NULL;      /* mailbox transfer object for Object description upload */
    EC_T_MBXTFER*       pMbxGetEntryDescTfer    = EC_NULL;      /* mailbox transfer object for Entry description upload */

    EC_T_WORD*          pwODList                = EC_NULL;      /* is going to carry ALL list of OD */
    EC_T_WORD           wODListLen              = 0;            /* used entries in pwODList */
    EC_T_WORD           wIndex                  = 0;            /* index to parse OD List */

    EC_T_BOOL           bExecutionCancelled     = EC_FALSE;     /* if Master is no more accessible, Mailboxes cannot be deleted, API must not be called */
    EC_T_BYTE           byValueInfoType         = 0;
    EC_T_DWORD          dwUniqueTransferId      = 0;
    EC_T_BOOL           bReadingMasterOD        = EC_FALSE;
    
    /* Check Parameters */
    if(  (EC_NULL == poLog)
       ||(EC_NULL == pbStopReading) 
       ||(EC_NOWAIT == dwTimeout) 
      )
    {
        dwRetVal = EC_E_INVALIDPARM;
        goto Exit;
    }

    /* Create Memory */
    pbyODLTferBuffer        = (EC_T_BYTE*)OsMalloc(CROD_ODLTFER_SIZE);
    pbyOBDescTferBuffer     = (EC_T_BYTE*)OsMalloc(CROD_OBDESC_SIZE);
    pbyGetEntryTferBuffer   = (EC_T_BYTE*)OsMalloc(CROD_ENTRYDESC_SIZE);

    /* check if alloc was ok */
    if( 
        (EC_NULL == pbyODLTferBuffer) 
     || (EC_NULL == pbyOBDescTferBuffer)
     || (EC_NULL == pbyGetEntryTferBuffer)
      )
    {
        dwRetVal = EC_E_NOMEMORY;
        goto Exit;
    }

    OsMemset(pbyODLTferBuffer,      0, CROD_ODLTFER_SIZE);
    OsMemset(pbyOBDescTferBuffer,   0, CROD_OBDESC_SIZE);
    OsMemset(pbyGetEntryTferBuffer, 0, CROD_ENTRYDESC_SIZE);

    /* create required MBX Transfer Objects */
    /* mailbox transfer object for OD list upload */
    MbxTferDesc.dwMaxDataLen        = CROD_ODLTFER_SIZE;
    MbxTferDesc.pbyMbxTferDescData  = pbyODLTferBuffer;

    pMbxGetODLTfer = emMbxTferCreate( dwInstanceID, &MbxTferDesc );

    if( EC_NULL == pMbxGetODLTfer )
    {
        dwRetVal = EC_E_NOMEMORY;
        goto Exit;
    }

    /* mailbox transfer object for Object description upload */
    MbxTferDesc.dwMaxDataLen        = CROD_OBDESC_SIZE;
    MbxTferDesc.pbyMbxTferDescData  = pbyOBDescTferBuffer;
    
    pMbxGetObDescTfer = emMbxTferCreate( dwInstanceID, &MbxTferDesc );
    
    if( EC_NULL == pMbxGetObDescTfer )
    {
        dwRetVal = EC_E_NOMEMORY;
        goto Exit;
    }

    /* mailbox transfer object for Entry description upload */
    MbxTferDesc.dwMaxDataLen        = CROD_ENTRYDESC_SIZE;
    MbxTferDesc.pbyMbxTferDescData  = pbyGetEntryTferBuffer;
    
    pMbxGetEntryDescTfer = emMbxTferCreate( dwInstanceID, &MbxTferDesc );
    
    if( EC_NULL == pMbxGetEntryDescTfer )
    {
        dwRetVal = EC_E_NOMEMORY;
        goto Exit;
    }

    /* Get OD List Type: ALL */
    pMbxGetODLTfer->dwClntId    = dwClientId;
    pMbxGetODLTfer->dwTferId    = dwUniqueTransferId++; 
    pMbxGetODLTfer->dwDataLen   = pMbxGetODLTfer->MbxTferDesc.dwMaxDataLen;

    dwRes = emCoeGetODList(dwInstanceID, pMbxGetODLTfer, dwNodeId, eODListType_ALL, dwTimeout);
    if( dwRes == EC_E_SLAVE_NOT_PRESENT )
    {
        dwRetVal = dwRes;
        goto Exit;
    }
    else if( (EC_E_NOERROR != dwRes) && (pMbxGetODLTfer->eTferStatus == eMbxTferStatus_Idle) )
    {
        /* something got wrong, leave function (only if the transfer object is idle!) */
        CRODLError( "CoeReadObjectDictionary: Error in emCoeGetODList(ALL) : %s (0x%lx)", EcErrorText(dwRes), dwRes);
        dwRetVal = dwRes;
        goto Exit;
    }

    /* wait for completion or error */
    for (; (pMbxGetODLTfer->eTferStatus != eMbxTferStatus_Idle) && (pMbxGetODLTfer->eTferStatus != eMbxTferStatus_TferReqError); )
    {
        OsSleep(1);
    }

    /* handle mailbox transfer errors and wait until transfer object is available */
    HandleMbxTferReqError( poLog, (EC_T_CHAR*)"CoeReadObjectDictionary: Error in emCoeGetODList(ALL)", dwRes, pMbxGetODLTfer );
    OsDbgAssert( pMbxGetODLTfer->eTferStatus == eMbxTferStatus_Idle );
    if( pMbxGetODLTfer->dwErrorCode == EC_E_SLAVE_NOT_PRESENT )
    {
        dwRetVal = pMbxGetODLTfer->dwErrorCode;
        goto Exit;
    }
    else if( EC_E_NOERROR != pMbxGetODLTfer->dwErrorCode )
    {
        CRODLError( "CoeReadObjectDictionary: Error in emCoeGetODList(ALL) MBXTferObject : %s (0x%lx)", EcErrorText(pMbxGetODLTfer->dwErrorCode), pMbxGetODLTfer->dwErrorCode);
        dwRetVal = pMbxGetODLTfer->dwErrorCode;
        goto Exit;
    }

    /* OD Tfer object now shall contain complete list of OD Objects, store it for more processing */
    pwODList    = (EC_T_WORD*)OsMalloc((sizeof(EC_T_WORD)*(pMbxGetODLTfer->MbxData.CoE_ODList.wLen)));
    if( EC_NULL == pwODList )
    {
        dwRetVal = EC_E_NOMEMORY;
        goto Exit;
    }
    OsMemset(pwODList, 0, (sizeof(EC_T_WORD)*(pMbxGetODLTfer->MbxData.CoE_ODList.wLen)));

    /* reading master OD */
    if( emGetSlaveId( dwInstanceID, 0 ) == dwNodeId )
    {
        bReadingMasterOD = EC_TRUE;
    }

    /* now display Entries of ODList and store non-empty values */
    if( nVerbosePrinting > 1 )
    {
        CRODLMsg( "Complete OD list:" );
    }

    /* iterate through all entries in list */
    for( wODListLen = 0, wIndex = 0; wIndex < (pMbxGetODLTfer->MbxData.CoE_ODList.wLen); wIndex++ )
    {
        /* store next index */
        pwODList[wODListLen] = (pMbxGetODLTfer->MbxData.CoE_ODList.pwOdList)[wIndex];

        /* show indices */
        if( nVerbosePrinting > 1 )
        {
            CRODLMsgAdd("%04X ", pwODList[wODListLen]);
            if( ((wIndex+1) % 10) == 0) CRODLMsg(""); /* break lines each 10 entries */
        }

        /* to store only non empty index entries, increment List Length only if non zero entry */
        if( 0 != pwODList[wODListLen] )
        {
            wODListLen++;
        }
    } /* for( wODListLen = 0, wIndex = 0; wIndex < (pMbxGetODLTfer->MbxData.CoE_ODList.wLen); wIndex++ ) */

    if( nVerbosePrinting > 1 ) 
    {
        CRODLMsg("");
        OsSleep(2);     /* give logging task a chance to flush */
    }

    /* Get OD List Type: RX PDO Map */
    pMbxGetODLTfer->dwClntId    = dwClientId;
    pMbxGetODLTfer->dwTferId    = dwUniqueTransferId++; 
    pMbxGetODLTfer->dwDataLen   = pMbxGetODLTfer->MbxTferDesc.dwMaxDataLen;

    dwRes = emCoeGetODList(dwInstanceID, pMbxGetODLTfer, dwNodeId, eODListType_RxPdoMap, dwTimeout);
    if( dwRes == EC_E_SLAVE_NOT_PRESENT )
    {
        dwRetVal = dwRes;
        goto Exit;
    }
    else if( (EC_E_NOERROR != dwRes) && (pMbxGetODLTfer->eTferStatus == eMbxTferStatus_Idle) )
    {
        /* something got wrong, leave function (only if the transfer object is idle!) */
        CRODLError( "CoeReadObjectDictionary: Error in emCoeGetODList(RxPdoMap) : %s (0x%lx)", EcErrorText(dwRes), dwRes);
        dwRetVal = dwRes;
        goto Exit;
    }

    /* wait for completion or error */
    for (; (pMbxGetODLTfer->eTferStatus != eMbxTferStatus_Idle) && (pMbxGetODLTfer->eTferStatus != eMbxTferStatus_TferReqError); )
    {
        OsSleep(1);
    }

    /* handle MBX Tfer errors and wait until tfer object is available */
    HandleMbxTferReqError( poLog, (EC_T_CHAR*)"CoeReadObjectDictionary: Error in emCoeGetODList(RxPdoMap)", dwRes, pMbxGetODLTfer );
    OsDbgAssert( pMbxGetODLTfer->eTferStatus == eMbxTferStatus_Idle );
    if( pMbxGetODLTfer->dwErrorCode == EC_E_SLAVE_NOT_PRESENT )
    {
        dwRetVal = pMbxGetODLTfer->dwErrorCode;
        goto Exit;
    }
    else if( EC_E_NOERROR != pMbxGetODLTfer->dwErrorCode )
    {
        CRODLError( "CoeReadObjectDictionary: Error in emCoeGetODList(RxPdoMap) MBXTferObject : %s (0x%lx)", EcErrorText(pMbxGetODLTfer->dwErrorCode), pMbxGetODLTfer->dwErrorCode);
        dwRetVal = pMbxGetODLTfer->dwErrorCode;
        goto Exit;
    }

    /* now display Entries of ODList */
    if( nVerbosePrinting > 1 )
    {
        CRODLMsg( "RX PDO Mappable Objects:" );
        /* iterate through all entries in list */
        for( wIndex = 0; wIndex < (pMbxGetODLTfer->MbxData.CoE_ODList.wLen); wIndex++ )
        {
            CRODLMsgAdd("%04X ", pMbxGetODLTfer->MbxData.CoE_ODList.pwOdList[wIndex]);

            if( ((wIndex+1) % 10) == 0) CRODLMsg(""); /* break lines each 10 entries */
        } /* for( wIndex = 0; wIndex < (pMbxGetODLTfer->MbxData.CoE_ODList.wLen); wIndex++ ) */
    
        CRODLMsg("");
    }

    /* Get OD List Type: TX PDO Map */
    pMbxGetODLTfer->dwClntId    = dwClientId;
    pMbxGetODLTfer->dwTferId    = dwUniqueTransferId++; 
    pMbxGetODLTfer->dwDataLen   = pMbxGetODLTfer->MbxTferDesc.dwMaxDataLen;
    
    dwRes = emCoeGetODList(dwInstanceID, pMbxGetODLTfer, dwNodeId, eODListType_TxPdoMap, dwTimeout);
    if( dwRes == EC_E_SLAVE_NOT_PRESENT )
    {
        dwRetVal = dwRes;
        goto Exit;
    }
    else if( (EC_E_NOERROR != dwRes) && (pMbxGetODLTfer->eTferStatus == eMbxTferStatus_Idle) )
    {
        /* something got wrong, leave function (only if the transfer object is idle!) */
        CRODLError( "CoeReadObjectDictionary: Error in emCoeGetODList(TxPdoMap) : %s (0x%lx)", EcErrorText(dwRes), dwRes);
        dwRetVal = dwRes;
        goto Exit;
    }

    /* wait for completion or error */
    for (; (pMbxGetODLTfer->eTferStatus != eMbxTferStatus_Idle) && (pMbxGetODLTfer->eTferStatus != eMbxTferStatus_TferReqError); )
    {
        OsSleep(1);
    }

    /* handle MBX Tfer errors and wait until tfer object is available */
    HandleMbxTferReqError( poLog, (EC_T_CHAR*)"CoeReadObjectDictionary: Error in emCoeGetODList(TxPdoMap)", dwRes, pMbxGetODLTfer );
    OsDbgAssert( pMbxGetODLTfer->eTferStatus == eMbxTferStatus_Idle );
    
    if( pMbxGetODLTfer->dwErrorCode == EC_E_SLAVE_NOT_PRESENT )
    {
        dwRetVal = pMbxGetODLTfer->dwErrorCode;
        goto Exit;
    }
    else if( EC_E_NOERROR != pMbxGetODLTfer->dwErrorCode )
    {
        CRODLError( "CoeReadObjectDictionary: Error in emCoeGetODList(TxPdoMap) MBXTferObject : %s (0x%lx)", EcErrorText(pMbxGetODLTfer->dwErrorCode), pMbxGetODLTfer->dwErrorCode);
        dwRetVal = pMbxGetODLTfer->dwErrorCode;
        goto Exit;
    }

    /* now display Entries of ODList */
    if( nVerbosePrinting > 1 )
    {
        CRODLMsg( "TX PDO Mappable Objects:" );
        /* iterate through all entries in list */
        for( wIndex = 0; wIndex < (pMbxGetODLTfer->MbxData.CoE_ODList.wLen); wIndex++ )
        {
            CRODLMsgAdd("%04X ", pMbxGetODLTfer->MbxData.CoE_ODList.pwOdList[wIndex]);
            
            if( ((wIndex+1) % 10) == 0) CRODLMsg(""); /* break lines each 10 entries */
        } /* for( wIndex = 0; wIndex < (pMbxGetODLTfer->MbxData.CoE_ODList.wLen); wIndex++ ) */
        
        CRODLMsg("");
    }

    /* now iterate through Index list, to get closer info, sub indexes and values */
    
    /* get object description for all objects */
    if( nVerbosePrinting > 1 ) 
    {
        CRODLMsg( "" );
        CRODLMsg( "*************************************************************" );
        CRODLMsg( "****                  OBJECT DESCRIPTION                 ****" );
        CRODLMsg( "*************************************************************" );
    }

    /* init value info type */
    byValueInfoType = EC_COE_ENTRY_ObjAccess 
                    | EC_COE_ENTRY_ObjCategory
                    | EC_COE_ENTRY_PdoMapping   
                    | EC_COE_ENTRY_UnitType     
                    | EC_COE_ENTRY_DefaultValue 
                    | EC_COE_ENTRY_MinValue     
                    | EC_COE_ENTRY_MaxValue;

    /* now dwRetVal is used to recognize loop break condition on error, so set it to OK here */
    dwRetVal = EC_E_NOERROR;
    for( wIndex = 0; (wIndex < wODListLen) && (EC_E_NOERROR == dwRetVal) && !*pbStopReading; wIndex++ )
    {
        EC_T_WORD   bySubIndexLimit     = 0xffff; // koh, fix > Value has to be gt 8bit because bySubIndexLimit can go to 0x100 if subindex is 0xff 
        EC_T_WORD   wSubIndex           = 0;

        /* get Object Description */
        pMbxGetObDescTfer->dwClntId     = dwClientId;
        pMbxGetObDescTfer->dwDataLen    = pMbxGetObDescTfer->MbxTferDesc.dwMaxDataLen;
        pMbxGetObDescTfer->dwTferId     = dwUniqueTransferId++;

        dwRes = emCoeGetObjectDesc( dwInstanceID, pMbxGetObDescTfer, dwNodeId, pwODList[wIndex], dwTimeout);
        if( dwRes == EC_E_SLAVE_NOT_PRESENT )
        {
            dwRetVal = dwRes;
            goto Exit;
        }
        else if( (EC_E_NOERROR != dwRes) && (pMbxGetObDescTfer->eTferStatus == eMbxTferStatus_Idle) )
        {
            /* something got wrong, leave function (only if the transfer object is idle!) */
            CRODLError( "CoeReadObjectDictionary: Error in emCoeGetObjectDesc(0x%x) : %s (0x%lx)", pwODList[wIndex], EcErrorText(dwRes), dwRes);
            dwRetVal = dwRes;
            continue;
        }
        /* wait for completion or error */
        for (; (pMbxGetObDescTfer->eTferStatus != eMbxTferStatus_Idle) && (pMbxGetObDescTfer->eTferStatus != eMbxTferStatus_TferReqError); )
        {
            OsSleep(1);
        }
        
        /* handle MBX Tfer errors and wait until tfer object is available */
        HandleMbxTferReqError( poLog, (EC_T_CHAR*)"CoeReadObjectDictionary: Error in emCoeGetObjectDesc", dwRes, pMbxGetObDescTfer );
        OsDbgAssert( pMbxGetObDescTfer->eTferStatus == eMbxTferStatus_Idle );
        if( pMbxGetObDescTfer->dwErrorCode == EC_E_SLAVE_NOT_PRESENT )
        {
            dwRetVal = pMbxGetObDescTfer->dwErrorCode;
            goto Exit;
        }
        else if( EC_E_NOERROR != pMbxGetObDescTfer->dwErrorCode )
        {
            CRODLError( "CoeReadObjectDictionary: Error in emCoeGetObjectDesc(0x%x) MBXTferObject : %s (0x%lx)", pwODList[wIndex], EcErrorText(pMbxGetObDescTfer->dwErrorCode), pMbxGetObDescTfer->dwErrorCode);
            dwRetVal = pMbxGetObDescTfer->dwErrorCode;
            continue;
        }

        /* display ObjectDesc */
        if( nVerbosePrinting > 1 )
        {
            EC_T_WORD   wNameLen                    = 0;
            EC_T_CHAR   szObName[MAX_OBNAME_LEN]    = {0};

            wNameLen = pMbxGetObDescTfer->MbxData.CoE_ObDesc.wObNameLen;
            wNameLen = (EC_T_WORD)EC_MIN(wNameLen, MAX_OBNAME_LEN - 1);

            OsStrncpy(szObName, pMbxGetObDescTfer->MbxData.CoE_ObDesc.pchObName, (EC_T_INT)wNameLen);
            szObName[wNameLen] = '\0';

            CRODLMsg( "%04x \"%27s\": type 0x%04x, code=0x%02x, %s, SubIds=%d",
                pMbxGetObDescTfer->MbxData.CoE_ObDesc.wObIndex,
                szObName,
                pMbxGetObDescTfer->MbxData.CoE_ObDesc.wDataType,
                pMbxGetObDescTfer->MbxData.CoE_ObDesc.byObjCode,
                ((pMbxGetObDescTfer->MbxData.CoE_ObDesc.byObjCategory==0)?"optional":"mandatory"),
                pMbxGetObDescTfer->MbxData.CoE_ObDesc.byMaxNumSubIndex
                  );
            
            if( bReadingMasterOD )  /* give logging task a chance to flush */
                OsSleep(2);
        }

        /* if Object is Single Variable, only subindex 0 is defined */
        if( OBJCODE_VAR == pMbxGetObDescTfer->MbxData.CoE_ObDesc.byObjCode )
        {
            bySubIndexLimit = 1;
        }
        else
        {
            bySubIndexLimit = 0xffff;
        }

        /* iterate through sub-indexes */
        for( wSubIndex = 0; (wSubIndex < bySubIndexLimit) && (EC_E_NOERROR == dwRetVal); wSubIndex++ )
        {
            /* Get Entry Description */
            pMbxGetEntryDescTfer->dwClntId     = dwClientId;
            pMbxGetEntryDescTfer->dwDataLen    = pMbxGetEntryDescTfer->MbxTferDesc.dwMaxDataLen;
            pMbxGetEntryDescTfer->dwTferId     = dwUniqueTransferId++;
            
            dwRes = emCoeGetEntryDesc( 
                dwInstanceID, pMbxGetEntryDescTfer, dwNodeId, pwODList[wIndex], EC_LOBYTE(wSubIndex), byValueInfoType, dwTimeout 
                                     );
            if( dwRes == EC_E_SLAVE_NOT_PRESENT )
            {
                dwRetVal = dwRes;
                goto Exit;
            }
            else if( (EC_E_NOERROR != dwRes) && (pMbxGetEntryDescTfer->eTferStatus == eMbxTferStatus_Idle) )
            {
                /* something got wrong, leave function (only if the transfer object is idle!) */
                CRODLError( "CoeReadObjectDictionary: Error in emCoeGetEntryDesc(0x%x, 0x%x) : %s (0x%lx)", pwODList[wIndex], EC_LOBYTE(wSubIndex), EcErrorText(dwRes), dwRes);
                dwRetVal = dwRes;
                continue;
            }

            /* wait for completion or error */
            for (; (pMbxGetEntryDescTfer->eTferStatus != eMbxTferStatus_Idle) && (pMbxGetEntryDescTfer->eTferStatus != eMbxTferStatus_TferReqError); )
            {
                OsSleep(1);
            }

            /* handle MBX Tfer errors and wait until tfer object is available */
            HandleMbxTferReqError( poLog, (EC_T_CHAR*)"CoeReadObjectDictionary: Error in emCoeGetEntryDesc", dwRes, pMbxGetEntryDescTfer );
            OsDbgAssert( pMbxGetEntryDescTfer->eTferStatus == eMbxTferStatus_Idle );
            if( pMbxGetEntryDescTfer->dwErrorCode == EC_E_SLAVE_NOT_PRESENT )
            {
                dwRetVal = pMbxGetEntryDescTfer->dwErrorCode;
                goto Exit;
            }
            else if( 
                (EC_E_INVALIDDATA == pMbxGetEntryDescTfer->dwErrorCode) 
            ||  (EC_E_SDO_ABORTCODE_OFFSET == pMbxGetEntryDescTfer->dwErrorCode)
              )
            {
                /* no more subindexes !! */
                break;
            }
            else if( EC_E_NOERROR != pMbxGetEntryDescTfer->dwErrorCode )
            {
                CRODLError( "CoeReadObjectDictionary: Error in emCoeGetEntryDesc(0x%x, 0x%x) MBXTferObject : %s (0x%lx)", pwODList[wIndex], EC_LOBYTE(wSubIndex), EcErrorText(pMbxGetEntryDescTfer->dwErrorCode), pMbxGetEntryDescTfer->dwErrorCode);
                dwRetVal = pMbxGetEntryDescTfer->dwErrorCode;
                continue;
            }

            /* display EntryDesc */
            if( nVerbosePrinting > 1 )
            {
                EC_T_CHAR   szAccess[50]        = {0};
                EC_T_CHAR   szPdoMapInfo[50]    = {0};
                EC_T_INT    nIdx                = 0;
                
                OsMemset(szAccess, 0, sizeof(szAccess));
                OsMemset(szPdoMapInfo, 0, sizeof(szPdoMapInfo));

                /* build Access Right String */
                if( pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.byObAccess & EC_COE_ENTRY_Access_R_PREOP )
                    szAccess[nIdx++] ='R';
                else
                    szAccess[nIdx++] =' ';
                if( pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.byObAccess & EC_COE_ENTRY_Access_W_PREOP )
                    szAccess[nIdx++] ='W';
                else
                    szAccess[nIdx++] =' ';
                
                szAccess[nIdx++] ='.';
                
                if( pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.byObAccess & EC_COE_ENTRY_Access_R_SAFEOP )
                    szAccess[nIdx++] ='R';
                else
                    szAccess[nIdx++] =' ';
                if( pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.byObAccess & EC_COE_ENTRY_Access_W_SAFEOP )
                    szAccess[nIdx++] ='W';
                else
                    szAccess[nIdx++] =' ';
                
                szAccess[nIdx++] ='.';
                
                if( pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.byObAccess & EC_COE_ENTRY_Access_R_OP )
                    szAccess[nIdx++] ='R';
                else
                    szAccess[nIdx++] =' ';
                if( pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.byObAccess & EC_COE_ENTRY_Access_W_OP )
                    szAccess[nIdx++] ='W';
                else
                    szAccess[nIdx++] =' ';

                if( pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.bRxPdoMapping )
                {
                    OsStrncpy( szPdoMapInfo, "-RxPDO", sizeof(szPdoMapInfo) - 1);
                    if( pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.bTxPdoMapping )
                    {
                        OsStrncpy(&(szPdoMapInfo[OsStrlen(szPdoMapInfo)]), "+TxPDO", sizeof(szPdoMapInfo) - OsStrlen(szPdoMapInfo) - 1);
                    }
                }
                
                if( pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.byValueInfo & (EC_COE_ENTRY_DefaultValue | EC_COE_ENTRY_MinValue | EC_COE_ENTRY_MaxValue) )
                {
                    CRODLMsg( "%04x:%d => type=0x%04x, len=%02d, %s%s",
                        pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.wObIndex,
                        pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.byObSubIndex,
                        pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.wDataType,
                        pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.wBitLen,
                        szAccess, szPdoMapInfo
                        );
                }
                else
                {
                    EC_T_CHAR   szObName[MAX_OBNAME_LEN]    = {0};
                    EC_T_WORD   wNameLen                    = 0;
                    
                    wNameLen = pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.wDataLen;
                    wNameLen = (EC_T_WORD)EC_MIN(wNameLen, MAX_OBNAME_LEN - 1);
                    
                    OsStrncpy(szObName, (const EC_T_CHAR*)pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.pbyData, wNameLen);
                    szObName[wNameLen] = '\0';
                    
                    CRODLMsg( "%04x:%d \"%27s\" => type=0x%04x, len=%02d, %s%s",
                        pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.wObIndex,
                        pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.byObSubIndex,
                        szObName,
                        pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.wDataType,
                        pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.wBitLen,
                        szAccess, szPdoMapInfo
                        );
                }

                if( bReadingMasterOD )  /* give logging task a chance to flush */
                    OsSleep(2);
            } /* display EntryDesc */

            if( 0 == pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.wDataType )
            {
                /* unknown datatype */
                continue;
            }
        
            /* do SDO Upload */

            if( bPerformUpload )
            {
                EC_T_BYTE   abySDOValue[CROD_MAXSISDO_SIZE]     = {0};
                EC_T_DWORD  dwUploadBytes                       = 0;

                dwRes = emCoeSdoUpload(
                    dwInstanceID, dwNodeId, pwODList[wIndex], EC_LOBYTE(wSubIndex), 
                    abySDOValue, EC_MIN( (EC_T_DWORD)(sizeof(abySDOValue)), (EC_T_DWORD)(((pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.wBitLen)+7)/8) ),
                    &dwUploadBytes, dwTimeout, 0
                                      );
                if( dwRes == EC_E_SLAVE_NOT_PRESENT )
                {
                    dwRetVal = dwRes;
                    goto Exit;
                }
                else if( EC_E_NOERROR != dwRes )
                {
                    /* Upload error */
                    CRODLError( "CoeReadObjectDictionary: Error in ecatCoeSdoUpload : %s (0x%lx)", EcErrorText(dwRes), dwRes );
                    dwRetVal = dwRes;
                    continue;
                }

                if(
                    ((OBJCODE_REC == pMbxGetObDescTfer->MbxData.CoE_ObDesc.byObjCode) && (0 == wSubIndex))
                  ||((OBJCODE_ARR == pMbxGetObDescTfer->MbxData.CoE_ObDesc.byObjCode) && (0 == wSubIndex))
                  )
                {
                    bySubIndexLimit = (EC_T_BYTE)(((EC_T_WORD) abySDOValue[0]) + 1);
                }

                if( nVerbosePrinting > 1 )
                {
                    switch( pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.wDataType )
                    {
                    case 	DEFTYPE_BOOLEAN:
                    case 	DEFTYPE_BIT1:
                    case 	DEFTYPE_BIT2:
                    case 	DEFTYPE_BIT3:
                    case 	DEFTYPE_BIT4:
                    case 	DEFTYPE_BIT5:
                    case 	DEFTYPE_BIT6:
                    case 	DEFTYPE_BIT7:
                    case 	DEFTYPE_BIT8:
                    case	DEFTYPE_INTEGER8:
                    case	DEFTYPE_UNSIGNED8:
                        {
                            CRODLMsg("%04x:%d BYTE : 0x%02X",
                                pwODList[wIndex], EC_LOBYTE(wSubIndex), abySDOValue[0]
                                  );
                        } break;
                    case	DEFTYPE_INTEGER16:
                        {
                            CRODLMsg("%04x:%d SI16 : %04d",
                                pwODList[wIndex], EC_LOBYTE(wSubIndex), EC_NTOHS(EC_GETWORD(&abySDOValue[0]))
                                );
                        } break;
                    case	DEFTYPE_UNSIGNED16:
                        {
                            CRODLMsg("%04x:%d WORD : 0x%04X",
                                pwODList[wIndex], EC_LOBYTE(wSubIndex), EC_NTOHS(EC_GETWORD(&abySDOValue[0]))
                                  );
                        } break;
                    case	DEFTYPE_INTEGER32:
                        {
                            CRODLMsg("%04x:%d SI32 : %08ld",
                                pwODList[wIndex], EC_LOBYTE(wSubIndex), EC_NTOHL(EC_GETDWORD(&abySDOValue[0]))
                                );
                        } break;
                    case	DEFTYPE_UNSIGNED32:
                        {
                            CRODLMsg("%04x:%d DWRD : 0x%08lX",
                                pwODList[wIndex], EC_LOBYTE(wSubIndex), EC_NTOHL(EC_GETDWORD(&abySDOValue[0]))
                                  );
                        } break;
                    case	DEFTYPE_VISIBLESTRING:
                        {
                            CRODLMsg("%04x:%d STRG : %s",
                                pwODList[wIndex], EC_LOBYTE(wSubIndex), (EC_T_CHAR*)abySDOValue
                                  );
                        } break;
                    case	DEFTYPE_OCTETSTRING:
                        {
#if (defined INCLUDE_MASTER_OBD)
                            if( (COEOBJID_HISTORY_OBJECT == (pwODList[wIndex])) && EC_LOBYTE(wSubIndex) > 5 )
                            {
                                /* Diag Entry */
                                EC_T_OBJ10F3_DIAGMSG*   pDiag = (EC_T_OBJ10F3_DIAGMSG*)abySDOValue;
                                CRODLMsg("%04x:%d DIAG # 0x%lx type <%s> Text 0x%x Time: 0x%x.%x",
                                    pwODList[wIndex], EC_LOBYTE(wSubIndex), 
                                    pDiag->dwDiagNumber,
                                    ((pDiag->wFlags==DIAGFLAGERROR)?"ERR":((pDiag->wFlags==DIAGFLAGWARN)?"WARN":((pDiag->wFlags==DIAGFLAGINFO)?"INF":"UNK"))),
                                    pDiag->wTextId,
#ifdef  __TMS470__
                                    pDiag->dwTimeStampHi, pDiag->dwTimeStampLo
#else
                                    EC_HIDWORD(pDiag->qwTimeStamp), EC_LODWORD(pDiag->qwTimeStamp)
#endif
                                      );
                                ParseDiagMsg(poLog, pDiag);
                            }
                            else
#endif
                            {
                                CRODLMsg("%04x:%d OCTS : %s",
                                    pwODList[wIndex], EC_LOBYTE(wSubIndex), (EC_T_CHAR*)abySDOValue
                                    );
                            }
                        } break;
                    case    DEFTYPE_UNSIGNED48:
                        {
                            CRODLMsg("%04x:%d US48 : %02X:%02X:%02X:%02X:%02X:%02X",
                                pwODList[wIndex], EC_LOBYTE(wSubIndex), 
                                abySDOValue[0], 
                                abySDOValue[1], 
                                abySDOValue[2], 
                                abySDOValue[3], 
                                abySDOValue[4], 
                                abySDOValue[5]
                                    );
                        } break;
                    case	DEFTYPE_UNSIGNED64:
                        {
                            CRODLMsg("%04x:%d QWRD : 0x%08lX.%08lX",
                                pwODList[wIndex], EC_LOBYTE(wSubIndex), 
                                EC_HIDWORD(EC_NTOHLL(EC_GETQWORD(&abySDOValue[0]))),
                                EC_LODWORD(EC_NTOHLL(EC_GETQWORD(&abySDOValue[0])))
                                  );
                        } break;
                    default:
                        {
                            EC_T_DWORD  dwIdx = 0;
                            
                            CRODLMsg("%04x:%d DFLT : ", pwODList[wIndex], EC_LOBYTE(wSubIndex));
                            for(dwIdx = 0; dwIdx < dwUploadBytes; dwIdx++)
                            {
                                CRODLMsgAdd("%02x ", abySDOValue[dwIdx]);
                                if( (0 != dwIdx) && (0 == (dwIdx%8)) )
                                {
                                    CRODLMsgAdd(" ");
                                }
                                if( (0 != dwIdx) && (0 == (dwIdx%32)) )
                                {
                                    CRODLMsgAdd("\n");
                                }
                            }
                            CRODLMsg("");
                        } break;
                    }   /* switch( pMbxGetEntryDescTfer->MbxData.CoE_EntryDesc.wDataType ) */

#if (defined INCLUDE_MASTER_OBD)
                    if( COEOBJID_SLAVECFGINFOBASE <= pwODList[wIndex] && 1 == EC_LOBYTE(wSubIndex) )
                    {
                        EC_T_BOOL bEntryValid   = EC_FALSE;
                        bEntryValid = EC_NTOHL(EC_GETDWORD(abySDOValue));

                        if( !bEntryValid )
                        {
                            /* do not show unused Slave Entries */
                            break;
                        }
                    }
#endif
                    if( bReadingMasterOD )  /* give logging task a chance to flush */
                        OsSleep(2);
                } /* if( nVerbosePrinting > 1 ) */
            } /* if( bPerformUpload ) */
        } /* for( wSubIndex = 0; (wSubIndex < bySubIndexLimit) && (EC_E_NOERROR == dwRetVal); wSubIndex++ ) */
    } /* for( wIndex = 0; (wIndex < wODListLen) && (EC_E_NOERROR == dwRetVal); wIndex++ ) */

    
    dwRetVal = EC_E_NOERROR;
Exit:
    if( bExecutionCancelled )
    {
        /* Master Disappeared, cannot delete so NULL Ptrs */
        pMbxGetODLTfer          = EC_NULL;
        pMbxGetObDescTfer       = EC_NULL;
        pMbxGetEntryDescTfer    = EC_NULL;
    }
    else
    {
        /* Delete MBX Transfer objects */
        if( EC_NULL != pMbxGetODLTfer )
        {
            emMbxTferDelete( dwInstanceID, pMbxGetODLTfer );
            pMbxGetODLTfer = EC_NULL;
        }
        
        if( EC_NULL != pMbxGetObDescTfer )
        {
            emMbxTferDelete( dwInstanceID, pMbxGetObDescTfer );
            pMbxGetObDescTfer = EC_NULL;
        }
        
        if( EC_NULL != pMbxGetEntryDescTfer )
        {
            emMbxTferDelete( dwInstanceID, pMbxGetEntryDescTfer );
            pMbxGetEntryDescTfer = EC_NULL;
        }
    }


    /* Free allocated memory */
    if(EC_NULL != pwODList)
    {
        OsFree(pwODList);
        pwODList = EC_NULL;
    }
    if( EC_NULL != pbyODLTferBuffer )
    {
        OsFree(pbyODLTferBuffer);
        pbyODLTferBuffer = EC_NULL;
    }
    if( EC_NULL != pbyOBDescTferBuffer )
    {
        OsFree(pbyOBDescTferBuffer);
        pbyOBDescTferBuffer = EC_NULL;
    }
    if( EC_NULL != pbyGetEntryTferBuffer )
    {
        OsFree(pbyGetEntryTferBuffer);
        pbyGetEntryTferBuffer = EC_NULL;
    }

    return dwRetVal;
}
#endif /* #ifndef EXCLUDE_ATEM */

/********************************************************************************/
/** \brief  Create parameters for Intel Pro 1000 family (I8254x)
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaI8254x
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    return ::CreateLinkParaI8254x(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
}
EC_T_BOOL CreateLinkParaI8254x
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    EC_T_LINK_OPENPARMS_I8254x* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc        = EC_NULL;
        
    if(EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    if( dwInstance == 0 )
    {
        OsDbgMsg( "8254x PCI device unit must be greater than 0, the first unit is 1!\n" );
        return EC_FALSE;
    }

    if( eLinkMode != EcLinkMode_INTERRUPT && eLinkMode != EcLinkMode_POLLING && eLinkMode != EcLinkMode_RANDOM)
    {
        OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",eLinkMode );
        return EC_FALSE;
        
    }

    poDrvSpecificParam = &poLinkDevOpenParam->linkOpenParms.oI8254x;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_I8254x));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "I8254x", MAX_DRIVER_IDENT_LEN-1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = &poLinkDevOpenParam->linkOpenParms.oI8254x;

	/* Link Layer Parameters for Intel Pro 1000 */
	poDrvSpecificParam->dwType              = EC_LINK_TYPE_I8254x;
	poDrvSpecificParam->dwUnit              = dwInstance;
	poDrvSpecificParam->pfDoIntHandling     = EC_NULL;
	poDrvSpecificParam->dwRxBuffers         = 96;
	poDrvSpecificParam->dwTxBuffers         = 96;
    poDrvSpecificParam->dwIstPriority       = dwRecvPriority;

    return EC_TRUE;
}

/********************************************************************************/
/** \brief  Create parameters for FreeScale FEC
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaFslFec
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
 )
{
    return ::CreateLinkParaFslFec(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
}
EC_T_BOOL CreateLinkParaFslFec
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
 )
{
    EC_T_LINK_OPENPARMS_FSLFEC* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc        = EC_NULL;

    if(EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    if( dwInstance == 0 )
    {
        OsDbgMsg( "FslFEC device unit must be greater than 0, the first unit is 1!\n" );
        return EC_FALSE;
    }

    if( eLinkMode != EcLinkMode_INTERRUPT && eLinkMode != EcLinkMode_POLLING && eLinkMode != EcLinkMode_RANDOM)
    {
        OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",eLinkMode );
        return EC_FALSE;

    }

    poDrvSpecificParam = &poLinkDevOpenParam->linkOpenParms.oFslFec;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_FSLFEC));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "FslFec", MAX_DRIVER_IDENT_LEN-1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = &poLinkDevOpenParam->linkOpenParms.oFslFec;

    /* Link Layer Parameters for FreeScale FEC */
    poDrvSpecificParam->dwType              = EC_LINK_TYPE_FSLFEC;
    poDrvSpecificParam->dwUnit              = dwInstance;
    poDrvSpecificParam->pfDoIntHandling     = EC_NULL;
    poDrvSpecificParam->dwRxBuffers         = 96;
    poDrvSpecificParam->dwTxBuffers         = 96;
    poDrvSpecificParam->dwIstPriority       = dwRecvPriority;

    return EC_TRUE;
}

#if (defined(LINUX) && defined(_ARCH_PPC)) || (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC32))

#define TSEC_CFG_P2020RDB /* Freescale P2020RDB reference board */
#undef TSEC_CFG_ISTMPC8548 /* Instron "MPC8548 MiniTower" board */
#undef TSEC_CFG_TWRP1025 /* Freescale TWR-P1025 board */

#if defined(VXWORKS) && ( defined(TSEC_CFG_P2020RDB) || defined(TSEC_CFG_ISTMPC8548) )
   extern "C" UINT32 sysGetPeripheralBase (void); /* from sysLib.c */
#  include <hwif/intCtlr/vxbEpicIntCtlr.h>
#endif

/********************************************************************************/
/** \brief  Create parameters for FreeScale TSEC/eTSEC
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaFslEtsec
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
 )
{
    return ::CreateLinkParaFslEtsec(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
}
EC_T_BOOL CreateLinkParaFslEtsec
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
 )
{
    EC_T_LINK_OPENPARMS_ETSEC* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc        = EC_NULL;
    EC_T_DWORD                  dwCcsrbar = 0;
  
    if(EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    if( dwInstance == 0 )
    {
        OsDbgMsg( "Device unit must be greater than 0, the first unit is 1!\n" );
        return EC_FALSE;
    }

    if( eLinkMode != EcLinkMode_INTERRUPT && eLinkMode != EcLinkMode_POLLING && eLinkMode != EcLinkMode_RANDOM)
    {
        OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",eLinkMode );
        return EC_FALSE;
    }

    poDrvSpecificParam  = &poLinkDevOpenParam->linkOpenParms.oFslEtsec;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_ETSEC));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "ETSEC", MAX_DRIVER_IDENT_LEN - 1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = &poLinkDevOpenParam->linkOpenParms.oFslEtsec;

    /* Link Layer Parameters for FreeScale TSEC/eTSEC */
    poDrvSpecificParam->dwType               = EC_LINK_TYPE_ETSEC;
    poDrvSpecificParam->pfDoIntHandling      = EC_NULL;
    poDrvSpecificParam->dwIstPriority        = dwRecvPriority;

#ifdef TSEC_CFG_P2020RDB

    /*
     * The following configuration is for Freescale P2020RDB board 
     * (CPU P2020E, VxWorks 6.8 PPC / Linux 3.0.9-PREEMPT PPC)
     */
    
    if (dwInstance > 3)
    {
       OsDbgMsg( "Device unit must be <= 3! There are only 3 eTSEC's on P2020RDB.\n" );
       return EC_FALSE;
    }

    {
       EC_T_BYTE abyStationAddress[] = { 0x00, 0x04, 0x9F, 0x01, 0x79, 0x00 };
       memcpy(poDrvSpecificParam->abyStationAddress, abyStationAddress, sizeof(poDrvSpecificParam->abyStationAddress));
    }
    
#if VXWORKS
    dwCcsrbar = sysGetPeripheralBase();
#elif defined LINUX
    dwCcsrbar = 0xffe00000; /* from p2020si.dtsi */

    /* Get interrupt number from Nth eTSEC device in PowerPC device tree */
    poDrvSpecificParam->dwRxInterrupt = dwInstance - 1;
#endif
    
    poDrvSpecificParam->dwPhyMdioBase = dwCcsrbar + 0x24000; /* eTSEC 1 */
    poDrvSpecificParam->dwTbiPhyAddr = 16; /* Dummy address assigned to internal TBI PHY */
    poDrvSpecificParam->oMiiBusMtx = EC_NULL; /* LinkOsCreateLock(eLockType_DEFAULT); */
    
    if (dwInstance == 1) // eTSEC1 (4 x 1Gb switchports)
    {
       poDrvSpecificParam->abyStationAddress[5] = 0x81;
       poDrvSpecificParam->dwRegisterBase = dwCcsrbar + 0x24000;
       poDrvSpecificParam->dwPhyAddr = ETSEC_FIXED_LINK; /* RGMII, No Phy, Switchport */
       poDrvSpecificParam->dwFixedLinkVal = ETSEC_LINKFLAG_1000baseT_Full | ETSEC_LINKFLAG_LINKOK;
#if defined(VXWORKS)
       poDrvSpecificParam->dwRxInterrupt = EPIC_TSEC1RX_INT_VEC; /* from vxbEpicIntCtlr.h */
#endif
    }
    else if (dwInstance == 2) // eTSEC2
    {
       poDrvSpecificParam->abyStationAddress[5] = 0x82;
       poDrvSpecificParam->dwRegisterBase = dwCcsrbar + 0x25000;
       poDrvSpecificParam->dwPhyAddr = 0; /* SGMII Phy on addr. 0 (from P2020RDB Ref. Manual) */
#ifdef VXWORKS
       poDrvSpecificParam->dwRxInterrupt = EPIC_TSEC2RX_INT_VEC; /* from vxbEpicIntCtlr.h */
#endif
    }
    else if (dwInstance == 3) // eTSEC3
    {
       poDrvSpecificParam->abyStationAddress[5] = 0x83;
       poDrvSpecificParam->dwRegisterBase = dwCcsrbar + 0x26000;
       poDrvSpecificParam->dwPhyAddr = 1; /* RGMII Phy on addr. 1 (from P2020RDB Ref. Manual) */
#ifdef VXWORKS
       poDrvSpecificParam->dwRxInterrupt = EPIC_TSEC3RX_INT_VEC; /* from vxbEpicIntCtlr.h */
#endif
    }
    
    poDrvSpecificParam->dwLocalMdioBase = poDrvSpecificParam->dwRegisterBase;

#elif defined(TSEC_CFG_TWRP1025)

    /*
     * The following configuration is for Freescale TWR-P1025 board
     * (CPU P1025, Freescale-Linuxkernel 3.0.4)
     * Hardware resource informations (phy-addr, interrupts, io-base, ...)
     * are taken from Linux-kernel's device tree for the TWR-P1025 (twr-p1025_32b.dts)
     */

    if (dwInstance > 2) /* TWR-P1025 has 3 eTSEC's, but only eTSEC1 and eTSEC3 are routed out to RJ45 ports */
    {
       OsDbgMsg( "Device unit must be <= 2! There are only 2 eTSEC's on P1025TWR.\n" );
       return EC_FALSE;
    }

    {
       EC_T_BYTE abyStationAddress[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xBE };
       memcpy(poDrvSpecificParam->abyStationAddress, abyStationAddress, sizeof(poDrvSpecificParam->abyStationAddress));
    }

    dwCcsrbar = 0xffe00000;

    poDrvSpecificParam->dwPhyMdioBase = dwCcsrbar + 0x24000; /* eTSEC 1, MDIO */
    poDrvSpecificParam->dwTbiPhyAddr = 16; /* Dummy address assigned to internal TBI PHY */
    poDrvSpecificParam->oMiiBusMtx = EC_NULL; /* LinkOsCreateLock(eLockType_DEFAULT); */

    /* Get interrupt number from Nth eTSEC device in PowerPC device tree */
    poDrvSpecificParam->dwRxInterrupt = dwInstance - 1;

    if (dwInstance == 1) // eTSEC1
    {
       poDrvSpecificParam->dwRegisterBase = dwCcsrbar + 0xB0000; /* eTSEC1, Group 0 */
       poDrvSpecificParam->dwLocalMdioBase = dwCcsrbar + 0x24000; /* eTSEC1, MDIO */
       poDrvSpecificParam->dwPhyAddr = 2;
    }
    else if (dwInstance == 2) // eTSEC3
    {
       poDrvSpecificParam->dwRegisterBase = dwCcsrbar + 0xB2000; /* eTSEC3, Group 0 */
       poDrvSpecificParam->dwLocalMdioBase = dwCcsrbar + 0x26000; /* eTSEC3, MDIO */
       poDrvSpecificParam->dwPhyAddr = 1;
    }

#elif defined(TSEC_CFG_ISTMPC8548)
    
    /*
     * The following configuration is for the Instron "MPC8548 MiniTower" board 
     * (CPU MPC8548, VxWorks 6.9 PPC)
     */
    
    if (dwInstance > 2) /* MPC8548 has 4 eTSEC's, but only the first 2 are routed out to RJ45 ports */
    {
       OsDbgMsg( "Device unit must be 1 or 2!\n" );
       return EC_FALSE;
    }

    {
       EC_T_BYTE abyStationAddress[] = { 0x00, 0x02, 0xCE, 0x90, 0x02, 0x24 };
       memcpy(poDrvSpecificParam->abyStationAddress, abyStationAddress, sizeof(poDrvSpecificParam->abyStationAddress));
    }
    
    dwCcsrbar = sysGetPeripheralBase();
    
    poDrvSpecificParam->dwPhyMdioBase = dwCcsrbar + 0x24000; /* eTSEC 0 */
    poDrvSpecificParam->dwTbiPhyAddr = 16; /* Dummy address assigned to internal TBI PHY */
    poDrvSpecificParam->oMiiBusMtx = EC_NULL; /* LinkOsCreateLock(eLockType_DEFAULT); */
    
    if (dwInstance == 1) // eTSEC1 (Assigned to VxWorks, don't use!)
    {
       poDrvSpecificParam->abyStationAddress[5] = 0x24;
       poDrvSpecificParam->dwRegisterBase = dwCcsrbar + 0x24000;
       poDrvSpecificParam->dwPhyAddr = 25; /* from hwconf.c */
       poDrvSpecificParam->dwRxInterrupt = EPIC_TSEC1RX_INT_VEC; /* from vxbEpicIntCtlr.h */
    }
    else if (dwInstance == 2) // eTSEC2 (This one is used by EtherCAT)
    {
       poDrvSpecificParam->abyStationAddress[5] = 0x25;
       poDrvSpecificParam->dwRegisterBase = dwCcsrbar + 0x25000;
       poDrvSpecificParam->dwPhyAddr = 26; /* from hwconf.c */
       poDrvSpecificParam->dwRxInterrupt = EPIC_TSEC2RX_INT_VEC; /* from vxbEpicIntCtlr.h */
    }
    
    poDrvSpecificParam->dwLocalMdioBase = poDrvSpecificParam->dwRegisterBase;
    
#else
#   error "Board configuration must be selected"
#endif
    
#ifdef VXWORKS
   // Unload VxWorks VxBus driver if loaded
   muxDevUnload("motetsec", dwInstance - 1);
#endif
    
    return EC_TRUE;
}

#endif /* if (defined(LINUX) && defined(_ARCH_PPC)) || (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC32)) */

#if (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC440))

/********************************************************************************/
/** \brief  Create parameters for Xilinx LogiCORE IP XPS EMAC
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaEmac
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
 )
{
    return ::CreateLinkParaEmac(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
}
EC_T_BOOL CreateLinkParaEmac
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
 )
{
    EC_T_LINK_OPENPARMS_EMAC* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc        = EC_NULL;
  
    if(EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    if( dwInstance == 0 )
    {
        OsDbgMsg( "Device unit must be greater than 0, the first unit is 1!\n" );
        return EC_FALSE;
    }

    if( eLinkMode != EcLinkMode_INTERRUPT && eLinkMode != EcLinkMode_POLLING && eLinkMode != EcLinkMode_RANDOM)
    {
        OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",eLinkMode );
        return EC_FALSE;
    }

    poDrvSpecificParam  = &poLinkDevOpenParam->linkOpenParms.oEMAC;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_EMAC));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "EMAC", MAX_DRIVER_IDENT_LEN - 1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = &poLinkDevOpenParam->linkOpenParms.oEMAC;

    /* Link Layer Parameters for Xilinx EMAC */
    poDrvSpecificParam->dwType               = EC_LINK_TYPE_EMAC;
    poDrvSpecificParam->dwRegisterBase       = 0x86000000;
    poDrvSpecificParam->dwPhyAddr            = 0;
    poDrvSpecificParam->dwIstPriority        = dwRecvPriority;
    poDrvSpecificParam->dwRxInterrupt        = 1; // from xparameters.h
    
    if (dwInstance != 1)
    {
       OsDbgMsg( "Device unit must be 1.\n" );
       return EC_FALSE;
    }
    
    return EC_TRUE;
}

#endif /* (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC440)) */

/********************************************************************************/
/** \brief  Create parameters for Intel Pro 100 family (I8255x)
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaI8255x
(   
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] unit. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    return ::CreateLinkParaI8255x(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
}

EC_T_BOOL CreateLinkParaI8255x
(   
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] unit. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{

    EC_T_LINK_OPENPARMS_I8255x* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc        = EC_NULL;
    
    if(EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    if( dwInstance == 0 )
    {
        OsDbgMsg( "8255x PCI device unit must be greater than 0, the first unit is 1!\n" );
        return EC_FALSE;
    }

    if( eLinkMode != EcLinkMode_INTERRUPT && eLinkMode != EcLinkMode_POLLING && eLinkMode != EcLinkMode_RANDOM)
    {
        OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",eLinkMode );
        return EC_FALSE;

    }

    poDrvSpecificParam  = &poLinkDevOpenParam->linkOpenParms.oI8255x;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_I8255x));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "I8255x", MAX_DRIVER_IDENT_LEN - 1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = &poLinkDevOpenParam->linkOpenParms.oI8255x;

    /* Link Layer specific parameters */
	poDrvSpecificParam->dwType              = EC_LINK_TYPE_I8255x;
	poDrvSpecificParam->dwUnit              = dwInstance;
	poDrvSpecificParam->pfDoIntHandling     = EC_NULL;
    poDrvSpecificParam->dwIstPriority       = dwRecvPriority;

    return EC_TRUE;
}




/********************************************************************************/
/** \brief  Create parameters for Realtek 8139
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaRTL8139
(                 
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    return ::CreateLinkParaRTL8139(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
}

EC_T_BOOL CreateLinkParaRTL8139
(                 
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    EC_T_LINK_OPENPARMS_RTL8139* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*         pLinkDrvDesc        = EC_NULL;
        
    if(EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    if( dwInstance == 0 )
    {
        OsDbgMsg( "RTL8139 PCI device unit must be greater than 0, the first unit is 1!\n" );
        return EC_FALSE;
    }

    if( eLinkMode != EcLinkMode_INTERRUPT && eLinkMode != EcLinkMode_POLLING && eLinkMode != EcLinkMode_RANDOM)
    {
        OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",eLinkMode );
        return EC_FALSE;
        
    }

    poDrvSpecificParam = &poLinkDevOpenParam->linkOpenParms.oRtl8139;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_RTL8139));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "RTL8139", MAX_DRIVER_IDENT_LEN - 1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = &poLinkDevOpenParam->linkOpenParms.oRtl8139;

    /* Link Layer specific parameters */
	poDrvSpecificParam->dwType              = EC_LINK_TYPE_RTL8139;
	poDrvSpecificParam->dwUnit              = dwInstance;
	poDrvSpecificParam->pfDoIntHandling     = EC_NULL;
	poDrvSpecificParam->dwRxBuffers         = 96;
	poDrvSpecificParam->dwTxBuffers         = 96;
    poDrvSpecificParam->dwIstPriority       = dwRecvPriority;

    return EC_TRUE;

}

/********************************************************************************/
/** \brief  Create parameters for Realtek 8169
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaRTL8169
(                 
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    return ::CreateLinkParaRTL8169(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
}

EC_T_BOOL CreateLinkParaRTL8169
(                 
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    EC_T_LINK_OPENPARMS_RTL8169* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*         pLinkDrvDesc        = EC_NULL;
        
    if (EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    if ( dwInstance == 0 )
    {
        OsDbgMsg( "RTL8169 PCI device unit must be greater than 0, the first unit is 1!\n" );
        return EC_FALSE;
    }

    if ( eLinkMode != EcLinkMode_INTERRUPT && eLinkMode != EcLinkMode_POLLING && eLinkMode != EcLinkMode_RANDOM)
    {
        OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",eLinkMode );
        return EC_FALSE;
        
    }

    poDrvSpecificParam = &poLinkDevOpenParam->linkOpenParms.oRtl8169;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_RTL8169));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "RTL8169", MAX_DRIVER_IDENT_LEN - 1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = poDrvSpecificParam;

    /* Link Layer specific parameters */
    poDrvSpecificParam->dwType              = EC_LINK_TYPE_RTL8169;
    poDrvSpecificParam->dwUnit              = dwInstance;
    poDrvSpecificParam->pfDoIntHandling     = EC_NULL;
    poDrvSpecificParam->dwIstPriority       = dwRecvPriority;

    return EC_TRUE;

}


/********************************************************************************/
/** \brief  Create parameters for WinPCap
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaWinPcap
(                  
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_BYTE*                arrIpAddress,         /* [in] IP-address */
    EC_T_LINKMODE             eLinkMode             /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
)
{
    return ::CreateLinkParaWinPcap(poLinkDevOpenParam, arrIpAddress, eLinkMode);
}

EC_T_BOOL CreateLinkParaWinPcap
(                  
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_BYTE*                arrIpAddress,         /* [in] IP-address */
    EC_T_LINKMODE             eLinkMode             /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
)
{
    EC_T_LINK_OPENPARMS_WINPCAP* poDrvSpecificParam     = EC_NULL;
    EC_T_LINK_DEV_PARAM*         pLinkDrvDesc           = EC_NULL;
        
    if(EC_NULL == poLinkDevOpenParam || EC_NULL == arrIpAddress)
    {
        return EC_FALSE;
    }

    poDrvSpecificParam  = &poLinkDevOpenParam->linkOpenParms.oWinPcap;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_WINPCAP));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "Pcap", MAX_DRIVER_IDENT_LEN - 1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = &poLinkDevOpenParam->linkOpenParms.oWinPcap;

    /* Link Layer specific parameters */    
    poDrvSpecificParam->arrIpAddress[0]          = arrIpAddress[0];
    poDrvSpecificParam->arrIpAddress[1]          = arrIpAddress[1];
    poDrvSpecificParam->arrIpAddress[2]          = arrIpAddress[2];
    poDrvSpecificParam->arrIpAddress[3]          = arrIpAddress[3];

    return EC_TRUE;
}



/********************************************************************************/
/** \brief  Create parameters for Snarf
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaSnarf
(   EC_T_LINK_DEV_OPEN_PARAM*   poLinkDevOpenParam, 
    EC_T_WCHAR*                 aszAdapterName 
)
{
    return ::CreateLinkParaSnarf(poLinkDevOpenParam, aszAdapterName);
}

EC_T_BOOL CreateLinkParaSnarf
(   EC_T_LINK_DEV_OPEN_PARAM*   poLinkDevOpenParam, 
    EC_T_WCHAR*                 aszAdapterName 
)
{

    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc         = EC_NULL;
    EC_T_LINK_OPENPARMS_SNARF*  poDrvSpecificParam   = EC_NULL;

    if(EC_NULL == poLinkDevOpenParam || EC_NULL == aszAdapterName || OsStrlen(aszAdapterName) > MAX_LEN_SNARF_ADAPTER_NAME )
    {
        return EC_FALSE;
    }

    poDrvSpecificParam  = &poLinkDevOpenParam->linkOpenParms.oSnarf;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_SNARF));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));
    
    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "Snarf", MAX_DRIVER_IDENT_LEN - 1);
    pLinkDrvDesc->eLinkMode                      = EcLinkMode_POLLING;
    pLinkDrvDesc->pvDrvSpecificParam             = &poLinkDevOpenParam->linkOpenParms.oWinPcap;

    /* Link Layer specific parameters */
    poDrvSpecificParam =  &poLinkDevOpenParam->linkOpenParms.oSnarf;

    OsStrncpy(poDrvSpecificParam->aszAdapterName, aszAdapterName, sizeof(poDrvSpecificParam->aszAdapterName) - 1);

    return EC_TRUE;
}


/********************************************************************************/
/** \brief  Create parameters for Etherlib
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaEtherlib
(   EC_T_LINK_DEV_OPEN_PARAM*   poLinkDevOpenParam, 
    EC_T_WCHAR*                 aszAdapterName 
)
{
    return ::CreateLinkParaEtherlib(poLinkDevOpenParam, aszAdapterName);
}
EC_T_BOOL CreateLinkParaEtherlib
(   EC_T_LINK_DEV_OPEN_PARAM*   poLinkDevOpenParam, 
    EC_T_WCHAR*                 aszAdapterName 
)
{

    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc         = EC_NULL;
    EC_T_INT                    slen = (EC_T_INT) OsStrlen(aszAdapterName);

    if(EC_NULL == poLinkDevOpenParam || EC_NULL == aszAdapterName || OsStrlen(aszAdapterName) > MAX_LEN_SNARF_ADAPTER_NAME )
    {
        return EC_FALSE;
    }

    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));
    
    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "Etherlib", sizeof(pLinkDrvDesc->szDriverIdent) - 1);
    pLinkDrvDesc->eLinkMode                      = EcLinkMode_POLLING;
    pLinkDrvDesc->pvDrvSpecificParam             = OsMalloc(slen + 1);
    OsStrncpy(pLinkDrvDesc->pvDrvSpecificParam, aszAdapterName, slen);

    return EC_TRUE;
}

/********************************************************************************/
/** \brief  Create parameters for Marvell PXA MFU
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaPxaMfu
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    return ::CreateLinkParaPxaMfu(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
}

EC_T_BOOL CreateLinkParaPxaMfu
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    EC_T_LINK_OPENPARMS_PXAMFU* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc        = EC_NULL;

    if( EC_NULL == poLinkDevOpenParam )
    {
        return EC_FALSE;
    }

    if( dwInstance == 0 )
    {
        OsDbgMsg( "PXA MFU device unit must be greater than 0, the first unit is 1!\n" );
        return EC_FALSE;
    }

    if( eLinkMode != EcLinkMode_INTERRUPT && eLinkMode != EcLinkMode_POLLING && eLinkMode != EcLinkMode_RANDOM)
    {
        OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",eLinkMode );
        return EC_FALSE;

    }

    poDrvSpecificParam  = &poLinkDevOpenParam->linkOpenParms.oPxaMfu;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_PXAMFU));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "PXAMFU", MAX_DRIVER_IDENT_LEN-1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = poDrvSpecificParam;

    /* Link Layer Parameters for FreeScale FEC */
    poDrvSpecificParam->dwType              = EC_LINK_TYPE_PXAMFU;
    poDrvSpecificParam->dwUnit              = dwInstance;
    poDrvSpecificParam->pfDoIntHandling     = EC_NULL;
//  poDrvSpecificParam->dwRxBuffers         = 96;
//  poDrvSpecificParam->dwTxBuffers         = 96;
    poDrvSpecificParam->dwIstPriority       = dwRecvPriority;

    return EC_TRUE;
}

/********************************************************************************/
/** \brief  Create parameters for Beckhoff CCAT
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaCCAT
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    return ::CreateLinkParaCCAT(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
}

EC_T_BOOL CreateLinkParaCCAT
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwInstance,                     /* [in] instance. starts with 1 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    EC_T_LINK_OPENPARMS_CCAT* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*      pLinkDrvDesc        = EC_NULL;
        
    if(EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    if( dwInstance == 0 )
    {
        OsDbgMsg( "CCAT PCI device unit must be greater than 0, the first unit is 1!\n" );
        return EC_FALSE;
    }

    if( eLinkMode != EcLinkMode_INTERRUPT && eLinkMode != EcLinkMode_POLLING && eLinkMode != EcLinkMode_RANDOM)
    {
        OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",eLinkMode );
        return EC_FALSE;
        
    }

    poDrvSpecificParam = &poLinkDevOpenParam->linkOpenParms.oCCAT;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_CCAT));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "CCAT", MAX_DRIVER_IDENT_LEN-1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = &poLinkDevOpenParam->linkOpenParms.oCCAT;

	/* Link Layer Parameters for Beckhoff CCAT */
	poDrvSpecificParam->dwType              = EC_LINK_TYPE_CCAT;
	poDrvSpecificParam->dwUnit              = dwInstance;
	poDrvSpecificParam->pfDoIntHandling     = EC_NULL;
    poDrvSpecificParam->dwIstPriority       = dwRecvPriority;

    return EC_TRUE;
}

#if (defined INCLUDE_CPSW)

#ifdef STARTERWARE_NOOS
#  define CPSW_CFG_BEAGLEBONE /* TI's Beaglebone */
#  undef  CPSW_CFG_387X_EVM
#else
#  undef  CPSW_CFG_387X_EVM /* Mistral / TI Sitara 387x Evaluation Board */
#  define CPSW_CFG_BEAGLEBONE
#endif

/********************************************************************************/
/** \brief  Create parameters for TI's CPSW
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaCPSW
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwPort,                         /* [in] port. 1 or 2 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority,                 /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
    EC_T_DWORD dwPortPriority,
    EC_T_BOOL bMaster,
    EC_T_BOOL bUseStarterWareDriver
 )
{
    return ::CreateLinkParaCPSW(poLinkDevOpenParam, dwPort, eLinkMode, dwRecvPriority, dwPortPriority, bMaster, bUseStarterWareDriver);
}

EC_T_BOOL CreateLinkParaCPSW
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_DWORD      dwPort,                         /* [in] port. 1 or 2 */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority,                 /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
    EC_T_DWORD dwPortPriority,
    EC_T_BOOL bMaster,
    EC_T_BOOL bUseStarterWareDriver
 )
{
    EC_T_LINK_OPENPARMS_CPSW* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc        = EC_NULL;
    EC_T_BYTE abyStationAddress[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    if(EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    if (dwPort < 1 || dwPort > 2)
    {
        OsDbgMsg( "Port number must be 1 or 2\n" );
        return EC_FALSE;
    }

    if (dwPortPriority != 0 && dwPortPriority != 1)
    {
        OsDbgMsg( "Port priority must be 0 or 1\n" );
        return EC_FALSE;
    }

    if( eLinkMode != EcLinkMode_INTERRUPT && eLinkMode != EcLinkMode_POLLING)
    {
        OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",eLinkMode );
        return EC_FALSE;

    }

    poDrvSpecificParam = &poLinkDevOpenParam->linkOpenParms.oCPSW;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_CPSW));

    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    if( bUseStarterWareDriver )
        OsStrncpy(pLinkDrvDesc->szDriverIdent, "SWCPSW", MAX_DRIVER_IDENT_LEN-1);
    else
        OsStrncpy(pLinkDrvDesc->szDriverIdent, "CPSW", MAX_DRIVER_IDENT_LEN-1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = poDrvSpecificParam;

    /* Link Layer Parameters for CPSW */
    poDrvSpecificParam->dwType              = EC_LINK_TYPE_CPSW;
    poDrvSpecificParam->bMaster             = bMaster;
    poDrvSpecificParam->dwPortIdx           = dwPort - 1;
    poDrvSpecificParam->dwPortPrio          = dwPortPriority;
    poDrvSpecificParam->dwIstPriority       = dwRecvPriority;
    poDrvSpecificParam->bPhyRestartAutoNegotiation = EC_TRUE;

    abyStationAddress[5] = (EC_T_BYTE) dwPort;
    OsMemcpy(poDrvSpecificParam->abyStationAddress, abyStationAddress, 
       sizeof(poDrvSpecificParam->abyStationAddress));

#if defined CPSW_CFG_387X_EVM
    poDrvSpecificParam->dwPhyAddr = dwPort - 1; // 0 -> Port1, 1 -> Port2
    OsMemcpy(&poDrvSpecificParam->oPlatCfg, &AM387x_pdata, sizeof(AM387x_pdata));
#elif defined CPSW_CFG_BEAGLEBONE
    poDrvSpecificParam->dwPhyAddr = 0; // 0 -> Port1
    OsMemcpy(&poDrvSpecificParam->oPlatCfg, &AM33xx_pdata, sizeof(AM33xx_pdata));
#else
#   error Board selection required
#endif

    return EC_TRUE;
}
#endif /* if (defined INCLUDE_CPSW) */

#if (defined INCLUDE_SMSC)
/********************************************************************************/
/** \brief  Create parameters for SMSC LAN9218i
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaSmscL9218i
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
 )
{
    return ::CreateLinkParaSmscL9218i(poLinkDevOpenParam, eLinkMode, dwRecvPriority);
}

EC_T_BOOL CreateLinkParaSmscL9218i
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_LINKMODE   eLinkMode,                      /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
 )
{
    EC_T_LINK_OPENPARMS_L9218i* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc        = EC_NULL;

    if (EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    poDrvSpecificParam = &poLinkDevOpenParam->linkOpenParms.oL9218i;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_L9218i));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "L9218i", MAX_DRIVER_IDENT_LEN-1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = &poLinkDevOpenParam->linkOpenParms.oL9218i;

    /* Link Layer Parameters for L9218i */
    poDrvSpecificParam->dwType              = EC_LINK_TYPE_L9218i;
    poDrvSpecificParam->dwIstPriority       = dwRecvPriority;

    return EC_TRUE;
}
#endif /* if (defined INCLUDE_SMSC) */


#if (defined INCLUDE_GEM)
/********************************************************************************/
/** \brief  Create parameters for Zynq-7000
*
*
* \return  Status value.
*/
EC_T_BOOL CEmDemoCommon::CreateLinkParaGem
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_LINKMODE             eLinkMode,            /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD                dwPort                /* [in] port 0 or 1 */
 )
{
    return ::CreateLinkParaGem(poLinkDevOpenParam, eLinkMode, dwPort);
}

EC_T_BOOL CreateLinkParaGem
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_LINKMODE             eLinkMode,            /* [in] mode EcLinkMode_POLLING or EcLinkMode_INTERRUPT */
    EC_T_DWORD                dwPort                /* [in] port 0 or 1 */
 )
{
    EC_T_LINK_OPENPARMS_GEM*    poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc        = EC_NULL;

    if(EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    if( eLinkMode != EcLinkMode_INTERRUPT && eLinkMode != EcLinkMode_POLLING)
    {
        OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",eLinkMode );
        return EC_FALSE;
    }
    if( dwPort != 0 && dwPort != 1)
    {
        OsDbgMsg( "Invalid port index (port == %d)",dwPort );
        return EC_FALSE;
    }

    poDrvSpecificParam  = &poLinkDevOpenParam->linkOpenParms.oGEM;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_GEM));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "GEM", MAX_DRIVER_IDENT_LEN - 1);
    pLinkDrvDesc->eLinkMode                      = eLinkMode;
    pLinkDrvDesc->pvDrvSpecificParam             = &poLinkDevOpenParam->linkOpenParms.oGEM;

    /* Link Layer Parameters for Xilinx EMAC */
    poDrvSpecificParam->dwType               = EC_LINK_TYPE_GEM;
    poDrvSpecificParam->dwPortIdx            = dwPort;
    if (dwPort == 1)
    {
        poDrvSpecificParam->eRxSource            = eGemRxSource_EMIO;
        poDrvSpecificParam->dwPhyAddr            = 1;
    }
    else
    {
        poDrvSpecificParam->eRxSource            = eGemRxSource_MIO;
        poDrvSpecificParam->dwPhyAddr            = 4;
    }
    poDrvSpecificParam->dwRxInterrupt = poDrvSpecificParam->dwPortIdx;

    return EC_TRUE;
}
#endif /* if (defined INCLUDE_GEM) */

#if (defined EC_VERSION_NIOS)
/********************************************************************************/
/** \brief  Create parameters for Nios II
*
*
* \return  Status value.
*/
EC_T_BOOL CreateLinkParaNios
(
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam   /* [in] pointer to parameter buffer */
)
{
    EC_T_LINK_DEV_PARAM*        pLinkDrvDesc        = EC_NULL;

    if(EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "NIOS", MAX_DRIVER_IDENT_LEN - 1);

    pLinkDrvDesc->eLinkMode                      = EcLinkMode_POLLING;
    pLinkDrvDesc->pvDrvSpecificParam             = EC_NULL;

    // TODO specify correct parameters

    return EC_TRUE;
}
#endif /* if (defined EC_VERSION_NIOS) */


#if (defined UNDER_CE)
/********************************************************************************/
/** \brief  Create parameters for Windows CE/EC NDISUIO
*
*
* \return  Status value.
*/
EC_T_BOOL CreateLinkParaNdisuio
(                 
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,   /* [in] pointer to parameter buffer */
    EC_T_CHAR*      pszName,
    EC_T_DWORD      dwRecvPriority                  /* [in] interrupt service thread priority. Only when EcLinkMode_INTERRUPT*/
)
{
    EC_T_LINK_OPENPARMS_NDISUIO* poDrvSpecificParam  = EC_NULL;
    EC_T_LINK_DEV_PARAM*         pLinkDrvDesc        = EC_NULL;
    EC_T_WCHAR*                  pszAdapterName      = EC_NULL;

    if (EC_NULL == poLinkDevOpenParam)
    {
        return EC_FALSE;
    }

    poDrvSpecificParam  = &poLinkDevOpenParam->linkOpenParms.oNdisUio;
    OsMemset(poDrvSpecificParam, 0, sizeof(EC_T_LINK_OPENPARMS_NDISUIO));
    pLinkDrvDesc = &poLinkDevOpenParam->oLinkDevParam;
    OsMemset(pLinkDrvDesc, 0, sizeof(EC_T_LINK_DEV_PARAM));

    /* Common Link Layer parameters */
    pLinkDrvDesc->dwValidationPattern            = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDrvDesc->dwDevParamVersion              = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy(pLinkDrvDesc->szDriverIdent, "NdisUio", MAX_DRIVER_IDENT_LEN - 1);
    pLinkDrvDesc->eLinkMode                      = EcLinkMode_POLLING;
    pLinkDrvDesc->pvDrvSpecificParam             = poDrvSpecificParam;

    /* NdisUio uses the network adapter name to select the appropriate network interface */
    poDrvSpecificParam->dwType               = EC_LINK_TYPE_NDISUIO;
    pszAdapterName = (EC_T_WCHAR*)&(((EC_T_BYTE*)poDrvSpecificParam)[sizeof(EC_T_LINK_OPENPARMS_NDISUIO)]);
    _snwprintf((wchar_t*)pszAdapterName, 64, L"%S", pszName);
    poDrvSpecificParam->szNetworkAdapterName = pszAdapterName;
    poDrvSpecificParam->dwRcvThreadPriority  = dwRecvPriority;

    /* check if NDISUIO driver started */
    {
    #define NDISUIO_DEVNAME   TEXT("ECT1:")
    #define NDISUIO_DRIVERKEY TEXT("Drivers\\BuiltIn\\ECAT")
        HANDLE                      hNdisUioDevice  = EC_NULL;
        HANDLE                      hNdisUioDriver  = EC_NULL;

        OsDbgMsg("Check if ECAT driver is started...");
        hNdisUioDevice = CreateFile( NDISUIO_DEVNAME, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
        if ((hNdisUioDevice != EC_NULL) && (hNdisUioDevice != INVALID_HANDLE_VALUE))
        {
            OsDbgMsg("Ok!\n");
        }
        else
        {
            OsDbgMsg("Failed!\n");
            hNdisUioDevice = EC_NULL;
        }
        /* try to load driver if not already loaded */
        if (hNdisUioDevice == EC_NULL)
        {
            OsDbgMsg("Try to load ECAT driver...");
            hNdisUioDriver = ActivateDeviceEx (NDISUIO_DRIVERKEY, 0,  0, 0);
            if ((hNdisUioDriver != EC_NULL) && (hNdisUioDriver != INVALID_HANDLE_VALUE))
            {
                OsDbgMsg( "Ok!\n" );
            }
            else
            {
                OsDbgMsg( "Failed!\n" );
                hNdisUioDriver = EC_NULL;
                return EC_FALSE;
            }
        }
        else
        {
            /* close handle, it was just for the check */
            CloseHandle(hNdisUioDevice);
            hNdisUioDevice = EC_NULL;
        }
    }
    return EC_TRUE;
}
#endif

/*-END OF SOURCE FILE--------------------------------------------------------*/
