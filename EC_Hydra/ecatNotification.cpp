/*-----------------------------------------------------------------------------
 * ecatNotification.cpp
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Willig, Andreas
 * Description              EtherCAT Master generic notification handler
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/

#include "ecatNotification.h"
#include "TimeoutHelper.h"
#include "ecatDemoCommon.h"
#include <EcOs.h>

#include <AtEmRasError.h>

/*-DEFINES-------------------------------------------------------------------*/
#define MAX_MSG_PER_ERROR       1           /* maximum number of error messages printed */

#define SB_PORT_BLOCK_TIMEOUT   2000        /* msecs to wait for invalid slave node block */

#define LogError    this->m_pcLogging->LogError
#define LogErrorAdd this->m_pcLogging->LogErrorAdd
#define LogMsg      this->m_pcLogging->LogMsg

/*-TYPEDEFS------------------------------------------------------------------*/

/*-CLASS---------------------------------------------------------------------*/

/*-FUNCTION DECLARATION------------------------------------------------------*/

/*-LOCAL VARIABLES-----------------------------------------------------------*/

/*-FUNCTIONS-----------------------------------------------------------------*/

/***************************************************************************************************/
/**
\brief  Constructor.
*/
CEmNotification::CEmNotification(
                EC_T_DWORD      dwMasterInstance,   /**< [in]   Master Instance */
                CAtEmLogging*   pcLogging,          /**< [in]   Logging */
                EC_T_BOOL       bRasClient          /**< [in]   Remote API client */
                                )
{
    m_dwMasterInstance = dwMasterInstance;
    m_pcLogging = pcLogging;

    OsMemset(m_adwErrorCounter, 0, sizeof(m_adwErrorCounter));

    m_pfProcessNotificationHookInstance = EC_NULL;
    m_pfProcessNotificationHookFunction = EC_NULL;

    //m_nVerbosePrinting                  = EC_FALSE;
    m_nVerbosePrinting                  = EC_TRUE;
    m_bAllDevsOperational               = EC_FALSE;

    m_bRasServerDisconnect              = EC_FALSE;
    m_bRasClient                        = bRasClient;
    m_bDidIssueBlock                    = EC_FALSE;

    OsMemset(&m_oSlaveJobQueue, 0, sizeof(T_SLAVEJOBQUEUE));

    m_oSlaveJobQueue.pRead              = &(m_oSlaveJobQueue.Jobs[0]);
    m_oSlaveJobQueue.pWrite             = &(m_oSlaveJobQueue.Jobs[0]);

    m_dwClientID = 0;
}


/********************************************************************************/
/** \brief  EtherCAT notification
*
* This function is called on EtherCAT events.
* No EtherCAT functions may be called here (unless explicitly allowed in the documentation)!!
* It should be left as fast as possible.
*
*
* \return Currently always EC_E_NOERROR has to be returned.
*/
EC_T_DWORD CEmNotification::ecatNotify(
    EC_T_DWORD          dwCode,         /**< [in]   Notification code */
    EC_T_NOTIFYPARMS*   pParms          /**< [in]   Notification data */
                                      )
{
    EC_T_ERROR_NOTIFICATION_DESC*   pErrorNotificationDesc  = EC_NULL;
    EC_T_NOTIFICATION_DESC*         pNotificationDesc       = EC_NULL;
    static EC_T_DWORD               s_dwClearErrorMsecCount = 0;
    EC_T_DWORD                      dwRetVal                = EC_E_NOERROR;
    EC_T_DWORD                      dwRes                   = EC_E_ERROR;

    EC_T_BOOL                       bVerbose                = EC_FALSE;
    T_SLAVEJOBS                     oCurJob;

    bVerbose    = m_nVerbosePrinting;

    switch (dwCode)
    {
        /************************/
        /* generic notification */
        /************************/
    case EC_NOTIFY_STATECHANGED: /* GEN|1 */
        {
            EC_T_STATECHANGE* pStateChangeParms = (EC_T_STATECHANGE*)pParms->pbyInBuf;

            if(bVerbose)
            {
                LogMsg(
                    ecatGetText(EC_TXT_MASTER_STATE_CHANGE),
                    EcStateString(pStateChangeParms->oldState),
                    EcStateString(pStateChangeParms->newState)
                    );
            }

            if( (eEcatState_PREOP == (pStateChangeParms->newState)) )
            {
            }
            else if( (eEcatState_OP == (pStateChangeParms->newState)) )
            {
                m_bAllDevsOperational = EC_TRUE;
            }
        } break ;
    case EC_NOTIFY_ETH_LINK_CONNECTED: /* GEN|2 */
        {
            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;

            /* Hint: No error, but show error clearance in Error.log */
            LogError(ecatGetText(EC_TXT_CABLE_CONNECTED));

        } break;
    case EC_NOTIFY_SB_STATUS:   /* GEN|3 */
        {
            /* Scanbus did finish it's processing when this Notification is called */
            pNotificationDesc = (EC_T_NOTIFICATION_DESC*)pParms->pbyInBuf;

            dwRes = pNotificationDesc->desc.ScanBusNtfyDesc.dwResultCode;

            if (bVerbose && (EC_E_NOERROR == dwRes))
            {
                LogMsg( ecatGetText(EC_TXT_SB_RESULT_OK),
                    pNotificationDesc->desc.ScanBusNtfyDesc.dwSlaveCount
                    );
            }

            if (EC_E_NOERROR != dwRes)
            {
                oCurJob.dwCode = EC_NOTIFY_SB_STATUS;
                OsMemcpy(&(oCurJob.JobData), &(pNotificationDesc->desc.ScanBusNtfyDesc), sizeof(EC_T_SB_STATUS_NTFY_DESC) );
                if( !EnqueueJob(&oCurJob) )
                {
                    LogError("Unable to enqueue Scan Bus Error Processing Job (missing calls to ProcessNotificationJobs)");
                }
            }
        } break;
    case EC_NOTIFY_DC_STATUS:   /* GEN|4 */
        {
            EC_T_DWORD  dwStatus = 0;
            
            /* Initialization of DC Instance finished when this notification is called */
            dwStatus  = EC_GETDWORD((pParms->pbyInBuf));

            if( EC_E_NOERROR != dwStatus )
            {
                LogError("Error: DC Initialization failed. ErrorCode = 0x%x",dwStatus);
            }
        } break;
    case EC_NOTIFY_DC_SLV_SYNC: /* GEN|5 */
        {
            /* This notification is called if state of slave deviation surveillance changes */
            EC_T_DC_SYNC_NTFY_DESC* pNtfySlv    = EC_NULL;

            pNotificationDesc   = (EC_T_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pNtfySlv            = &(pNotificationDesc->desc.SyncNtfyDesc);

            if(pNtfySlv->IsInSync)
            {
                if(bVerbose)
                {
                    LogMsg(ecatGetText(EC_TXT_DCSLVSYNC_INSYNC), pNtfySlv->dwDeviation);
                }
            }
            else
            {
                LogError(ecatGetText(EC_TXT_DCSLVSYNC_OUTOFSYNC), pNtfySlv->dwDeviation);
                LogError("DC Slave  \"%s\": - EtherCAT address=%d", pNtfySlv->SlaveProp.achName, pNtfySlv->SlaveProp.wStationAddress);
            }
        } break;
    case EC_NOTIFY_DCL_STATUS:  /* GEN|8 */
        {
            /* Initialization of DC Latching Instance finished when this notification is called */

            if(bVerbose)
            {
                LogMsg(
                    ecatGetText(EC_TXT_DCL_STATUS),
                    ecatGetText(EC_LOWORD(EC_GETDWORD((pParms->pbyInBuf)))),
                    EC_GETDWORD((pParms->pbyInBuf))
                      );
            }
        } break;
    case EC_NOTIFY_DCM_SYNC:  /* GEN|9 */
        {
            /* This notification is called if state of DCM error monitoring changes */
            EC_T_DCM_SYNC_NTFY_DESC* pDcmInSyncNotify = EC_NULL;

            pNotificationDesc = (EC_T_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pDcmInSyncNotify  = &(pNotificationDesc->desc.DcmInSyncDesc);

            if (pDcmInSyncNotify->IsInSync)
            {
                if (bVerbose)
                {
                    LogMsg(ecatGetText(EC_TXT_DCM_INSYNC), pDcmInSyncNotify->nCtlErrorNsecCur, pDcmInSyncNotify->nCtlErrorNsecAvg, pDcmInSyncNotify->nCtlErrorNsecMax);
                }
            }
            else
            {
                LogError(ecatGetText(EC_TXT_DCM_OUTOFSYNC), pDcmInSyncNotify->nCtlErrorNsecCur, pDcmInSyncNotify->nCtlErrorNsecAvg, pDcmInSyncNotify->nCtlErrorNsecMax);
            }
        } break;
    case EC_NOTIFY_SLAVE_STATECHANGED: /* GEN|21 */
        {
            pNotificationDesc = (EC_T_NOTIFICATION_DESC*)pParms->pbyInBuf;

            LogMsg( ecatGetText(EC_TXT_SLAVE_STATECHANGED),
                pNotificationDesc->desc.SlaveStateChangedDesc.SlaveProp.wStationAddress,
                EcStateString(pNotificationDesc->desc.SlaveStateChangedDesc.newState));
        } break;

    case EC_NOTIFY_RAWCMD_DONE:     /* GEN|100 */
        {
            /* This notification is called to provide the response to a ecatQueueRawCmd Call */
            EC_T_RAWCMDRESPONSE_NTFY_DESC* pRawCmdDesc = EC_NULL;

            pNotificationDesc   = (EC_T_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pRawCmdDesc         = &(pNotificationDesc->desc.RawCmdRespNtfyDesc);

            oCurJob.dwCode       = EC_NOTIFY_RAWCMD_DONE;
            OsMemcpy(&(oCurJob.JobData), pRawCmdDesc, sizeof(EC_T_RAWCMDRESPONSE_NTFY_DESC)-sizeof(EC_T_BYTE*));
            EC_T_DWORD dwLength = pRawCmdDesc->dwLength;
            if( dwLength > MAX_EC_DATA_LEN )
            {
                dwLength = MAX_EC_DATA_LEN;
            }

            OsMemcpy(oCurJob.JobData.RawCmdJob.abyData, pRawCmdDesc->pbyData, dwLength);

            if( !EnqueueJob(&oCurJob) )
            {
                LogError("Unable to enqueue raw command Processing Job (missing calls to ProcessNotificationJobs)");
            }
        } break;

        /************************/
        /* mailbox notification */
        /************************/
    case EC_NOTIFY_MBOXRCV:     /* MBOXRCV|0 */
            {
                EC_T_MBXTFER*           pMbxTfer = EC_NULL;       /* mailbox transfer object */

                OsDbgAssert( pParms->dwInBufSize == sizeof(EC_T_MBXTFER) );
                pMbxTfer = (EC_T_MBXTFER*)pParms->pbyInBuf;
                OsDbgAssert( pMbxTfer != EC_NULL );

                switch( pMbxTfer->eMbxTferType )
                {
                    /***************************************************************************************************************************/
                case eMbxTferType_COE_SDO_DOWNLOAD:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        LogError( ecatGetText(EC_TXT_COE_SDO_DNLD_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                                );
                    }
                    pMbxTfer->eTferStatus = eMbxTferStatus_Idle;
                    break;

                    /***************************************************************************************************************************/
                case eMbxTferType_COE_SDO_UPLOAD:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        LogError( ecatGetText(EC_TXT_COE_SDO_UPLD_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                                );
                    }
                    pMbxTfer->eTferStatus = eMbxTferStatus_Idle;
                    break;

                    /***************************************************************************************************************************/
                case eMbxTferType_COE_GETODLIST:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        LogError( ecatGetText(EC_TXT_COE_GETODL_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                                );
                    }
                    pMbxTfer->eTferStatus = eMbxTferStatus_Idle;
                    break;

                    /***************************************************************************************************************************/
                case eMbxTferType_COE_GETOBDESC:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        LogError( ecatGetText(EC_TXT_COE_GETOBDESC_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                                );
                    }
                    pMbxTfer->eTferStatus = eMbxTferStatus_Idle;
                    break;

                    /***************************************************************************************************************************/
                case eMbxTferType_COE_GETENTRYDESC:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        /* don't show message if sub-index does not exist */
                        if(
                            (pMbxTfer->dwErrorCode != EC_E_INVALIDDATA)
                        &&  (pMbxTfer->dwErrorCode != EC_E_SDO_ABORTCODE_OFFSET)
                          )
                        {
                            LogError( ecatGetText(EC_TXT_COE_GETENTRYDESC_ERROR),
                                pMbxTfer->eTferStatus,
                                pMbxTfer->dwErrorCode,
                                ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                                    );
                        }
                    }
                    pMbxTfer->eTferStatus = eMbxTferStatus_Idle;
                    break;

                    /***************************************************************************************************************************/
                case eMbxTferType_COE_EMERGENCY:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        LogError( ecatGetText(EC_TXT_COE_EMRG_TFER_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                                );
                    }
                    else
                    {
                        LogMsg( ecatGetText(EC_TXT_COE_EMRG),
                            pMbxTfer->dwTferId,
                            pMbxTfer->dwDataLen,
                            (EC_T_INT)pMbxTfer->MbxData.CoE_Emergency.wStationAddress,
                            (EC_T_INT)pMbxTfer->MbxData.CoE_Emergency.wErrorCode,
                            (EC_T_INT)pMbxTfer->MbxData.CoE_Emergency.byErrorRegister,
                            (EC_T_INT)pMbxTfer->MbxData.CoE_Emergency.abyData[0],
                            (EC_T_INT)pMbxTfer->MbxData.CoE_Emergency.abyData[1],
                            (EC_T_INT)pMbxTfer->MbxData.CoE_Emergency.abyData[2],
                            (EC_T_INT)pMbxTfer->MbxData.CoE_Emergency.abyData[3],
                            (EC_T_INT)pMbxTfer->MbxData.CoE_Emergency.abyData[4]
                              );
                    }
                    break;
#ifdef INCLUDE_FOE_SUPPORT
                    /***************************************************************************************************************************/
                case eMbxTferType_FOE_FILE_DOWNLOAD:
                    if (pMbxTfer->eTferStatus == eMbxTferStatus_TferReqError)
                    {
                        LogError( ecatGetText(EC_TXT_FOE_DNLD_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                            );
                        pMbxTfer->eTferStatus = eMbxTferStatus_Idle;
                    }
                    else if (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone)
                    {
			            if(bVerbose)
	                    {
	                        LogMsg("FoE download %3d%%", ((pMbxTfer->MbxData.dwFoETransferredBytes*100)/pMbxTfer->dwDataLen));
	                    }
                    }
                    else
                    {
			            if(bVerbose)
	                    {
	                        LogMsg("FoE download done");
	                    }
                        pMbxTfer->eTferStatus = eMbxTferStatus_Idle;
                    }
                    break;
                    /***************************************************************************************************************************/
                case eMbxTferType_FOE_FILE_UPLOAD:
                    if (pMbxTfer->eTferStatus == eMbxTferStatus_TferReqError)
                    {
                        LogError( ecatGetText(EC_TXT_FOE_UPLD_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                            );
                        pMbxTfer->eTferStatus = eMbxTferStatus_Idle;
                    }
                    else if (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone)
                    {
			            if(bVerbose)
	                    {
	                        LogMsg("FoE upload %3d%%", ((pMbxTfer->MbxData.dwFoETransferredBytes*100)/pMbxTfer->dwDataLen));
	                    }
                    }
                    else
                    {
			            if(bVerbose)
	                    {
	                        LogMsg("FoE upload done");
	                    }
                        pMbxTfer->eTferStatus = eMbxTferStatus_Idle;
                    }
                    break;
#endif /* INCLUDE_FOE_SUPPORT */
#ifdef INCLUDE_SOE_SUPPORT
                    /***************************************************************************************************************************/
                case eMbxTferType_SOE_WRITEREQUEST:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        LogError( ecatGetText(EC_TXT_SOE_WRITE_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                            );

                    }
                    
                    break;
                    /***************************************************************************************************************************/
                case eMbxTferType_SOE_READREQUEST:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        LogError( ecatGetText(EC_TXT_SOE_READ_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                            );

                    }
                    
                    break;
                    /***************************************************************************************************************************/
                case eMbxTferType_SOE_EMERGENCY:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
   
                        LogError( ecatGetText(EC_TXT_SOE_EMRG_TFER_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                            );

                    }
                    else
                    {
  
                        LogMsg( ecatGetText(EC_TXT_SOE_EMRG),
                            pMbxTfer->dwTferId,
                            pMbxTfer->dwDataLen,
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Emergency.wStationAddress,
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Emergency.wHeader,
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Emergency.abyData[0],
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Emergency.abyData[1],
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Emergency.abyData[2],
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Emergency.abyData[3],
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Emergency.abyData[4]
                            );

                    }
                    break;
                    /***************************************************************************************************************************/
                case eMbxTferType_SOE_NOTIFICATION:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        LogError( ecatGetText(EC_TXT_SOE_NOTIFIC_TFER_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                            );
                    }
                    else
                    {
         
                        LogMsg( ecatGetText(EC_TXT_SOE_NOTIFICATION),
                            pMbxTfer->dwTferId,
                            pMbxTfer->dwDataLen,
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Notification.wStationAddress,
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Notification.wHeader,
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Notification.wIdn,
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Notification.abyData[0],
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Notification.abyData[1],
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Notification.abyData[2],
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Notification.abyData[3],
                            (EC_T_INT)pMbxTfer->MbxData.SoE_Notification.abyData[4]
                            );

                    }
                    break;
#endif
#ifdef INCLUDE_VOE_SUPPORT
                case eMbxTferType_VOE_MBX_WRITE:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        LogError( ecatGetText(EC_TXT_VOE_DNLD_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                            );
                    }
                    break;
                case eMbxTferType_VOE_MBX_READ:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        LogError( ecatGetText(EC_TXT_VOE_UPLD_ERROR),
                            pMbxTfer->eTferStatus,
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode))
                            );
                    }
                    break;
#endif
#ifdef INCLUDE_AOE_SUPPORT
                case eMbxTferType_AOE_WRITE:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR) )
                    {
                        LogError( ecatGetText(EC_TXT_AOE_CMD_ERROR),
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode)),
                            pMbxTfer->MbxData.AoE_Response.dwErrorCode,
                            pMbxTfer->MbxData.AoE_Response.dwCmdResult
                            );
                    }
                    break;
                case eMbxTferType_AOE_READ:
                    if( (pMbxTfer->eTferStatus != eMbxTferStatus_TferDone) || (pMbxTfer->dwErrorCode != EC_E_NOERROR))
                    {
                        LogError( ecatGetText(EC_TXT_AOE_CMD_ERROR),
                            pMbxTfer->dwErrorCode,
                            ecatGetText(EC_LOWORD(pMbxTfer->dwErrorCode)),
                            pMbxTfer->MbxData.AoE_Response.dwErrorCode,
                            pMbxTfer->MbxData.AoE_Response.dwCmdResult
                            );   
                    }
                    break;
#endif

                default:
                    OsDbgAssert( EC_FALSE );
                    break;
            }
        } break;
#ifdef INCLUDE_COE_PDO_SUPPORT
#error "Notification needs to be done ! Unconditionally !"
    case EC_NOTIFY_COE_TX_PDO:  /* MBOXRCV|1 */
        {
            OsDbgAssert(EC_FALSE);
        } break;
#endif
        /************************/
        /* Scanbus notification */
        /************************/
    case EC_NOTIFY_SB_MISMATCH:
    case EC_NOTIFY_SB_DUPLICATE_HC_NODE:
        {
            EC_T_SB_MISMATCH_DESC*  pSbMismatchDesc     = EC_NULL;
            
            pNotificationDesc   = (EC_T_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSbMismatchDesc     = &(pNotificationDesc->desc.ScanBusMismatch);

            oCurJob.dwCode       = dwCode;
            OsMemcpy(&(oCurJob.JobData), pSbMismatchDesc, sizeof(EC_T_SB_MISMATCH_DESC));
            
            if( !EnqueueJob(&oCurJob) )
            {
                LogError("Unable to enqueue SB Mismatch command Processing Job (missing calls to ProcessNotificationJobs)");
            }
        } break;
    
        /**********************/
        /* error notification */
        /**********************/
    case EC_NOTIFY_CYCCMD_WKC_ERROR:    /* ERR|1 */
        {
            m_adwErrorCounter[dwCode & 0xFFFF]++;
            if( m_adwErrorCounter[dwCode & 0xFFFF] > MAX_MSG_PER_ERROR )
                break;
            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            LogError(ecatGetText(EC_TXT_CYCCMD_WKC_ERROR),
                     EcatCmdShortText(pErrorNotificationDesc->desc.WkcErrDesc.byCmd),
                     pErrorNotificationDesc->desc.WkcErrDesc.dwAddr,
                     pErrorNotificationDesc->desc.WkcErrDesc.wWkcAct,
                     pErrorNotificationDesc->desc.WkcErrDesc.wWkcSet
                    );
        } break;
    case EC_NOTIFY_MASTER_INITCMD_WKC_ERROR:    /* ERR|2 */
        {
            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;

            LogError(
                ecatGetText(EC_TXT_MASTINITCMD_WKC_ERROR),
                EcatCmdShortText(pErrorNotificationDesc->desc.WkcErrDesc.byCmd),
                pErrorNotificationDesc->desc.WkcErrDesc.dwAddr,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcAct,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcSet
                    );
        } break;
    case EC_NOTIFY_SLAVE_INITCMD_WKC_ERROR:     /* ERR|3 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.WkcErrDesc.SlaveProp);

            LogError(ecatGetText(EC_TXT_SLVINITCMD_WKC_ERROR),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                EcatCmdShortText(pErrorNotificationDesc->desc.WkcErrDesc.byCmd),
                pErrorNotificationDesc->desc.WkcErrDesc.dwAddr,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcAct,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcSet
                    );
        } break;
    case EC_NOTIFY_EOE_MBXSND_WKC_ERROR:    /* ERR|7 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.WkcErrDesc.SlaveProp);

            LogError(ecatGetText(EC_TXT_EOEMBXSND_WKC_ERROR),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                EcatCmdShortText(pErrorNotificationDesc->desc.WkcErrDesc.byCmd),
                pErrorNotificationDesc->desc.WkcErrDesc.dwAddr,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcAct,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcSet
                    );
        } break;
    case EC_NOTIFY_COE_MBXSND_WKC_ERROR:    /* ERR|8 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.WkcErrDesc.SlaveProp);

            LogError(ecatGetText(EC_TXT_COEMBXSND_WKC_ERROR),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                EcatCmdShortText(pErrorNotificationDesc->desc.WkcErrDesc.byCmd),
                pErrorNotificationDesc->desc.WkcErrDesc.dwAddr,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcAct,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcSet
                    );
        } break;
    case EC_NOTIFY_FOE_MBXSND_WKC_ERROR:    /* ERR|9 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.WkcErrDesc.SlaveProp);

            LogError(ecatGetText(EC_TXT_FOEMBXSND_WKC_ERROR),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                EcatCmdShortText(pErrorNotificationDesc->desc.WkcErrDesc.byCmd),
                pErrorNotificationDesc->desc.WkcErrDesc.dwAddr,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcAct,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcSet
                    );
        } break;
    case EC_NOTIFY_VOE_MBXSND_WKC_ERROR:    /* ERR|34 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.WkcErrDesc.SlaveProp);

            LogError(ecatGetText(EC_TXT_VOEMBXSND_WKC_ERROR),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                EcatCmdShortText(pErrorNotificationDesc->desc.WkcErrDesc.byCmd),
                pErrorNotificationDesc->desc.WkcErrDesc.dwAddr,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcAct,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcSet
                    );
        } break;
    case EC_NOTIFY_FRAME_RESPONSE_ERROR: /* ERR|10 */
        {
        /*
        This error will be indicated if the actually received Ethernet frame does
        not match to the frame expected or if a expected frame was not received.

          Missing response (timeout, eRspErr_NO_RESPONSE/eRspErr_FRAME_RETRY) acyclic frames:
          Acyclic Ethernet frames are internally queued by the master and sent to the slaves
          at a later time (usually after sending cyclic frames).
          The master will monitor the time between queueing such a frame and receiving the result.
          If a maximum time is exceeded then this error will be indicated.
          This maximum time will be determined by the parameter MasterConfig.dwEcatCmdTimeout when
          the master is initialized.
          The master will retry sending the frame if the master configuration parameter dwEcatCmdMaxRetries
          is set to a value greater than 1.
          In case of a retry the eRspErr_FRAME_RETRY error is signaled, if the number of retries has
          elapsed the eRspErr_NO_RESPONSE error is signaled.
          Possible reasons:
          a)	the frame was not received at all (due to bus problems)
          In this case the achErrorInfo[] member of the error notification descriptor will
          contain the string "L".
          b)	the frame was sent too late by the master due to an improper configuration.
          In this case the achErrorInfo[] member of the error notification descriptor will
          contain the string "T".
          to avoid this error the configuration may be changed as follows:
          - higher value for master configuration parameter dwMaxSentQueuedFramesPerCyc
          - smaller value for the master clock multiplier (init parameter nMasterMultiplier)
          - higher timeout value (master configuration parameter dwEcatCmdTimeout)
          Note: If the frame was sent too late by the master (due to improper configuration values)
          it will also be received too late and the master then signals an eRspErr_WRONG_IDX
          or eRspErr_UNEXPECTED error (as the master then doesn't expect to receive this frame).

            Missing response (timeout, eRspErr_NO_RESPONSE) cyclic frames:
            A response to all cyclic frames must occur until the next cycle starts.
            If the first cyclic frame is sent the master checks whether all cyclic frames of the
            last cycle were received. If there is one frame missing this error is indicated.
            Possible reasons:
            a)	the frame was not received (due to bus problems)
            b)	to many or too long acyclic frames are sent in between sending cyclic frames by the
            master due to a improper configuration,
            to avoid these error notifications the configuration may be changed as follows:
            - lower value for master configuration parameter dwMaxSentQueuedFramesPerCyc
            - lower value for the time between sending cyclic frames (less calls to ecatRefreshProcessData or
            a smaller value for the cyclic clock multiplier (init parameter nCyclicMultiplier)
            c)	non-deterministic sending of acyclic frames. To avoid these error notifications in such cases,
            the init parameter dwSendAcycAfterSyncFramesOnly has to be set to a value of 1.
            */
            m_adwErrorCounter[dwCode & 0xFFFF]++;
            if( m_adwErrorCounter[dwCode & 0xFFFF] > MAX_MSG_PER_ERROR )
                break;
            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            if( pErrorNotificationDesc->desc.FrameRspErrDesc.EErrorType == eRspErr_FRAME_RETRY ||
                pErrorNotificationDesc->desc.FrameRspErrDesc.EErrorType == eRspErr_RETRY_FAIL
              )
            {
                LogError(
                    ecatGetText(EC_TXT_FRMRESP_RETRY),
                    pErrorNotificationDesc->desc.FrameRspErrDesc.bIsCyclicFrame?"cyclic":"acyclic",
                    pErrorNotificationDesc->achErrorInfo[0]=='T'?"timeout":"frame loss"
                        );
            }
            else
            {
                EC_T_CHAR*  pszTextCause = EC_NULL;
                switch( pErrorNotificationDesc->desc.FrameRspErrDesc.EErrorType )
                {
                case eRspErr_NO_RESPONSE:   pszTextCause = (EC_T_CHAR*)"no";          break;
                case eRspErr_WRONG_IDX:     pszTextCause = (EC_T_CHAR*)"wrong";       break;
                case eRspErr_UNEXPECTED:    pszTextCause = (EC_T_CHAR*)"unexpected";  break;
                default:                    pszTextCause = (EC_T_CHAR*)"@@internal error@@";  break;
                }
                LogError(
                    ecatGetText(EC_TXT_FRMRESP_NORETRY),
                    pszTextCause,
                    (pErrorNotificationDesc->desc.FrameRspErrDesc.bIsCyclicFrame?"cyclic":"acyclic")
                        );
            }

            if( pErrorNotificationDesc->desc.FrameRspErrDesc.bIsCyclicFrame )
            {
                if( pErrorNotificationDesc->achErrorInfo[0] != '\0' )
                {
                    LogError( ecatGetText(EC_TXT_ADDERRINFO),
                        pErrorNotificationDesc->achErrorInfo
                            );
                }

                if (pErrorNotificationDesc->desc.FrameRspErrDesc.EErrorType == eRspErr_WRONG_IDX)
                {
                    LogError( ecatGetText(EC_TXT_CMDIDXACTVAL),
                        pErrorNotificationDesc->desc.FrameRspErrDesc.byEcCmdHeaderIdxAct
                            );
                }
            }
            else
            {
                if( (pErrorNotificationDesc->desc.FrameRspErrDesc.EErrorType != eRspErr_UNEXPECTED) )
                {
                    LogError( ecatGetText(EC_TXT_CMDIDXSETVAL),
                        pErrorNotificationDesc->desc.FrameRspErrDesc.byEcCmdHeaderIdxSet
                            );
                }

                if(
                    ((pErrorNotificationDesc->desc.FrameRspErrDesc.EErrorType == eRspErr_UNEXPECTED)
                 || (pErrorNotificationDesc->desc.FrameRspErrDesc.EErrorType == eRspErr_WRONG_IDX))
                  )
                {
                    LogError( ecatGetText(EC_TXT_CMDIDXACTVAL),
                        pErrorNotificationDesc->desc.FrameRspErrDesc.byEcCmdHeaderIdxAct
                            );

                }
            }
        } break;
    case EC_NOTIFY_SLAVE_INITCMD_RESPONSE_ERROR: /* ERR|11 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.InitCmdErrDesc.SlaveProp);

            switch( pErrorNotificationDesc->desc.InitCmdErrDesc.EErrorType )
            {
            case eInitCmdErr_NO_RESPONSE:
                {
                    LogError(ecatGetText(EC_TXT_SLVINITCMDRSPERR_NR),
                        pSlaveProp->achName,
                        pSlaveProp->wStationAddress,
                        pErrorNotificationDesc->desc.InitCmdErrDesc.achStateChangeName
                            );
                } break;
            case eInitCmdErr_VALIDATION_ERR:
                {
                    LogError(ecatGetText(EC_TXT_SLVINITCMDRSPERR_VE),
                        pSlaveProp->achName,
                        pSlaveProp->wStationAddress,
                        pErrorNotificationDesc->desc.InitCmdErrDesc.achStateChangeName
                            );
                } break;
            case eInitCmdErr_FAILED:
                {
                    LogError(ecatGetText(EC_TXT_SLVINITCMDRSPERR_FLD),
                        pSlaveProp->achName,
                        pSlaveProp->wStationAddress,
                        pErrorNotificationDesc->desc.InitCmdErrDesc.achStateChangeName
                            );
                } break;
            default:
                {
                    LogError( "@@internal error@@" );
                } break;
            }
        } break;
    case EC_NOTIFY_MASTER_INITCMD_RESPONSE_ERROR:   /* ERR|12 */
        {
            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;

            switch( pErrorNotificationDesc->desc.InitCmdErrDesc.EErrorType )
            {
            case eInitCmdErr_NO_RESPONSE:
                {
                    LogError(ecatGetText(EC_TXT_MASTINITCMDRSPERR_NR),
                        pErrorNotificationDesc->desc.InitCmdErrDesc.achStateChangeName
                            );
                } break;
            case eInitCmdErr_VALIDATION_ERR:
                {
                    LogError(ecatGetText(EC_TXT_MASTINITCMDRSPERR_VE),
                        pErrorNotificationDesc->desc.InitCmdErrDesc.achStateChangeName
                            );
                } break;
            default:
                {
                    LogError( "@@internal error@@" );
                } break;
            }
        } break;
    case EC_NOTIFY_MBSLAVE_INITCMD_TIMEOUT:     /* ERR|14 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.InitCmdErrDesc.SlaveProp);

            LogError(ecatGetText(EC_TXT_MBSLV_INITCMDTO),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                pErrorNotificationDesc->desc.InitCmdErrDesc.achStateChangeName
                    );
        } break;
    case EC_NOTIFY_NOT_ALL_DEVICES_OPERATIONAL:     /* ERR|15 */
        {
            m_bAllDevsOperational = EC_FALSE;
            m_adwErrorCounter[dwCode & 0xFFFF]++;
            if( m_adwErrorCounter[dwCode & 0xFFFF] > MAX_MSG_PER_ERROR )
                break;
            LogError(ecatGetText(EC_TXT_NOT_ALL_DEVS_OP));
        } break;
    case EC_NOTIFY_ETH_LINK_NOT_CONNECTED: /* ERR|16 */
        {
            m_adwErrorCounter[dwCode & 0xFFFF]++;
            if( m_adwErrorCounter[dwCode & 0xFFFF] > MAX_MSG_PER_ERROR )
                break;
            LogError(ecatGetText(EC_TXT_CABLE_NOT_CONNECTED));
        } break;
    case EC_NOTIFY_RED_LINEBRK:         /* ERR|18 */
        {
            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            LogError( ecatGetText(EC_TXT_REDLINEBREAK),
                pErrorNotificationDesc->desc.RedChangeDesc.wNumOfSlavesMain,
                pErrorNotificationDesc->desc.RedChangeDesc.wNumOfSlavesMain+1);
        } break;
    case EC_NOTIFY_STATUS_SLAVE_ERROR:  /* ERR|19 */
        {
            m_adwErrorCounter[dwCode & 0xFFFF]++;
            if( m_adwErrorCounter[dwCode & 0xFFFF] > MAX_MSG_PER_ERROR )
                break;
            LogError( ecatGetText(EC_TXT_SLVERR_DETECTED) );
        } break;
    case EC_NOTIFY_SLAVE_ERROR_STATUS_INFO:     /* ERR|20 */
        {
            EC_T_SLAVE_PROP*            pSlaveProp  = EC_NULL;
            EC_T_SLAVE_ERROR_INFO_DESC* pSlaveError = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveError = &(pErrorNotificationDesc->desc.SlaveErrInfoDesc);
            pSlaveProp = &(pSlaveError->SlaveProp);

            LogError(ecatGetText(EC_TXT_SLVERR_INFO),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                SlaveDevStateText(((pErrorNotificationDesc->desc.SlaveErrInfoDesc.wStatus)&0xf)),
                (((pErrorNotificationDesc->desc.SlaveErrInfoDesc.wStatus)&0x10)?" ERROR":""),
                pErrorNotificationDesc->desc.SlaveErrInfoDesc.wStatus,
                SlaveDevStatusCodeText((pErrorNotificationDesc->desc.SlaveErrInfoDesc.wStatusCode)),
                pErrorNotificationDesc->desc.SlaveErrInfoDesc.wStatusCode
                    );
        } break;
    case EC_NOTIFY_SLAVE_NOT_ADDRESSABLE:       /* ERR|21 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.WkcErrDesc.SlaveProp);

            LogError( ecatGetText(EC_TXT_SLV_NOT_ADDRABLE),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress);
        } break;
#ifdef INCLUDE_SOE_SUPPORT
    case EC_NOTIFY_SOE_MBXSND_WKC_ERROR:    /* ERR|23 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.WkcErrDesc.SlaveProp);

            LogError(ecatGetText(EC_TXT_SOEMBXSND_WKC_ERROR),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                EcatCmdShortText(pErrorNotificationDesc->desc.WkcErrDesc.byCmd),
                pErrorNotificationDesc->desc.WkcErrDesc.dwAddr,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcAct,
                pErrorNotificationDesc->desc.WkcErrDesc.wWkcSet
                    );
        } break;
    case EC_NOTIFY_SOE_WRITE_ERROR:         /* ERR|24 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;
            
            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.InitCmdErrDesc.SlaveProp);
            
            LogError(ecatGetText(EC_TXT_SOEMBX_WRITE_ERROR),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                pErrorNotificationDesc->desc.InitCmdErrDesc.achStateChangeName
                );
        } break;
#endif
    case EC_NOTIFY_MBSLAVE_COE_SDO_ABORT:   /* ERR|25 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.SdoAbortDesc.SlaveProp);

            LogError(ecatGetText(EC_TXT_MBSLV_SDO_ABORT),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                EcErrorText(pErrorNotificationDesc->desc.SdoAbortDesc.dwErrorCode),
                pErrorNotificationDesc->desc.SdoAbortDesc.dwErrorCode,
                pErrorNotificationDesc->desc.SdoAbortDesc.wObjIndex,
                pErrorNotificationDesc->desc.SdoAbortDesc.bySubIndex
                    );
        } break;
    case EC_NOTIFY_CLIENTREGISTRATION_DROPPED:  /* ERR|26 */
        {
            EC_T_DWORD dwDeinitForConfiguration = EC_FALSE;

            if (pParms->dwInBufSize >= sizeof(EC_T_DWORD))
            {
                dwDeinitForConfiguration = *((EC_T_DWORD*)pParms->pbyInBuf);
            }
            if (dwDeinitForConfiguration == 1)
            {
                if (bVerbose)
                {
                    LogMsg("Re-configuring EC-Master");
                }
                m_dwClientID = 0;
            }
            else 
            {
                if (bVerbose)
                {
                    LogMsg(ecatGetText(EC_TXT_CLNTREGDROP));
                }
            }
        } break;


    case EC_NOTIFY_RED_LINEFIXED:  /* ERR|27 */
        {
            LogError( ecatGetText(EC_TXT_REDLINEFIXED) );
        } break;
#ifdef INCLUDE_FOE_SUPPORT
    case EC_NOTIFY_FOE_MBSLAVE_ERROR:   /* ERR|28 */
        {
            EC_T_SLAVE_PROP*    pSlaveProp  = EC_NULL;
            
            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.FoeErrorDesc.SlaveProp);
            
            LogError(ecatGetText(EC_TXT_MBSLV_FOE_ABORT),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                pErrorNotificationDesc->desc.FoeErrorDesc.achErrorString,
                pErrorNotificationDesc->desc.FoeErrorDesc.dwErrorCode
                );

        } break;
#endif
    case EC_NOTIFY_MBXRCV_INVALID_DATA: /* ERR|29 */
        {
            EC_T_SLAVE_PROP* pSlaveProp = EC_NULL;
            
            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.MbxRcvInvalidDataDesc.SlaveProp);
            
            LogError(ecatGetText(EC_TXT_MBXRCV_INVALID_DATA),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress
                );
        } break;
    case EC_NOTIFY_PDIWATCHDOG:         /* ERR|30 */
        {
            EC_T_SLAVE_PROP* pSlaveProp = EC_NULL;
            
            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.PdiWatchdogDesc.SlaveProp);
            
            LogError(ecatGetText(EC_TXT_PDIWATCHDOG),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress
                );
        } break;
    case EC_NOTIFY_SLAVE_NOTSUPPORTED:      /* ERR|31 */
        {
            EC_T_SLAVE_PROP* pSlaveProp = EC_NULL;
            
            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.SlaveNotSupportedDesc.SlaveProp);
            
            LogError(ecatGetText(EC_TXT_SLAVE_NOTSUPPORTED),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress
                );
        } break;
    case EC_NOTIFY_SLAVE_UNEXPECTED_STATE:      /* ERR|32 */
        {
            EC_T_SLAVE_PROP* pSlaveProp = EC_NULL;
            EC_T_STATE       curState   = eEcatState_UNKNOWN;
            EC_T_STATE       expState   = eEcatState_UNKNOWN;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.SlaveUnexpectedStateDesc.SlaveProp);
            curState   = pErrorNotificationDesc->desc.SlaveUnexpectedStateDesc.curState;
            expState   = pErrorNotificationDesc->desc.SlaveUnexpectedStateDesc.expState;
            
            LogError(ecatGetText(EC_TXT_SLAVE_UNEXPECTED_STATE),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress,
                EcStateString(curState),
                EcStateString(expState)
                );
        } break;
    case EC_NOTIFY_ALL_DEVICES_OPERATIONAL:     /* ERR|33 */
        {
            m_bAllDevsOperational = EC_TRUE;
            LogError(ecatGetText(EC_TXT_ALL_DEVS_OP));
        } break;
    case EC_NOTIFY_EEPROM_CHECKSUM_ERROR:       /* ERR|36 */
        {
            EC_T_SLAVE_PROP* pSlaveProp = EC_NULL;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.EEPROMChecksumErrorDesc.SlaveProp);

            LogError(ecatGetText(EC_TXT_EEPROM_CHECKSUM_ERROR),
                pSlaveProp->achName,
                pSlaveProp->wStationAddress
                );
        } break;
    case EC_NOTIFY_JUNCTION_RED_CHANGE:         /* ERR|37 */
        {
            EC_T_SLAVE_PROP* pSlaveProp = EC_NULL;
            EC_T_BOOL        bLineBreak = EC_FALSE;

            pErrorNotificationDesc = (EC_T_ERROR_NOTIFICATION_DESC*)pParms->pbyInBuf;
            pSlaveProp = &(pErrorNotificationDesc->desc.JunctionRedChangeDesc.SlaveProp);
            bLineBreak =   pErrorNotificationDesc->desc.JunctionRedChangeDesc.bLineBreak;

            LogError(ecatGetText(EC_TXT_JUNCTION_RED_CHANGE),
                bLineBreak,
                pSlaveProp->achName,
                pSlaveProp->wStationAddress
                );
        } break;
#ifdef INCLUDE_HOTCONNECT
    /*case EC_NOTIFY_HC_DETECTALLGROUPS:      / * HC| 1: HC Detect All Groups done * / */
    case EC_NOTIFY_HC_DETECTADDGROUPS:      /* HC| 2: HC Enhance Detect All Groups done */
    case EC_NOTIFY_HC_PROBEALLGROUPS:       /* HC| 3: HC Probe All Groups done */
        {
            /* HC Group Detection did finish it's processing when this Notification is called */
            pNotificationDesc = (EC_T_NOTIFICATION_DESC*)pParms->pbyInBuf;
            
            dwRes = pNotificationDesc->desc.HCDetAllGrps.dwResultCode;
            
            if (bVerbose && (EC_E_NOERROR == dwRes))
            {
                LogMsg( ecatGetText(EC_TXT_HC_DETAGRESULT_OK),
                    pNotificationDesc->desc.HCDetAllGrps.dwGroupCount,
                    pNotificationDesc->desc.HCDetAllGrps.dwGroupsPresent,
                    pNotificationDesc->desc.HCDetAllGrps.dwGroupMask
                    );
            }
            
            if (EC_E_NOERROR != dwRes)
            {
                LogError( ecatGetText(EC_TXT_HC_DETAGRESULT_ERROR),
                    EcErrorText(pNotificationDesc->desc.HCDetAllGrps.dwResultCode),
                    pNotificationDesc->desc.HCDetAllGrps.dwResultCode,
                    pNotificationDesc->desc.HCDetAllGrps.dwGroupCount
                    );
            }
            
        } break;
#endif /* INCLUDE_HOTCONNECT */
    case EC_NOTIFY_HC_TOPOCHGDONE:          /* HC| 4:  HC Topology Change done */
        {
            pNotificationDesc = (EC_T_NOTIFICATION_DESC*)pParms->pbyInBuf;
            if(bVerbose)
            {
                LogMsg( ecatGetText(EC_TXT_HC_TOPOCHGDONE), EcErrorText(pNotificationDesc->desc.StatusCode), pNotificationDesc->desc.StatusCode );
            }
        } break;
    case EC_NOTIFY_SLAVE_DISAPPEARS:        /* HC| 5: ScanBus Slave Disappears*/
        {
            pNotificationDesc = (EC_T_NOTIFICATION_DESC*)pParms->pbyInBuf;
            LogMsg( 
                ecatGetText(EC_TXT_SLAVE_DISAPPEARS),  
                pNotificationDesc->desc.HCSlvChgDesc.SlaveProp.achName, 
                pNotificationDesc->desc.HCSlvChgDesc.SlaveProp.wStationAddress
                  );
        } break;
    case EC_NOTIFY_SLAVE_APPEARS:           /* HC| 6: ScanBus Slave Appears */
        {
            pNotificationDesc = (EC_T_NOTIFICATION_DESC*)pParms->pbyInBuf;
            LogMsg( 
                ecatGetText(EC_TXT_SLAVE_APPEARS),  
                pNotificationDesc->desc.HCSlvChgDesc.SlaveProp.achName, 
                pNotificationDesc->desc.HCSlvChgDesc.SlaveProp.wStationAddress
                );
        } break;
    default:
        {
            /* print notification name and code */
            OsDbgAssert(EC_FALSE);
            LogError("ecatNotify: name = %s code = 0x%x", ecatGetNotifyText(dwCode), dwCode);
        } break ;
    }

    /* clear error counter every 5 seconds */
    if( OsQueryMsecCount() > s_dwClearErrorMsecCount )
    {
        /* reset error counters */
        OsMemset( m_adwErrorCounter, 0, sizeof(m_adwErrorCounter) );
        s_dwClearErrorMsecCount = OsQueryMsecCount() + 5000;
    }

    return dwRetVal;
}


/***************************************************************************************************/
/**
\brief  Process Notification Jobs.

This function processes the results enqueued by EcatNotify in an asynchronuous Matter
\return EC_FALSE on error, EC_TRUE otherwise.
*/
EC_T_BOOL CEmNotification::ProcessNotificationJobs( EC_T_VOID )
{
    T_SLAVEJOBS     TmpJob;
    EC_T_BOOL       bProcessed  = EC_FALSE;

    /* is there something to do ? */
    while( m_oSlaveJobQueue.pRead != m_oSlaveJobQueue.pWrite)
    {
        if( !DequeueJob(&TmpJob) )
        {
            goto Exit;
        }

        /* check if hook is available */
        if(( m_pfProcessNotificationHookInstance != EC_NULL ) && (m_pfProcessNotificationHookFunction != EC_NULL))
        {
            bProcessed = (*m_pfProcessNotificationHookFunction)(m_pfProcessNotificationHookInstance, &TmpJob);
            /* is this notification handled by hook function ? */
            if( bProcessed )
            {
                continue;       /* no further actions necessary */
            }
        }

        switch( TmpJob.dwCode )
        {
        case EC_NOTIFY_SB_STATUS:
            {
                EC_T_SB_STATUS_NTFY_DESC* pScanBusJob = EC_NULL;

                pScanBusJob = &(TmpJob.JobData.ScanBusJob);

                if( EC_E_NOERROR != pScanBusJob->dwResultCode )
                {
                    LogError(
                        "\nScan Bus returned with error: %s (0x%x)",
                        EcErrorText(pScanBusJob->dwResultCode),
                        pScanBusJob->dwResultCode
                            );
                }
            } break;

        case EC_NOTIFY_SB_MISMATCH:
        case EC_NOTIFY_SB_DUPLICATE_HC_NODE:
            {
                EC_T_SB_MISMATCH_DESC*  pScanBusMismatchJob = EC_NULL;
                EC_T_DWORD              dwRes               = 0;
                
                pScanBusMismatchJob = &(TmpJob.JobData.SBSlaveMismatchJob);

                /* Check if we have a mismatch for the first slave on the bus */
                if(pScanBusMismatchJob->wPrevAIncAddress != INVALID_AUTO_INC_ADDR)
                {
                    LogErrorAdd("\nSlave mismatch after %d, Auto-inc Address 0x%04x, port %d:\n",   
                        pScanBusMismatchJob->wPrevFixedAddress, 
                        pScanBusMismatchJob->wPrevAIncAddress, 
                        pScanBusMismatchJob->wPrevPort);
                }
                else
                {
                    LogErrorAdd("\nScan bus mismatch for the first slave on the bus! BUS VendorId: %s (0x%x); ProductCode: %s (0x%x).\n",   
                        SlaveVendorText(((_T_eEtherCAT_Vendor)pScanBusMismatchJob->dwBusVendorId)), 
                        pScanBusMismatchJob->dwBusVendorId,
                        SlaveProdCodeText((T_eEtherCAT_Vendor)pScanBusMismatchJob->dwBusVendorId,(T_eEtherCAT_ProductCode)pScanBusMismatchJob->dwBusProdCode), 
                        pScanBusMismatchJob->dwBusProdCode
                        );                     
                }

                /* handle hot connect group identification error */
                if(!pScanBusMismatchJob->bIdentValidationError )
                {
                    /* Expected vendor id and/or product code was unequal to the vendor id and/or product code on the bus. */
                    LogErrorAdd(
                        "CFG/BUS VendorId: %s (0x%x) / %s (0x%x); ProductCode: %s (0x%x) / %s (0x%x)\n",
                        SlaveVendorText(((_T_eEtherCAT_Vendor)pScanBusMismatchJob->dwCfgVendorId)), 
                        pScanBusMismatchJob->dwCfgVendorId, 
                        SlaveVendorText(((_T_eEtherCAT_Vendor)pScanBusMismatchJob->dwBusVendorId)), 
                        pScanBusMismatchJob->dwBusVendorId,
                        SlaveProdCodeText((T_eEtherCAT_Vendor)pScanBusMismatchJob->dwCfgVendorId, (T_eEtherCAT_ProductCode)pScanBusMismatchJob->dwCfgProdCode), 
                        pScanBusMismatchJob->dwCfgProdCode, 
                        SlaveProdCodeText((T_eEtherCAT_Vendor)pScanBusMismatchJob->dwBusVendorId,(T_eEtherCAT_ProductCode)pScanBusMismatchJob->dwBusProdCode), 
                        pScanBusMismatchJob->dwBusProdCode
                        );
                }
                else
                {
                    if(EC_NOTIFY_SB_DUPLICATE_HC_NODE == TmpJob.dwCode)
                    {     
                        LogErrorAdd(
                            "Identification value of slave %s (Auto inc. address: 0x%x) is duplicated: %d\n",
                            SlaveProdCodeText((T_eEtherCAT_Vendor)pScanBusMismatchJob->dwBusVendorId,(T_eEtherCAT_ProductCode)pScanBusMismatchJob->dwBusProdCode),
                            pScanBusMismatchJob->wBusAIncAddress,
                            pScanBusMismatchJob->dwCmdData);
                    }
                    else
                    {
                        LogErrorAdd(
                            "Identification value of slave %s (Auto inc. address: 0x%x) is incorrect. Actual: %d -> expected: %d\n",
                            SlaveProdCodeText((T_eEtherCAT_Vendor)pScanBusMismatchJob->dwBusVendorId,(T_eEtherCAT_ProductCode)pScanBusMismatchJob->dwBusProdCode),
                            pScanBusMismatchJob->wBusAIncAddress,
                            pScanBusMismatchJob->dwCmdData,
                            pScanBusMismatchJob->dwCmdVData);
                    }
                }
                /* can't block off first node on bus */
                if( INVALID_AUTO_INC_ADDR != pScanBusMismatchJob->wPrevAIncAddress )
                {
                    /* is HotConnect active / configured? */
                    if( 0 < emHCGetNumGroupMembers(m_dwMasterInstance, 1) )
                    {
                        /* now block off invalid node */
                        dwRes = emBlockNode(m_dwMasterInstance, pScanBusMismatchJob, SB_PORT_BLOCK_TIMEOUT);
                        if (dwRes != EC_E_NOERROR)
                        {
                            LogError("Cannot block invalid slave node! (Result = %s (0x%x))",EcErrorText(dwRes), dwRes);
                        }
                        else
                        {
                            LogMsg("Blocked node : %d (0x%x) port %d", pScanBusMismatchJob->wPrevFixedAddress, pScanBusMismatchJob->wPrevFixedAddress, pScanBusMismatchJob->wPrevPort);
                            m_bDidIssueBlock = EC_TRUE;
                        }
                    }
                }
            } break;
        default:
            continue;            /* next job queued */
        } /* switch job type */
    } /* while job queued */
    m_pcLogging->ProcessAllMsgs();

    bProcessed = EC_TRUE;
Exit:
    return bProcessed;
}


/***************************************************************************************************/
/**
\brief  AtEmRas Layer Notification function callback.

\return EC_E_NOERROR on success, error code otherwise.
*/
EC_T_DWORD CEmNotification::emRasNotify(
    EC_T_DWORD          dwCode,     /**< [in]   Notification code identifier */
    EC_T_NOTIFYPARMS*   pParms      /**< [in]   Notification data portion */
                                       )
{
    EC_T_DWORD  dwRetVal    = EC_E_ERROR;

    if( (EC_NULL == pParms) )
    {
        dwRetVal = EC_E_INVALIDPARM;
        goto Exit;
    }

    switch(dwCode)
    {
    case ATEMRAS_NOTIFY_CONNECTION:         /* GENERIC RAS | 1 */
        {
            ATEMRAS_PT_CONNOTIFYDESC    pConNot = EC_NULL;
            if( sizeof(ATEMRAS_T_CONNOTIFYDESC) > pParms->dwInBufSize )
            {
                LogError("RAS Invalid Parameter size for ATEMRAS_NOTIFY_CONNECTION");
                dwRetVal = EC_E_INVALIDPARM;
                goto Exit;
            }

            pConNot = (ATEMRAS_PT_CONNOTIFYDESC)pParms->pbyInBuf;

            if(pConNot->dwCause == EC_E_NOERROR)
            {
                LogMsg("RAS Connection changed: Established!\n");
                m_bRasServerDisconnect = EC_FALSE;
            }
            else
            {
                LogMsg("RAS Connection changed: Cause: %s (0x%lx)\n",
                    ((EMRAS_E_ERROR == (pConNot->dwCause&EMRAS_E_ERROR))?ecatGetText(EC_LOWORD(pConNot->dwCause)):ecatGetText(EC_LOWORD(pConNot->dwCause))),
                    pConNot->dwCause
                      );
                m_bRasServerDisconnect = EC_TRUE;
            }
        } break;
    case ATEMRAS_NOTIFY_REGISTER:           /* GENERIC RAS | 2 */
        {
            /* ATEMRAS_PT_REGNOTIFYDESC pReg = EC_NULL; */

            if( sizeof(ATEMRAS_T_REGNOTIFYDESC) > pParms->dwInBufSize )
            {
                LogError("RAS Invalid Parameter size for ATEMRAS_NOTIFY_REGISTER\n");
                dwRetVal = EC_E_INVALIDPARM;
                goto Exit;
            }

/*
            pReg = (ATEMRAS_PT_REGNOTIFYDESC) pParms->pbyInBuf;
            LogMsg("Client registered from cookie 0x%lx Instance 0x%lx Id 0x%lx Result %s (0x%lx)",
                pReg->dwCookie, pReg->dwInstanceId, pReg->dwClientId,
                ((EMRAS_E_ERROR == (pReg->dwResult&EMRAS_E_ERROR))?ecatGetText(EC_LOWORD(pReg->dwResult)):ecatGetText(EC_LOWORD(pReg->dwResult))),
                pReg->dwResult
                );
*/
        } break;
    case ATEMRAS_NOTIFY_UNREGISTER:         /* GENERIC RAS | 3 */
        {
            /* ATEMRAS_PT_REGNOTIFYDESC pReg = EC_NULL; */

            if( sizeof(ATEMRAS_T_REGNOTIFYDESC) > pParms->dwInBufSize )
            {
                LogError("RAS Invalid Parameter size for ATEMRAS_NOTIFY_UNREGISTER");
                dwRetVal = EC_E_INVALIDPARM;
                goto Exit;
            }

/*
            pReg = (ATEMRAS_PT_REGNOTIFYDESC) pParms->pbyInBuf;
            LogMsg("Client un-registered from cookie 0x%lx Instance 0x%lx Id 0x%lx Result %s (0x%lx)",
                pReg->dwCookie, pReg->dwInstanceId, pReg->dwClientId,
                ((EMRAS_E_ERROR == (pReg->dwResult&EMRAS_E_ERROR))?ecatGetText(EC_LOWORD(pReg->dwResult)):ecatGetText(EC_LOWORD(pReg->dwResult))),
                pReg->dwResult
                );
*/
        } break;
    case ATEMRAS_NOTIFY_MARSHALERROR:       /* ERROR RAS | 1 */
        {
            ATEMRAS_PT_MARSHALERRORDESC     pMarshNot = EC_NULL;
            if( sizeof(ATEMRAS_T_MARSHALERRORDESC) > pParms->dwInBufSize )
            {
                LogError("Invalid Parameter size for ATEMRAS_NOTIFY_MARSHALERROR");
                dwRetVal = EC_E_INVALIDPARM;
                goto Exit;
            }

            pMarshNot = (ATEMRAS_PT_MARSHALERRORDESC)pParms->pbyInBuf;
            LogError("Marshaling error ! Cookie : 0x%lx", pMarshNot->dwCookie);
            LogError("Command : 0x%lx, Cause : %s (0x%lx)",
                pMarshNot->dwCommandCode,
                ((EMRAS_E_ERROR == (pMarshNot->dwCause&EMRAS_E_ERROR))?ecatGetText(EC_LOWORD(pMarshNot->dwCause)):ecatGetText(EC_LOWORD(pMarshNot->dwCause))),
                pMarshNot->dwCause
                    );
            LogError("Protocol Header : 0x%lx", pMarshNot->dwLenStatCmd);
        } break;
    case ATEMRAS_NOTIFY_ACKERROR:           /* ERROR RAS | 2 */
        {
            LogError("Acknowledge error! Error : %s (0x%lx)", ecatGetText(EC_LOWORD(EC_GETDWORD(pParms->pbyInBuf))), EC_GETDWORD(pParms->pbyInBuf));
        } break;
    case ATEMRAS_NOTIFY_NONOTIFYMEMORY:     /* ERROR RAS | 3 */
        {
            ATEMRAS_T_NONOTIFYMEMORYDESC NoNotifyMemoryDesc;
            ATEMRAS_PT_NONOTIFYMEMORYDESC pNoNotifyMemoryDescSerialized = (ATEMRAS_PT_NONOTIFYMEMORYDESC)pParms->pbyInBuf;
            OsDbgAssert(pParms->dwInBufSize >= sizeof(ATEMRAS_T_NONOTIFYMEMORYDESC));
            NoNotifyMemoryDesc.dwCookie = EC_GET_FRM_DWORD(&pNoNotifyMemoryDescSerialized->dwCookie);
            NoNotifyMemoryDesc.dwCode = EC_GET_FRM_DWORD(&pNoNotifyMemoryDescSerialized->dwCode);

            LogError("Out of notification memory! %s (0x%08X), cookie 0x%lx.\n",
                ecatGetNotifyText(NoNotifyMemoryDesc.dwCode), NoNotifyMemoryDesc.dwCode, NoNotifyMemoryDesc.dwCookie);
        } break;
    case ATEMRAS_NOTIFY_STDNOTIFYMEMORYSMALL:   /* ERROR RAS | 4 */
        {
            ATEMRAS_T_NONOTIFYMEMORYDESC NoNotifyMemoryDesc;
            ATEMRAS_PT_NONOTIFYMEMORYDESC pNoNotifyMemoryDescSerialized = (ATEMRAS_PT_NONOTIFYMEMORYDESC)pParms->pbyInBuf;
            OsDbgAssert(pParms->dwInBufSize >= sizeof(ATEMRAS_T_NONOTIFYMEMORYDESC));
            NoNotifyMemoryDesc.dwCookie = EC_GET_FRM_DWORD(&pNoNotifyMemoryDescSerialized->dwCookie);
            NoNotifyMemoryDesc.dwCode = EC_GET_FRM_DWORD(&pNoNotifyMemoryDescSerialized->dwCode);

            LogError("Out of non-mailbox notification memory! %s (0x%08X), cookie 0x%lx.\n",
                ecatGetNotifyText(NoNotifyMemoryDesc.dwCode), NoNotifyMemoryDesc.dwCode, NoNotifyMemoryDesc.dwCookie);
        } break;
    case ATEMRAS_NOTIFY_MBXNOTIFYMEMORYSMALL:   /* ERROR RAS | 5 */
        {
            ATEMRAS_T_NONOTIFYMEMORYDESC NoNotifyMemoryDesc;
            ATEMRAS_PT_NONOTIFYMEMORYDESC pNoNotifyMemoryDescSerialized = (ATEMRAS_PT_NONOTIFYMEMORYDESC)pParms->pbyInBuf;
            OsDbgAssert(pParms->dwInBufSize >= sizeof(ATEMRAS_T_NONOTIFYMEMORYDESC));
            NoNotifyMemoryDesc.dwCookie = EC_GET_FRM_DWORD(&pNoNotifyMemoryDescSerialized->dwCookie);
            NoNotifyMemoryDesc.dwCode = EC_GET_FRM_DWORD(&pNoNotifyMemoryDescSerialized->dwCode);

            LogError("Out of mailbox notification memory! %s (0x%08X), cookie 0x%lx.\n",
                ecatGetNotifyText(NoNotifyMemoryDesc.dwCode), NoNotifyMemoryDesc.dwCode, NoNotifyMemoryDesc.dwCookie);
        } break;
    default:
        {
            LogError("emRasNotify: name = %s code = 0x%x", ecatGetNotifyText(dwCode), dwCode);
        } break;
    }

    dwRetVal = EC_E_NOERROR;
Exit:
    return dwRetVal;
}


/***************************************************************************************************/
/**
\brief  SetProcessNotificationHook

Set process notification hook.
\return N/A.
*/
EC_T_VOID CEmNotification::SetProcessNotificationHook
    (EC_T_PVOID                   pInstance
    ,PF_PROCESS_NOTIFICATION_HOOK pfProcessNotificationHook)
{
    m_pfProcessNotificationHookInstance = pInstance;
    m_pfProcessNotificationHookFunction = pfProcessNotificationHook;
}


/********************************************************************************/
/** \brief  C Wrapper function to ecatNotify
*
*
* \return  Status value.
*/
EC_T_DWORD CEmNotification::NotifyWrapper(
    EC_T_DWORD         dwCode,  /**< [in]   Notifcation code */
    EC_T_NOTIFYPARMS*  pParms   /**< [in]   Notification parameters */
                                         )
{
    EC_T_DWORD                      dwRetVal                = EC_E_NOERROR;
    CEmNotification*                pNotifyInstance;

    if( (EC_NULL == pParms)||(EC_NULL==pParms->pCallerData) )
    {
        dwRetVal = EC_E_INVALIDPARM;
        goto Exit;
    }

    pNotifyInstance = (CEmNotification*)pParms->pCallerData;

    dwRetVal = pNotifyInstance->ecatNotify(dwCode, pParms);
Exit:

    return dwRetVal;
}


/***************************************************************************************************/
/**
\brief  Reset Error counters.
*/
EC_T_VOID CEmNotification::ResetErrorCounters( EC_T_VOID )
{
    OsMemset( m_adwErrorCounter, 0, sizeof(m_adwErrorCounter) );
}


/***************************************************************************************************/
/**
\brief  Increment Pointer in Queue.

\return New Pointer.
*/
PT_SLAVEJOBS CEmNotification::IncrementJobPtr(
    PT_SLAVEJOBS       pPtr
                            )
{
    PT_SLAVEJOBS pRet = EC_NULL;

    if (EC_NULL == pPtr)
    {
        goto Exit;
    }

    if( (EC_T_PVOID)(pPtr+1)>(EC_T_PVOID)(&m_oSlaveJobQueue.Jobs[JOB_QUEUE_LENGTH-1]) )
    {
        /* wrap */
        pRet = &(m_oSlaveJobQueue.Jobs[0]);
    }
    else
    {
        pRet = (pPtr+1);
    }
Exit:
    return pRet;
}


/***************************************************************************************************/
/**
\brief  EnqueueJob.

Enqueue new Job to queue.
\return EC_TRUE on success, EC_FALSE otherwise.
*/

EC_T_BOOL CEmNotification::EnqueueJob(
    PT_SLAVEJOBS pJob              /**< [in]   Job to enqueue */
                    )
{
    EC_T_BOOL       bRet = EC_FALSE;
    PT_SLAVEJOBS    pNewWrite = EC_NULL;

    if (EC_NULL == pJob)
    {
        goto Exit;
    }

    pNewWrite = IncrementJobPtr(m_oSlaveJobQueue.pWrite);

    if( pNewWrite == m_oSlaveJobQueue.pRead )
    {
        /* no more space in queue */
        goto Exit;
    }

    /* store job */
    OsMemcpy(m_oSlaveJobQueue.pWrite, pJob, sizeof(T_SLAVEJOBS));

    /* increment write pointer */
    m_oSlaveJobQueue.pWrite = pNewWrite;

    bRet = EC_TRUE;
Exit:

    return bRet;
}


/***************************************************************************************************/
/**
\brief  DequeueJob.

Dequeue next job if possible.
\return EC_TRUE on success, EC_FALSE otherwise.
*/

EC_T_BOOL CEmNotification::DequeueJob(
    PT_SLAVEJOBS       pJob        /**< [out]  Dequeued Job Object*/
                                     )
{
    EC_T_BOOL bRet    = EC_FALSE;

    if( (EC_NULL == pJob) )
    {
        goto Exit;
    }

    OsMemcpy(pJob, m_oSlaveJobQueue.pRead, sizeof(T_SLAVEJOBS));

    /* Increment Read Pointer */
    m_oSlaveJobQueue.pRead = IncrementJobPtr(m_oSlaveJobQueue.pRead);

    bRet = EC_TRUE;
Exit:

    return bRet;
}

/*-END OF SOURCE FILE--------------------------------------------------------*/
