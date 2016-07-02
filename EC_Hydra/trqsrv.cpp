/*-----------------------------------------------------------------------------
 * trqsrv.cpp            
 * Copyright                toyota
 * Response                 okamoto
 * Description              --
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include <AtEthercat.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

#include "DCDemo.h"
#include "Logging.h"

#include "trqsrv.h"
#include "hydra.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;
/*-DEFINES-------------------------------------------------------------------*/
#define LogMsg      	poLog->LogMsg
#define LogError    	poLog->LogError
#define LogDcm      	poLog->LogDcm

/*-TYPEDEFS------------------------------------------------------------------*/

/*-GLOBAL VARIABLES-----------------------------------------------------------*/
extern EC_T_DWORD          	dwClntId;		// from DCDemo.cpp

EC_T_BOOL	flg_NewFile		= EC_FALSE;
EC_T_BOOL	flg_NewLabel	= EC_TRUE;
EC_T_BOOL	flg_FileUpdate	= EC_FALSE;
/*-FUNCTION ------------------------------------------------------*/

/*-FUNCTION DEFINITIONS------------------------------------------------------*/

//採用関数
static EC_T_DWORD myAppConsole(
    T_ALL_SLAVE_INFO*	pAllSlv,
    CAtEmLogging*       poLog,
    EC_T_DWORD          dwClntId,           /* [in]  EtherCAT master client id */
    EC_T_INT            nVerbose);          /* [in]  verbosity level */
static EC_T_DWORD myAppCS_SdoCmd(
    T_ALL_SLAVE_INFO*		pAllSlv,
    CAtEmLogging*       	poLog);           	/* [in]   Application instance */ 
static EC_T_DWORD myAppCS_ESCreg(
    T_ALL_SLAVE_INFO*		pAllSlv,
    CAtEmLogging*       	poLog,
    EC_T_DWORD              dwClntId,           /* [in]  EtherCAT master client id */
    EC_T_INT                nVerbose);          /* [in]  verbosity level */
static EC_T_DWORD myAppCS_StsChg(
    CAtEmLogging*       	poLog);          /* [in]   Application instance */ 
static EC_T_DWORD myAppCS_BatchSDO(
    CAtEmLogging*       	poLog);          /* [in]   Application instance */ 
static EC_T_DWORD myAppCS_LogSaveCtrl(
    T_ALL_SLAVE_INFO*		pAllSlv,
    CAtEmLogging*       	poLog);           /* [in]   Application instance */ 
static EC_T_DWORD myAppCS_DumpEHAPosition( 
	T_ALL_SLAVE_INFO* pAllSlv, 
	CAtEmLogging* poLog);


static EC_T_DWORD TargetPrint( T_ALL_SLAVE_INFO* pAllSlv, EC_T_INT* pTrgt_grp, EC_T_INT* pTrgt_slave, EC_T_DWORD* pTrgt_dwSlaveId);

/********************************************************************************/
/** \brief  Console Task
*
* \return N/A
*/
EC_T_VOID tConsoleTask( EC_T_VOID* pvThreadParamDesc )
{
    T_THREAD_PARAM* pThreadParam  	= (T_THREAD_PARAM*)pvThreadParamDesc;
  
    EC_T_DWORD          dwRes     = EC_E_ERROR;
    EC_T_CPUSET         CpuSet;
    T_ALL_SLAVE_INFO*   pAllSlv	= EC_NULL;
    EC_CPUSET_ZERO( CpuSet );
    EC_CPUSET_SET( CpuSet, pThreadParam->dwCpuIndex );
    OsSetThreadAffinity( EC_NULL, CpuSet );
    
    pAllSlv  = (T_ALL_SLAVE_INFO*)pThreadParam->pvApplSpecParm1;

    pThreadParam->bThreadRunning = EC_TRUE;

    // Masterの状態がOPERATIONALになるまで待つ
    OsWaitForEvent(pThreadParam->pvTimingEvent, EC_WAITINFINITE);

    /* demo loop */
    while( !pThreadParam->bThreadShutdown ) {
        dwRes = myAppConsole( pAllSlv,
                              pThreadParam->pLogInst, 
                              pThreadParam->pNotInst->GetClientID(), 
                              pThreadParam->pNotInst->Verbose() );
        if (dwRes != EC_E_NOERROR) {
            /* 動作終了 */
            bRun = EC_FALSE;
            break;
	}

        OsSleep(5);
    } /* while( !bThreadShutdown ) */

    pThreadParam->bThreadRunning = EC_FALSE;

    return;
}


EC_T_VOID	LogSave_NewFile(EC_T_VOID)
{
	flg_NewFile = EC_TRUE;
}
EC_T_VOID	LogSave_FwriteStart(EC_T_VOID)
{
	flg_FileUpdate = EC_TRUE;
}
EC_T_VOID	LogSave_FwriteStop(EC_T_VOID)
{
	flg_FileUpdate = EC_FALSE;
}

/********************************************************************************/
/** \brief  コンソールメニュー
*
* \return  Status value.
*/

static EC_T_DWORD myAppConsole(
    T_ALL_SLAVE_INFO*		pAllSlv,
    CAtEmLogging*           poLog,           /* [in]  Application instance */ 
    EC_T_DWORD              dwClntId,           /* [in]  EtherCAT master client id */
    EC_T_INT                nVerbose            /* [in]  verbosity level */
    )
{
    EC_T_DWORD	dwRes = EC_E_NOERROR;
    
    EC_T_CHAR	stdin_buf[128];
    EC_T_BYTE	menu_buf[80];
    EC_T_INT	menu_int 		= 0;

    printf("\n**********************************\n");
    printf("*   EtherCAT Master Console      *\n");
    printf("**********************************\n");
    printf(" [1]. CoE SDO command\n");
    printf(" [2]. ESC register \n");
    printf(" [3]. ECAT state change\n");
    printf(" [4]. Set SDO(Batch)\n");
    printf(" ..\n");
    printf(" [E]. LogSave Ctrl\n");
    printf(" [p]. Dump EHA Pos\n");
    printf(" [j]. Dump Joint Pos\n");
    printf(" ..\n");
    printf(" [Q]. Quit of Console\n=> ");
    
    scanf("%s", &menu_buf[0]);
    //fgets(&stdin_buf[0], 128, stdin); sscanf(&stdin_buf[0], "%s", &menu_buf[0]);
    if(menu_buf[0] == 'Q') {
        LogMsg("Quit of Console...");
        return EC_E_CANCEL;
    }
    /* menu check */
    switch(menu_buf[0]) {
        /* CoE Command */
    case '1' :
        myAppCS_SdoCmd(pAllSlv, poLog);
        break;

        /* ESC register */
    case '2' :
        myAppCS_ESCreg(pAllSlv, poLog, dwClntId, nVerbose);
        break;

        /* ECAT state change*/
    case '3' :
        myAppCS_StsChg(poLog);
        break;

        /* ECAT Batch SDO */
    case '4' :
        myAppCS_BatchSDO(poLog);
        break;

    case 'p' :
        myAppCS_DumpEHAPosition(pAllSlv, poLog);
        break;

    case 'E' :
        myAppCS_LogSaveCtrl(pAllSlv, poLog);
        break;

    default :
        printf(" !!Invalid Key!! \n");
        break;
    }

    return EC_E_NOERROR;
}

/********************************************************************************/
/** \brief  コンソールメニュー(SDO送受信)
*
* \return  Status value.
*/

#define	COE_BUF_SIZE	4
static EC_T_DWORD myAppCS_SdoCmd(
	T_ALL_SLAVE_INFO*		pAllSlv,
    CAtEmLogging*           poLog)           /* [in]   Application instance */ 
{
	EC_T_BYTE				byUpData[256] 	= {0,};
	EC_T_BYTE				byDownData[256] = {0,};
	EC_T_WORD				wObIndex 		= 0;
	EC_T_BYTE				byObSubIndex 	= 0;
	EC_T_DWORD				dwOutDataLen 	= 0;
	EC_T_DWORD				dwRes 			= EC_E_NOERROR;
	EC_T_UINT				cnt 			= 0;
	EC_T_INT				loop;

	EC_T_INT				trgt_grp		= 0;
	EC_T_INT				trgt_slave		= 0;
    EC_T_DWORD				trgt_dwSlaveId	= 0;
	EC_T_CHAR				menu_buf[80] 	= {0,};
	EC_T_INT				menu_int 		= 0;
	EC_T_UINT				menu_uint		= 0;
	float					menu_float		= 0;

	printf("\n [1]. CoE SDO command(manual)\n");

	// Select
	dwRes = TargetPrint(pAllSlv, &trgt_grp, &trgt_slave, &trgt_dwSlaveId);
	if(dwRes != EC_E_NOERROR) {
		//エラー時は何もしない
		return dwRes;
	}

	printf("  Index    : 0x");	scanf("%04x", &menu_uint); wObIndex 	= (EC_T_WORD)menu_uint;
	printf("  subIndex : 0x");	scanf("%02x", &menu_uint); byObSubIndex = (EC_T_BYTE)menu_uint;

#if 0  
	// 完了復帰型
	/* Performs a CoE SDO upload from an EtherCAT slave device to the master. */
	dwRes = ecatCoeSdoUpload( 
			trgt_dwSlaveId, 				// dwSlaveId    : EtherCAT slave ID. To determine the slave ID the function ecatGetSlaveId has to be used.
			wObIndex,						// wObIndex     : Object index.
			byObSubIndex,					// byObSubIndex : Object sub index.
			&byUpData[0],					// pbyData      : Data buffer to upload data to.
			sizeof(byUpData),				// dwDataLen    : Data size of pbyData
			&dwOutDataLen,					// pdwOutDataLen: Pointer returning size of data uploaded from slave.
			MBX_TIMEOUT,					// dwTimeout    : Timeout in milliseconds. The function will block at most for this time.
			0);								// dwFlags      : Mailbox Flags. Bit 0: if 1 Complete Access (EC_MAILBOX_FLAG_SDO_COMPLETE).
	if( dwRes != EC_E_NOERROR )
	{
		LogError( "====> error in COE SDO Upload! (Result = %s)", EcErrorText(dwRes) );
		return EC_E_ERROR;
	}
#else
	// 即時復帰型(あとでecatNotifyに完了が通知される)
	// !! 確保した領域の使いまわしは可能だが、DownloadとUploadでの切り替えての使いまわしは不可！！
	{
		EC_T_MBXTFER_DESC   MbxTferDesc			= {0}; 
		EC_T_MBXTFER*		pMbxCoETfer			= EC_NULL;
		EC_T_BYTE*			pbyCoEBuffer		= EC_NULL;

		do {
			/* create memory */
			pbyCoEBuffer = (EC_T_BYTE*)OsMalloc(COE_BUF_SIZE);
			if(pbyCoEBuffer == EC_NULL)
			{
				LogError("Error: NO MEMORY!!");
				dwRes = EC_E_ERROR;
				break;
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
				dwRes = EC_E_ERROR;
				break;
			}    

			OsMemset(pMbxCoETfer->pbyMbxTferData, 0, COE_BUF_SIZE);

			pMbxCoETfer->dwClntId = dwClntId;
			pMbxCoETfer->dwTferId = 1;

			/* Performs a CoE SDO download to an EtherCAT slave device. */
			dwRes = ecatCoeSdoUploadReq( 
					pMbxCoETfer,
					trgt_dwSlaveId, 			// dwSlaveId    : EtherCAT slave ID. To determine the slave ID the function ecatGetSlaveId has to be used.
					wObIndex,					// wObIndex     : Object index.
					byObSubIndex,				// byObSubIndex : Object sub index.
					MBX_TIMEOUT,				// dwTimeout    : Timeout in milliseconds. The function will block at most for this time.
					0);							// dwFlags      : Mailbox Flags. Bit 0: if 1 Complete Access (EC_MAILBOX_FLAG_SDO_COMPLETE).
			if( dwRes != EC_E_NOERROR )
			{
				LogError( "====> error in COE SDO Upload! (Result = %s)", EcErrorText(dwRes) );
				dwRes = EC_E_ERROR;
				break;
			}

			// Wait
			while(pMbxCoETfer->eTferStatus == eMbxTferStatus_Pend)
			{
				OsSleep(1);
			}

            if( pMbxCoETfer->dwErrorCode != EC_E_NOERROR ) 
			{
				dwRes = EC_E_ERROR;
				break;
			}
			
			// 取得したデータを格納する
			dwOutDataLen = pMbxCoETfer->dwDataLen;
			OsMemcpy(byUpData, pMbxCoETfer->pbyMbxTferData, dwOutDataLen);

		} while(0);

		/* Delete MBX Transfer objects */
		if( EC_NULL != pMbxCoETfer)
		{
			ecatMbxTferDelete( pMbxCoETfer );
			pMbxCoETfer = EC_NULL;
		}
		if( EC_NULL != pbyCoEBuffer)
		{
			OsFree(pbyCoEBuffer);               
			pbyCoEBuffer = EC_NULL;
		}

		// return check
		if(dwRes != EC_E_NOERROR)
		{
			return dwRes;
		}	
	}
#endif

	switch(dwOutDataLen)
	{
		case sizeof(EC_T_BYTE):
			{
				EC_T_BYTE	tmpVal;
				OsMemcpy(&tmpVal, &byUpData[0], sizeof(EC_T_BYTE));
				printf("<CoE> => Byte %d: SC(%d),UC(%d),HEX(%x)\n", dwOutDataLen, (EC_T_SBYTE)tmpVal, (EC_T_BYTE)tmpVal, tmpVal);
				LogMsg("<CoE> => Byte %d: SC(%d),UC(%d),HEX(%x)", dwOutDataLen, (EC_T_SBYTE)tmpVal, (EC_T_BYTE)tmpVal, tmpVal);
			}
			break;
		case sizeof(EC_T_WORD):
			{
				EC_T_WORD	tmpVal;
				OsMemcpy(&tmpVal, &byUpData[0], sizeof(EC_T_WORD));
				printf("<CoE> => Byte %d: SS(%d),US(%d),HEX(%x)\n", dwOutDataLen, (EC_T_SWORD)tmpVal, (EC_T_WORD)tmpVal, tmpVal);
				LogMsg("<CoE> => Byte %d: SS(%d),US(%d),HEX(%x)", dwOutDataLen, (EC_T_SWORD)tmpVal, (EC_T_WORD)tmpVal, tmpVal);
			}
			break;
		case sizeof(EC_T_DWORD):
			{
				EC_T_DWORD	tmpVal;
				float		tmpfloat;
				OsMemcpy(&tmpVal, &byUpData[0], sizeof(EC_T_DWORD));
				OsMemcpy(&tmpfloat, &byUpData[0], sizeof(float));
				printf("<CoE> => Byte %d: SL(%d),UL(%d),REAL(%G),HEX(%x)\n", dwOutDataLen, (EC_T_SDWORD)tmpVal, (EC_T_DWORD)tmpVal, tmpfloat, tmpVal);
				LogMsg("<CoE> => Byte %d: SL(%ld),UL(%d),REAL(%G),HEX(%x)", dwOutDataLen, (EC_T_SDWORD)tmpVal, (EC_T_DWORD)tmpVal, tmpfloat, tmpVal);
			}
			break;
		default:
			LogMsg("<CoE> => Byte %d: (%s)", dwOutDataLen, &byUpData[0]);

			for(cnt = 0; cnt < dwOutDataLen; cnt += 8)
			{
				LogMsg("<CoE> => HEX%04x: %02x %02x %02x %02x %02x %02x %02x %02x ",cnt,
						byUpData[cnt + 0], byUpData[cnt + 1], 
						byUpData[cnt + 2], byUpData[cnt + 3],
						byUpData[cnt + 4], byUpData[cnt + 5], 
						byUpData[cnt + 6], byUpData[cnt + 7]);
			}
			break;
	}

	/* Slaveに書き込むか? */
	printf("\n To Slave? (Y/N) ");	scanf("%s",&menu_buf[0]);
	if((menu_buf[0] == 'y') || (menu_buf[0] == 'Y'))
	{
		printf(" Data? [d]ecimal, [h]exadecimal, [f]loat  ");	scanf("%s",&menu_buf[0]);
		switch(menu_buf[0])
		{
			case 'f':
			case 'F':
				printf("  value?: "); scanf("%G",&menu_float);
				OsMemcpy(&byDownData[0], &menu_float, (dwOutDataLen>sizeof(float))?sizeof(float):dwOutDataLen);
				break;

			case 'h':
			case 'H':
				for (cnt = 0; cnt < dwOutDataLen;cnt++)
				{
					printf("  DATA[%d] : 0x",cnt); scanf("%02x", &menu_uint); byDownData[cnt] = (EC_T_BYTE)menu_uint;
				}
				break;

			case 'd':
			case 'D':
			default:
				printf("  value?: "); scanf("%d",&menu_int);
				OsMemcpy(&byDownData[0], &menu_int, (dwOutDataLen>sizeof(EC_T_INT))?sizeof(EC_T_INT):dwOutDataLen);
				break;
		}

#if 0
		// 完了復帰型
		/* Performs a CoE SDO download to an EtherCAT slave device. */
		dwRes = ecatCoeSdoDownload( 
				trgt_dwSlaveId, 				// dwSlaveId    : EtherCAT slave ID. To determine the slave ID the function ecatGetSlaveId has to be used.
				wObIndex,						// wObIndex     : Object index.
				byObSubIndex,					// byObSubIndex : Object sub index.
				(EC_T_BYTE*)&byDownData[0],		// pbyData      : Data to be transferred.
				dwOutDataLen,					// dwDataLen    : Data size of pbyData
				MBX_TIMEOUT,					// dwTimeout    : Timeout in milliseconds. The function will block at most for this time.
				0);								// dwFlags      : Mailbox Flags. Bit 0: if 1 Complete Access (EC_MAILBOX_FLAG_SDO_COMPLETE).
		if( dwRes != EC_E_NOERROR )
		{
			LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
			return EC_E_ERROR;
		}
#else
		// 即時復帰型(あとでecatNotifyに完了が通知される)
		// !! 確保した領域の使いまわしは可能だが、DownloadとUploadでの切り替えての使いまわしは不可！！
		{
			EC_T_MBXTFER_DESC   MbxTferDesc			= {0}; 
			EC_T_MBXTFER*		pMbxCoETfer			= EC_NULL;
			EC_T_BYTE*			pbyCoEBuffer		= EC_NULL;

			do {
				/* create memory */
				pbyCoEBuffer = (EC_T_BYTE*)OsMalloc(COE_BUF_SIZE);
				if(pbyCoEBuffer == EC_NULL)
				{
					LogError("Error: NO MEMORY!!");
					dwRes = EC_E_ERROR;
					break;
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
					dwRes = EC_E_ERROR;
					break;
				}    

				OsMemset(pMbxCoETfer->pbyMbxTferData, 0, COE_BUF_SIZE);
				OsMemcpy(pMbxCoETfer->pbyMbxTferData, byDownData, dwOutDataLen);

				pMbxCoETfer->dwClntId 	= dwClntId;
				pMbxCoETfer->dwTferId 	= 1;
				pMbxCoETfer->dwDataLen  = dwOutDataLen;

				/* Performs a CoE SDO download to an EtherCAT slave device. */
				dwRes = ecatCoeSdoDownloadReq( 
						pMbxCoETfer,
						trgt_dwSlaveId, 			// dwSlaveId    : EtherCAT slave ID. To determine the slave ID the function ecatGetSlaveId has to be used.
						wObIndex,					// wObIndex     : Object index.
						byObSubIndex,				// byObSubIndex : Object sub index.
						MBX_TIMEOUT,				// dwTimeout    : Timeout in milliseconds. The function will block at most for this time.
						0);							// dwFlags      : Mailbox Flags. Bit 0: if 1 Complete Access (EC_MAILBOX_FLAG_SDO_COMPLETE).
				if( dwRes != EC_E_NOERROR )
				{
					LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
					dwRes = EC_E_ERROR;
					break;
				}

				// Wait
				while(pMbxCoETfer->eTferStatus == eMbxTferStatus_Pend)
				{
					OsSleep(1);
				}

				if( pMbxCoETfer->dwErrorCode != EC_E_NOERROR ) 
				{
					dwRes = EC_E_ERROR;
				}
				
			} while(0);

			/* Delete MBX Transfer objects */
			if( EC_NULL != pMbxCoETfer)
			{
				ecatMbxTferDelete( pMbxCoETfer );
				pMbxCoETfer = EC_NULL;
			}
			if( EC_NULL != pbyCoEBuffer)
			{
				OsFree(pbyCoEBuffer);               
				pbyCoEBuffer = EC_NULL;
			}

			// return check
			if(dwRes != EC_E_NOERROR)
			{
				return dwRes;
			}	
		}
#endif

		//reload
		/* Performs a CoE SDO upload from an EtherCAT slave device to the master. */
		dwRes = ecatCoeSdoUpload( 
				trgt_dwSlaveId, 				// dwSlaveId    : EtherCAT slave ID. To determine the slave ID the function ecatGetSlaveId has to be used.
				wObIndex,						// wObIndex     : Object index.
				byObSubIndex,					// byObSubIndex : Object sub index.
				&byUpData[0],					// pbyData      : Data buffer to upload data to.
				sizeof(byUpData),				// dwDataLen    : Data size of pbyData
				&dwOutDataLen,					// pdwOutDataLen: Pointer returning size of data uploaded from slave.
				MBX_TIMEOUT,					// dwTimeout    : Timeout in milliseconds. The function will block at most for this time.
				0);								// dwFlags      : Mailbox Flags. Bit 0: if 1 Complete Access (EC_MAILBOX_FLAG_SDO_COMPLETE).
		if( dwRes != EC_E_NOERROR )
		{
			LogError( "====> error in COE SDO Upload! (Result = %s)", EcErrorText(dwRes) );
			return EC_E_ERROR;
		}

		switch(dwOutDataLen)
		{
			case sizeof(EC_T_BYTE):
				{
					EC_T_BYTE	tmpVal;
					OsMemcpy(&tmpVal, &byUpData[0], sizeof(EC_T_BYTE));
					printf("[reload] <CoE> => Byte %d: SC(%d),UC(%d),HEX(%x)\n", dwOutDataLen, (EC_T_SBYTE)tmpVal, (EC_T_BYTE)tmpVal, tmpVal);
					LogMsg("[reload] <CoE> => Byte %d: SC(%d),UC(%d),HEX(%x)", dwOutDataLen, (EC_T_SBYTE)tmpVal, (EC_T_BYTE)tmpVal, tmpVal);
				}
				break;
			case sizeof(EC_T_WORD):
				{
					EC_T_WORD	tmpVal;
					OsMemcpy(&tmpVal, &byUpData[0], sizeof(EC_T_WORD));
					printf("[reload] <CoE> => Byte %d: SS(%d),US(%d),HEX(%x)\n", dwOutDataLen, (EC_T_SWORD)tmpVal, (EC_T_WORD)tmpVal, tmpVal);
					LogMsg("[reload] <CoE> => Byte %d: SS(%d),US(%d),HEX(%x)", dwOutDataLen, (EC_T_SWORD)tmpVal, (EC_T_WORD)tmpVal, tmpVal);
				}
				break;
			case sizeof(EC_T_DWORD):
				{
					EC_T_DWORD	tmpVal;
					float		tmpfloat;
					OsMemcpy(&tmpVal, &byUpData[0], sizeof(EC_T_DWORD));
					OsMemcpy(&tmpfloat, &byUpData[0], sizeof(float));
					printf("[reload] <CoE> => Byte %d: SL(%d),UL(%d),REAL(%G),HEX(%x)\n", dwOutDataLen, (EC_T_SDWORD)tmpVal, (EC_T_DWORD)tmpVal, tmpfloat, tmpVal);
					LogMsg("[reload] <CoE> => Byte %d: SL(%ld),UL(%d),REAL(%G),HEX(%x)", dwOutDataLen, (EC_T_SDWORD)tmpVal, (EC_T_DWORD)tmpVal, tmpfloat, tmpVal);
				}
				break;
			default:
				LogMsg("[reload] <CoE> => Byte %d: (%s)", dwOutDataLen, &byUpData[0]);

				for(cnt = 0; cnt < dwOutDataLen; cnt += 8)
				{
					LogMsg("[reload] <CoE> => HEX%04x: %02x %02x %02x %02x %02x %02x %02x %02x ",cnt,
							byUpData[cnt + 0], byUpData[cnt + 1], 
							byUpData[cnt + 2], byUpData[cnt + 3],
							byUpData[cnt + 4], byUpData[cnt + 5], 
							byUpData[cnt + 6], byUpData[cnt + 7]);
				}
				break;
		}
	}

	return EC_E_NOERROR;
}

/********************************************************************************/
/** \brief  コンソールメニュー(ESC read/write)
*
* \return  Status value.
*/

static EC_T_DWORD myAppCS_ESCreg(
    T_ALL_SLAVE_INFO*		pAllSlv,
    CAtEmLogging*           poLog,           /* [in]  Application instance */ 
    EC_T_DWORD              dwClntId,           /* [in]  EtherCAT master client id */
    EC_T_INT                nVerbose            /* [in]  verbosity level */
    )
{
    EC_T_DWORD				dwRes 			= EC_E_NOERROR;
    EC_T_WORD				wOffset 		= 0;
    EC_T_BYTE				byReadData[16] 	= {0,};
    EC_T_BYTE				byWriteData[16]	= {0,};
    EC_T_DWORD				dwDataLen		= 0;
    EC_T_UINT				cnt 			= 0;
    
    EC_T_WORD				wCfgPhyAddress;
	EC_T_INT				trgt_grp		= 0;
	EC_T_INT				trgt_slave		= 0;
    EC_T_DWORD				trgt_dwSlaveId	= 0;
    EC_T_INT				trgt_axis;
    EC_T_BYTE				menu_buf[80] 	= {0,};
    EC_T_INT				menu_int 		= 0;
    EC_T_UINT				menu_uint		= 0;

	printf("\n [2]. ESC register\n");

    do {
		// Select
		dwRes = TargetPrint(pAllSlv, &trgt_grp, &trgt_slave, &trgt_dwSlaveId);
		if(dwRes != EC_E_NOERROR) {
			//エラー時は何もしない
			return dwRes;
		}

		// 物理アドレスを取得する
		switch(trgt_grp)
		{
		case BDTYPE_MD4KW_2MFS:
            wCfgPhyAddress = pAllSlv->MD4KW_2MFS[trgt_slave].Info.wCfgPhyAddress;
			break;
		case BDTYPE_MD4KW_HAND:
            wCfgPhyAddress = pAllSlv->MD4KW_Hand[trgt_slave].Info.wCfgPhyAddress;
			break;
		case BDTYPE_MD4KW_IMU:
            wCfgPhyAddress = pAllSlv->MD4KW_IMU[trgt_slave].Info.wCfgPhyAddress;
			break;

		case BDTYPE_MD4KW_3M:
		default:
            wCfgPhyAddress = pAllSlv->MD4KW_3M[trgt_slave].Info.wCfgPhyAddress;
			break;
		}

        printf("  Offset   : 0x");	scanf("%04x",&menu_uint);	wOffset 	= (EC_T_WORD)menu_uint;
        printf("  Size     : ");	scanf("%d",&menu_uint);		dwDataLen	= (EC_T_WORD)menu_uint;
        LogMsg("<ESC> SlaveAdr[%x] Read Register :%04xh + (%d)", wCfgPhyAddress, wOffset, dwDataLen);

        /* ESCレジスター読み出し */
        dwRes = ecatReadSlaveRegister( 
            EC_TRUE,        // bFixedAddress	: EC_TRUE:used station address, EC_FALSE:use Auto Increment Address.
            wCfgPhyAddress, // dwSlaveId    : EtherCAT slave ID. To determine the slave ID the function ecatGetSlaveId has to be used.
            wOffset,        // wRegisterOffset	: Register offset. I.e use 0x0130 to read the AL status register.
            &byReadData[0], // pvData		    : Pointer to the receive buffer
            dwDataLen,      // wLen				: Number of bytes to receive.
            MBX_TIMEOUT);   // dwTimeout    	: Timeout in milliseconds. The function will block at most for this time.
        if( dwRes != EC_E_NOERROR ) {
            LogError( "====> error in ESC Read Register! (Result = %s)", EcErrorText(dwRes) );
            break;
        }
        for(cnt = 0; cnt < dwDataLen; cnt += 4) {
            LogMsg("<ESC> => HEX%04x: %02x %02x %02x %02x ",(cnt+wOffset),
                   byReadData[cnt + 0], byReadData[cnt + 1], byReadData[cnt + 2], byReadData[cnt + 3]);
        }

        /* Slaveに書き込むか? */
        printf("\n To Slave? (Y/N) ");	scanf("%s",&menu_buf[0]);
        if((menu_buf[0] == 'n') || (menu_buf[0] == 'N')) {
            break;
        }

        for(cnt = 0; cnt < dwDataLen; cnt++) {
            printf("  DATA[%d] : 0x",cnt); scanf("%x",(unsigned int *)&byWriteData[cnt]);
        }

        LogMsg("<ESC> SlaveAdr[%x] Write Register :%04xh + (%d)", wCfgPhyAddress, wOffset, dwDataLen);
        for(cnt = 0; cnt < dwDataLen; cnt += 4) {
            LogMsg("<ESC> => HEX: %02x %02x %02x %02x ", byWriteData[0], byWriteData[1], byWriteData[2], byWriteData[3]);
        }

        dwRes = ecatWriteSlaveRegister( 
            EC_TRUE,						// bFixedAddress	: EC_TRUE:used station address, EC_FALSE:use Auto Increment Address.
            wCfgPhyAddress, 				// dwSlaveId    	: EtherCAT slave ID. To determine the slave ID the function ecatGetSlaveId has to be used.
            wOffset,						// wRegisterOffset	: Register offset. I.e use 0x0130 to read the AL status register.
            &byWriteData[0],				// pvData		    : Pointer to the data to send. 
            4,								// wLen				: Number of bytes to send.
            MBX_TIMEOUT);							// dwTimeout    	: Timeout in milliseconds. The function will block at most for this time.
        if( dwRes != EC_E_NOERROR ) {
            LogError( "====> error in ESC Write Register! (Result = %s)", EcErrorText(dwRes) );
            break;
        }
    } while (0);

    return EC_E_NOERROR;
}

static EC_T_DWORD myAppCS_StsChg( CAtEmLogging* poLog)
{
    EC_T_DWORD	dwRes		= EC_E_NOERROR;
    EC_T_INT	menu_int 	= 0;
    EC_T_STATE	eEcatState 	= eEcatState_UNKNOWN;
    
    printf("\n [3]. ECAT state change\n");
    printf("  [1]:INIT, [2]:PRE-OP, [3]:SAFE-OP, [4]:OP\n");
    printf("   =>");	scanf("%d",&menu_int);	

    switch(menu_int) {
    case 1:
        eEcatState = eEcatState_INIT;
        break;
    case 2:
        eEcatState = eEcatState_PREOP;
        break;
    case 3:
        eEcatState = eEcatState_SAFEOP;
        break;
    case 4:
        eEcatState = eEcatState_OP;
        break;
    }

    LogMsg("<ECAT> EtherCAT State Change [%d]", eEcatState);
    dwRes = ecatSetMasterState( ETHERCAT_START_TIMEOUT + ETHERCAT_SCANBUS_TIMEOUT, eEcatState);
    if( dwRes != EC_E_NOERROR ) {
        LogError( "====> Cannot state change!  (Result = %s (0x%lx))", EcErrorText(dwRes), dwRes );
    }

    return dwRes;
}


static EC_T_DWORD myAppCS_BatchSDO( CAtEmLogging* poLog)
{
    EC_T_DWORD	dwRes		= EC_E_NOERROR;
	int			iSel;
	ifstream   	ifs;
	string     	line;
	string		csv_file_path;

	cout << "\n [4]. Set SDO(Batch)\n";
	cout << "   [1]. Batch Start 'BatchSdo1.csv'\n";
	cout << "   [2]. Batch Start 'BatchSdo2.csv'\n";
	cout << "   [3]. Batch Start 'BatchSdo3.csv'\n";
	cout << "   ..\n";
	cout << "   [9]. Input Batch File\n";

	// 選択毎にバッチファイルを指定する
	cin >> iSel;
	switch(iSel)
	{
	case 1:
		ifs.open("BatchSdo1.csv");
		break;

	case 2:
		ifs.open("BatchSdo2.csv");
		break;

	case 3:
		ifs.open("BatchSdo3.csv");
		break;

	case 9:
		cout << "\n   Input File : ";
		cin >> csv_file_path;

		ifs.open(csv_file_path.c_str() );
		break;

	default:
		// 対象外の場合は抜ける
		cout << "input error!!";
    	return dwRes;
	}

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
        LogError( "====> Cannot file open!");
	}

    return dwRes;
}


static EC_T_DWORD myAppCS_DumpEHAPosition( T_ALL_SLAVE_INFO* pAllSlv, CAtEmLogging* poLog)
{
    EC_T_DWORD	dwRes		= EC_E_NOERROR;
    EC_T_INT	loop;
    EC_T_INT	tmp;
    EC_T_BYTE	bTmp;
    EC_T_INT	trgt_axis;
    EC_T_WORD   obj;
    EC_T_INT    jnt;
    EC_T_DWORD  eha_pos;
    EC_T_DWORD	dwOutDataLen 	= 0;
    printf(" EHA Position\n ");
    for(loop = 0; loop < pAllSlv->MD4KW_3MSlaves; loop++) {
      for(jnt = 0; jnt<3; jnt++) {
	obj = (0x6001|jnt<<8);
	dwRes = ecatCoeSdoUpload( 
				 pAllSlv->MD4KW_3M[loop].Info.dwSlaveId, // dwSlaveId    : EtherCAT slave ID. To determine the slave ID the function ecatGetSlaveId has to be used.
				 obj,				// wObIndex     : Object index.
				 0x00,				// byObSubIndex : Object sub index.
				 (EC_T_BYTE*)&eha_pos,			// pbyData      : Data buffer to upload data to.
				 sizeof(eha_pos),		// dwDataLen    : Data size of pbyData
				 &dwOutDataLen,			// pdwOutDataLen: Pointer returning size of data uploaded from slave.
				 MBX_TIMEOUT,			// dwTimeout    : Timeout in milliseconds. The function will block at most for this time.
				 0);				// dwFlags      : Mailbox Flags. Bit 0: if 1 Complete Access (EC_MAILBOX_FLAG_SDO_COMPLETE).
	if( dwRes != EC_E_NOERROR ) {
	  LogError( "====> error in COE SDO Upload! (Result = %s)", EcErrorText(dwRes) );
	  break;
	}
	LogMsg("<CoE> Slave:%s(cfg-addr:%d)\t EHA Position(6x01h)  => %08x ", 
	       pAllSlv->MD4KW_3M[loop].Info.abyDeviceName, 
	       pAllSlv->MD4KW_3M[loop].Info.wPhysAddress,
	       eha_pos);
      }
    }
#if 0
    // 有効判定
    switch(tmp) {
        // 全トルクセンサOFFSET再計算
    case 1:
        bTmp = 1;
        for(loop = 0; loop < pAllSlv->TMSlaves; loop++) {
            /* Performs a CoE SDO download to an EtherCAT slave device. */
            dwRes = ecatCoeSdoDownload( pAllSlv->TM[loop].Info.dwSlaveId, 0x204B, 0x00, &bTmp, 1, MBX_TIMEOUT, 0);
            if( dwRes != EC_E_NOERROR ) {
                LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
                break;
            }
            LogMsg("<CoE> Slave:%s(cfg-addr:%d)\tTrqSns offset Reset(204Bh)  => %d ", 
                   pAllSlv->TM[loop].Info.abyDeviceName, pAllSlv->TM[loop].Info.wPhysAddress, bTmp);
        }

        break;

        // 個別選択式
    case 3:
        //右詰めでシフトする
        if(TargetPrint(pAllSlv, &trgt_axis) != EC_E_NOERROR) {
            //エラー時は何もしない
            LogError("!!!Slaves mismatch :TMSlave [%d]\n", pAllSlv->TMSlaves);
            return dwRes;
        }

        bTmp = 1;
        /* Performs a CoE SDO download to an EtherCAT slave device. */
        dwRes = ecatCoeSdoDownload( pAllSlv->TM[trgt_axis].Info.dwSlaveId, 0x204B, 0x00, &bTmp, 1, MBX_TIMEOUT, 0);
        if( dwRes != EC_E_NOERROR ) {
            LogError( "====> error in COE SDO Download! (Result = %s)", EcErrorText(dwRes) );
            break;
        }
        LogMsg("<CoE> Slave:%s(cfg-addr:%d)\tTrqSns offset Reset(204Bh)  => %d ", 
               pAllSlv->TM[trgt_axis].Info.abyDeviceName, pAllSlv->TM[trgt_axis].Info.wPhysAddress, bTmp);
        break;

    default:
        printf(" !!Invalid !! \n");
        break;
    }
#endif
    return dwRes;
}


static EC_T_DWORD myAppCS_LogSaveCtrl( T_ALL_SLAVE_INFO* pAllSlv, CAtEmLogging* poLog)
{
    EC_T_DWORD	dwRes		= EC_E_NOERROR;
    EC_T_INT	tmp;
    EC_T_REAL	fValue;
    EC_T_INT	trgt_axis;

    printf(" LogSave Ctrl\n");
    printf(" (1)  : New File\n");
    printf(" (2)  : Fwrite Start\n");
    printf(" (3)  : Fwrite Stop\n");
    printf("   =>");	scanf("%d",&tmp);	

    // 有効判定
    switch(tmp) {
    case 1:
        {
            time_t				timer;
            struct tm			*local;
        
            //現在の日付、時刻を取得
            time(&timer);
            local = localtime(&timer);
        
            printf("<LOG> NewFile!! [***_%04d%02d%02d_%02d%02d%02d%s]", 
                   local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
                   local->tm_hour, local->tm_min, local->tm_sec, 
                   FILE_EXT);
            LogMsg("<LOG> NewFile!! [***_%04d%02d%02d_%02d%02d%02d%s]", 
                   local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
                   local->tm_hour, local->tm_min, local->tm_sec, 
                   FILE_EXT);

        }
        LogSave_NewFile();
        break;
    
    case 2:
        LogMsg("<LOG> Fwrite Start");
        LogSave_FwriteStart();
        break;

    case 3:
        LogMsg("<LOG> Fwrite Stop");
        LogSave_FwriteStop();
        break;

    default:
        printf(" !!Invalid !! \n");
        break;
    }

    return dwRes;
}


static EC_T_DWORD TargetPrint( 
	T_ALL_SLAVE_INFO* 		pAllSlv, 
	EC_T_INT*				pTrgt_grp,
	EC_T_INT*				pTrgt_slave,
    EC_T_DWORD*				pTrgt_dwSlaveId)
{
	EC_T_DWORD				dwRes 			= EC_E_NOERROR;
	EC_T_INT				loop;
	EC_T_INT				tmp_grp;
	EC_T_INT				tmp_slave;
    EC_T_DWORD				tmp_dwSlaveId;

	printf(" Target Group?\n");

	printf("  [%d] :MD4KW 3M\n", BDTYPE_MD4KW_3M);
	printf("  [%d] :MD4KW 2MFS\n", BDTYPE_MD4KW_2MFS);
	printf("  [%d] :MD4KW Hand\n", BDTYPE_MD4KW_HAND);
	printf("  [%d] :MD4KW IMU\n", BDTYPE_MD4KW_IMU);

	printf("   Group =>");	
	scanf("%d", &tmp_grp);	

	// グループ毎のSlave群を出力する
	switch(tmp_grp)
	{
		case BDTYPE_MD4KW_3M:
			if(pAllSlv->MD4KW_3MSlaves <= 0)
			{
				printf("\n Slave Not Fould !!\n");	
				dwRes = EC_E_ERROR;
				break;
			}
			for(loop = 0; loop < pAllSlv->MD4KW_3MSlaves; loop++)
			{
				printf("-- MD4KW_3M[%d] SlaveId:%d, PhysAddr:%d(0x%x), DevName:%s\n", loop, 
						pAllSlv->MD4KW_3M[loop].Info.dwSlaveId, 
						pAllSlv->MD4KW_3M[loop].Info.wPhysAddress, pAllSlv->MD4KW_3M[loop].Info.wPhysAddress,
						pAllSlv->MD4KW_3M[loop].Info.abyDeviceName);
			}
			break;

		case BDTYPE_MD4KW_2MFS:
			if(pAllSlv->MD4KW_2MFSSlaves <= 0)
			{
				printf("\n Slave Not Fould !!\n");	
				dwRes = EC_E_ERROR;
				break;
			}
			for(loop = 0; loop < pAllSlv->MD4KW_2MFSSlaves; loop++)
			{
				printf("-- MD4KW_2MFS[%d] SlaveId:%d, PhysAddr:%d(0x%x), DevName:%s\n", loop, 
						pAllSlv->MD4KW_2MFS[loop].Info.dwSlaveId, 
						pAllSlv->MD4KW_2MFS[loop].Info.wPhysAddress, pAllSlv->MD4KW_2MFS[loop].Info.wPhysAddress,
						pAllSlv->MD4KW_2MFS[loop].Info.abyDeviceName);
			}
			break;

		case BDTYPE_MD4KW_HAND:
			if(pAllSlv->MD4KW_HandSlaves <= 0)
			{
				printf("\n Slave Not Fould !!\n");	
				dwRes = EC_E_ERROR;
				break;
			}
			for(loop = 0; loop < pAllSlv->MD4KW_HandSlaves; loop++)
			{
				printf("-- MD4KW_Hand[%d] SlaveId:%d, PhysAddr:%d(0x%x), DevName:%s\n", loop, 
						pAllSlv->MD4KW_Hand[loop].Info.dwSlaveId, 
						pAllSlv->MD4KW_Hand[loop].Info.wPhysAddress, pAllSlv->MD4KW_Hand[loop].Info.wPhysAddress,
						pAllSlv->MD4KW_Hand[loop].Info.abyDeviceName);
			}
			break;

		case BDTYPE_MD4KW_IMU:
			if(pAllSlv->MD4KW_IMUSlaves <= 0)
			{
				printf("\n Slave Not Fould !!\n");	
				dwRes = EC_E_ERROR;
				break;
			}
			for(loop = 0; loop < pAllSlv->MD4KW_IMUSlaves; loop++)
			{
				printf("-- MD4KW_IMU[%d] SlaveId:%d, PhysAddr:%d(0x%x), DevName:%s\n", loop, 
						pAllSlv->MD4KW_IMU[loop].Info.dwSlaveId, 
						pAllSlv->MD4KW_IMU[loop].Info.wPhysAddress, pAllSlv->MD4KW_IMU[loop].Info.wPhysAddress,
						pAllSlv->MD4KW_IMU[loop].Info.abyDeviceName);
			}
			break;

		default:
			printf("\n Invalid Group !!\n");	
			dwRes = EC_E_ERROR;
			break;
	}
	// 問題があれば、直ちに抜ける
	if(dwRes != EC_E_NOERROR)
	{
		return dwRes;
	}	

	printf("   Slave =>");	
	scanf("%d", &tmp_slave);	

	// 対象スレーブIDを取得する
	switch(tmp_grp)
	{
		case BDTYPE_MD4KW_3M:
			if(pAllSlv->MD4KW_3MSlaves < tmp_slave)
			{
				printf("\n Invalid SlaveNo!!\n");	
				dwRes = EC_E_ERROR;
				break;
			}
			tmp_dwSlaveId	= pAllSlv->MD4KW_3M[tmp_slave].Info.dwSlaveId;
			break;

		case BDTYPE_MD4KW_2MFS:
			if(pAllSlv->MD4KW_2MFSSlaves < tmp_slave)
			{
				printf("\n Invalid SlaveNo!!\n");	
				dwRes = EC_E_ERROR;
				break;
			}
			tmp_dwSlaveId	= pAllSlv->MD4KW_2MFS[tmp_slave].Info.dwSlaveId;
			break;

		case BDTYPE_MD4KW_HAND:
			if(pAllSlv->MD4KW_HandSlaves < tmp_slave)
			{
				printf("\n Invalid SlaveNo!!\n");	
				dwRes = EC_E_ERROR;
				break;
			}
			tmp_dwSlaveId	= pAllSlv->MD4KW_Hand[tmp_slave].Info.dwSlaveId;
			break;

		case BDTYPE_MD4KW_IMU:
			if(pAllSlv->MD4KW_IMUSlaves< tmp_slave)
			{
				printf("\n Invalid SlaveNo!!\n");	
				dwRes = EC_E_ERROR;
				break;
			}
			tmp_dwSlaveId	= pAllSlv->MD4KW_IMU[tmp_slave].Info.dwSlaveId;
			break;

		default:
			printf("\n Invalid Group !!\n");	
			dwRes = EC_E_ERROR;
			break;
	}
	if(dwRes != EC_E_NOERROR)
	{
		return dwRes;
	}	

	// 正常に指定できた場合のみ、値を設定する
	*pTrgt_grp 			= tmp_grp;
	*pTrgt_slave 		= tmp_slave;
    *pTrgt_dwSlaveId 	= tmp_dwSlaveId;

	return dwRes;
}

