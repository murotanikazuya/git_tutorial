/*-----------------------------------------------------------------------------
 * AtEthercat.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              interface to the ethercat master
 *---------------------------------------------------------------------------*/

#ifndef INC_ATETHERCAT
#define INC_ATETHERCAT 1

/*-INCLUDE-------------------------------------------------------------------*/
#include "EcType.h"
#include "EcError.h"
#include "EcInterfaceCommon.h"
#include "EcOs.h"
#include "EcVersion.h"
#include "EcLink.h"
#include "EthernetServices.h"


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

#elif defined __ghs

#define EC_PACKED __attribute__((aligned(4), packed))
#pragma pack (4)

#elif defined(__BORLANDC__)

#pragma pack (push, 4)
#define EC_PACKED	
	
#else
#error Please check pack settings for this compiler!!!
#endif


/*-DEFINES/MACROS------------------------------------------------------------*/

#define ATECAT_VALIDATION_PATTERN 0xAECAAECA
/* EtherCat specific control codes */
#define EC_IOCTL_GENERIC                            0x00000000
#define EC_IOCTL_DC                                 0x00030000
#define EC_IOCTL_SB                                 0x00050000
#define EC_IOCTL_HC                                 0x00060000
#define EC_IOCTL_DCM                                0x00070000
#define EC_IOCTL_USER                               0x00F00000  /* for user extension */
#define EC_IOCTL_PRIVATE                            0x00FF0000  /* private, internal IOCTL values */
#define EC_IOCTL_LINKLAYER                          0xCA000000
#define EC_IOCTL_LINKLAYER_MAIN                     EC_IOCTL_LINKLAYER
#define EC_IOCTL_LINKLAYER_RED                      0xCB000000
#define EC_IOCTL_PRIVATE2                           0xCC000000  /* private, internal IOCTL values */

#define EC_IOCTL_REGISTERCLIENT                     (EC_IOCTL_GENERIC |  2)
#define EC_IOCTL_UNREGISTERCLIENT                   (EC_IOCTL_GENERIC |  3)

#define EC_IOCTL_ISLINK_CONNECTED                   (EC_IOCTL_GENERIC |  6)

#define EC_IOCTL_SET_CYC_ERROR_NOTIFY_MASK          (EC_IOCTL_GENERIC |  8)

#define EC_IOCTL_LINKLAYER_DBG_MSG                  (EC_IOCTL_GENERIC | 10)

#define EC_IOCTL_RESET_SLAVE                        (EC_IOCTL_GENERIC | 13)
#define EC_IOCTL_SLAVE_LINKMESSAGES                 (EC_IOCTL_GENERIC | 14)
#define EC_IOCTL_GET_CYCLIC_CONFIG_INFO             (EC_IOCTL_GENERIC | 15)
#define EC_IOCTL_GET_LINKLAYER_MODE                 (EC_IOCTL_GENERIC | 16)
#define EC_IOCTL_IS_SLAVETOSLAVE_COMM_CONFIGURED    (EC_IOCTL_GENERIC | 17)

#define EC_IOCTL_ADD_BRD_SYNC_WINDOW_MONITORING     (EC_IOCTL_GENERIC | 20)
#define EC_IOCTL_ONLY_PROCESS_DATA_IN_IMAGE         (EC_IOCTL_GENERIC | 21)
#define EC_IOCTL_REGISTER_CYCFRAME_RX_CB            (EC_IOCTL_GENERIC | 22)
#define EC_IOCTL_SET_PD_OFFSET_COMPAT_MODE          (EC_IOCTL_GENERIC | 23)
#define EC_IOCTL_IS_MAIN_LINK_CONNECTED             (EC_IOCTL_GENERIC | 24)
#define EC_IOCTL_IS_RED_LINK_CONNECTED              (EC_IOCTL_GENERIC | 25)

    /* Memory Provider */
#define EC_IOCTL_GET_PDMEMORYSIZE                   (EC_IOCTL_GENERIC | 40)
#define EC_IOCTL_REGISTER_PDMEMORYPROVIDER          (EC_IOCTL_GENERIC | 41)

#define EC_IOCTL_FORCE_BROADCAST_DESTINATION        (EC_IOCTL_GENERIC | 42)     /* obsolete */

    /* Slave Statistics Retrieval */
#if (defined INCLUDE_SLAVE_STATISTICS)
#define EC_IOCTL_SET_SLVSTAT_PERIOD                 (EC_IOCTL_GENERIC | 43)
#define EC_IOCTL_FORCE_SLVSTAT_COLLECTION           (EC_IOCTL_GENERIC | 44)
#define EC_IOCTL_GET_SLVSTATISTICS                  (EC_IOCTL_GENERIC | 45)
#define EC_IOCTL_CLR_SLVSTATISTICS                  (EC_IOCTL_GENERIC | 46)
#endif

#define EC_IOCTL_SET_MBX_RETRYACCESS_COUNT          (EC_IOCTL_GENERIC | 47)
#define EC_IOCTL_SET_MBX_RETRYACCESS_PERIOD         (EC_IOCTL_GENERIC | 48)

#define EC_IOCTL_ALL_SLAVES_MUST_REACH_MASTER_STATE (EC_IOCTL_GENERIC | 49)

#define EC_IOCTL_SET_NOTIFICATION_CTL               (EC_IOCTL_GENERIC | 50)

#define EC_IOCTL_MASTEROD_SET_VALUE                 (EC_IOCTL_GENERIC | 51)
#define EC_IOCTL_SET_CYCFRAME_LAYOUT                (EC_IOCTL_GENERIC | 52)

#if (defined INCLUDE_DC_SUPPORT)
    /* Distributed Clocks (DC) */                   
#define EC_IOCTL_REG_DC_SLV_SYNC_NTFY               (EC_IOCTL_DC |  3)
#define EC_IOCTL_UNREG_DC_SLV_SYNC_NTFY             (EC_IOCTL_DC |  4)
#define EC_IOCTL_DC_SLV_SYNC_STATUS_GET             (EC_IOCTL_DC |  5)
#define EC_IOCTL_DC_SLV_SYNC_DEVLIMIT_SET           (EC_IOCTL_DC |  6)
#define EC_IOCTL_DC_SLV_SYNC_DEVLIMIT_GET           (EC_IOCTL_DC |  7)
#define EC_IOCTL_DC_SLV_RESERVED                    (EC_IOCTL_DC | 11)

#define EC_IOCTL_DC_SHIFT_SYSTIME                   (EC_IOCTL_DC | 16)
#define EC_IOCTL_DC_SETSYNCSTARTOFFSET              (EC_IOCTL_DC | 17)
#define EC_IOCTL_DC_RESERVED                        (EC_IOCTL_DC | 18)

    /* DC Master Sync (DCM) */
#define EC_IOCTL_DCM_REGISTER_TIMESTAMP             (EC_IOCTL_DCM |  1)
#define EC_IOCTL_DCM_UNREGISTER_TIMESTAMP           (EC_IOCTL_DCM |  2)
#define EC_IOCTL_DCM_REGISTER_STARTSO_CALLBACK      (EC_IOCTL_DCM |  3)
#endif /* INCLUDE_DC_SUPPORT */

    /* Scan Bus (SB) */
#define EC_IOCTL_SB_RESTART                         (EC_IOCTL_SB | 1)
#define EC_IOCTL_SB_STATUS_GET                      (EC_IOCTL_SB | 2)
#define EC_IOCTL_SB_SET_BUSCNF_VERIFY               (EC_IOCTL_SB | 3)
#define EC_IOCTL_SB_SET_BUSCNF_VERIFY_PROP          (EC_IOCTL_SB | 4)
#define EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO            (EC_IOCTL_SB | 5)
#define EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO_EEP        (EC_IOCTL_SB | 6)
#define EC_IOCTL_SB_ENABLE                          (EC_IOCTL_SB | 7)
#define EC_IOCTL_SB_BUSCNF_GETSLAVE_INFO_EX         (EC_IOCTL_SB | 9)
#define EC_IOCTL_SLV_ALIAS_ENABLE                   (EC_IOCTL_SB |10)
#define EC_IOCTL_SB_SET_BUSCNF_READ_PROP            (EC_IOCTL_SB |12)
#define EC_IOCTL_SB_SET_TOPOLOGY_CHANGED_DELAY      (EC_IOCTL_SB |13)
#define EC_IOCTL_SB_SET_ERROR_ON_CROSSED_LINES      (EC_IOCTL_SB |14)
#define EC_IOCTL_SB_SET_TOPOLOGY_CHANGE_AUTO_MODE   (EC_IOCTL_SB |15)
#define EC_IOCTL_SB_ACCEPT_TOPOLOGY_CHANGE          (EC_IOCTL_SB |16)

    /* Hot Connect (HC) */
#define EC_IOCTL_HC_SETMODE                         (EC_IOCTL_HC | 1)
#define EC_IOCTL_HC_GETMODE                         (EC_IOCTL_HC | 2)
#define EC_IOCTL_HC_CONFIGURETIMEOUTS               (EC_IOCTL_HC | 3)


    /* private (PRIVATE) */
#define EC_IOCTL_SET_FRAME_LOSS_SIMULATION          (EC_IOCTL_PRIVATE | 1)
#define EC_IOCTL_SET_RXFRAME_LOSS_SIMULATION        (EC_IOCTL_PRIVATE | 2)
#define EC_IOCTL_SET_TXFRAME_LOSS_SIMULATION        (EC_IOCTL_PRIVATE | 3)
#define EC_IOCTL_SET_SOFT_ASSERTIONS                (EC_IOCTL_PRIVATE | 4)
#define EC_IOCTL_SET_HARD_ASSERTIONS                (EC_IOCTL_PRIVATE | 5)

/* EtherCat CoE entry description value information bit definitions */
#define EC_COE_ENTRY_ObjAccess              0x01
#define EC_COE_ENTRY_ObjCategory            0x02
#define EC_COE_ENTRY_PdoMapping             0x04
#define EC_COE_ENTRY_UnitType               0x08
#define EC_COE_ENTRY_DefaultValue           0x10
#define EC_COE_ENTRY_MinValue               0x20
#define EC_COE_ENTRY_MaxValue               0x40

/* EtherCat CoE entry access bit definitions */
#define EC_COE_ENTRY_Access_R_PREOP         0x01
#define EC_COE_ENTRY_Access_R_SAFEOP        0x02
#define EC_COE_ENTRY_Access_R_OP            0x04
#define EC_COE_ENTRY_Access_W_PREOP         0x08
#define EC_COE_ENTRY_Access_W_SAFEOP        0x10
#define EC_COE_ENTRY_Access_W_OP            0x20

#define INVALID_SLAVE_ID                    ((EC_T_DWORD)0xFFFFFFFF)
#define INVALID_FIXED_ADDR                  ((EC_T_WORD)0x0)
#define INVALID_AUTO_INC_ADDR               ((EC_T_WORD)0x1)


#define MAX_PROCESS_VAR_NAME_LEN            ((EC_T_DWORD)72)  /* STD: Maximum length of a process variable name */
#define MAX_PROCESS_VAR_NAME_LEN_EX         ((EC_T_DWORD)128) /* EX:  Maximum length of a process variable name */
                     
#define INSTANCE_MASTER_DEFAULT             0                /* first (default) master instance used by ecatXXXXX */
#define MAX_NUMOF_MASTER_INSTANCES          10               /* maximum number of master instances */


/*-TYPEDEFS/ENUMS------------------------------------------------------------*/

struct _EC_T_NOTIFYPARMS;

/* EtherCat callback function pointer */
typedef EC_T_DWORD (*EC_PF_NOTIFY)(                 EC_T_DWORD dwCode, 
                                                    struct _EC_T_NOTIFYPARMS* pParms    );

/* EtherCAT ioctl parameters */                     
typedef struct _EC_T_IOCTLOPARMS                    
{                                                   
    EC_T_BYTE*      pbyInBuf;                       /*< in  input data buffer */
    EC_T_DWORD      dwInBufSize;                    /*< in  size of input data buffer in byte */
    EC_T_BYTE*      pbyOutBuf;                      /*< out output data buffer */
    EC_T_DWORD      dwOutBufSize;                   /*< in  size of output data buffer in byte */
    EC_T_DWORD*     pdwNumOutData;                  /*< out number of output data bytes stored in output data buffer */
}EC_PACKED EC_T_IOCTLPARMS;                                  
                                                    
/* Client register parameters */                    
typedef struct _EC_T_REGISTERPARMS                  
{                                                   
    EC_T_VOID*      pCallerData;                    /*< in  used by all callback functions */
    EC_PF_NOTIFY    pfnNotify;                      /*< in  notify callback function pointer */
}/*EC_PACKED */EC_T_REGISTERPARMS; /* Packed function pointer causes error with VxWorks DIAB compiler */
                                                    
/* Client register result */                        
typedef struct _EC_T_REGISTERRESULTS                
{                                                   
    EC_T_DWORD      dwClntId;                       /*< out client ID */
    EC_T_BYTE*      pbyPDIn;                        /*< out incoming process data shared memory */
    EC_T_DWORD      dwPDInSize;                     /*< out size of the incoming process data area */
    EC_T_BYTE*      pbyPDOut;                       /*< out outgoing process data shared memory */
    EC_T_DWORD      dwPDOutSize;                    /*< out size of the outgoing process data area */
}EC_PACKED EC_T_REGISTERRESULTS;

/* Master initialization parameters */
typedef struct _EC_T_INITMASTERPARMS
{
    EC_T_DWORD          dwValPattern;                   /*< in  Validation pattern */
    EC_T_DWORD          dwVersion;                      /*< in  Version information */
    EC_T_VOID*          pvConfigParms;                  /*< in  obsolete */
    EC_T_VOID*          pvLinkParms;                    /*< in  Link Layer parameter */
    EC_T_OS_INIT_DESC*  pOsParms;                       /*< in  OS Layer parameter */
    EC_T_DWORD          dwReserved2;                    /*< in  reserved */
    EC_T_BOOL           bUseRedundancy;                 /*< in  Use redundancy device */
    EC_T_VOID*          pvLinkParmsRed;                 /*< in  Link Layer parameter for red device */
    EC_T_DWORD          dwVlanConfig;                   /*< in  VLAN support configuration 
                                                         *      Errr.rrrr|rrrr.rrrr|rPPP.IIII|IIII.IIII 
                                                         *      E=enable (1/0), P=Prio (3Bit), I=VLAN Id (12Bit) */
    
    EC_T_WORD           wMaxBusSlaves;                  /*< in  the maximum number of pre-allocated bus slave objects */
    EC_T_WORD           wReserved;                      /*< in  reserved */
    EC_T_MASTER_CONFIG* pMasterConfig;                  /*< in  pointer to static master configuration */
    EC_T_MASTER_CONFIG  MasterConfig;                   /*< in  static master configuration */
}EC_PACKED EC_T_INITMASTERPARMS;

/* link layer debug message */                    
typedef struct _EC_T_LINKLAYER_DBG_MSG_DESC
{                                                   
    EC_T_BYTE       byEthTypeByte0;                 /*< in  Ethernet type byte 0 */
    EC_T_BYTE       byEthTypeByte1;                 /*< in  Ethernet type byte 0 */
    EC_T_WORD       wRes;
    EC_T_CHAR*      szMsg;                          /*< in  message to send to link layer */
}EC_PACKED EC_T_LINKLAYER_DBG_MSG_DESC;

#if (defined INCLUDE_DC_SUPPORT)
/* Distributed clocks */
typedef struct _EC_T_DC_SYSTIME_ADD_OFFSET_DESC
{
    EC_T_DWORD      IsNegative;                     /*< in  System time offset is Negative value */
    EC_T_DWORD      dwOffset;                       /*< in  absolute system time offset (nsec) to add */
    EC_T_DWORD      dwTimeout;                      /*< in  timeout for proceeding add offset */
}EC_PACKED EC_T_DC_SYSTIME_ADD_OFFSET_DESC;

typedef struct _EC_T_DC_SYNCSO_CB_PARM
{
    EC_T_VOID*              pvContext;          /* in       Calling environment context */
    EC_T_DWORD              dwTimeStamp;        /* in       Pre-Send Timestamp */
    EC_T_DWORD              dwPostTimeStamp;    /* in       Post-Send Timestamp */
    EC_T_DWORD              dwBusTimeLo;        /* in       Bus Time Lower 32 Bit */
    EC_T_DWORD              dwBusTimeHi;        /* in       Bus Time Upper 32 Bit */
    EC_T_DWORD              dwTimeStampResult;  /* in       Result of stamp */

    EC_T_DWORD	            dwSyncPeriodLength; /* in       Time between two consecutive SYNC0 signals of clock master slave in nsec. */

    EC_T_DWORD              dwStartSyncTimeLo;  /* out      Sync Start Time Lower 32 Bit */
    EC_T_DWORD              dwStartSyncTimeHi;  /* out      Sync Start Time Upper 32 Bit */

}EC_PACKED  EC_T_DC_SYNCSO_CB_PARM, *EC_PT_DC_SYNCSO_CB_PARM;

typedef EC_T_DWORD (*EC_T_PFSYNCSO_CB)(EC_T_DC_SYNCSO_CB_PARM* pParm);
       
typedef struct _EC_T_DC_SYNCSO_REGDESC
{
    EC_T_DC_SYNCSO_CB_PARM*     pCallbackParm;      /*< in  Callback Parameter */
    EC_T_PFSYNCSO_CB            pfnCallback;        /*< in  Callback Function */
}EC_PACKED  EC_T_DC_SYNCSO_REGDESC, *EC_PT_DC_SYNCSO_REGDESC;
 
typedef struct _EC_T_DC_CONFIGURE
{
    EC_T_DWORD                  dwClntId;           /*< in  Reserved */
    EC_T_DWORD                  dwTimeout;          /*< in  DC Timeout value in msec */
    EC_T_DWORD                  dwDevLimit;         /*< in  deviation limit value*/
    EC_T_DWORD                  dwSettleTime;       /*< in  config values for settle time*/
    EC_T_DWORD                  dwTotalBurstLength;
    EC_T_DWORD                  dwBurstBulk;
    EC_T_DWORD                  dwReserved[5];      /*< in/out 5 DWORD reserved*/
}EC_PACKED  EC_T_DC_CONFIGURE, *EC_PT_DC_CONFIGURE;

typedef struct _EC_T_DCM_BUSSHIFT_CONFIG
{
    EC_T_INT                    nCtlSetVal;         /*< in  controller set val in nsec */
    EC_T_INT                    nCtlGain;           /*< in  controller gain */
    EC_T_INT                    nCtlDriftErrorGain; /*< in  controller drift error gain */
    EC_T_INT                    nMaxValidVal;       /*< in  max valid input value */
    EC_T_BOOL                   bLogEnabled;        /*< in  EC_TRUE if logging information should be generated */
    EC_T_DWORD                  dwInSyncLimit;      /*< in  limit in nsec for InSync monitoring */
    EC_T_DWORD                  dwInSyncSettleTime; /*< in  settle time in msec for InSync monitoring */
    EC_T_DWORD                  dwReserved[2];      /*< in/out 4 DWORD reserved*/
}EC_PACKED  EC_T_DCM_BUSSHIFT_CONFIG, *EC_PT_DCM_BUSSHIFT_CONFIG;

typedef struct _EC_T_DCM_MASTERSHIFT_CONFIG
{
    EC_T_DWORD                  dwReserved[4];      /*< in/out 4 DWORD reserved*/
}EC_PACKED  EC_T_DCM_MASTERSHIFT_CONFIG, *EC_PT_DCM_MASTERSHIFT_CONFIG;
#endif /* INCLUDE_DC_SUPPORT */

/* ecatGetSlaveInfo */
typedef struct _EC_T_GET_SLAVE_INFO
{
    EC_T_DWORD                  dwScanBusStatus;    /* 0x00 */  /*< Status during last Bus Scan */
    
    EC_T_DWORD                  dwVendorId;         /* 0x01 */  /*< Vendor Identification */
    EC_T_DWORD                  dwProductCode;      /* 0x02 */  /*< Product Code */
    EC_T_DWORD                  dwRevisionNumber;   /* 0x03 */  /*< Revision Number */
    EC_T_DWORD                  dwSerialNumber;     /* 0x04 */  /*< Serial Number */
    
    EC_T_WORD                   wPortState;         /* 0x05 */  /*< out port link state (SB Instance)*/
    EC_T_WORD                   wReserved;                      /*< Res */

    EC_T_BOOL                   bDcSupport;         /* 0x06 */  /*< out slave does support DC*/
    EC_T_BOOL                   bDc64Support;       /* 0x07 */  /*< out slave does support 64Bit DC*/
    
    EC_T_WORD                   wAliasAddress;      /* 0x08 */  /*< out slave alias address*/
    EC_T_WORD                   wPhysAddress;                   /*< out slave station address*/
    
    EC_T_DWORD                  dwPdOffsIn;         /* 0x09 */  /*< out process data offset of Input Data (in Bits)*/
    EC_T_DWORD                  dwPdSizeIn;         /* 0x0A */  /*< out process data size of Input Data (in Bits)*/
    EC_T_DWORD                  dwPdOffsOut;        /* 0x0B */  /*< out process data offset of Output Data (in Bits)*/
    EC_T_DWORD                  dwPdSizeOut;        /* 0x0C */  /*< out process data size of Output Data*/
    EC_T_DWORD                  dwPdOffsIn2;        /* 0x0D */  /*< out process data offset of Input data (in Bits)*/
    EC_T_DWORD                  dwPdSizeIn2;        /* 0x0E */  /*< out process data size of Input Data (in Bits)*/
    EC_T_DWORD                  dwPdOffsOut2;       /* 0x0F */  /*< out process data offset of Output Data (in Bits)*/
    EC_T_DWORD                  dwPdSizeOut2;       /* 0x10 */  /*< out process data size of Output Data*/
    EC_T_DWORD                  dwPdOffsIn3;        /* 0x11 */  /*< out process data offset of Input Data (in Bits)*/
    EC_T_DWORD                  dwPdSizeIn3;        /* 0x12 */  /*< out process data size of Input Data (in Bits)*/
    EC_T_DWORD                  dwPdOffsOut3;       /* 0x13 */  /*< out process data offset of Output Data (in Bits)*/
    EC_T_DWORD                  dwPdSizeOut3;       /* 0x14 */  /*< out process data size of Output Data*/
    EC_T_DWORD                  dwPdOffsIn4;        /* 0x15 */  /*< out process data offset of Input Data (in Bits)*/
    EC_T_DWORD                  dwPdSizeIn4;        /* 0x16 */  /*< out process data size of Input Data (in Bits)*/
    EC_T_DWORD                  dwPdOffsOut4;       /* 0x17 */  /*< out process data offset of Output Data (in Bits)*/
    EC_T_DWORD                  dwPdSizeOut4;       /* 0x18 */  /*< out process data size of Output Data*/
    
    EC_T_WORD                   wCfgPhyAddress;     /* 0x19 */  /*< out slave configured station address*/
    EC_T_WORD                   wReserved2;                     /*< reserved */
    
    EC_T_CHAR                   abyDeviceName[ECAT_DEVICE_NAMESIZE];    
                                                    /* 0x1A */  /*< out slave name of configuration*/
    EC_T_BOOL                   bIsMailboxSlave;    /* 0x2E */  /*< out whether slave support mailboxes*/
    EC_T_DWORD                  dwMbxOutSize;       /* 0x2F */  /*< out mailbox 1 output size*/
    EC_T_DWORD                  dwMbxInSize;        /* 0x30 */  /*< out mailbox 1 input size*/
    EC_T_DWORD                  dwMbxOutSize2;      /* 0x31 */  /*< out mailbox 2 output size*/
    EC_T_DWORD                  dwMbxInSize2;       /* 0x32 */  /*< out mailbox 2 input size*/

    EC_T_DWORD                  dwErrorCode;        /* 0x33 */  /*< out last return code*/
    EC_T_DWORD                  dwSBErrorCode;      /* 0x34 */  /*< out last return value from SB*/

    EC_T_BYTE                   byPortDescriptor;   /* 0x35 */  /*< out Port Descriptor (ESC register 0x0007) */
    EC_T_BYTE                   byESCType;                      /*< out ESC Node Type */
    EC_T_WORD                   wSupportedMbxProtocols;         /*< out supported mailbox protocols: AoE, EoE, CoE, FoE, SoE */
    
    EC_T_WORD                   wAlStatusValue;     /* 0x36 */  /*< out AL Status Register Value */
    EC_T_WORD                   wAlStatusCode;                  /*< out AL Status Code */

    EC_T_BOOL                   bIsOptional;        /* 0x37 */  /*< out slave is in an optional hot connect group */
    EC_T_BOOL                   bIsPresent;         /* 0x38 */  /*< out slave is currently present on bus */

    EC_T_WORD                   wNumProcessVarsInp; /* 0x39 */  /*< out number of output process data variables*/
    EC_T_WORD                   wNumProcessVarsOutp;            /*< out number of input process data variables */

    EC_T_DWORD                  dwSlaveId;          /* 0x3A */  /*< out slave ID */
    EC_T_BOOL                   bIsHCGroupPresent;  /* 0x3B */  /*< out the hot connect group of the slave is present */

    EC_T_DWORD                  aPortSlaveIds[ESC_PORT_COUNT];  /* 0x3C */  /*< out slave IDs connected to ports */

    EC_T_DWORD                  dwSystemTimeDifference;         /* 0x40   < out System time difference (ESC register 0x092C) */

    EC_T_DWORD                  adwReserved[18];    /* 0x41 */  /*< out Reserved*/

} EC_PACKED  EC_T_GET_SLAVE_INFO, *EC_PT_GET_SLAVE_INFO;
                                                    /* Size = 0x53 * DWORD */

/* emGetSlaveInpVarInfo / emGetSlaveOutpVarInfo*/
typedef struct _EC_T_PROCESS_VAR_INFO
{
    EC_T_CHAR                   szName[MAX_PROCESS_VAR_NAME_LEN];   /**< [out] Name of the found process variable */
    EC_T_WORD                   wDataType;                          /**< [out] Data type of the found process variable */
    EC_T_WORD                   wFixedAddr;                         /**< [out] Station address of the slave that is owner of this variable */
    EC_T_INT                    nBitSize;                           /**< [out] Size in bit of the found process variable */
    EC_T_INT                    nBitOffs;                           /**< [out] Bit offset in the process data image */
    EC_T_BOOL                   bIsInputData;                       /**< [out] Determines whether the found process variable is an input variable or an output variable */
} EC_PACKED EC_T_PROCESS_VAR_INFO, *EC_PT_PROCESS_VAR_INFO;


typedef struct _EC_T_PROCESS_VAR_INFO_EX
{
    EC_T_CHAR                   szName[MAX_PROCESS_VAR_NAME_LEN_EX]; /**< [out] Name of the found process variable */
    EC_T_WORD                   wDataType;                          /**< [out] Data type of the found process variable */
    EC_T_WORD                   wFixedAddr;                         /**< [out] Station address of the slave that is owner of this variable */
    EC_T_INT                    nBitSize;                           /**< [out] Size in bit of the found process variable */
    EC_T_INT                    nBitOffs;                           /**< [out] Bit offset in the process data image */
    EC_T_BOOL                   bIsInputData;                       /**< [out] Determines whether the found process variable is an input variable or an output variable */
    EC_T_WORD                   wIndex;                             /**< [out] object index */
    EC_T_WORD                   wSubIndex;                          /**< [out] object subindex */
    EC_T_WORD                   wPdoIndex;                          /**< [out] PDO Index */
    EC_T_WORD                   wRes;
    EC_T_DWORD                  dwRes1;
    EC_T_DWORD                  dwRes2;
} EC_PACKED EC_T_PROCESS_VAR_INFO_EX, *EC_PT_PROCESS_VAR_INFO_EX;

/* EtherCAT CoE OD list type values */
typedef enum _EC_T_COE_ODLIST_TYPE
{
    eODListType_Lengths     = 0,                    /*< lengths of each list type */
    eODListType_ALL         = 1,                    /*< all objects */
    eODListType_RxPdoMap    = 2,                    /*< only pdo mappable objects */
    eODListType_TxPdoMap    = 3,                    /*< only pdo mappable objects that can be changed */
    eODListType_StoredFRepl = 4,                    /*< only stored for a device replacement objects */
    eODListType_StartupParm = 5,                    /*< only startup parameter objects */

    /* Borland C++ datatype alignment correction */
    eODListType_BCppDummy   = 0xFFFFFFFF
} EC_T_COE_ODLIST_TYPE;
#define CoeOdListTypeText(EType)                                \
    ((EType)==eODListType_Lengths?"Lengths":                    \
     ((EType)==eODListType_ALL?"All":                           \
      ((EType)==eODListType_RxPdoMap?"RxPDO":                   \
       ((EType)==eODListType_TxPdoMap?"TxPDO":                  \
        ((EType)==eODListType_StoredFRepl?"Device Replacement": \
         ((EType)==eODListType_StartupParm?"Startup Param":     \
         "Unknown"                                              \
    ))))))


/* EtherCAT CoE OD list */
typedef struct _EC_T_COE_ODLIST
{
    EC_T_COE_ODLIST_TYPE    eOdListType;            /*< type of OD list */
    EC_T_WORD               wLen;                   /*< length of the OD list */
    EC_T_WORD               wRes;
    EC_T_WORD*              pwOdList;               /*< OD list */
}EC_PACKED EC_T_COE_ODLIST;                                  
                                                    
/* EtherCAT CoE Object description */               
typedef struct _EC_T_COE_OBDESC                     
{                                                   
    EC_T_WORD   wObIndex;                           /*< object index */
    EC_T_WORD   wDataType;                          /*< data type */
    EC_T_BYTE   byObjCode;                          /*< object code defined in DS 301 (Table 37) */
    EC_T_BYTE   byObjCategory;                      /*< category: 0=optional, 1=mandatory */
    EC_T_BYTE   byMaxNumSubIndex;                   /*< maximum number of sub-indexes */
    EC_T_BYTE   byReserve;
    EC_T_WORD   wObNameLen;                         /*< length of the object name */
    EC_T_WORD   wRes;
    EC_T_CHAR*  pchObName;                          /*< object name (not a NULL terminated string!) */
}EC_PACKED EC_T_COE_OBDESC;

/* EtherCAT CoE Object Entry description */
typedef struct _EC_T_COE_ENTRYDESC
{
    EC_T_WORD   wObIndex;                           /*< object index */
    EC_T_BYTE   byObSubIndex;                       /*< object sub-index */
    EC_T_BYTE   byValueInfo;                        /*< value information, determines which of the following elements are valid */
    EC_T_WORD   wDataType;                          /*< data type */
    EC_T_WORD   wBitLen;                            /*< bit length of the object */
    EC_T_BYTE   byObAccess;                         /*< object access */
    EC_T_BYTE   byReserve[3];
    EC_T_BOOL   bRxPdoMapping;                      /*< EC_TRUE if it is PDO mappable */
    EC_T_BOOL   bTxPdoMapping;                      /*< EC_TRUE if PDO mappable object can be changed */
    EC_T_BOOL   bObCanBeUsedForBackup;              /*< Parameter may be back upped */
    EC_T_BOOL   bObCanBeUsedForSettings;            /*< Parameter may be used for Settings */
    EC_T_WORD   wUnitType;                          /*< unit type of the object, only valid if EC_COE_ENTRY_UnitType bit is set in byValueInfo */
    EC_T_WORD   wDataLen;                           /*< length of the data */
    EC_T_BYTE*  pbyData;                            /*< rest of the entry description, content see below */
    /*EC_T_BYTE   pbyDefaultValue[];                 *< default value, only valid if EC_COE_ENTRY_DefaultValue bit is set in byValueInfo */
    /*EC_T_BYTE   pbyMinValue[];                     *< minimum value, only valid if EC_COE_ENTRY_MinValue bit is set in byValueInfo */
    /*EC_T_BYTE   pbyMaxValue[];                     *< maximum value, only valid if EC_COE_ENTRY_MaxValue bit is set in byValueInfo */
    /*EC_T_BYTE   pbyDescription[];                  *< object description */
}EC_PACKED EC_T_COE_ENTRYDESC;                               
                                                    
/* EtherCAT CoE emergency request */                
#define EC_COE_EMERGENCY_DATASIZE 5                 
typedef struct _EC_T_COE_EMERGENCY                  
{                                                   
    EC_T_WORD   wErrorCode;                         /*< error code, see specification for details */
    EC_T_BYTE   byErrorRegister;                    /*< error register */
    EC_T_BYTE   abyData[EC_COE_EMERGENCY_DATASIZE]; /*< error data */
    EC_T_WORD   wStationAddress;                    /*< which slave caused the emergency request */
}EC_PACKED EC_T_COE_EMERGENCY;                               

#if (defined INCLUDE_SOE_SUPPORT)
/* EtherCAT SoE notification */
#define EC_SOE_NOTIFICATION_DATASIZE 5
typedef struct _EC_T_SOE_NOTIFICATION
{
    EC_T_WORD   wHeader;                                /*< SoE Header */
    EC_T_WORD   wIdn;                                   /*< IDN number of the appendent call */
    EC_T_BYTE   abyData[EC_SOE_NOTIFICATION_DATASIZE];  /*< error data */
    EC_T_WORD   wStationAddress;                        /*< which slave caused the emergency request */
}EC_PACKED EC_T_SOE_NOTIFICATION;

/* EtherCAT SoE emergency request */
#define EC_SOE_EMERGENCY_DATASIZE 5
typedef struct _EC_T_SOE_EMERGENCY
{
    EC_T_WORD   wHeader;                            /*< SoE Header */
    EC_T_BYTE   abyData[EC_SOE_EMERGENCY_DATASIZE]; /*< error data */
    EC_T_WORD   wStationAddress;                    /*< which slave caused the emergency request */
}EC_PACKED EC_T_SOE_EMERGENCY;
#endif /* INCLUDE_SOE_SUPPORT */

/* AoE NetID */
typedef struct _EC_T_AOE_NETID
{
    EC_T_BYTE   aby[6];
}EC_PACKED EC_T_AOE_NETID;
#define EC_T_AOE_NETID_SIZE         (6)

/* AoE mailbox response error codes */
typedef struct _EC_T_AOE_RESPONSE_CODES
{                     
    EC_T_DWORD   dwErrorCode;
    EC_T_DWORD   dwCmdResult;
    EC_T_DWORD   dwRsvd;
}EC_PACKED EC_T_AOE_CMD_RESPONSE;   
                                                    
/* EtherCAT mailbox data */                         
typedef union _EC_T_MBX_DATA                        
{                                                   
    EC_T_COE_ODLIST         CoE_ODList;                 /*< CoE object dictionary list */
    EC_T_COE_OBDESC         CoE_ObDesc;                 /*< CoE object description */
    EC_T_COE_ENTRYDESC      CoE_EntryDesc;              /*< CoE object entry description */
    EC_T_COE_EMERGENCY      CoE_Emergency;              /*< CoE emergency request */
#if (defined INCLUDE_SOE_SUPPORT)
    EC_T_SOE_NOTIFICATION   SoE_Notification;           /*< SoE notification request */
    EC_T_SOE_EMERGENCY      SoE_Emergency;              /*< SoE emergency request */
#endif
#if (defined INCLUDE_AOE_SUPPORT)
    EC_T_AOE_CMD_RESPONSE   AoE_Response;               /*< AoE response information */
#endif
#if (defined INCLUDE_FOE_SUPPORT)
    EC_T_DWORD              dwFoETransferredBytes;      /*< FoE number of transferred bytes */
#endif
}EC_PACKED EC_T_MBX_DATA;

/* EtherCAT mailbox type values */
typedef enum _EC_T_MBXTFER_TYPE
{
    eMbxTferType_COE_SDO_DOWNLOAD   = 0,              /*< CoE SDO download */
    eMbxTferType_COE_SDO_UPLOAD     = 1,              /*< CoE SDO upload */
    eMbxTferType_COE_GETODLIST      = 2,              /*< CoE Get object dictionary list */
    eMbxTferType_COE_GETOBDESC      = 3,              /*< CoE Get object description */
    eMbxTferType_COE_GETENTRYDESC   = 4,              /*< CoE Get object entry description */
    eMbxTferType_COE_EMERGENCY      = 5,              /*< CoE emergency request */
#if (defined INCLUDE_COE_PDO_SUPPORT)
    eMbxTferType_COE_RX_PDO         = 6,              /*< CoE RxPDO */
#endif                              
    eMbxTferType_FOE_FILE_UPLOAD    = 7,             /*<  FOE upload */
    eMbxTferType_FOE_FILE_DOWNLOAD  = 8,             /*<  FOE Download */
#if (defined INCLUDE_SOE_SUPPORT)
    eMbxTferType_SOE_READREQUEST    = 9,             /*<  SOE types */
    eMbxTferType_SOE_READRESPONSE   = 10,
    eMbxTferType_SOE_WRITEREQUEST   = 11,
    eMbxTferType_SOE_WRITERESPONSE  = 12,
    eMbxTferType_SOE_NOTIFICATION   = 13,
    eMbxTferType_SOE_EMERGENCY      = 14,
#endif
#if (defined INCLUDE_VOE_SUPPORT)
    eMbxTferType_VOE_MBX_READ       = 15,            /*<  VoE read */
    eMbxTferType_VOE_MBX_WRITE      = 16,            /*<  VoE write */
#endif
#if (defined INCLUDE_AOE_SUPPORT)
    eMbxTferType_AOE_READ           = 17,          /*<  AoE read  */
    eMbxTferType_AOE_WRITE          = 18,          /*<  AoE write  */
    eMbxTferType_AOE_READWRITE      = 19,          /*<  AoE read/write  */
    eMbxTferType_AOE_WRITECONTROL   = 20,          /*<  AoE write control  */
#endif

    /* Borland C++ datatype alignment correction */
    eMbxTferType_BCppDummy          = 0xFFFFFFFF
} EC_T_MBXTFER_TYPE;

/* EtherCAT mailbox transfer descriptor */
typedef struct _EC_T_MBXTFER_DESC
{
    EC_T_DWORD          dwMaxDataLen;               /*< maximum number of bytes to be transferred */
    EC_T_BYTE*          pbyMbxTferDescData;         /*< Data buffer to hold transfer data */
}EC_PACKED EC_T_MBXTFER_DESC;
                                                    
/* EtherCAT mailbox status values */                
typedef enum _EC_T_MBXTFER_STATUS                   
{
    eMbxTferStatus_Idle         = 0,                /*< Mailbox transfer object currently idle, no transfer running */
    eMbxTferStatus_Pend         = 1,                /*< Mailbox transfer object currently idle, no transfer running */
    eMbxTferStatus_TferDone     = 2,                /*< transfer successfully executed */
    eMbxTferStatus_TferReqError = 3,                /*< transfer request error occurred */

    /* Borland C++ datatype alignment correction */
    eMbxTferStatus_BCppDummy    = 0xFFFFFFFF
} EC_T_MBXTFER_STATUS;                              
#define MbxTferStatusText(EStatus)                                \
    ((EStatus)==eMbxTferStatus_Idle?"Idle":                    \
     ((EStatus)==eMbxTferStatus_Pend?"Pend":                           \
      ((EStatus)==eMbxTferStatus_TferDone?"TferDone":                   \
       ((EStatus)==eMbxTferStatus_TferReqError?"TferReqError":                  \
         "Unknown"                                              \
    ))))

/* EtherCAT mailbox transfer object */              
typedef struct _EC_T_MBXTFER                        
{                                                   
    EC_T_DWORD          dwClntId;                   /*< client ID */
    EC_T_MBXTFER_DESC   MbxTferDesc;                /*< mailbox transfer descriptor */
    EC_T_MBXTFER_TYPE   eMbxTferType;               /*< mailbox transfer type */
    EC_T_DWORD          dwDataLen;                  /*< number of bytes to be transferred */
    EC_T_BYTE*          pbyMbxTferData;             /*< pointer to mailbox data */
    EC_T_MBXTFER_STATUS eTferStatus;                /*< current transfer status */
    EC_T_DWORD          dwErrorCode;                /*< transfer error code */
    EC_T_DWORD          dwTferId;                   /*< unique transfer ID */
    EC_T_MBX_DATA       MbxData;                    /*< mailbox data */
}EC_PACKED EC_T_MBXTFER;


/* Supported EtherCAT commands for the ecatTferSingleRawCmd() function */
typedef enum _EC_T_RAWCMD
{
    eRawCmd_APRD    = EC_CMD_TYPE_APRD,     /* auto-increment physical read */
    eRawCmd_APWR    = EC_CMD_TYPE_APWR,     /* auto-increment physical write */
    eRawCmd_APRW    = EC_CMD_TYPE_APRW,     /* auto-increment physical read/write */
    eRawCmd_BRD     = EC_CMD_TYPE_BRD,      /* broadcast (wire-or) read */
    eRawCmd_BWR     = EC_CMD_TYPE_BWR,      /* broadcast write */
    eRawCmd_BRW     = EC_CMD_TYPE_BRW,      /* broadcast read/write */
    eRawCmd_LRD     = EC_CMD_TYPE_LRD,      /* logical read */
    eRawCmd_LWR     = EC_CMD_TYPE_LWR,      /* logical write */
    eRawCmd_LRW     = EC_CMD_TYPE_LRW,      /* logical read/write */
    eRawCmd_ARMW    = EC_CMD_TYPE_ARMW,     /* auto-increment physical read, multiple write */
    eRawCmd_FPRD    = EC_CMD_TYPE_FPRD,     /* fixed address physical read */
    eRawCmd_FPWR    = EC_CMD_TYPE_FPWR,     /* fixed address physical write */
    eRawCmd_FPRW    = EC_CMD_TYPE_FPRW,     /* fixed address physical read/write */

    /* Borland C++ datatype alignment correction */
    eRawCmd_BCppDummy   = 0xFFFFFFFF
} EC_T_RAWCMD;

/* Descriptor for EC_IOCTL_GET_PDMEMORYSIZE call */
typedef struct _EC_T_MEMREQ_DESC
{
    EC_T_DWORD      dwPDOutSize;                    /* Size of the output process data image */
    EC_T_DWORD      dwPDInSize;                     /* Size of the input  process data image */
}EC_PACKED EC_T_MEMREQ_DESC, *EC_PT_MEMREQ_DESC;

/* Descriptor for EC_IOCTL_UPDATE_ALL_SLAVE_STATE call */
typedef struct _EC_T_SLAVE_STATE_DESC
{
    EC_T_DWORD  dwNumSlaves;        /* [in]	 size of the awCurrDevState[] and awReqDevState[] arrays */
    EC_T_BOOL   bUseFixedAddr;      /* [in]	 EC_TRUE if station address shall be used, EC_FALSE if auto-increment address shall be used */
    EC_T_DWORD  dwNumRespSlaves;    /* [out] number of slaves responding */
    EC_T_DWORD* adwSlaveId;         /* [out] slave id */
    EC_T_BOOL*  abSlaveResponded;   /* [out] EC_TRUE if slave responded */
    EC_T_WORD*  awCurrDevState;     /* [out] current slave device state */
    EC_T_WORD*  awReqDevState;      /* [out] requested slave device state */
}EC_PACKED EC_T_SLAVE_STATE_DESC;

/* Descriptor for EC_IOCTL_HC_CONFIGURETIMEOUTS call */
typedef struct _EC_T_HC_CONFIGURETIMEOUTS_DESC
{
    EC_T_DWORD  dwDetectionTimeout;         /* [in] Timeout for Group detection */
    EC_T_DWORD  dwTopologyChangeTimeout;    /* [in] Topology Change Timeout (usually larger than dwDetectionTimeout) */
    EC_T_DWORD  dwRsvd[2];
}EC_PACKED EC_T_HC_CONFIGURETIMEOUTS_DESC;

/* EtherCAT user controlled execution job */
typedef enum _EC_T_USER_JOB
{
    eUsrJob_ProcessAllRxFrames              = 1,        /*< polling mode: receive frames and process all received data */
    eUsrJob_SendAllCycFrames                = 2,        /*< send all cyclic frames */
    eUsrJob_RunMcSm                         = 3,        /*< obsolete */
    eUsrJob_MasterTimer                     = 4,        /*< run master timer routine (generic management) */
    eUsrJob_FlushQueuedCmds                 = 5,        /*< obsolete */
    eUsrJob_SendAcycFrames                  = 6,        /*< send acyclic frames (number of frames can be limited by master parameter) */
    eUsrJob_SendCycFramesByTaskId           = 7,        /*< send cyclic frames related to a specific task id (TaskId entry in the XML file) */
    eUsrJob_MasterTimerMinimal              = 8,        /*< run minimal master timer routine: no state change possible  */
                                            
    eUsrJob_StampSendAllCycFrames           = 22,       /*< send all cyclic frames (stamped) */
    eUsrJob_StampSendCycFramesByTaskId      = 27,       /*< send cyclic frames related to a specific task id (TaskId entry in the XML file) */

    /* Borland C++ datatype alignment correction */
    eUsrJob_BCppDummy                       = 0xFFFFFFFF
} EC_T_USER_JOB;


typedef EC_T_VOID (*EC_PF_CYCFRAME_RECV)(EC_T_VOID*);
typedef struct _EC_T_CYCFRAME_RX_CBDESC
{
    EC_T_VOID*                  pCallbackParm;      /*< in  Callback Parameter */
    EC_PF_CYCFRAME_RECV         pfnCallback;        /*< in  Callback Function */
}EC_PACKED  EC_T_CYCFRAME_RX_CBDESC;

typedef enum _EC_T_CYCFRAME_LAYOUT
{
    eCycFrameLayout_STANDARD = 0,    /* Frame layout according ENI with command add/reordering, no relationship to PD */
    eCycFrameLayout_DYNAMIC  = 1,    /* Frame layout is dynamically modified to send as less as possible cyclic commands in as less as possible frames */
    eCycFrameLayout_FIXED    = 2,    /* Frame layout strictly match ENI, frame buffers and PD area overlapped */
    /* Borland C++ datatype alignment correction */
    eCycFrameLayout_BCppDummy   = 0xFFFFFFFF
} EC_T_CYCFRAME_LAYOUT;

/* pass through server states */
typedef enum _EC_PTS_STATE
{
    ePtsStateNone                  = 0x0000,
    ePtsStateNotRunning            = 0x0001,
    ePtsStateRunningDisabled       = 0x0002,
    ePtsStateRunningEnabled        = 0x0003,

    /* Borland C++ datatype alignment correction */
    ePtsStateDummy                 = 0xFFFFFFFF
} EC_PTS_STATE;


/* Start parameter for the Pass-Through-Server */
typedef struct _EC_T_PTS_SRV_START_PARMS
{
    EC_T_IPADDR oIpAddr;
    EC_T_DWORD  dwPtsThreadPriority;
    EC_T_WORD   wPort;
    EC_T_WORD   wReserved;
}EC_PACKED  EC_T_PTS_SRV_START_PARMS;

typedef struct _EC_T_NOTIFICATION_CTL_PARMS
{
    EC_T_DWORD dwCode; /*< Notification code */
    EC_T_WORD  wMode;  /*< Control Mode: 0=Off, 1=On, 2=Auto (if applicable) */
} EC_PACKED EC_T_NOTIFICATION_CTL_PARMS;

typedef struct _EC_T_MASTEROD_OBJECT_PARMS
{
    EC_T_WORD  wIndex;      /*< Object's index, e.g. 0x1018 */
    EC_T_BYTE  bySubindex;  /*< Object's sub-index, e.g. 1 */
    EC_T_BYTE  byReserved;
    EC_T_BYTE* pbyData;     /*< Pointer to object's data to be written */
    EC_T_DWORD dwLength;    /*< Data length to be written */
    EC_T_DWORD dwReserved;
} EC_PACKED EC_T_MASTEROD_OBJECT_PARMS;

/* Start parameters for ADS Adapter */
typedef struct _EC_T_ADS_ADAPTER_START_PARMS
{
    EC_T_DWORD      dwSize;             /* structure size */
    EC_T_DWORD      dwThreadPriority;
    EC_T_AOE_NETID  targetNetID;
    EC_T_WORD       targetPort;
    EC_T_WORD       wReserved;
} EC_PACKED EC_T_ADS_ADAPTER_START_PARMS;

/* Frame logging */
#define EC_LOG_FRAME_FLAG_MASTERSTATE_MASK 0xFFFF      /* Bit 0 to 15 */
#define EC_LOG_FRAME_FLAG_ACYC_FRAME       (1<<16)     /* Bit 16: 0=cyclic frame, 1=acyclic frame */
#define EC_LOG_FRAME_FLAG_DBG_FRAME        (1<<17)     /* Bit 17: 0=EtherCAT frame, 1=debug frame */
#define EC_LOG_FRAME_FLAG_RED_FRAME        (1<<18)     /* Bit 18: 0=main frame, 1=red frame */
#define EC_LOG_FRAME_FLAG_RX_FRAME         (1<<19)     /* Bit 19: 0=TX frame, 1=RX frame */

typedef EC_T_VOID (*EC_T_PFLOGFRAME_CB)(EC_T_VOID* pvContext, EC_T_DWORD dwLogFlags, EC_T_DWORD dwFrameSize, EC_T_BYTE* pbyFrame);

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

#elif defined __ghs

#undef EC_PACKED
#pragma pack()
		
#elif defined(__BORLANDC__)

#pragma pack (pop) 
#undef EC_PACKED
	
#else
#endif

/*-FORWARD DECLARATIONS------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

ATECAT_API EC_T_CHAR*  EcErrorText(                 EC_T_DWORD              dwError         );
ATECAT_API const EC_T_CHAR* EcStateString(          EC_T_STATE              eMasterState    );
                                                    
ATECAT_API EC_T_CHAR*  ecatGetText(                 EC_T_WORD               wTextId         );
ATECAT_API EC_T_CHAR*  ecatGetNotifyText(           EC_T_DWORD              dwNotifyCode    );
ATECAT_API EC_T_DWORD  ecatInitMaster(              EC_T_INITMASTERPARMS*   pParms          );
ATECAT_API EC_T_DWORD  ecatDeinitMaster(            EC_T_VOID                               );
                                                    
ATECAT_API EC_T_DWORD  ecatStart(                   EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  ecatStop(                    EC_T_DWORD              dwTimeout       );
                                                    
ATECAT_API EC_T_DWORD  ecatIoControl(               EC_T_DWORD              dwCode, 
                                                    EC_T_IOCTLPARMS*        pParms          );
                                                    
ATECAT_API EC_T_DWORD  ecatGetSlaveId(              EC_T_WORD               wStationAddress );

ATECAT_API EC_T_DWORD  ecatGetSlaveFixedAddr(       EC_T_DWORD              dwSlaveId,              
                                                    EC_T_WORD*              pwFixedAddr     );

ATECAT_API EC_T_DWORD  ecatGetSlaveIdAtPosition(    EC_T_WORD               wAutoIncAddress );
ATECAT_API EC_T_BOOL   ecatGetSlaveProp(            EC_T_DWORD              dwSlaveId, 
                                                    EC_T_SLAVE_PROP*        pSlaveProp      );
ATECAT_API EC_T_DWORD  ecatGetSlavePortState(       EC_T_DWORD              dwSlaveId,
                                                    EC_T_WORD*              pwPortState     );
ATECAT_API EC_T_DWORD  ecatGetSlaveState(           EC_T_DWORD              dwSlaveId,
                                                    EC_T_WORD*              pwCurrDevState, 
                                                    EC_T_WORD*              pwReqDevState, 
                                                    EC_T_DWORD              dwReserved      );
ATECAT_API EC_T_DWORD  ecatSetSlaveState(           EC_T_DWORD              dwSlaveId, 
                                                    EC_T_WORD               wNewReqDevState, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  ecatTferSingleRawCmd(        EC_T_DWORD              dwSlaveId, 
                                                    EC_T_BYTE               byCmd, 
                                                    EC_T_DWORD              dwMemoryAddress, 
                                                    EC_T_VOID*              pvData, 
                                                    EC_T_WORD               wLen, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  ecatReadSlaveRegister(       EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wRegisterOffset, 
                                                    EC_T_VOID*              pvData, 
                                                    EC_T_WORD               wLen, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  ecatWriteSlaveRegister(      EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wRegisterOffset, 
                                                    EC_T_VOID*              pvData, 
                                                    EC_T_WORD               wLen, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  ecatQueueRawCmd(             EC_T_DWORD              dwSlaveId,
                                                    EC_T_WORD               wInvokeId,
                                                    EC_T_BYTE               byCmd,
                                                    EC_T_DWORD              dwMemoryAddress,
                                                    EC_T_VOID*              pvData,
                                                    EC_T_WORD               wLen            );
ATECAT_API EC_T_DWORD  ecatClntQueueRawCmd(         EC_T_DWORD              dwClntId,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_WORD               wInvokeId,
                                                    EC_T_BYTE               byCmd,
                                                    EC_T_DWORD              dwMemoryAddress,
                                                    EC_T_VOID*              pvData,
                                                    EC_T_WORD               wLen            );
ATECAT_API EC_T_DWORD  ecatGetNumConfiguredSlaves(  EC_T_VOID                               );


ATECAT_API EC_T_MBXTFER* ecatMbxTferCreate(         EC_T_MBXTFER_DESC*      pMbxTferDesc    );
ATECAT_API EC_T_DWORD    ecatMbxTferAbort(          EC_T_MBXTFER*           pMbxTferDesc    );
ATECAT_API EC_T_VOID     ecatMbxTferDelete(         EC_T_MBXTFER*           pMbxTfer        );
                                                    
ATECAT_API EC_T_DWORD  ecatCoeSdoDownloadReq(       EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_WORD               wObIndex, 
                                                    EC_T_BYTE               byObSubIndex, 
                                                    EC_T_DWORD              dwTimeout,
                                                    EC_T_DWORD              dwFlags         );
ATECAT_API EC_T_DWORD   ecatCoeSdoDownload(         EC_T_DWORD              dwSlaveId
                                                   ,EC_T_WORD               wObIndex
                                                   ,EC_T_BYTE               byObSubIndex
                                                   ,EC_T_BYTE*              pbyData
                                                   ,EC_T_DWORD              dwDataLen
                                                   ,EC_T_DWORD              dwTimeout
                                                   ,EC_T_DWORD              dwFlags         );
ATECAT_API EC_T_DWORD  ecatCoeSdoUploadReq(         EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_WORD               wObIndex, 
                                                    EC_T_BYTE               byObSubIndex, 
                                                    EC_T_DWORD              dwTimeout,
                                                    EC_T_DWORD              dwFlags         );
ATECAT_API EC_T_DWORD  ecatCoeSdoUpload(            EC_T_DWORD              dwSlaveId
                                                   ,EC_T_WORD               wObIndex
                                                   ,EC_T_BYTE               byObSubIndex
                                                   ,EC_T_BYTE*              pbyData
                                                   ,EC_T_DWORD              dwDataLen
                                                   ,EC_T_DWORD*             pdwOutDataLen
                                                   ,EC_T_DWORD              dwTimeout
                                                   ,EC_T_DWORD              dwFlags         );
ATECAT_API EC_T_DWORD  ecatCoeGetODList(            EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_COE_ODLIST_TYPE    eListType, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  ecatCoeGetObjectDesc(        EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_WORD               wObIndex, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  ecatCoeGetEntryDesc(         EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_WORD               wObIndex, 
                                                    EC_T_BYTE               byObSubIndex, 
                                                    EC_T_BYTE               byValueInfo,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD ecatCoeRxPdoTfer(             EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_DWORD              dwNumber,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  ecatFoeFileUpload(           EC_T_DWORD              dwSlaveId,
                                                    EC_T_CHAR*              achFileName,
                                                    EC_T_DWORD              dwFileNameLen,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,
                                                    EC_T_DWORD*             pdwOutDataLen,
                                                    EC_T_DWORD              dwPassWd,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  ecatFoeFileDownload(         EC_T_DWORD              dwSlaveId,
                                                    EC_T_CHAR*              achFileName,
                                                    EC_T_DWORD              dwFileNameLen,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,
                                                    EC_T_DWORD              dwPassWd,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  ecatFoeUploadReq(            EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_CHAR*              achFileName,
                                                    EC_T_DWORD              dwFileNameLen,
                                                    EC_T_DWORD              dwPassWd,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  ecatFoeDownloadReq(          EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_CHAR*              achFileName,
                                                    EC_T_DWORD              dwFileNameLen,
                                                    EC_T_DWORD              dwPassWd,
                                                    EC_T_DWORD              dwTimeout       );

#ifdef INCLUDE_SOE_SUPPORT
ATECAT_API  EC_T_DWORD ecatSoeWrite(                EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE               byDriveNo,
                                                    EC_T_BYTE*              pbyElementFlags,
                                                    EC_T_WORD               wIDN,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,
                                                    EC_T_DWORD              dwTimeout        );
ATECAT_API  EC_T_DWORD ecatSoeRead(                 EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE               byDriveNo,
                                                    EC_T_BYTE*              pbyElementFlags,
                                                    EC_T_WORD               wIDN, 
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,  
                                                    EC_T_DWORD*             pdwOutDataLen,
                                                    EC_T_DWORD              dwTimeout        );
ATECAT_API  EC_T_DWORD ecatSoeAbortProcCmd(         EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE               byDriveNo,
                                                    EC_T_BYTE*              pbyElementFlags,
                                                    EC_T_WORD               wIDN,
                                                    EC_T_DWORD              dwTimeout        );
ATECAT_API  EC_T_DWORD ecatSoeWriteReq(             EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE               byDriveNo,
                                                    EC_T_BYTE*              pbyElementFlags,
                                                    EC_T_WORD               wIDN,
                                                    EC_T_DWORD              dwTimeout        );
ATECAT_API  EC_T_DWORD ecatSoeReadReq(              EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE               byDriveNo,
                                                    EC_T_BYTE*              pbyElementFlags,
                                                    EC_T_WORD               wIDN, 
                                                    EC_T_DWORD              dwTimeout        );
#endif

#ifdef INCLUDE_AOE_SUPPORT
ATECAT_API EC_T_DWORD ecatAoeGetSlaveNetId(         EC_T_DWORD              dwSlaveId, 
                                                    EC_T_AOE_NETID*         poAoeNetId       );

ATECAT_API EC_T_DWORD  ecatAoeRead(                 EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_DWORD              dwIndexGroup,
                                                    EC_T_DWORD              dwIndexOffset,
                                                    EC_T_DWORD              dwDataLength,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD*             pdwDataOutLen,
                                                    EC_T_DWORD*             pdwErrorCode,
                                                    EC_T_DWORD*             pdwCmdResult,
                                                    EC_T_DWORD              dwTimeout        );

ATECAT_API EC_T_DWORD  ecatAoeReadReq(              EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,           
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_DWORD              dwIndexGroup,
                                                    EC_T_DWORD              dwIndexOffset,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  ecatAoeWrite(                EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_DWORD              dwIndexGroup,
                                                    EC_T_DWORD              dwIndexOffset,
                                                    EC_T_DWORD              dwDataLength,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD*             pdwErrorCode,
                                                    EC_T_DWORD*             pdwCmdResult,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD ecatAoeWriteReq(              EC_T_MBXTFER*           pMbxTfer,         
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,           
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_DWORD              dwIndexGroup,
                                                    EC_T_DWORD              dwIndexOffset,
                                                    EC_T_DWORD              dwTimeout       ); 

ATECAT_API EC_T_DWORD  ecatAoeReadWrite(            EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_DWORD              dwIndexGroup,
                                                    EC_T_DWORD              dwIndexOffset,
                                                    EC_T_DWORD              dwReadDataLength,
                                                    EC_T_DWORD              dwWriteDataLength,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD*             pdwDataOutLen,
                                                    EC_T_DWORD*             pdwErrorCode,
                                                    EC_T_DWORD*             pdwCmdResult,
                                                    EC_T_DWORD              dwTimeout        );

ATECAT_API EC_T_DWORD  ecatAoeWriteControl(         EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_WORD               wAoEState,
                                                    EC_T_WORD               wDeviceState,
                                                    EC_T_DWORD              dwDataLength,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD*             pdwErrorCode,
                                                    EC_T_DWORD*             pdwCmdResult,
                                                    EC_T_DWORD              dwTimeout       );
#endif

ATECAT_API EC_T_DWORD  ecatVoeRead(                 EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,
                                                    EC_T_DWORD*             pdwOutDataLen,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  ecatVoeWrite(                EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  ecatVoeWriteReq(             EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_DWORD              dwTimeout       );
                                         
                                                    
ATECAT_API EC_T_DWORD  ecatEoeRegisterEndpoint(     EC_T_CHAR*              szEoEDrvIdent,
                                                    EC_T_VOID*              pLinkDrvDesc    ); 

ATECAT_API EC_T_DWORD  ecatConfigureMaster(         EC_T_CNF_TYPE           eCnfType,
                                                    EC_T_PBYTE              pbyCnfData, 
                                                    EC_T_DWORD              dwCnfDataLen    );

ATECAT_API EC_T_DWORD  ecatSetMasterState(          EC_T_DWORD              dwTimeout,
                                                    EC_T_STATE              eReqState       );
ATECAT_API EC_T_STATE  ecatGetMasterState(          EC_T_VOID                               );

ATECAT_API EC_T_DWORD  ecatExecJob(                 EC_T_USER_JOB           eUserJob,
                                                    EC_T_PVOID              pvParam         );

ATECAT_API EC_T_DWORD  ecatGetProcessData(          EC_T_BOOL               bOutputData,
                                                    EC_T_DWORD              dwOffset,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwLength,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  ecatSetProcessData(          EC_T_BOOL               bOutputData,
                                                    EC_T_DWORD              dwOffset,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwLength,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD ecatSetProcessDataBits(       EC_T_BOOL               bOutputData,    
                                                    EC_T_DWORD              dwBitOffsetPd,  
                                                    EC_T_BYTE*              pbyDataSrc,     
                                                    EC_T_DWORD              dwBitLengthSrc, 
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD ecatGetProcessDataBits(       EC_T_BOOL               bOutputData,    
                                                    EC_T_DWORD              dwBitOffsetPd,  
                                                    EC_T_BYTE*              pbyDataDst,     
                                                    EC_T_DWORD              dwBitLengthDst, 
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  ecatGetNumConnectedSlaves(   EC_T_VOID                               );

#ifdef INCLUDE_RED_DEVICE
ATECAT_API EC_T_DWORD  ecatGetNumConnectedSlavesMain(EC_T_VOID                               );
ATECAT_API EC_T_DWORD  ecatGetNumConnectedSlavesRed(EC_T_VOID                               );
#endif

ATECAT_API EC_T_DWORD  ecatReadSlaveEEPRom(         EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wEEPRomStartOffset,
                                                    EC_T_WORD*              pwReadData,
                                                    EC_T_DWORD              dwReadLen,
                                                    EC_T_DWORD*             pdwNumOutData,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD ecatWriteSlaveEEPRom(         EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wEEPRomStartOffset,
                                                    EC_T_WORD*              pwWriteData,
                                                    EC_T_DWORD              dwWriteLen,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD ecatReloadSlaveEEPRom(        EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD ecatResetSlaveController(     EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD ecatAssignSlaveEEPRom(        EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_BOOL               bSlavePDIAccessEnable,
                                                    EC_T_BOOL               bForceAssign,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD ecatActiveSlaveEEPRom(        EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_BOOL*              pbSlavePDIAccessActive,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD ecatHCAcceptTopoChange(       EC_T_VOID                               );
ATECAT_API EC_T_DWORD ecatHCGetNumGroupMembers(     EC_T_DWORD              dwGroupIndex    );
ATECAT_API EC_T_DWORD ecatHCGetSlaveIdsOfGroup(     EC_T_DWORD              dwGroupIndex,
                                                    EC_T_DWORD*             adwSlaveId,
                                                    EC_T_DWORD              dwMaxNumSlaveIds );
ATECAT_API EC_T_DWORD ecatSetSlavePortState(        EC_T_DWORD              dwSlaveID,
                                                    EC_T_WORD               wPort,
                                                    EC_T_BOOL               bClose,
                                                    EC_T_BOOL               bForce,
                                                    EC_T_DWORD              dwTimeout       );
                                                                                            
ATECAT_API EC_T_DWORD  ecatSlaveSerializeMbxTfers(  EC_T_DWORD              dwSlaveId       );

ATECAT_API EC_T_DWORD  ecatSlaveParallelMbxTfers(   EC_T_DWORD              dwSlaveId       );

ATECAT_API EC_T_DWORD  ecatRegisterClient(          EC_PF_NOTIFY            pfnNotify,
                                                    EC_T_VOID*              pCallerData,
                                                    EC_T_REGISTERRESULTS*   pRegResults     );
ATECAT_API EC_T_DWORD  ecatUnregisterClient(        EC_T_DWORD              dwClntId        ); 

#if (defined INCLUDE_DC_SUPPORT)
ATECAT_API EC_T_DWORD ecatDcEnable(                 EC_T_VOID                               );
ATECAT_API EC_T_DWORD ecatDcDisable(                EC_T_VOID                               );
ATECAT_API EC_T_DWORD ecatDcConfigure(              EC_T_DC_CONFIGURE*      pDcConfigure    );
ATECAT_API EC_T_DWORD ecatDcContDelayCompEnable(    EC_T_VOID                               );
ATECAT_API EC_T_DWORD ecatDcContDelayCompDisable(   EC_T_VOID                               );

ATECAT_API EC_T_DWORD ecatDcmConfigureBusShift(     EC_T_DCM_BUSSHIFT_CONFIG* pDcmConfig,
                                                    EC_T_DWORD              dwInSyncTimeout );
ATECAT_API EC_T_DWORD ecatDcmConfigureMasterShift(  EC_T_DCM_MASTERSHIFT_CONFIG* pDcmConfig,
                                                    EC_T_DWORD              dwInSyncTimeout );
ATECAT_API EC_T_DWORD ecatDcmGetStatus(             EC_T_DWORD*             pdwErrorCode,
                                                    EC_T_INT*               pnDiffCur,
                                                    EC_T_INT*               pnDiffAvg,
                                                    EC_T_INT*               pnDiffMax       );
ATECAT_API EC_T_DWORD ecatDcmResetStatus(           EC_T_VOID                               );
ATECAT_API EC_T_DWORD ecatDcmGetLog(                EC_T_CHAR**             pszLog          );
ATECAT_API EC_T_DWORD ecatDcmShowStatus(            EC_T_VOID                               );
#endif /* INCLUDE_DC_SUPPORT */

ATECAT_API EC_T_DWORD  ecatGetSlaveInfo(            EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_GET_SLAVE_INFO*    pGetSlaveInfo   );
ATECAT_API EC_T_DWORD  ecatGetCfgSlaveInfo(         EC_T_BOOL               bStationAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_CFG_SLAVE_INFO*    pSlaveInfo      );
ATECAT_API EC_T_DWORD  ecatGetBusSlaveInfo(         EC_T_BOOL               bStationAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_BUS_SLAVE_INFO*    pSlaveInfo      );

ATECAT_API EC_T_DWORD ecatGetSlaveInpVarInfoNumOf(  EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD*              pwSlaveInpVarInfoNumOf);

ATECAT_API EC_T_DWORD ecatGetSlaveOutpVarInfoNumOf( EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD*              pwSlaveOutpVarInfoNumOf );

ATECAT_API EC_T_DWORD ecatGetSlaveInpVarInfo(       EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wNumOfVarsToRead,
                                                    EC_T_PROCESS_VAR_INFO*  pSlaveProcVarInfoEntries,
                                                    EC_T_WORD*              pwReadEntries    );


ATECAT_API EC_T_DWORD ecatGetSlaveOutpVarInfo(      EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wNumOfVarsToRead,
                                                    EC_T_PROCESS_VAR_INFO*  pSlaveProcVarInfoEntries,
                                                    EC_T_WORD*              pwReadEntries );


ATECAT_API EC_T_DWORD ecatFindOutpVarByName(        EC_T_CHAR*              szVariableName,                 
                                                    EC_T_PROCESS_VAR_INFO*  pProcessVarInfoEntry);


ATECAT_API EC_T_DWORD ecatFindInpVarByName (        EC_T_CHAR*              szVariableName,
                                                    EC_T_PROCESS_VAR_INFO*  pSlaveOutpVarInfo );

ATECAT_API EC_T_DWORD  ecatEthDbgMsg(               EC_T_BYTE               byEthTypeByte0,
                                                    EC_T_BYTE               byEthTypeByte1,
                                                    EC_T_CHAR*              szMsg           );

ATECAT_API EC_T_DWORD   ecatBlockNode(              EC_T_SB_MISMATCH_DESC*  pMisMatch,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD   ecatOpenBlockedPorts(       EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD   ecatForceTopologyChange(    EC_T_VOID                               );
ATECAT_API EC_T_DWORD   ecatIsSlavePresent(         EC_T_DWORD              dwSlaveId,
                                                    EC_T_BOOL*              pbPresence      );


ATECAT_API EC_T_DWORD ecatPassThroughSrvStart(      EC_T_PTS_SRV_START_PARMS* poPtsStartParams,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD ecatPassThroughSrvStop(       EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD ecatPassThroughSrvEnable(     EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD ecatPassThroughSrvDisable(    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_PTS_STATE ecatPassThroughSrvGetStatus(EC_T_VOID                               );

ATECAT_API EC_T_DWORD ecatAdsAdapterStart(          EC_T_ADS_ADAPTER_START_PARMS* poStartParams,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD ecatAdsAdapterStop(           EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_BYTE* ecatGetProcessImageInputPtr(  EC_T_VOID                               );

ATECAT_API EC_T_BYTE* ecatGetProcessImageOutputPtr( EC_T_VOID                               );

ATECAT_API EC_T_DWORD ecatConfigLoad(               EC_T_CNF_TYPE           eCnfType,
                                                    EC_T_PBYTE              pbyCnfData, 
                                                    EC_T_DWORD              dwCnfDataLen    );
ATECAT_API EC_T_DWORD ecatConfigExcludeSlave(       EC_T_WORD               wPhysAddress    );
ATECAT_API EC_T_DWORD ecatConfigIncludeSlave(       EC_T_WORD               wPhysAddress    );
ATECAT_API EC_T_DWORD ecatConfigApply(              EC_T_VOID                               );

ATECAT_API EC_T_DWORD ecatNotifyApp(                EC_T_DWORD                dwCode,
                                                    EC_T_NOTIFYPARMS*         pParms);

ATECAT_API EC_T_DWORD ecatLogFrameEnable(           EC_T_PFLOGFRAME_CB      pvLogFrameCallBack,
                                                    EC_T_VOID*              pvContext);
ATECAT_API EC_T_DWORD ecatLogFrameDisable(          EC_T_VOID);

ATECAT_API EC_T_DWORD ecatGetSrcMacAddress(         ETHERNET_ADDRESS* pMacSrc);
ATECAT_API EC_T_DWORD ecatSetLicenseKey(            EC_T_CHAR*        pszLicenseKey);

ATECAT_API EC_T_DWORD ecatGetMasterProperties(      EC_T_DWORD*               pdwMasterPropNumEntries,
                                                    EC_T_MASTER_PROP_DESC**   ppaMasterPropEntries);

ATECAT_API EC_T_VOID ecatPerfMeasInit( EC_T_TSC_MEAS_DESC* pTscMeasDesc, EC_T_UINT64 dwlFreqSet, EC_T_DWORD dwNumMeas, EC_T_FNMESSAGE pfnMessage);
ATECAT_API EC_T_VOID ecatPerfMeasDeinit( EC_T_TSC_MEAS_DESC* pTscMeasDesc );
ATECAT_API EC_T_VOID ecatPerfMeasEnable( EC_T_TSC_MEAS_DESC* pTscMeasDesc );
ATECAT_API EC_T_VOID ecatPerfMeasDisable( EC_T_TSC_MEAS_DESC* pTscMeasDesc );
ATECAT_API EC_T_VOID ecatPerfMeasStart( EC_T_TSC_MEAS_DESC* pTscMeasDesc, EC_T_DWORD dwIndex );
ATECAT_API EC_T_TSC_TIME* ecatPerfMeasEnd( EC_T_TSC_MEAS_DESC* pTscMeasDesc, EC_T_DWORD dwIndex );
ATECAT_API EC_T_VOID ecatPerfMeasReset( EC_T_TSC_MEAS_DESC* pTscMeasDesc, EC_T_DWORD dwIndex );
ATECAT_API EC_T_VOID ecatPerfMeasShow( EC_T_TSC_MEAS_DESC* pTscMeasDesc, EC_T_DWORD dwIndex, EC_T_CHAR** aszMeasInfo );

/* Multi instance API functions */

ATECAT_API EC_T_DWORD  emInitMaster(                EC_T_DWORD              dwInstanceID,
                                                    EC_T_INITMASTERPARMS*   pParms          );
ATECAT_API EC_T_DWORD  emDeinitMaster(              EC_T_DWORD              dwInstanceID);

ATECAT_API EC_T_DWORD  emStart(                     EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  emStop(                      EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  emIoControl(                 EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwCode, 
                                                    EC_T_IOCTLPARMS*        pParms          );

ATECAT_API EC_T_DWORD  emGetSlaveId(                EC_T_DWORD              dwInstanceID,
                                                    EC_T_WORD               wStationAddress );

ATECAT_API EC_T_DWORD  emGetSlaveFixedAddr(         EC_T_DWORD              dwInstanceID,           
                                                    EC_T_DWORD              dwSlaveId,              
                                                    EC_T_WORD*              pwFixedAddr     );

ATECAT_API EC_T_DWORD  emGetSlaveIdAtPosition(      EC_T_DWORD              dwInstanceID,
                                                    EC_T_WORD               wAutoIncAddress );
ATECAT_API EC_T_BOOL   emGetSlaveProp(              EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_SLAVE_PROP*        pSlaveProp      );
ATECAT_API EC_T_DWORD  emGetSlavePortState(         EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_WORD*              pwPortState     );
ATECAT_API EC_T_DWORD  emGetSlaveState(             EC_T_DWORD                  dwInstanceID,
                                                    EC_T_DWORD                  dwSlaveId, 
                                                    EC_T_WORD*                  pwCurrDevState, 
                                                    EC_T_WORD*                  pwReqDevState, 
                                                    EC_T_DWORD /* DEPRECATED */ dwTimeout       );
ATECAT_API EC_T_DWORD  emSetSlaveState(             EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_WORD               wNewReqDevState, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  emTferSingleRawCmd(          EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_BYTE               byCmd, 
                                                    EC_T_DWORD              dwMemoryAddress, 
                                                    EC_T_VOID*              pvData, 
                                                    EC_T_WORD               wLen, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  emReadSlaveRegister(         EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress, 
                                                    EC_T_WORD               wRegisterOffset, 
                                                    EC_T_VOID*              pvData, 
                                                    EC_T_WORD               wLen, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  emWriteSlaveRegister(        EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wRegisterOffset, 
                                                    EC_T_VOID*              pvData, 
                                                    EC_T_WORD               wLen, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  emQueueRawCmd(               EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_WORD               wInvokeId,
                                                    EC_T_BYTE               byCmd,
                                                    EC_T_DWORD              dwMemoryAddress,
                                                    EC_T_VOID*              pvData,
                                                    EC_T_WORD               wLen            );
ATECAT_API EC_T_DWORD  emClntQueueRawCmd(           EC_T_DWORD              dwClntId,
                                                    EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_WORD               wInvokeId,
                                                    EC_T_BYTE               byCmd,
                                                    EC_T_DWORD              dwMemoryAddress,
                                                    EC_T_VOID*              pvData,
                                                    EC_T_WORD               wLen            );
ATECAT_API EC_T_DWORD  emGetNumConfiguredSlaves(    EC_T_DWORD              dwInstanceID);


ATECAT_API EC_T_MBXTFER* emMbxTferCreate(           EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER_DESC*      pMbxTferDesc    );
ATECAT_API EC_T_DWORD    emMbxTferAbort(            EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer        );
ATECAT_API EC_T_VOID     emMbxTferDelete(           EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer        );

ATECAT_API EC_T_DWORD  emCoeSdoDownloadReq(         EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_WORD               wObIndex, 
                                                    EC_T_BYTE               byObSubIndex, 
                                                    EC_T_DWORD              dwTimeout,
                                                    EC_T_DWORD              dwFlags         );
ATECAT_API EC_T_DWORD   emCoeSdoDownload(           EC_T_DWORD              dwInstanceID
                                                   ,EC_T_DWORD              dwSlaveId
                                                   ,EC_T_WORD               wObIndex
                                                   ,EC_T_BYTE               byObSubIndex
                                                   ,EC_T_BYTE*              pbyData
                                                   ,EC_T_DWORD              dwDataLen
                                                   ,EC_T_DWORD              dwTimeout
                                                   ,EC_T_DWORD              dwFlags         );

ATECAT_API EC_T_DWORD  emCoeSdoUploadReq(           EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_WORD               wObIndex, 
                                                    EC_T_BYTE               byObSubIndex, 
                                                    EC_T_DWORD              dwTimeout,
                                                    EC_T_DWORD              dwFlags         );
ATECAT_API EC_T_DWORD  emCoeSdoUpload(              EC_T_DWORD              dwInstanceID
                                                   ,EC_T_DWORD              dwSlaveId
                                                   ,EC_T_WORD               wObIndex
                                                   ,EC_T_BYTE               byObSubIndex
                                                   ,EC_T_BYTE*              pbyData
                                                   ,EC_T_DWORD              dwDataLen
                                                   ,EC_T_DWORD*             pdwOutDataLen
                                                   ,EC_T_DWORD              dwTimeout
                                                   ,EC_T_DWORD              dwFlags         );

ATECAT_API EC_T_DWORD  emCoeGetODList(              EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_COE_ODLIST_TYPE    eListType, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  emCoeGetObjectDesc(          EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_WORD               wObIndex, 
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  emCoeGetEntryDesc(           EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer, 
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_WORD               wObIndex, 
                                                    EC_T_BYTE               byObSubIndex, 
                                                    EC_T_BYTE               byValueInfo,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD emCoeRxPdoTfer(               EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_DWORD              dwNumber,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  emFoeFileUpload(             EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_CHAR*              achFileName,
                                                    EC_T_DWORD              dwFileNameLen,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,
                                                    EC_T_DWORD*             pdwOutDataLen,
                                                    EC_T_DWORD              dwPassWd,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  emFoeFileDownload(           EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_CHAR*              achFileName,
                                                    EC_T_DWORD              dwFileNameLen,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,
                                                    EC_T_DWORD              dwPassWd,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  emFoeUploadReq(              EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_CHAR*              achFileName,
                                                    EC_T_DWORD              dwFileNameLen,
                                                    EC_T_DWORD              dwPassWd,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  emFoeDownloadReq(            EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_CHAR*              achFileName,
                                                    EC_T_DWORD              dwFileNameLen,
                                                    EC_T_DWORD              dwPassWd,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  emEoeRegisterEndpoint(       EC_T_DWORD              dwInstanceID,
                                                    EC_T_CHAR*              szEoEDrvIdent,
                                                    EC_T_VOID*              pLinkDrvDesc    ); 

#ifdef INCLUDE_SOE_SUPPORT
ATECAT_API EC_T_DWORD  emSoeWrite(                  EC_T_DWORD              dwInstanceID,            
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE               byDriveNo,
                                                    EC_T_BYTE*              pbyElementFlags,
                                                    EC_T_WORD               wIDN,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,
                                                    EC_T_DWORD              dwTimeout        );
ATECAT_API EC_T_DWORD  emSoeRead(                   EC_T_DWORD              dwInstanceID,               
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE               byDriveNo,
                                                    EC_T_BYTE*              pbyElementFlags,
                                                    EC_T_WORD               wIDN, 
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,  
                                                    EC_T_DWORD*             pdwOutDataLen,
                                                    EC_T_DWORD              dwTimeout        );
ATECAT_API EC_T_DWORD  emSoeAbortProcCmd(           EC_T_DWORD              dwInstanceID,  
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE               byDriveNo,
                                                    EC_T_BYTE*              pbyElementFlags,
                                                    EC_T_WORD               wIDN,
                                                    EC_T_DWORD              dwTimeout        );
ATECAT_API EC_T_DWORD  emSoeWriteReq(               EC_T_DWORD              dwInstanceID,               
                                                    EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE               byDriveNo,
                                                    EC_T_BYTE*              pbyElementFlags,
                                                    EC_T_WORD               wIDN,
                                                    EC_T_DWORD              dwTimeout        );
ATECAT_API EC_T_DWORD  emSoeReadReq(                EC_T_DWORD              dwInstanceID,               
                                                    EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE               byDriveNo,
                                                    EC_T_BYTE*              pbyElementFlags,
                                                    EC_T_WORD               wIDN,
                                                    EC_T_DWORD              dwTimeout        );
#endif

#ifdef INCLUDE_AOE_SUPPORT

ATECAT_API EC_T_DWORD emAoeGetSlaveNetId(           EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId, 
                                                    EC_T_AOE_NETID*         poAoeNetId       );


ATECAT_API EC_T_DWORD  emAoeRead(                   EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,           
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_DWORD              dwIndexGroup,
                                                    EC_T_DWORD              dwIndexOffset,
                                                    EC_T_DWORD              dwDataLength,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD*             pdwDataOutLen,
                                                    EC_T_DWORD*             pdwErrorCode,
                                                    EC_T_DWORD*             pdwCmdResult,
                                                    EC_T_DWORD              dwTimeout        );

ATECAT_API EC_T_DWORD  emAoeReadReq(                EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,           
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_DWORD              dwIndexGroup,
                                                    EC_T_DWORD              dwIndexOffset,
                                                    EC_T_DWORD              dwTimeout       );


ATECAT_API EC_T_DWORD  emAoeWrite(                  EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,           
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_DWORD              dwIndexGroup,
                                                    EC_T_DWORD              dwIndexOffset,
                                                    EC_T_DWORD              dwDataLength,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD*             pdwErrorCode,
                                                    EC_T_DWORD*             pdwCmdResult,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD emAoeWriteReq(                EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer,         
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,           
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_DWORD              dwIndexGroup,
                                                    EC_T_DWORD              dwIndexOffset,
                                                    EC_T_DWORD              dwTimeout       ); 


ATECAT_API EC_T_DWORD  emAoeReadWrite(              EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,           
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_DWORD              dwIndexGroup,
                                                    EC_T_DWORD              dwIndexOffset,
                                                    EC_T_DWORD              dwReadDataLength,
                                                    EC_T_DWORD              dwWriteDataLength,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD*             pdwDataOutLen,
                                                    EC_T_DWORD*             pdwErrorCode,
                                                    EC_T_DWORD*             pdwCmdResult,
                                                    EC_T_DWORD              dwTimeout        );


ATECAT_API EC_T_DWORD  emAoeWriteControl(           EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_AOE_NETID*         poTargetNetId,           
                                                    EC_T_WORD               wTargetPort,
                                                    EC_T_WORD               wAoEState,
                                                    EC_T_WORD               wDeviceState,
                                                    EC_T_DWORD              dwDataLength,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD*             pdwErrorCode,
                                                    EC_T_DWORD*             pdwCmdResult,
                                                    EC_T_DWORD              dwTimeout       );
#endif


ATECAT_API EC_T_DWORD  emVoeRead(                 EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,
                                                    EC_T_DWORD*             pdwOutDataLen,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  emVoeWrite(               EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwDataLen,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  emVoeWriteReq(            EC_T_DWORD              dwInstanceID,
                                                    EC_T_MBXTFER*           pMbxTfer,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_DWORD              dwTimeout       );


ATECAT_API EC_T_DWORD  emConfigureMaster(           EC_T_DWORD              dwInstanceID,
                                                    EC_T_CNF_TYPE           eCnfType,
                                                    EC_T_PBYTE              pbyCnfData, 
                                                    EC_T_DWORD              dwCnfDataLen    );

ATECAT_API EC_T_DWORD  emSetMasterState(            EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwTimeout,
                                                    EC_T_STATE              eReqState       );
ATECAT_API EC_T_STATE  emGetMasterState(            EC_T_DWORD              dwInstanceID    );

ATECAT_API EC_T_DWORD  emExecJob(                   EC_T_DWORD              dwInstanceID,
                                                    EC_T_USER_JOB           eUserJob,
                                                    EC_T_PVOID              pvParam         );

ATECAT_API EC_T_DWORD  emGetProcessData(            EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bOutputData,
                                                    EC_T_DWORD              dwOffset,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwLength,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  emSetProcessData(            EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bOutputData,
                                                    EC_T_DWORD              dwOffset,
                                                    EC_T_BYTE*              pbyData,
                                                    EC_T_DWORD              dwLength,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD emSetProcessDataBits(         EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bOutputData,    
                                                    EC_T_DWORD              dwBitOffsetPd,  
                                                    EC_T_BYTE*              pbyDataSrc,     
                                                    EC_T_DWORD              dwBitLengthSrc, 
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD emGetProcessDataBits(         EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bOutputData,    
                                                    EC_T_DWORD              dwBitOffsetPd,  
                                                    EC_T_BYTE*              pbyDataDst,     
                                                    EC_T_DWORD              dwBitLengthDst, 
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  emGetNumConnectedSlaves(     EC_T_DWORD              dwInstanceID    );
#ifdef INCLUDE_RED_DEVICE
ATECAT_API EC_T_DWORD  emGetNumConnectedSlavesMain( EC_T_DWORD              dwInstanceID    );
ATECAT_API EC_T_DWORD  emGetNumConnectedSlavesRed(  EC_T_DWORD              dwInstanceID    );
#endif

ATECAT_API EC_T_DWORD  emReadSlaveEEPRom(           EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wEEPRomStartOffset,
                                                    EC_T_WORD*              pwReadData,
                                                    EC_T_DWORD              dwReadLen,
                                                    EC_T_DWORD*             pdwNumOutData,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD  emWriteSlaveEEPRom(          EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wEEPRomStartOffset,
                                                    EC_T_WORD*              pwWriteData,
                                                    EC_T_DWORD              dwWriteLen,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD emReloadSlaveEEPRom(          EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD emResetSlaveController(       EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD emAssignSlaveEEPRom(          EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_BOOL               bSlavePDIAccessEnable,
                                                    EC_T_BOOL               bForceAssign,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD emActiveSlaveEEPRom(          EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddressing,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_BOOL*              pbSlavePDIAccessActive,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD emHCAcceptTopoChange(         EC_T_DWORD              dwInstanceID    );
ATECAT_API EC_T_DWORD emHCGetNumGroupMembers(       EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwGroupIndex    );
ATECAT_API EC_T_DWORD emHCGetSlaveIdsOfGroup(       EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwGroupIndex,
                                                    EC_T_DWORD*             adwSlaveId,
                                                    EC_T_DWORD              dwMaxNumSlaveIds );
ATECAT_API EC_T_DWORD emSetSlavePortState(          EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveID,
                                                    EC_T_WORD               wPort,
                                                    EC_T_BOOL               bClose,
                                                    EC_T_BOOL               bForce,
                                                    EC_T_DWORD              dwTimeout       );

ATECAT_API EC_T_DWORD  emSlaveSerializeMbxTfers(    EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId       );

ATECAT_API EC_T_DWORD  emSlaveParallelMbxTfers(     EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId       );

ATECAT_API EC_T_DWORD emRegisterClient(             EC_T_DWORD              dwInstanceID,    
                                                    EC_PF_NOTIFY            pfnNotify,
                                                    EC_T_VOID*              pCallerData,     
                                                    EC_T_REGISTERRESULTS*   pRegResults     );
ATECAT_API EC_T_DWORD emUnregisterClient(           EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwClntId        );                              
#if (defined INCLUDE_DC_SUPPORT)
ATECAT_API EC_T_DWORD emDcEnable(                   EC_T_DWORD              dwInstanceID  );
ATECAT_API EC_T_DWORD emDcDisable(                  EC_T_DWORD              dwInstanceID  );
ATECAT_API EC_T_DWORD emDcConfigure(                EC_T_DWORD              dwInstanceID,
                                                    EC_T_DC_CONFIGURE*      pDcConfigure  );
ATECAT_API EC_T_DWORD emDcContDelayCompEnable(      EC_T_DWORD              dwInstanceID  );
ATECAT_API EC_T_DWORD emDcContDelayCompDisable(     EC_T_DWORD              dwInstanceID  );

ATECAT_API EC_T_DWORD emDcmConfigureBusShift(       EC_T_DWORD              dwInstanceID,
                                                    EC_T_DCM_BUSSHIFT_CONFIG* pDcmConfig,
                                                    EC_T_DWORD              dwInSyncTimeout );
ATECAT_API EC_T_DWORD emDcmConfigureMasterShift(    EC_T_DWORD              dwInstanceID,
                                                    EC_T_DCM_MASTERSHIFT_CONFIG* pDcmConfig,
                                                    EC_T_DWORD              dwInSyncTimeout );
ATECAT_API EC_T_DWORD emDcmGetStatus(               EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD*             pdwErrorCode,
                                                    EC_T_INT*               pnDiffCur,
                                                    EC_T_INT*               pnDiffAvg,
                                                    EC_T_INT*               pnDiffMax     );
ATECAT_API EC_T_DWORD emDcmResetStatus(             EC_T_DWORD              dwInstanceID  );
ATECAT_API EC_T_DWORD emDcmGetLog(                  EC_T_DWORD              dwInstanceID,
                                                    EC_T_CHAR**             pszLog        );
ATECAT_API EC_T_DWORD emDcmShowStatus(              EC_T_DWORD              dwInstanceID  );
#endif /* INCLUDE_DC_SUPPORT */

ATECAT_API EC_T_DWORD emGetSlaveInfo(               EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_GET_SLAVE_INFO*    pGetSlaveInfo   );
ATECAT_API EC_T_DWORD emGetCfgSlaveInfo(            EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bStationAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_CFG_SLAVE_INFO*    pSlaveInfo      );
ATECAT_API EC_T_DWORD emGetBusSlaveInfo(            EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bStationAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_BUS_SLAVE_INFO*    pSlaveInfo      );

ATECAT_API EC_T_DWORD emGetSlaveInpVarInfoNumOf(    EC_T_DWORD              dwInstanceID,     
                                                    EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD*              pwSlaveInpVarInfoNumOf);

ATECAT_API EC_T_DWORD emGetSlaveOutpVarInfoNumOf(   EC_T_DWORD              dwInstanceID,     
                                                    EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD*              pwSlaveOutpVarInfoNumOf );

ATECAT_API EC_T_DWORD emGetSlaveInpVarInfo(         EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wNumOfVarsToRead,
                                                    EC_T_PROCESS_VAR_INFO*  pSlaveProcVarInfoEntries,
                                                    EC_T_WORD*              pwReadEntries    );

ATECAT_API EC_T_DWORD emGetSlaveInpVarInfoEx(       EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wNumOfVarsToRead,
                                                    EC_T_PROCESS_VAR_INFO_EX*  pSlaveProcVarInfoEntries,
                                                    EC_T_WORD*              pwReadEntries    );

ATECAT_API EC_T_DWORD emGetSlaveOutpVarInfo(        EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wNumOfVarsToRead,
                                                    EC_T_PROCESS_VAR_INFO*  pSlaveProcVarInfoEntries,
                                                    EC_T_WORD*              pwReadEntries    );

ATECAT_API EC_T_DWORD emGetSlaveOutpVarInfoEx(      EC_T_DWORD              dwInstanceID,
                                                    EC_T_BOOL               bFixedAddress,
                                                    EC_T_WORD               wSlaveAddress,
                                                    EC_T_WORD               wNumOfVarsToRead,
                                                    EC_T_PROCESS_VAR_INFO_EX*  pSlaveProcVarInfoEntries,
                                                    EC_T_WORD*              pwReadEntries    );


ATECAT_API EC_T_DWORD emFindOutpVarByName  (        EC_T_DWORD              dwInstanceID,
                                                    EC_T_CHAR*              szVariableName,
                                                    EC_T_PROCESS_VAR_INFO*  pSlaveOutpVarInfo );

ATECAT_API EC_T_DWORD emFindOutpVarByNameEx(        EC_T_DWORD              dwInstanceID,
                                                    EC_T_CHAR*              szVariableName,
                                                    EC_T_PROCESS_VAR_INFO_EX*  pSlaveOutpVarInfo );


ATECAT_API EC_T_DWORD emFindInpVarByName  (         EC_T_DWORD              dwInstanceID,
                                                    EC_T_CHAR*              szVariableName,
                                                    EC_T_PROCESS_VAR_INFO*  pSlaveOutpVarInfo );

ATECAT_API EC_T_DWORD emFindInpVarByNameEx(         EC_T_DWORD              dwInstanceID,
                                                    EC_T_CHAR*              szVariableName,
                                                    EC_T_PROCESS_VAR_INFO_EX*  pSlaveOutpVarInfo );

ATECAT_API EC_T_DWORD emEthDbgMsg(                  EC_T_DWORD              dwInstanceID,
                                                    EC_T_BYTE               byEthTypeByte0,
                                                    EC_T_BYTE               byEthTypeByte1,
                                                    EC_T_CHAR*              szMsg           );
ATECAT_API EC_T_DWORD emBlockNode(                  EC_T_DWORD              dwInstanceID,
                                                    EC_T_SB_MISMATCH_DESC*  pMisMatch,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD emOpenBlockedPorts(           EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwTimeout       );
ATECAT_API EC_T_DWORD emForceTopologyChange(        EC_T_DWORD              dwInstanceID    );
ATECAT_API EC_T_DWORD emIsSlavePresent(             EC_T_DWORD              dwInstanceID,
                                                    EC_T_DWORD              dwSlaveId,
                                                    EC_T_BOOL*              pbPresence      );

ATECAT_API EC_PTS_STATE emPassThroughSrvGetStatus(  EC_T_DWORD              dwInstanceID    );

ATECAT_API EC_T_DWORD emPassThroughSrvStart(        EC_T_DWORD                dwInstanceID,
                                                    EC_T_PTS_SRV_START_PARMS* poPtsStartParams,
                                                    EC_T_DWORD                dwTimeout     );

ATECAT_API EC_T_DWORD emPassThroughSrvStop(         EC_T_DWORD                dwInstanceID,
                                                    EC_T_DWORD                dwTimeout     );

ATECAT_API EC_T_DWORD emPassThroughSrvEnable(       EC_T_DWORD                dwInstanceID,  
                                                    EC_T_DWORD                dwTimeout     );

ATECAT_API EC_T_DWORD emPassThroughSrvDisable(      EC_T_DWORD                dwInstanceID,  
                                                    EC_T_DWORD                dwTimeout     );

ATECAT_API EC_T_DWORD emAdsAdapterStart(            EC_T_DWORD                dwInstanceID,
                                                    EC_T_ADS_ADAPTER_START_PARMS* poStartParams,
                                                    EC_T_DWORD                dwTimeout     );

ATECAT_API EC_T_DWORD emAdsAdapterStop(             EC_T_DWORD                dwInstanceID,
                                                    EC_T_DWORD                dwTimeout     );

ATECAT_API EC_T_BYTE* emGetProcessImageInputPtr(    EC_T_DWORD                dwInstanceID  );

ATECAT_API EC_T_BYTE* emGetProcessImageOutputPtr(   EC_T_DWORD                dwInstanceID  );

ATECAT_API EC_T_DWORD emConfigLoad(                 EC_T_DWORD                dwInstanceID,
                                                    EC_T_CNF_TYPE             eCnfType,
                                                    EC_T_PBYTE                pbyCnfData, 
                                                    EC_T_DWORD                dwCnfDataLen  );
ATECAT_API EC_T_DWORD emConfigExcludeSlave(         EC_T_DWORD                dwInstanceID,
                                                    EC_T_WORD                 wPhysAddress  );
ATECAT_API EC_T_DWORD emConfigIncludeSlave(         EC_T_DWORD                dwInstanceID,
                                                    EC_T_WORD                 wPhysAddress  );
ATECAT_API EC_T_DWORD emConfigApply(                EC_T_DWORD                dwInstanceID  );

ATECAT_API EC_T_DWORD emNotifyApp(                  EC_T_DWORD                dwInstanceID,
                                                    EC_T_DWORD                dwCode,
                                                    EC_T_NOTIFYPARMS*         pParms);
ATECAT_API EC_T_DWORD emLogFrameEnable(             EC_T_DWORD                dwInstanceID,
                                                    EC_T_PFLOGFRAME_CB        pvLogFrameCallBack,
                                                    EC_T_VOID*                pvContext);
ATECAT_API EC_T_DWORD emLogFrameDisable(            EC_T_DWORD                dwInstanceID);

ATECAT_API EC_T_DWORD emGetSrcMacAddress(           EC_T_DWORD                dwInstanceID,
                                                    ETHERNET_ADDRESS*         pMacSrc);

ATECAT_API EC_T_DWORD emSetLicenseKey(              EC_T_DWORD                dwInstanceID,
                                                    EC_T_CHAR*                pszLicenseKey);
ATECAT_API EC_T_DWORD emGetMasterProperties(        EC_T_DWORD                dwInstanceID,
                                                    EC_T_DWORD*               pdwMasterPropNumEntries,
                                                    EC_T_MASTER_PROP_DESC**   ppaMasterPropEntries);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* INC_ATETHERCAT */

/*-END OF SOURCE FILE--------------------------------------------------------*/
