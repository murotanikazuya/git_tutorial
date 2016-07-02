/*-----------------------------------------------------------------------------
 * AtEmRasSrv.h             file
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Willig, Andreas
 * Description              description of file
 * Date                     2007/5/4::7:18
 *---------------------------------------------------------------------------*/

#ifndef __ATEMRASSRV_H__
#define __ATEMRASSRV_H__    1

/*-INCLUDES------------------------------------------------------------------*/

#include <EcType.h>
#include <AtEmRasError.h>
#include <EcOs.h>
#include <AtEmRasType.h>
#include <AtEthercat.h>
#include "AtEmRasSrvVersion.h"


/*-PACK SETTINGS-------------------------------------------------------------*/
#if defined  __GNUC__   /* GNU */

#define EC_PACKED __attribute__((aligned(4), packed))

#if (defined __RCX__)
#pragma pack (4)
#endif

#elif defined _DIAB_TOOL

#define EC_PACKED __attribute__((aligned(4), packed))

#elif defined _MSC_VER  /* MICROSOFT */

#define EC_PACKED
#pragma pack (push, 4)
#pragma warning(disable:4514)

#elif defined __MET__

#define EC_PACKED
/*#pragma pack (push, 4)*/
#pragma Push_align_members(4)
	
#elif defined(__BORLANDC__)

#pragma pack (push, 4)
#define EC_PACKED	

#elif defined __ghs

#undef  EC_PACKED
#define EC_PACKED __attribute__((aligned(4), packed))

#else
#error Please check pack settings for this compiler!!!
#endif
                           
/*-DEFINES-------------------------------------------------------------------*/
#if (defined INCLUDE_RAS_SPOCSUPPORT)

typedef enum _ATEMRAS_T_ORDINAL
{
    ord_emInitMaster                = 201,  /*  c9 */
    ord_emDeinitMaster              = 202,  /*  ca */
    ord_emStart                     = 203,  /*  cb */
    ord_emStop                      = 204,  /*  cc */
    ord_emIoControl                 = 205,  /*  cd */
    ord_emGetSlaveId                = 207,  /*  cf */
    ord_emMbxTferCreate             = 208,  /*  d0 */
    ord_emMbxTferDelete             = 209,  /*  d1 */
    ord_emCoeSdoDownloadReq         = 210,  /*  d2 */
    ord_emCoeSdoUploadReq           = 211,  /*  d3 */
    ord_emCoeGetODList              = 212,  /*  d4 */
    ord_emCoeGetObjectDesc          = 213,  /*  d5 */
    ord_emCoeGetEntryDesc           = 214,  /*  d6 */
    ord_emGetSlaveProp              = 218,  /*  da */
    ord_emGetSlaveState             = 219,  /*  db */
    ord_emSetSlaveState             = 220,  /*  dc */
    ord_emTferSingleRawCmd          = 221,  /*  dd */
    ord_emGetSlaveIdAtPosition      = 225,  /*  e1 */
    ord_emGetNumConfiguredSlaves    = 226,  /*  e2 */
    ord_emConfigureMaster           = 227,  /*  e3 */
    ord_emSetMasterState            = 228,  /*  e4 */
    ord_emQueueRawCmd               = 229,  /*  e5 */
    ord_emCoeRxPdoTfer              = 230,  /*  e6 */
    ord_emExecJob                   = 231,  /*  e7 */
    ord_emGetProcessData            = 234,  /*  ea */
    ord_emSetProcessData            = 235,  /*  eb */
    ord_emGetMasterState            = 236,  /*  ec */
    ord_emFoeFileUpload             = 237,  /*  ed */
    ord_emFoeFileDownload           = 238,  /*  ee */
    ord_emFoeUpoadReq               = 239,  /*  ef */
    ord_emFoeDownloadReq            = 240,  /*  f0 */
    ord_emCoeSdoDownload            = 241,  /*  f1 */
    ord_emCoeSdoUpload              = 242,  /*  f2 */
    ord_emGetNumConnectedSlaves     = 243,  /*  f3 */
    ord_emResetSlaveController      = 244,  /*  f4 */
    ord_emGetSlaveInfo              = 245,  /*  f5 */
    ord_emIsSlavePresent            = 246,  /*  f6 */
    ord_emAoeWriteReq               = 247,  /*  f7 */
    ord_emAoeReadReq                = 248,  /*  f8 */
    ord_emAoeWrite                  = 249,  /*  f9 */
    ord_emAoeRead                   = 250,  /*  fa */
    ord_emAoeGetSlaveNetId          = 251,  /*  fb */
    ord_emGetFixedAddr              = 252,  /*  fc */
    ord_emGetSlaveProcVarInfoNumOf  = 253,  /*  fd */
    ord_emGetSlaveProcVarInfo       = 254,  /*  fe */
    ord_emFindProcVarByName         = 255,  /*  ff */
    ord_emGetProcessDataBits        = 256,  /* 100 */
    ord_emSetProcessDataBits        = 257,  /* 101 */
    ord_emReloadSlaveEEPRom         = 258,  /* 102 */
    ord_emReadSlaveEEPRom           = 259,  /* 103 */
    ord_emWriteSlaveEEPRom          = 260,  /* 104 */
    ord_emAssignSlaveEEPRom         = 261,  /* 105 */
    ord_emSoeRead                   = 262,  /* 106 */
    ord_emSoeWrite                  = 263,  /* 107 */
    ord_emSoeAbortProcCmd           = 264,  /* 108 */
    ord_emGetNumConnectedSlavesMain = 265,  /* 109 */
    ord_emGetNumConnectedSlavesRed  = 266,  /* 10a */
    ord_emNotifyApp                 = 267,  /* 10b */
    ord_emAoeReadWriteReq           = 268,  /* 10c */
    ord_emAoeReadWrite              = 269,  /* 10d */
    ord_emGetCfgSlaveInfo           = 270,  /* 10e */
    ord_emGetBusSlaveInfo           = 271,  /* 10f */
    
    /* Borland C++ datatype alignment correction */
    ord_BCppDummy                   = 0xFFFFFFFF
} EC_PACKED ATEMRAS_T_ORDINAL;

/*
typedef enum _ATEMRAS_T_ACCESSLEVEL
{
*/
#define rasal_freeaccess        ((EC_T_DWORD)0x00000000)
#define rasal_readonlyaccess    ((EC_T_DWORD)0x00000100)
#define rasal_noaccess          ((EC_T_DWORD)0x00000f00)
#define rasal_neverrestrict     ((EC_T_DWORD)0xffffffff)
/*
} ATEMRAS_T_ACCESSLEVEL;
*/

#define PARAMETER_IGNORE        ((EC_T_DWORD)0xffffffff)

#define RASSPOCCFGINITDEFAULT \
    { /* binary mark */ \
        'b'+('i'<<8)+('n'<<16)+('a'<<24),'r'+('y'<<8)+('c'<<16)+('o'<<24),'n'+('f'<<8)+('i'<<16)+('g'<<24), \
    /*  class entry count */ \
        0x00000001, \
    /*  class,      access */ \
        0x00000001, rasal_freeaccess,     /* free access */ \
    /*  call class, access,                 ordinal,                        index ,                                     subindex,           cse=0 */ \
        0x00000001, rasal_freeaccess,       ord_emStart,                    PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emStart, free access */ \
        0x00000001, rasal_freeaccess,       ord_emStop,                     PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emStop, free access */ \
        0x00000001, rasal_neverrestrict,    ord_emIoControl,                EC_IOCTL_REGISTERCLIENT,                    PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_REGISTERCLIENT), any access */ \
        0x00000001, rasal_neverrestrict,    ord_emIoControl,                EC_IOCTL_UNREGISTERCLIENT,                  PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_UNREGISTERCLIENT), any access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SET_CYC_ERROR_NOTIFY_MASK,         PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SET_CYC_ERROR_NOTIFY_MASK), any access */ \
        0x00000001, rasal_readonlyaccess,   ord_emIoControl,                EC_IOCTL_ISLINK_CONNECTED,                  PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_ISLINK_CONNECTED), read access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_RESET_SLAVE,                       PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_RESET_SLAVE), free access */ \
        0x00000001, rasal_readonlyaccess,   ord_emIoControl,                EC_IOCTL_GET_PDMEMORYSIZE,                  PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_GET_PDMEMORYSIZE), read access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_FORCE_BROADCAST_DESTINATION,       PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_FORCE_BROADCAST_DESTINATION), free access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SLAVE_LINKMESSAGES,                PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SLAVE_LINKMESSAGES), free access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SET_FRAME_LOSS_SIMULATION,         PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SET_FRAME_LOSS_SIMULATION), free access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SET_RXFRAME_LOSS_SIMULATION,       PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SET_RXFRAME_LOSS_SIMULATION), free access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SET_TXFRAME_LOSS_SIMULATION,       PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SET_TXFRAME_LOSS_SIMULATION), free access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SET_SOFT_ASSERTIONS,               PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SET_SOFT_ASSERTIONS), free access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SET_HARD_ASSERTIONS,               PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SET_HARD_ASSERTIONS), free access */ \
        0x00000001, rasal_readonlyaccess,   ord_emIoControl,                EC_IOCTL_DC_SLV_SYNC_STATUS_GET,            PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_DC_SLV_SYNC_STATUS_GET), read access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_DC_SLV_SYNC_DEVLIMIT_SET,          PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_DC_SLV_SYNC_DEVLIMIT_SET), free access */ \
        0x00000001, rasal_readonlyaccess,   ord_emIoControl,                EC_IOCTL_DC_SLV_SYNC_DEVLIMIT_GET,          PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_DC_SLV_SYNC_DEVLIMIT_GET), read access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SB_RESTART,                        PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SB_RESTART), free access */ \
        0x00000001, rasal_readonlyaccess,   ord_emIoControl,                EC_IOCTL_SB_STATUS_GET,                     PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SB_STATUS_GET), read access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SB_SET_BUSCNF_VERIFY,              PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SB_SET_BUSCNF_VERIFY), free access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SB_SET_BUSCNF_VERIFY_PROP,         PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SB_SET_BUSCNF_VERIFY_PROP), free access */ \
        0x00000001, rasal_readonlyaccess,   ord_emIoControl,                EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO,           PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO), read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emIoControl,                EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO_EEP,       PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO_EEP), read access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SB_ENABLE,                         PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SB_ENABLE), free access */ \
        0x00000001, rasal_readonlyaccess,   ord_emIoControl,                EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO_EX,        PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO_EX), read access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SLV_ALIAS_ENABLE,                  PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SLV_ALIAS_ENABLE), free access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_SET_SLVSTAT_PERIOD,                PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_SET_SLVSTAT_PERIOD), free access */ \
        0x00000001, rasal_freeaccess,       ord_emIoControl,                EC_IOCTL_FORCE_SLVSTAT_COLLECTION,          PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_FORCE_SLVSTAT_COLLECTION), free access */ \
        0x00000001, rasal_readonlyaccess,   ord_emIoControl,                EC_IOCTL_GET_SLVSTATISTICS,                 PARAMETER_IGNORE,   0x00000000, /* emIoControl(EC_IOCTL_GET_SLVSTATISTICS), read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emGetSlaveId,               PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emGetSlaveId, read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emMbxTferCreate,            PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emMbxTferCreate, read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emMbxTferDelete,            PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emMbxTferDelete, read access */ \
        0x00000001, rasal_freeaccess,       ord_emCoeSdoDownloadReq,        PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emCoeSdoDownloadReq, free access */ \
        0x00000001, rasal_readonlyaccess,   ord_emCoeSdoUploadReq,          PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emCoeSdoUploadReq, read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emCoeGetODList,             PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emCoeGetODList, read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emCoeGetObjectDesc,         PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emCoeGetObjectDesc, read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emCoeGetEntryDesc,          PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emCoeGetEntryDesc, read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emGetSlaveProp,             PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emGetSlaveProp, read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emGetSlaveState,            PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emGetSlaveState, read access */ \
        0x00000001, rasal_noaccess,         ord_emTferSingleRawCmd,         0x00000000,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(NOP), no access */ \
        0x00000001, rasal_readonlyaccess,   ord_emTferSingleRawCmd,         0x00000001,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(APRD), read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emTferSingleRawCmd,         0x00000004,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(FPRD), read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emTferSingleRawCmd,         0x00000007,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(BRD), read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emTferSingleRawCmd,         0x0000000A,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(LRD), read access */ \
        0x00000001, rasal_freeaccess,       ord_emTferSingleRawCmd,         0x00000002,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(APWR), free access */ \
        0x00000001, rasal_freeaccess,       ord_emTferSingleRawCmd,         0x00000003,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(APRW), free access */ \
        0x00000001, rasal_freeaccess,       ord_emTferSingleRawCmd,         0x00000005,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(FPWR), free access */ \
        0x00000001, rasal_freeaccess,       ord_emTferSingleRawCmd,         0x00000006,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(FPRW), free access */ \
        0x00000001, rasal_freeaccess,       ord_emTferSingleRawCmd,         0x00000008,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(BWR), free access */ \
        0x00000001, rasal_freeaccess,       ord_emTferSingleRawCmd,         0x00000009,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(BRW), free access */ \
        0x00000001, rasal_freeaccess,       ord_emTferSingleRawCmd,         0x0000000B,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(LWR), free access */ \
        0x00000001, rasal_freeaccess,       ord_emTferSingleRawCmd,         0x0000000C,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(LRW), free access */ \
        0x00000001, rasal_freeaccess,       ord_emTferSingleRawCmd,         0x0000000D,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(ARMW), free access */ \
        0x00000001, rasal_freeaccess,       ord_emTferSingleRawCmd,         0x0000000E,                                 PARAMETER_IGNORE,   0x00000000, /* emTferSingleRawCmd(FRMW), free access */ \
        0x00000001, rasal_readonlyaccess,   ord_emGetSlaveIdAtPosition,     PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emGetSlaveIdAtPosition, read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emGetNumConfiguredSlaves,   PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emGetNumConfiguredSlaves, read access */ \
        0x00000001, rasal_freeaccess,       ord_emConfigureMaster,          PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emConfigureMaster, read access */ \
        0x00000001, rasal_freeaccess,       ord_emSetMasterState,           PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emSetMasterState, read access */ \
        0x00000001, rasal_noaccess,         ord_emQueueRawCmd,              EC_CMD_TYPE_NOP,                            PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(NOP), no access */ \
        0x00000001, rasal_readonlyaccess,   ord_emQueueRawCmd,              EC_CMD_TYPE_APRD,                           PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(APRD), read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emQueueRawCmd,              EC_CMD_TYPE_FPRD,                           PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(FPRD), read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emQueueRawCmd,              EC_CMD_TYPE_BRD,                            PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(BRD), read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emQueueRawCmd,              EC_CMD_TYPE_LRD,                            PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(LRD), read access */ \
        0x00000001, rasal_freeaccess,       ord_emQueueRawCmd,              EC_CMD_TYPE_APWR,                           PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(APWR), free access */ \
        0x00000001, rasal_freeaccess,       ord_emQueueRawCmd,              EC_CMD_TYPE_APRW,                           PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(APRW), free access */ \
        0x00000001, rasal_freeaccess,       ord_emQueueRawCmd,              EC_CMD_TYPE_FPWR,                           PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(FPWR), free access */ \
        0x00000001, rasal_freeaccess,       ord_emQueueRawCmd,              EC_CMD_TYPE_FPRW,                           PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(FPRW), free access */ \
        0x00000001, rasal_freeaccess,       ord_emQueueRawCmd,              EC_CMD_TYPE_BWR,                            PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(BWR), free access */ \
        0x00000001, rasal_freeaccess,       ord_emQueueRawCmd,              EC_CMD_TYPE_BRW,                            PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(BRW), free access */ \
        0x00000001, rasal_freeaccess,       ord_emQueueRawCmd,              EC_CMD_TYPE_LWR,                            PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(LWR), free access */ \
        0x00000001, rasal_freeaccess,       ord_emQueueRawCmd,              EC_CMD_TYPE_LRW,                            PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(LRW), free access */ \
        0x00000001, rasal_freeaccess,       ord_emQueueRawCmd,              EC_CMD_TYPE_ARMW,                           PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(ARMW), free access */ \
        0x00000001, rasal_freeaccess,       ord_emQueueRawCmd,              EC_CMD_TYPE_FRMW,                           PARAMETER_IGNORE,   0x00000000, /* emQueueRawCmd(FRMW), free access */ \
        0x00000001, rasal_readonlyaccess,   ord_emGetProcessData,           PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emGetProcessData, read access */ \
        0x00000001, rasal_freeaccess,       ord_emSetProcessData,           PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emSetProcessData, free access */ \
        0x00000001, rasal_noaccess,         ord_emGetMasterState,           PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emGetMasterState, no access */ \
        0x00000001, rasal_readonlyaccess,   ord_emFoeFileUpload,            PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emFoeFileUpload, read access */ \
        0x00000001, rasal_freeaccess,       ord_emFoeFileDownload,          PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emFoeFileDownload, free access */ \
        0x00000001, rasal_freeaccess,       ord_emCoeSdoDownload,           PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emCoeSdoDownload, free access */ \
        0x00000001, rasal_readonlyaccess,   ord_emCoeSdoUpload,             PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emCoeSdoUpload, read access */ \
        0x00000001, rasal_readonlyaccess,   ord_emGetNumConnectedSlaves,    PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emGetNumConnectedSlaves, read access */ \
        0x00000001, rasal_freeaccess,       ord_emResetSlaveController,     PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emReseSlaveController, free access */ \
        0x00000001, rasal_freeaccess,       ord_emGetSlaveInfo,             PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emGetSlaveInfo, free access */ \
        0x00000001, rasal_freeaccess,       ord_emIsSlavePresent,           PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emAoeWriteReq,              PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emAoeReadReq,               PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emAoeWrite,                 PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emAoeRead,                  PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emAoeGetSlaveNetId,         PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emGetFixedAddr,             PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emGetSlaveProcVarInfoNumOf, PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emGetSlaveProcVarInfo,      PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emFindProcVarByName,        PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emGetProcessDataBits,       PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emSetProcessDataBits,       PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emReloadSlaveEEPRom,        PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emReadSlaveEEPRom,          PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emWriteSlaveEEPRom,         PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emAssignSlaveEEPRom,        PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emSoeRead,                  PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emSoeWrite,                 PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emSoeAbortProcCmd,          PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emIsSlavePresent,           PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emIsSlavePresent,           PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        0x00000001, rasal_freeaccess,       ord_emIsSlavePresent,           PARAMETER_IGNORE,                           PARAMETER_IGNORE,   0x00000000, /* emIsSlavePresent, free access */ \
        /* end of list entry */ \
        0x00000000, 0x00000000,             0x00000000,                     0x00000000,                                 0x00000000,         0x00000000  /* eol entry class = 0 */ \
    }
#endif

/*-TYPEDEFS------------------------------------------------------------------*/

typedef struct _ATEMRAS_T_SRVPARMS
{
    ATEMRAS_T_IPADDR    oAddr;                  /**< [in]   Server Bind IP Address */
    EC_T_WORD           wPort;                  /**< [in]   Server Bind IP Port */
    EC_T_WORD           wReserved;              /**< [in]   Alignment of following members */
    EC_T_DWORD          dwCycleTime;            /**< [in]   Cycle Time of Ras Network access (acceptor, receiver)*/
                                                
    EC_T_DWORD          dwWDTOLimit;            /**< [in]   Amount of cycles without receiving commands (idles) before
                                                  *         Entering state wdexpired 
                                                  */
    EC_T_DWORD          dwReConTOLimit;         /**< [in]   Amount of cylces without receiving a reconnect request 
                                                  *         after entering state wdexpired before entering state disconnect
                                                  */
                                                
    EC_T_DWORD          dwMasterPrio;           /**< [in]   Thread Priority of RAS Master Thread */
    EC_T_DWORD          dwClientPrio;           /**< [in]   Thread Priority of RAS Client Threads */
                                                
    EC_T_DWORD          dwConcNotifyAmount;      /**< [in]   Amount of concurrently queue able Notifications */

    EC_T_DWORD          dwMbxNotifyAmount;      /**< [in]   Amount of concurrent active mailbox transfers */
    EC_T_DWORD          dwMbxUsrNotifySize;     /**< [in]   User definable Notification prolongation */
    
    EC_T_PVOID          pvNotifCtxt;            /**< [in]   Notification context returned while calling pfNotification */
    EC_PF_NOTIFY        pfNotification;         /**< [in]   Function pointer called to notify error and status 
                                                  *         information generated by Remote API Layer */
    EC_T_DWORD          dwCycErrInterval;       /**< [in]   Interval which allows cyclic Notifications */
} EC_PACKED ATEMRAS_T_SRVPARMS, *ATEMRAS_PT_SRVPARMS;

/*-PACK SETTINGS-------------------------------------------------------------*/
#if defined  __GNUC__   /* GNU */

#undef  EC_PACKED

#if (defined __RCX__)
#pragma pack ()
#endif

#elif defined _MSC_VER  /* MICROSOFT */

#undef  EC_PACKED
#pragma pack (pop)

#elif defined __MET__

#undef EC_PACKED
/*#pragma pack (pop)*/
#pragma Pop_align_members()

#elif defined(__BORLANDC__)

#pragma pack (pop)
#undef EC_PACKED	
	
#else
#endif

/*-CLASS---------------------------------------------------------------------*/

/*-FUNCTION DECLARATION------------------------------------------------------*/

/*-LOCAL VARIABLES-----------------------------------------------------------*/

/*-FORWARD DECLARATIONS------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

ATECAT_API EC_T_DWORD  emRasSrvGetVersion( EC_T_VOID );
ATECAT_API EC_T_DWORD  emRasSrvStart( ATEMRAS_T_SRVPARMS oParms, EC_T_PVOID* ppHandle);
ATECAT_API EC_T_DWORD  emRasSrvStop( EC_T_PVOID pvHandle, EC_T_DWORD dwTimeout);

#if (defined INCLUDE_RAS_TRACESUPPORT)
ATECAT_API EC_T_DWORD  emRasSrvTraceEnable(EC_T_BOOL bEnable);
#endif

ATECAT_API EC_T_DWORD   emRasSrvModifyAccessLevel(EC_T_PVOID pvHandle, EC_T_DWORD dwAccessClass, EC_T_DWORD dwAccessLevel);
ATECAT_API EC_T_DWORD   emRasSrvConfigAccessLevel(EC_T_PVOID pvHandle, EC_T_BYTE* pbyData, EC_T_DWORD dwLen);

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* __ATEMRASSRV_H__ */

/*-END OF SOURCE FILE--------------------------------------------------------*/
