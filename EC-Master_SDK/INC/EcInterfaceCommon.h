/*-----------------------------------------------------------------------------
 * EcInterfaceCommon.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master common definitions for interfaces
 *---------------------------------------------------------------------------*/

#ifndef INC_ECINTERFACECOMMON
#define INC_ECINTERFACECOMMON

#if !(defined __ECESCREG_H__)
#include "EcEscReg.h"
#endif

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
#define ECAT_DEVICE_NAMESIZE                80

#define SAFE_STRCPY(szTargetString, szSourceString, nMaxSize)   \
    {                                                           \
    EC_T_INT nTmp = nMaxSize;                                   \
        if (nTmp != 0)                                          \
        {                                                       \
            OsMemset((szTargetString),0,(nMaxSize));            \
            OsStrncpy((szTargetString),(szSourceString),(nMaxSize)-1); \
        }                                                       \
    }
#define MAX_ERRINFO_STRLEN  8      /* maximum length of error notification info strings */
#define MAX_SHORT_STRLEN    20      /* maximum length of a short info string */
#define MAX_STD_STRLEN      80      /* maximum length of a standard info string */

#define MAX_FILE_NAME_SIZE                  0x0020      /* Max file name length for FOE Download/Upload */

#define MASTER_SLAVE_ID     ((EC_T_DWORD)0x10000)

/* textual description of EtherCat commands */
#define EcatCmdShortText(byCmd)                     \
    ((byCmd)==EC_CMD_TYPE_NOP?"NOP":                \
     ((byCmd)==EC_CMD_TYPE_APRD?"APRD":             \
      ((byCmd)==EC_CMD_TYPE_APWR?"APWR":            \
       ((byCmd)==EC_CMD_TYPE_APRW?"APRW":           \
        ((byCmd)==EC_CMD_TYPE_FPRD?"FPRD":          \
         ((byCmd)==EC_CMD_TYPE_FPWR?"FPWR":         \
          ((byCmd)==EC_CMD_TYPE_FPRW?"FPRW":        \
           ((byCmd)==EC_CMD_TYPE_BRD?"BRD":         \
            ((byCmd)==EC_CMD_TYPE_BWR?"BWR":        \
             ((byCmd)==EC_CMD_TYPE_BRW?"BRW":       \
              ((byCmd)==EC_CMD_TYPE_LRD?"LRD":      \
               ((byCmd)==EC_CMD_TYPE_LWR?"LWR":     \
                ((byCmd)==EC_CMD_TYPE_LRW?"LRW":    \
                 ((byCmd)==EC_CMD_TYPE_ARMW?"ARMW": \
                  "INVALID ECAT CMD VALUE!!!"       \
    ))))))))))))))

/* EtherCAT state */
typedef enum _EC_T_STATE
{
    eEcatState_UNKNOWN  = 0,                        /*< unknown */
    eEcatState_INIT     = 1,                        /*< init */
    eEcatState_PREOP    = 2,                        /*< pre-operational */
    eEcatState_SAFEOP   = 4,                        /*< safe operational */
    eEcatState_OP       = 8,                        /*< operational */

    eEcatState_BOOTSTRAP = 3,                       /*< BootStrap */
    
    /* Borland C++ datatype alignment correction */
    eEcatState_BCppDummy   = 0xFFFFFFFF
} EC_T_STATE;

typedef enum _EC_T_STATEMACH_RES
{
    eStateMachRes_Pending   =   1,
    eStateMachRes_Done      =   2,
    eStateMachRes_Error     =   4,

    eStateMachRes_BCppDummy =   0xFFFFFFFF
}   EC_T_STATEMACH_RES;

typedef enum _EC_T_LINKMODE
{        
    EcLinkMode_UNDEFINED = 0,
    EcLinkMode_INTERRUPT = 1,
    EcLinkMode_POLLING   = 2,
    EcLinkMode_RANDOM    = 3,      /* for debug purposes only! */

    /* Borland C++ datatype alignment correction */
    EcLinkMode_BCppDummy   = 0xFFFFFFFF
} EC_T_LINKMODE;

typedef enum EC_MAILBOX_FLAG
{
    EC_MAILBOX_FLAG_SDO_COMPLETE			= 1,
    
    /* Borland C++ datatype alignment correction */
    EC_MAILBOX_FLAG_BCppDummy                               = 0xFFFFFFFF
} EC_MAILBOX_FLAG;

/* EtherCat specific notify codes */
#define EC_NOTIFY_GENERIC                       0x00000000
#define EC_NOTIFY_ERROR                         0x00010000

#define EC_NOTIFY_MBOXRCV                       0x00020000
#define EC_SZTXT_NOTIFY_MBOXRCV                 "EC_NOTIFY_MBOXRCV"

#define EC_NOTIFY_SCANBUS                       0x00030000
#define EC_NOTIFY_HOTCONNECT                    0x00040000        

#define EC_NOTIFY_APP                           0x00080000                  /* application specific codes used by ecatNotifyApp() */
#define EC_NOTIFY_APP_MAX_CODE                  0x0000FFFF                  /* max number app notify codes */

#define ATEMRAS_NOTIFY_GENERIC					0x00100000
#define ATEMRAS_NOTIFY_ERROR					0x00110000

#define EC_NOTIFY_STATECHANGED                  (EC_NOTIFY_GENERIC | 1)     /* EtherCAT operational state change */
#define EC_SZTXT_NOTIFY_STATECHANGED			"EC_NOTIFY_STATECHANGED"

#define EC_NOTIFY_ETH_LINK_CONNECTED            (EC_NOTIFY_GENERIC | 2)     /* Ethernet link (cable) connected */
#define EC_SZTXT_NOTIFY_ETH_LINK_CONNECTED		"EC_NOTIFY_ETH_LINK_CONNECTED"

#define EC_NOTIFY_SB_STATUS                     (EC_NOTIFY_GENERIC | 3)     /* ScanBus finished */ 
#define EC_SZTXT_NOTIFY_SB_STATUS		        "EC_NOTIFY_SB_STATUS"

#define EC_NOTIFY_DC_STATUS                     (EC_NOTIFY_GENERIC | 4)     /* Distributed clocks initialized */
#define EC_SZTXT_NOTIFY_DC_STATUS			    "EC_NOTIFY_DC_STATUS"

/* Distributed clocks (DC) */
#define EC_NOTIFY_DC_SLV_SYNC                   (EC_NOTIFY_GENERIC | 5)     /* DC Slave Synchronization deviation notification */
#define EC_SZTXT_NOTIFY_DC_SLV_SYNC		        "EC_NOTIFY_DC_SLV_SYNC"

/* Distributed Clocks Latching (DCL) */
#define EC_NOTIFY_DCL_STATUS                    (EC_NOTIFY_GENERIC | 8)     /* DCL initialized */
#define EC_SZTXT_NOTIFY_DCL_STATUS		        "EC_NOTIFY_DCL_STATUS"

/* Distributed clocks master sync (DCM) */
#define EC_NOTIFY_DCM_SYNC                      (EC_NOTIFY_GENERIC | 9)     /* DCM InSync */
#define EC_SZTXT_NOTIFY_DCM_SYNC	            "EC_NOTIFY_DCM_SYNC"

#define EC_NOTIFY_SLAVE_STATECHANGED            (EC_NOTIFY_GENERIC | 21)    /* Slave finished successfully a state transition. */
#define EC_SZTXT_NOTIFY_SLAVE_STATECHANGED      "EC_NOTIFY_SLAVE_STATECHANGED"

#define EC_NOTIFY_RAWCMD_DONE                   (EC_NOTIFY_GENERIC | 100)   /* Queue Raw Command Response Notification */
#define EC_SZTXT_NOTIFY_RAWCMD_DONE		        "EC_NOTIFY_RAWCMD_DONE"

#ifdef INCLUDE_COE_PDO_SUPPORT
#define EC_NOTIFY_COE_TX_PDO                    (EC_NOTIFY_MBOXRCV | 1)     /* TxPDO notification */
#define EC_SZTXT_NOTIFY_COE_TX_PDO			    "EC_NOTIFY_COE_TX_PDO"
#endif

/* errors */
#define EC_NOTIFY_CYCCMD_WKC_ERROR              (EC_NOTIFY_ERROR | 1)       /* cyclic command: working counter error */
#define EC_SZTXT_NOTIFY_CYCCMD_WKC_ERROR		"EC_NOTIFY_CYCCMD_WKC_ERROR"

#define EC_NOTIFY_MASTER_INITCMD_WKC_ERROR      (EC_NOTIFY_ERROR | 2)       /* master init command: working counter error */
#define EC_SZTXT_NOTIFY_MASTER_INITCMD_WKC_ERROR "EC_NOTIFY_MASTER_INITCMD_WKC_ERROR"

#define EC_NOTIFY_SLAVE_INITCMD_WKC_ERROR       (EC_NOTIFY_ERROR | 3)       /* slave init command: working counter error */
#define EC_SZTXT_NOTIFY_SLAVE_INITCMD_WKC_ERROR	"EC_NOTIFY_SLAVE_INITCMD_WKC_ERROR"

#define EC_NOTIFY_EOE_MBXRCV_WKC_ERROR          (EC_NOTIFY_ERROR | 4)       /* EOE mbox receive: working counter error (deprecated) */
#define EC_SZTXT_NOTIFY_EOE_MBXRCV_WKC_ERROR	"EC_NOTIFY_EOE_MBXRCV_WKC_ERROR"

#define EC_NOTIFY_COE_MBXRCV_WKC_ERROR          (EC_NOTIFY_ERROR | 5)       /* COE mbox receive: working counter error (deprecated) */
#define EC_SZTXT_NOTIFY_COE_MBXRCV_WKC_ERROR	"EC_NOTIFY_COE_MBXRCV_WKC_ERROR"

#define EC_NOTIFY_FOE_MBXRCV_WKC_ERROR          (EC_NOTIFY_ERROR | 6)       /* FOE mbox receive: working counter error (deprecated) */
#define EC_SZTXT_NOTIFY_FOE_MBXRCV_WKC_ERROR	"EC_NOTIFY_FOE_MBXRCV_WKC_ERROR"

#define EC_NOTIFY_EOE_MBXSND_WKC_ERROR          (EC_NOTIFY_ERROR | 7)       /* EOE mbox send: working counter error */
#define EC_SZTXT_NOTIFY_EOE_MBXSND_WKC_ERROR	"EC_NOTIFY_EOE_MBXSND_WKC_ERROR"

#define EC_NOTIFY_COE_MBXSND_WKC_ERROR          (EC_NOTIFY_ERROR | 8)       /* COE mbox send: working counter error */
#define EC_SZTXT_NOTIFY_COE_MBXSND_WKC_ERROR	"EC_NOTIFY_COE_MBXSND_WKC_ERROR"

#define EC_NOTIFY_FOE_MBXSND_WKC_ERROR          (EC_NOTIFY_ERROR | 9)       /* FOE mbox send: working counter error */
#define EC_SZTXT_NOTIFY_FOE_MBXSND_WKC_ERROR	"EC_NOTIFY_FOE_MBXSND_WKC_ERROR"

#define EC_NOTIFY_FRAME_RESPONSE_ERROR          (EC_NOTIFY_ERROR |10)       /* Got no response on a sent Ethernet frame */
#define EC_SZTXT_NOTIFY_FRAME_RESPONSE_ERROR	"EC_NOTIFY_FRAME_RESPONSE_ERROR"

#define EC_NOTIFY_SLAVE_INITCMD_RESPONSE_ERROR  (EC_NOTIFY_ERROR |11)       /* Got no response on a sent ecat init command from slave */
#define EC_SZTXT_NOTIFY_SLAVE_INITCMD_RESPONSE_ERROR "EC_NOTIFY_SLAVE_INITCMD_RESPONSE_ERROR"

#define EC_NOTIFY_MASTER_INITCMD_RESPONSE_ERROR (EC_NOTIFY_ERROR |12)       /* Got no response on a sent ecat master init command */
#define EC_SZTXT_NOTIFY_MASTER_INITCMD_RESPONSE_ERROR "EC_NOTIFY_MASTER_INITCMD_RESPONSE_ERROR"

#define EC_NOTIFY_CMD_MISSING                   (EC_NOTIFY_ERROR |13)       /* obsolete */
#define EC_SZTXT_NOTIFY_CMD_MISSING             "EC_NOTIFY_CMD_MISSING"

#define EC_NOTIFY_MBSLAVE_INITCMD_TIMEOUT       (EC_NOTIFY_ERROR |14)       /* Timeout when waiting for mailbox init command response */
#define EC_SZTXT_NOTIFY_MBSLAVE_INITCMD_TIMEOUT	"EC_NOTIFY_MBSLAVE_INITCMD_TIMEOUT"

#define EC_NOTIFY_NOT_ALL_DEVICES_OPERATIONAL   (EC_NOTIFY_ERROR |15)       /* Not all slave devices are in operational state when receiving cyclic frames */
#define EC_SZTXT_NOTIFY_NOT_ALL_DEVICES_OPERATIONAL	"EC_NOTIFY_NOT_ALL_DEVICES_OPERATIONAL"

#define EC_NOTIFY_ETH_LINK_NOT_CONNECTED        (EC_NOTIFY_ERROR |16)       /* Ethernet link (cable) not connected */
#define EC_SZTXT_NOTIFY_ETH_LINK_NOT_CONNECTED	"EC_NOTIFY_ETH_LINK_NOT_CONNECTED"

#define EC_NOTIFY_RED_LINEBRK                   (EC_NOTIFY_ERROR |18)       /* Redundancy: line break detected */
#define EC_SZTXT_NOTIFY_RED_LINEBRK	            "EC_NOTIFY_RED_LINEBRK"

#define EC_NOTIFY_STATUS_SLAVE_ERROR            (EC_NOTIFY_ERROR |19)       /* At least one slave is in error state when receiving cyclic frames (BRD AL-STATUS) */
#define EC_SZTXT_NOTIFY_STATUS_SLAVE_ERROR		"EC_NOTIFY_STATUS_SLAVE_ERROR"

#define EC_NOTIFY_SLAVE_ERROR_STATUS_INFO       (EC_NOTIFY_ERROR |20)       /* Slave Error status information */
#define EC_SZTXT_NOTIFY_SLAVE_ERROR_STATUS_INFO	"EC_NOTIFY_SLAVE_ERROR_STATUS_INFO"

#define EC_NOTIFY_SLAVE_NOT_ADDRESSABLE         (EC_NOTIFY_ERROR |21)       /* Station address lost (or slave missing) - FPRD to AL_STATUS failed  */
#define EC_SZTXT_NOTIFY_SLAVE_NOT_ADDRESSABLE	"EC_NOTIFY_SLAVE_NOT_ADDRESSABLE"

#define EC_NOTIFY_SOE_MBXRCV_WKC_ERROR          (EC_NOTIFY_ERROR |22)       /* SOE mbox receive: working counter error (deprecated) */
#define EC_SZTXT_NOTIFY_SOE_MBXRCV_WKC_ERROR	"EC_NOTIFY_SOE_MBXRCV_WKC_ERROR"

#define EC_NOTIFY_SOE_MBXSND_WKC_ERROR          (EC_NOTIFY_ERROR |23)       /* SOE mbox send: working counter error */
#define EC_SZTXT_NOTIFY_SOE_MBXSND_WKC_ERROR	"EC_NOTIFY_SOE_MBXSND_WKC_ERROR"

#define EC_NOTIFY_SOE_WRITE_ERROR               (EC_NOTIFY_ERROR |24)       /* SOE mbox write responded with an error */
#define EC_SZTXT_NOTIFY_SOE_WRITE_ERROR	        "EC_NOTIFY_SOE_WRITE_ERROR"

#define EC_NOTIFY_MBSLAVE_COE_SDO_ABORT         (EC_NOTIFY_ERROR |25)       /* COE mbox SDO abort */
#define EC_SZTXT_NOTIFY_MBSLAVE_COE_SDO_ABORT	"EC_NOTIFY_MBSLAVE_COE_SDO_ABORT"

#define EC_NOTIFY_CLIENTREGISTRATION_DROPPED    (EC_NOTIFY_ERROR |26)       /* Client registration dropped, possibly call to ecatConfigureMaster by other thread (RAS) */
#define EC_SZTXT_NOTIFY_CLIENTREGISTRATION_DROPPED "EC_NOTIFY_CLIENTREGISTRATION_DROPPED"

#define EC_NOTIFY_RED_LINEFIXED                 (EC_NOTIFY_ERROR |27)       /* Redundancy: line is repaired */
#define EC_SZTXT_NOTIFY_RED_LINEFIXED	        "EC_NOTIFY_RED_LINEFIXED"

#define EC_NOTIFY_FOE_MBSLAVE_ERROR             (EC_NOTIFY_ERROR |28)       /* FOE mbox abort */
#define EC_SZTXT_NOTIFY_FOE_MBSLAVE_ERROR		"EC_NOTIFY_FOE_MBSLAVE_ERROR"

#define EC_NOTIFY_MBXRCV_INVALID_DATA           (EC_NOTIFY_ERROR |29)       /* invalid mail box data received */
#define EC_SZTXT_NOTIFY_MBXRCV_INVALID_DATA		"EC_NOTIFY_MBXRCV_INVALID_DATA"

#define EC_NOTIFY_PDIWATCHDOG                   (EC_NOTIFY_ERROR |30)       /* PDI Watchdog expired on slave, thrown by IST */
#define EC_SZTXT_NOTIFY_PDIWATCHDOG	            "EC_NOTIFY_PDIWATCHDOG"

#define EC_NOTIFY_SLAVE_NOTSUPPORTED            (EC_NOTIFY_ERROR |31)       /* slave not supported (if redundancy is activated and slave doesn't fully support autoclose */
#define EC_SZTXT_NOTIFY_SLAVE_NOTSUPPORTED		"EC_NOTIFY_SLAVE_NOTSUPPORTED"

#define EC_NOTIFY_SLAVE_UNEXPECTED_STATE        (EC_NOTIFY_ERROR |32)       /* slave in unexpected state */
#define EC_SZTXT_NOTIFY_SLAVE_UNEXPECTED_STATE	"EC_NOTIFY_SLAVE_UNEXPECTED_STATE"

#define EC_NOTIFY_ALL_DEVICES_OPERATIONAL       (EC_NOTIFY_ERROR |33)       /* All slave devices are in operational state */
#define EC_SZTXT_NOTIFY_ALL_DEVICES_OPERATIONAL	"EC_NOTIFY_ALL_DEVICES_OPERATIONAL"

#define EC_NOTIFY_VOE_MBXSND_WKC_ERROR          (EC_NOTIFY_ERROR |34)       /* VOE mbox send: working counter error */
#define EC_SZTXT_NOTIFY_VOE_MBXSND_WKC_ERROR	"EC_NOTIFY_VOE_MBXSND_WKC_ERROR"

#define EC_NOTIFY_EEPROM_CHECKSUM_ERROR         (EC_NOTIFY_ERROR |35)       /* EEPROM checksum error detected */
#define EC_SZTXT_NOTIFY_EEPROM_CHECKSUM_ERROR	"EC_NOTIFY_EEPROM_CHECKSUM_ERROR"

#define EC_NOTIFY_LINE_CROSSED                  (EC_NOTIFY_ERROR |36)       /* Crossed lines detected */
#define EC_SZTXT_NOTIFY_LINE_CROSSED	        "EC_NOTIFY_LINE_CROSSED"

#define EC_NOTIFY_JUNCTION_RED_CHANGE           (EC_NOTIFY_ERROR |37)       /* Junction redundancy change */
#define EC_SZTXT_NOTIFY_JUNCTION_RED_CHANGE	    "EC_NOTIFY_JUNCTION_RED_CHANGE"

#define EC_NUM_ERROR_NOTIFICATIONS                                37        /* number of error notifications */

/* ScanBus Notification */
#define EC_NOTIFY_SB_MISMATCH                   (EC_NOTIFY_SCANBUS | 2)     /* ScanBus mismatch */
#define EC_SZTXT_NOTIFY_SB_MISMATCH             "EC_NOTIFY_SB_MISMATCH"

#define EC_NOTIFY_SB_DUPLICATE_HC_NODE          (EC_NOTIFY_SCANBUS | 3)     /* ScanBus mismatch. A duplicate HC group was detected equal product code, vendor id 
                                                                               and alias address (or switch id)  */
#define EC_SZTXT_NOTIFY_SB_DUPLICATE_HC_NODE	"EC_NOTIFY_SB_DUPLICATE_HC_NODE"


/* Hot Connect Action Results */
/* #define EC_NOTIFY_HC_DETECTALLGROUPS         (EC_NOTIFY_HOTCONNECT | 1)   * HC Detect All Groups done */
#define EC_NOTIFY_HC_DETECTADDGROUPS            (EC_NOTIFY_HOTCONNECT | 2)  /* HC Enhance Detect All Groups done */
#define EC_SZTXT_NOTIFY_HC_DETECTADDGROUPS		"EC_NOTIFY_HC_DETECTADDGROUPS"

#define EC_NOTIFY_HC_PROBEALLGROUPS             (EC_NOTIFY_HOTCONNECT | 3)  /* HC Probe All Groups done */
#define EC_SZTXT_NOTIFY_HC_PROBEALLGROUPS	    "EC_NOTIFY_HC_PROBEALLGROUPS"

#define EC_NOTIFY_HC_TOPOCHGDONE                (EC_NOTIFY_HOTCONNECT | 4)  /* HC Topology Change done */
#define EC_SZTXT_NOTIFY_HC_TOPOCHGDONE	        "EC_NOTIFY_HC_TOPOCHGDONE"

#define EC_NOTIFY_SLAVE_DISAPPEARS              (EC_NOTIFY_HOTCONNECT | 5)  /* Slave disappears */
#define EC_NOTIFY_HC_SLAVE_PART EC_NOTIFY_SLAVE_DISAPPEARS
#define EC_SZTXT_NOTIFY_SLAVE_DISAPPEARS		"EC_NOTIFY_SLAVE_DISAPPEARS"

#define EC_NOTIFY_SLAVE_APPEARS                 (EC_NOTIFY_HOTCONNECT | 6)  /* Slave appears */
#define EC_NOTIFY_HC_SLAVE_JOIN EC_NOTIFY_SLAVE_APPEARS
#define EC_SZTXT_NOTIFY_SLAVE_APPEARS		    "EC_NOTIFY_SLAVE_APPEARS"

/* Remote API */
#define ATEMRAS_NOTIFY_CONNECTION               (ATEMRAS_NOTIFY_GENERIC|0x0001)
#define EC_SZTXT_ATEMRAS_NOTIFY_CONNECTION	    "ATEMRAS_NOTIFY_CONNECTION"

#define ATEMRAS_NOTIFY_REGISTER                 (ATEMRAS_NOTIFY_GENERIC|0x0002)
#define EC_SZTXT_ATEMRAS_NOTIFY_REGISTER	    "ATEMRAS_NOTIFY_REGISTER"

#define ATEMRAS_NOTIFY_UNREGISTER               (ATEMRAS_NOTIFY_GENERIC|0x0003)
#define EC_SZTXT_ATEMRAS_NOTIFY_UNREGISTER	    "ATEMRAS_NOTIFY_UNREGISTER"

#define ATEMRAS_NOTIFY_MARSHALERROR             (ATEMRAS_NOTIFY_ERROR  |0x0001)
#define EC_SZTXT_ATEMRAS_NOTIFY_MARSHALERROR    "ATEMRAS_NOTIFY_MARSHALERROR"

#define ATEMRAS_NOTIFY_ACKERROR                 (ATEMRAS_NOTIFY_ERROR  |0x0002)
#define EC_SZTXT_ATEMRAS_NOTIFY_ACKERROR	    "ATEMRAS_NOTIFY_ACKERROR"

#define ATEMRAS_NOTIFY_NONOTIFYMEMORY           (ATEMRAS_NOTIFY_ERROR  |0x0003)
#define EC_SZTXT_ATEMRAS_NOTIFY_NONOTIFYMEMORY	"ATEMRAS_NOTIFY_NONOTIFYMEMORY"

#define ATEMRAS_NOTIFY_STDNOTIFYMEMORYSMALL     (ATEMRAS_NOTIFY_ERROR  |0x0004)
#define EC_SZTXT_ATEMRAS_NOTIFY_STDNOTIFYMEMORYSMALL	"ATEMRAS_NOTIFY_STDNOTIFYMEMORYSMALL"

#define ATEMRAS_NOTIFY_MBXNOTIFYMEMORYSMALL     (ATEMRAS_NOTIFY_ERROR  |0x0005)
#define EC_SZTXT_ATEMRAS_NOTIFY_MBXNOTIFYMEMORYSMALL	"ATEMRAS_NOTIFY_MBXNOTIFYMEMORYSMALL"


/* cyclic error notification bit masks */
#define EC_CYC_ERR_MASK_NOT_ALL_DEVICES_OPERATIONAL     0x00000001      /* mask for cyclic EC_NOTIFY_NOT_ALL_DEVICES_OPERATIONAL notifications */
#define EC_CYC_ERR_MASK_STATUS_SLAVE_ERROR              0x00000002      /* mask for cyclic EC_NOTIFY_STATUS_SLAVE_ERROR notifications */
#define EC_CYC_ERR_MASK_CYCCMD_WKC_ERROR                0x00000004      /* mask for cyclic EC_NOTIFY_CYCCMD_WKC_ERROR notifications */
#define EC_CYC_ERR_MASK_UNEXPECTED_FRAME_RESPONSE       0x00000008      /* mask for cyclic EC_NOTIFY_FRAME_RESPONSE_ERROR notifications with error type eRspErr_UNEXPECTED */
#define EC_CYC_ERR_MASK_NO_FRAME_RESPONSE_ERROR         0x00000010      /* mask for cyclic EC_NOTIFY_FRAME_RESPONSE_ERROR notifications with error type eRspErr_NO_RESPONSE */
#define EC_ERR_MASK_ETH_LINK_NOT_CONNECTED              0x00000020      /* mask for EC_NOTIFY_ETH_LINK_NOT_CONNECTED notifications */
#define EC_CYC_ERR_MASK_ALL                             0xFFFFFFFF      /* mask for all cyclic error notifications */

/* Supported mailbox protocols */
#define EC_MBX_PROTOCOL_AOE 0x01
#define EC_MBX_PROTOCOL_EOE 0x02
#define EC_MBX_PROTOCOL_COE 0x04
#define EC_MBX_PROTOCOL_FOE 0x08
#define EC_MBX_PROTOCOL_SOE 0x10
#define EC_MBX_PROTOCOL_VOE 0x20

/* CoE debug bit masks */
#define EC_COE_DBG_MASK_SDO_DOWNLOAD        0x00000001  /* mask for SDO download transfer debug support */
#define EC_COE_DBG_MASK_SDO_UPLOAD          0x00000002  /* mask for SDO upload transfer debug support */
#define EC_COE_DBG_MASK_GET_ODLIST          0x00000004  /* mask for CoE get object dictionary list transfer debug support */
#define EC_COE_DBG_MASK_GET_OBDESC          0x00000008  /* mask for CoE get object description transfer debug support */
#define EC_COE_DBG_MASK_GET_ENTRYDESC       0x00000010  /* mask for CoE get entry description transfer debug support */
#ifdef INCLUDE_COE_PDO_SUPPORT
#define EC_COE_DBG_MASK_RX_PDO              0x00000020  /* mask for RxPDO transfer debug support */
#define EC_COE_DBG_MASK_TX_PDO              0x00000040  /* mask for TxPDO transfer debug support */
#endif
#define EC_COE_DBG_MASK_ALL                 0x00000FFF  /* mask for all CoE transfers debug support */

/* FoE debug bit masks */
#define EC_FOE_DBG_MASK_FILE_DOWNLOAD       0x00001000  /* mask for FoE download transfer debug support */
#define EC_FOE_DBG_MASK_FILE_UPLOAD         0x00002000  /* mask for FoE download transfer debug support */
#define EC_FOE_DBG_MASK_ALL                 0x000FF000  /* mask for all FoE transfers debug support */

#define EC_MBX_DBG_MASK_ALL                 0x000FFFFF  /* mask for all CoE transfers debug support */


/* SoE debug bit masks */
#ifdef INCLUDE_SOE_SUPPORT
#define EC_SOE_DBG_MASK_IDN_WRITE        0x00000001  /* mask for SOE download transfer debug support */
#define EC_SOE_DBG_MASK_IDN_READ         0x00000002  /* mask for SOE upload transfer debug support */
#define EC_SOE_DBG_MASK_ALL              0x00000FFF  /* mask for all SoE transfers debug support */
#endif

/* VoE debug bit masks */
#ifdef INCLUDE_VOE_SUPPORT
#define EC_VOE_DBG_MASK_MBX_DOWNLOAD     0x00100000  /* mask for VoE write transfer debug support */
#define EC_VOE_DBG_MASK_MBX_UPLOAD       0x00200000  /* mask for VoE read transfer debug support */
#define EC_VOE_DBG_MASK_ALL              0x0FF00000  /* mask for all VoE transfers debug support */
#endif


/* EtherCAT commands */
typedef enum 
{
    EC_CMD_TYPE_NOP     = 0x00,
    EC_CMD_TYPE_APRD    = 0x01,
    EC_CMD_TYPE_APWR    = 0x02,
    EC_CMD_TYPE_APRW    = 0x03,
    EC_CMD_TYPE_FPRD    = 0x04,
    EC_CMD_TYPE_FPWR    = 0x05,
    EC_CMD_TYPE_FPRW    = 0x06,
    EC_CMD_TYPE_BRD     = 0x07,
    EC_CMD_TYPE_BWR     = 0x08,
    EC_CMD_TYPE_BRW     = 0x09,
    EC_CMD_TYPE_LRD     = 0x0A,
    EC_CMD_TYPE_LWR     = 0x0B,
    EC_CMD_TYPE_LRW     = 0x0C,
    EC_CMD_TYPE_ARMW    = 0x0D,
    EC_CMD_TYPE_FRMW    = 0x0E,
    EC_CMD_TYPE_EXT     = 0xFF,

    /* Borland C++ datatype alignment correction */
    EC_CMD_TYPE_BCppDummy   = 0xFFFFFFFF
} EC_CMD_TYPE;


/*-TYPEDEFS------------------------------------------------------------------*/

/* static master configuration */
typedef struct _EC_T_MASTER_CONFIG
{
    EC_T_DWORD  dwValPattern;                   /*< Validation pattern */
    EC_T_INT    nSlaveMultiplier;               /*< slave timer multiplier (based on master timer) */
    EC_T_DWORD  dwEcatCmdTimeout;               /*< timeout to send pending ethercat command frames */
    EC_T_DWORD  dwEcatCmdMaxRetries;            /*< maximum retries to send pending ethercat command frames */
    EC_T_DWORD  dwReserved;                     /*< obsolete */
    EC_T_DWORD  dwCoeTimeout;                   /*< obsolete timeout sending CoE frames  */
    EC_T_DWORD  dwEoeTimeout;                   /*< timeout sending EoE frames */
    EC_T_DWORD  dwFoeTimeout;                   /*< obsolete timeout sending FoE frames  */
    EC_T_DWORD  dwFoeBusyTimeout;               /*< timeout FoE busy */
    EC_T_DWORD  dwMaxQueuedEthFrames;           /*< maximum number of queued Ethernet frames */
    EC_T_DWORD  dwMaxSlaveCmdPerFrame;          /*< maximum number of slave commands per frame */
    EC_T_DWORD  dwMaxQueuedCoeSlaves;           /*< obsolete */
    EC_T_DWORD  dwMaxQueuedCoeCmds;             /*< obsolete: maximum number of queued CoE commands */
    EC_T_DWORD  dwBusCycleTimeUsec;             /*< [usec] bus cycle time in microseconds */
    EC_T_DWORD  dwStateChangeDebug;             /*< Slave state change debugging level: Bit 0 = OsDbgMsg Bit 1 = Link-Msg */
    EC_T_DWORD  dwErrorMsgToLinkLayer;          /*< if EC_TRUE error messages are sent to the link layer */
    EC_T_DWORD  dwMaxSentQueuedFramesPerCyc;    /*< max number of queued frames sent with eUsrJob_SendAcycFrames within one cycle */
} EC_PACKED EC_T_MASTER_CONFIG;


/* EtherCAT slave properties */
typedef struct _EC_T_SLAVE_PROP
{
    EC_T_WORD   wStationAddress;        /*< station address, 
                                         *  set by XML configuration file (Config/Slave/Info/PhysAddr) */
    EC_T_WORD   wAutoIncAddr;           /*< auto increment address */
    EC_T_CHAR   achName[MAX_STD_STRLEN];/*< device name */
} EC_PACKED EC_T_SLAVE_PROP;


/* EtherCAT notify parameters */
typedef struct _EC_T_NOTIFYPARMS
{
    EC_T_VOID*      pCallerData;        /*< in  caller data */
    EC_T_BYTE*      pbyInBuf;           /*< in  input data buffer */
    EC_T_DWORD      dwInBufSize;        /*< in  size of input data buffer in byte */
    EC_T_BYTE*      pbyOutBuf;          /*< out output data buffer */
    EC_T_DWORD      dwOutBufSize;       /*< in  size of output data buffer in byte */
    EC_T_DWORD*     pdwNumOutData;      /*< out number of output data bytes stored in output data buffer */
} EC_PACKED EC_T_NOTIFYPARMS, *PEC_T_NOTIFYPARMS;

/* EtherCAT command WKC error descriptor */
typedef struct _EC_T_WKCERR_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties if applicable */
    EC_T_BYTE       byCmd;              /*< EtherCAT command where the error occured */
    EC_T_BYTE       byRsvd[3];          /*< Alignment */
    EC_T_DWORD      dwAddr;             /*< logical or physical (adp/ado) address */
    EC_T_WORD       wWkcSet;            /*< working counter set value */
    EC_T_WORD       wWkcAct;            /*< working counter actual value */
} EC_PACKED EC_T_WKCERR_DESC;

/* EtherCAT frame response error descriptor */
typedef enum _EC_T_FRAME_RSPERR_TYPE
{
    eRspErr_UNDEFINED       = 0,        /*< undefined */
    eRspErr_NO_RESPONSE     = 1,        /*< no response (timeout)            obsolete for acyclic frames*/
    eRspErr_WRONG_IDX       = 2,        /*< frame with wrong IDX received    obsolete for acyclic frames*/
    eRspErr_UNEXPECTED      = 3,        /*< unexpected frame received */
    eRspErr_FRAME_RETRY     = 4,        /*< ethernet frame has to be re-sent due to an timeout (only acyclic frames)  obsolete*/
    eRspErr_RETRY_FAIL      = 5,        /*< all retry mechanism fails to re-sent acyclic frames*/
    /* Borland C++ datatype alignment correction */
    eRspErr_BCppDummy   = 0xFFFFFFFF
} EC_T_FRAME_RSPERR_TYPE;

typedef struct _EC_T_FRAME_RSPERR_DESC
{
    EC_T_BOOL               bIsCyclicFrame;         /*< EC_TRUE if it is a frame containing cyclic commands */
    EC_T_FRAME_RSPERR_TYPE  EErrorType;             /*< type of error */
    EC_T_BYTE               byEcCmdHeaderIdxSet;    /*< Expected index value */
    EC_T_BYTE               byEcCmdHeaderIdxAct;    /*< Actual index value */
} EC_PACKED EC_T_FRAME_RSPERR_DESC;

/* EtherCAT init command response error descriptor */
typedef enum _EC_T_INITCMD_ERR_TYPE
{
    eInitCmdErr_UNDEFINED        = 0,    /*< undefined */
    eInitCmdErr_NO_RESPONSE      = 1,    /*< no response */
    eInitCmdErr_VALIDATION_ERR   = 2,    /*< validation error */
    eInitCmdErr_FAILED           = 3,    /*< init command failed, requested state could not be reached */

    /* Borland C++ datatype alignment correction */
    eInitCmdErr_BCppDummy       = 0xFFFFFFFF
} EC_T_INITCMD_ERR_TYPE;

typedef struct _EC_T_INITCMD_ERR_DESC
{
    EC_T_SLAVE_PROP       SlaveProp;                              /*< slave properties */
    EC_T_CHAR             achStateChangeName[MAX_SHORT_STRLEN];   /*< description of the current ecat state change */
    EC_T_INITCMD_ERR_TYPE EErrorType;                             /*< type of error */
    EC_T_CHAR             szComment[MAX_STD_STRLEN];              /*< comment (ENI) */
} EC_PACKED EC_T_INITCMD_ERR_DESC;

/* EtherCAT missing command descriptor */
typedef struct _EC_T_MISSCMD_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties */
    EC_T_DWORD      dwCmdIndex;         /*< ethercat command/telegram index in ethernet frame that is missing */
    EC_T_BOOL       bMasterCmd;         /*< EC_TRUE if it is a master command or EC_FALSE if it is a slave command */
} EC_PACKED EC_T_MISSCMD_DESC;

/* EtherCAT Slave error status info descriptor */
typedef struct _EC_T_SLAVE_ERROR_INFO_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties */
    EC_T_WORD       wStatus;            /*< Slave Status (AL Status) */
    EC_T_WORD       wStatusCode;        /*< Error status code (AL Control Status) */
} EC_PACKED EC_T_SLAVE_ERROR_INFO_DESC;

/* EtherCAT Sdo abort */
typedef struct _EC_T_MBOX_SDO_ABORT_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties */
    EC_T_DWORD      dwErrorCode;        /*< error code EC_E_ */
    EC_T_WORD       wObjIndex;          /*< SDO object index */
    EC_T_BYTE       bySubIndex;         /*< SDO object sub index */
} EC_PACKED EC_T_MBOX_SDO_ABORT_DESC;

/* EtherCAT FoE error */
typedef struct _EC_T_MBOX_FOE_ABORT_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties */
    EC_T_DWORD      dwErrorCode;        /*< error code EC_E_ */
    EC_T_CHAR       achErrorString[MAX_STD_STRLEN];   /*< FOE error string */
} EC_PACKED EC_T_MBOX_FOE_ABORT_DESC;

/* Invalid mailbox data received error */
typedef struct _EC_T_MBXRCV_INVALID_DATA_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties */
} EC_PACKED EC_T_MBXRCV_INVALID_DATA_DESC;

/* PDI Watchdog expired */
typedef struct _EC_T_PDIWATCHDOG_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties */
} EC_PACKED EC_T_PDIWATCHDOG_DESC;

/* Slave not supported */
typedef struct _EC_T_SLAVE_NOTSUPPORTED_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties */
} EC_PACKED EC_T_SLAVE_NOTSUPPORTED_DESC;

/* Slave in unexpected state */
typedef struct _EC_T_SLAVE_UNEXPECTED_STATE_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties */
    EC_T_STATE      curState;
    EC_T_STATE      expState;
} EC_PACKED EC_T_SLAVE_UNEXPECTED_STATE_DESC;

/* Slave EEPROM checksum error */
typedef struct _EC_T_EEPROM_CHECKSUM_ERROR_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties */
} EC_PACKED EC_T_EEPROM_CHECKSUM_ERROR_DESC;

/* Redundancy break/fixed notification */
typedef struct _EC_T_RED_CHANGE_DESC
{
    EC_T_WORD       wNumOfSlavesMain;    /*< Number of Slaves on Main Line */
    EC_T_WORD       wNumOfSlavesRed;     /*< Number of Slaves on Red Line  */
} EC_PACKED EC_T_RED_CHANGE_DESC;

typedef struct _EC_T_JUNCTION_RED_CHANGE_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties of the slave with disconnected port 0 */
    EC_T_BOOL       bLineBreak;         /*< EC_TRUE for line break, EC_FALSE for line fixed */
} EC_PACKED EC_T_JUNCTION_RED_CHANGE_DESC;

/* Distributed clocks (DC) */
typedef struct _EC_T_DC_SYNC_NTFY_DESC
{
    EC_T_DWORD      IsInSync;           /*< EC_TRUE if all meant objects are in defined sync range, EC_FALSE otherwise */
    EC_T_DWORD      IsNegative;         /*< Deviation value is a negative value */
    EC_T_DWORD      dwDeviation;        /*< absolute deviation in nsecs (depending on notification nsec / wire or'ed value */
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties in case of "not in sync" */
} EC_PACKED EC_T_DC_SYNC_NTFY_DESC;

/* Distributed clocks master sync (DCM) */
typedef struct _EC_T_DCM_SYNC_NTFY_DESC
{
    EC_T_DWORD      IsInSync;           /*< EC_TRUE if time of master and reference clock are in sync */
    EC_T_INT        nCtlErrorNsecCur;   /*< current DCM controller error in nsec */
    EC_T_INT        nCtlErrorNsecAvg;   /*< average DCM controller error in nsec */
    EC_T_INT        nCtlErrorNsecMax;   /*< maximum DCM controller error in nsec */
} EC_PACKED EC_T_DCM_SYNC_NTFY_DESC;

/* Slave state change descriptor */
typedef struct _EC_T_SLAVE_STATECHANGED_NTFY_DESC
{
    EC_T_SLAVE_PROP SlaveProp;          /*< slave properties */
    EC_T_STATE      newState;           /*< new state */
} EC_PACKED EC_T_SLAVE_STATECHANGED_NTFY_DESC;

typedef struct _EC_T_DC_BURSTCONFIG_REQ
{
    EC_T_DWORD	dwTotalBurstLength;
    EC_T_DWORD	dwBurstBulk;
} EC_PACKED EC_T_DC_BURSTCONFIG_REQ;

typedef struct _EC_T_DC_SHIFTSYSTIME_DESC
{
    EC_T_DWORD	    dwCyclesToShift;    /*< Amount of cycles to apply shift */
    EC_T_INT        nShiftTime;         /*< Shift Time in nsecs (signed) */
} EC_PACKED EC_T_DC_SHIFTSYSTIME_DESC;

typedef  struct _EC_T_DC_STARTCYCSAFETY_DESC    /*< Start SYNC Cyc Safety in nsec (64Bit)*/
{
    EC_T_DWORD      dwStartCycSafetyLo;
    EC_T_DWORD      dwStartCycSafetyHi;
}   EC_T_DC_STARTCYCSAFETY_DESC;
           
typedef struct _EC_T_SB_STATUS_NTFY_DESC
{
    EC_T_DWORD      dwResultCode;       /*< Result of scanbus */
    EC_T_DWORD      dwSlaveCount;       /*< Number of slaves handles during scanbus */
} EC_PACKED EC_T_SB_STATUS_NTFY_DESC;

typedef struct _EC_T_SB_MISMATCH_DESC
{
    /* Previous */
    EC_T_WORD           wPrevFixedAddress;      /*< Previous fixed address */
    EC_T_WORD           wPrevPort;              /*< Previous port */
    EC_T_WORD           wPrevAIncAddress;       /*< Previous Auto-Inc address */
                                                
    /* error or mismatching slave */                        
    EC_T_WORD           wBusAIncAddress;        /*< Bus Auto-Inc address */
    EC_T_DWORD          dwBusVendorId;          /*< Bus Vendor ID */
    EC_T_DWORD          dwBusProdCode;          /*< Bus Product code */
    EC_T_DWORD          dwBusRevisionNo;        /*< Bus Revision Number */
    EC_T_DWORD          dwBusSerialNo;          /*< Bus Serial Number */
                                                
    EC_T_WORD           wBusFixedAddress;       /*< Bus Station Address */
    EC_T_WORD           wBusAliasAddress;       /*< Bus Alias Address */
                        
    EC_T_WORD           wCfgFixedAddress;       /*< Config Station Address */
    EC_T_WORD           wCfgAIncAddress;        /*< Config Auto-Increment Address */
                        
    EC_T_DWORD          dwCfgVendorId;          /*< Config Vendor ID */
    EC_T_DWORD          dwCfgProdCode;          /*< Config Product code */
    EC_T_DWORD          dwCfgRevisionNo;        /*< Config Revision Number */
    EC_T_DWORD          dwCfgSerialNo;          /*< Config Serial Number */
                                                
    EC_T_BOOL           bIdentValidationError;  /*< HotConnect Identification command validation error */

    EC_T_WORD           oIdentCmdHdr[5];        /*< HotConnect Identification command header (if bIdentValidationError) */
    EC_T_DWORD          dwCmdData;              /*< First DWORD of Data portion in identify command (received data) */
    EC_T_DWORD          dwCmdVMask;             /*< First DWORD of Validation mask in identify command */
    EC_T_DWORD          dwCmdVData;             /*< First DWORD of Validation data in identify command (expected data) */
} EC_PACKED EC_T_SB_MISMATCH_DESC;

typedef struct _EC_T_LINE_CROSSED_DESC
{
    EC_T_SLAVE_PROP     SlaveProp;       /*< slave properties */

    EC_T_WORD           wInputPort;      /*< Port where the telegram got in */

} EC_PACKED EC_T_LINE_CROSSED_DESC;

typedef struct _EC_T_HC_DETECTALLGROUP_NTFY_DESC
{
    EC_T_DWORD      dwResultCode;       /*< Result of Group detection */
    EC_T_DWORD      dwGroupCount;       /*< Number of Groups */
    EC_T_DWORD      dwGroupsPresent;    /*< Number of connected groups */
    EC_T_DWORD      dwGroupMask;        /*< Bitmask of first 32   Groups 1 = present, 0 = absent */
    EC_T_DWORD      adwGroupMask[100];  /*< Bitmask of first 3200 Groups */
} EC_PACKED EC_T_HC_DETECTALLGROUP_NTFY_DESC;

typedef struct _EC_T_RAWCMDRESPONSE_NTFY_DESC
{
    EC_T_DWORD	dwInvokeId;
    EC_T_DWORD	dwResult;
    EC_T_DWORD	dwWkc;
    EC_T_DWORD	dwCmdIdx;
    EC_T_DWORD	dwAddr;
    EC_T_DWORD	dwLength;
    EC_T_BYTE*      pbyData;
} EC_PACKED EC_T_RAWCMDRESPONSE_NTFY_DESC;
                                                   
#ifdef INCLUDE_COE_PDO_SUPPORT
typedef struct _EC_T_TX_PDO_NTFY_DESC
{
    EC_T_DWORD  wPhysAddr;          /* station address */
    EC_T_DWORD  dwNumber;           /* PDO number */
    EC_T_DWORD  wLen;               /* PDO size */
    EC_T_BYTE*  pbyData;
} EC_PACKED EC_T_TX_PDO_NTFY_DESC;
#endif

typedef struct _EC_T_SLAVE_LINKMSG_DESC
{
    EC_T_DWORD  dwSlaveId;          /* Slave Id */
    EC_T_BOOL   bEnableLogging;     /* EC_TRUE=> Enable LinkMsgs, EC_FALSE=> Disable */
} EC_PACKED EC_T_SLAVE_LINKMSG_DESC;
                                      

/* EtherCAT state change */                         
typedef struct _EC_T_STATECHANGE                    
{                                                   
    EC_T_STATE    oldState;                         /*< old state */
    EC_T_STATE    newState;                         /*< new state */
} EC_PACKED EC_T_STATECHANGE;                                 


/* List of slaves */
#define MAX_NUM_OF_SLAVES_WITH_ERR_STATE    128
typedef struct _EC_T_SLAVE_LIST_DESC
{                                                   
    EC_T_DWORD    dwListLen;                         /*< number of entrys in list */
    EC_T_DWORD    adwSlaveID[MAX_NUM_OF_SLAVES_WITH_ERR_STATE];  /*< array of Slave IDs */
} EC_PACKED EC_T_SLAVE_LIST_DESC;

typedef struct _EC_T_HC_SLAVE_CHANGE_DESC
{
    EC_T_SLAVE_PROP     SlaveProp;
} EC_PACKED EC_T_HC_SLAVE_CHANGE_DESC;

/* EtherCAT error notification descriptor */
typedef struct _EC_T_NOTIFICATION_DESC
{
    union _EC_T_NOTIFICATION_PARM
    {                
        EC_T_DWORD              StatusCode;             /* Generic status code only notification */
        EC_T_DC_SYNC_NTFY_DESC  SyncNtfyDesc;           /* DC Master / Slave Sync Notification descriptor */
        EC_T_RAWCMDRESPONSE_NTFY_DESC
                                RawCmdRespNtfyDesc;     /* Queue Raw Cmd Response notification descriptor */
        EC_T_SB_STATUS_NTFY_DESC
                                ScanBusNtfyDesc;        /* Scanbus Result notification descriptor */

        EC_T_SB_MISMATCH_DESC   ScanBusMismatch;        /* Scan Bus Mismatch notification descriptor */

        EC_T_STATECHANGE        StatusChngNtfyDesc;     /* Master EtherCAT State changed notification */
#ifdef INCLUDE_COE_PDO_SUPPORT
        EC_T_TX_PDO_NTFY_DESC   TxPdoNtfyDesc;          /* TxPDO transfer notification */
#endif
        EC_T_SLAVE_LIST_DESC    SlaveListDesc;          /* Contains a list of Slave IDs */
        EC_T_HC_DETECTALLGROUP_NTFY_DESC
                                HCDetAllGrps;           /* HC Group Detection */
        EC_T_HC_SLAVE_CHANGE_DESC
                                HCSlvChgDesc;           /* HotConnect Slave State Change */

        EC_T_LINE_CROSSED_DESC  CrossedLineDesc;        /* Line crossed */

        EC_T_DCM_SYNC_NTFY_DESC DcmInSyncDesc;

        EC_T_SLAVE_STATECHANGED_NTFY_DESC SlaveStateChangedDesc; /* Slave EtherCAT State changed notification */
    }desc;
} EC_PACKED EC_T_NOTIFICATION_DESC;

/* EtherCAT error notification descriptor */
typedef struct _EC_T_ERROR_NOTIFICATION_DESC
{
    EC_T_DWORD  dwNotifyErrorCode;                  /* notification code */
    EC_T_CHAR   achErrorInfo[MAX_ERRINFO_STRLEN];   /* additional error information string (NULL terminated) */
    union _EC_T_ERROR_NOTIFICATION_PARM
    {
        EC_T_WKCERR_DESC              WkcErrDesc;            /* WKC error descriptor */
        EC_T_FRAME_RSPERR_DESC        FrameRspErrDesc;       /* Frame response error descriptor */
        EC_T_INITCMD_ERR_DESC         InitCmdErrDesc;        /* Master/Slave init command error descriptor */
        EC_T_MISSCMD_DESC             MissCmdDesc;           /* EtherCAT missing command descriptor */
        EC_T_SLAVE_ERROR_INFO_DESC    SlaveErrInfoDesc;      /* Slave Error Info Descriptor */
        EC_T_MBOX_SDO_ABORT_DESC      SdoAbortDesc;          /* SDO Abort */
        EC_T_RED_CHANGE_DESC          RedChangeDesc;         /* Redundancy Descriptor */
        EC_T_MBOX_FOE_ABORT_DESC      FoeErrorDesc;          /* FOE error code and string */
        EC_T_MBXRCV_INVALID_DATA_DESC MbxRcvInvalidDataDesc; /* Invalid mailbox data received descriptor */
        EC_T_PDIWATCHDOG_DESC         PdiWatchdogDesc;       /* PDI Watchodg expired */
        EC_T_SLAVE_NOTSUPPORTED_DESC  SlaveNotSupportedDesc; /* Slave not supported */
        EC_T_SLAVE_UNEXPECTED_STATE_DESC SlaveUnexpectedStateDesc; /* Slave not supported */
        EC_T_EEPROM_CHECKSUM_ERROR_DESC  EEPROMChecksumErrorDesc;  /* EEPROM checksum error */
        EC_T_JUNCTION_RED_CHANGE_DESC JunctionRedChangeDesc; /* Junction redundancy change descriptor */
    } desc;
} EC_PACKED EC_T_ERROR_NOTIFICATION_DESC;

typedef enum _EC_T_CNF_TYPE
{
    eCnfType_Unknown            = 0,
    eCnfType_Filename           = 1,
    eCnfType_Data               = 2,
    eCnfType_Datadiag           = 3,                /* only for diagnosis. ENI data is not stored */
    eCnfType_GenPreopENI        = 4,                /* generate mini ENI based on bus-scan result to get into preop state */
    eCnfType_GenPreopENIWithCRC = 5,                /* same as eCnfType_GenPreopENI, but also generates CRC */

    /* Borland C++ datatype alignment correction */
    eCnfType_BCppDummy   = 0xFFFFFFFF
} EC_T_CNF_TYPE;

typedef EC_T_VOID (*EC_T_PFMEMREQ)(EC_T_PVOID pvContext, EC_T_PBYTE* ppbyPDData);
typedef EC_T_VOID (*EC_T_PFMEMREL)(EC_T_PVOID pvContext);

/* Descriptor for EC_IOCTL_REGISTER_PDMEMORYPROVIDER */
typedef struct _EC_T_MEMPROV_DESC
{
    EC_T_PVOID      pvContext;                      /* Context pointer. This pointer is used each time one of
                                                     * the callback functions is called. 
                                                     */
    EC_T_BOOL       bUseMasterShadowInUserSyncNowaitMode;
                                                    /* This flag determines whether the master shall use an 
                                                     * internally allocated shadow memory where new input 
                                                     * process data will be stored when received from 
                                                     * the link layer. Only valid in 
                                                     * eSyncMode_USER_SYNC_NOWAIT.
                                                     */
    EC_T_PBYTE      pbyPDOutData;                   /* Pointer to the default output process data buffer */
    EC_T_DWORD      dwPDOutDataLength;              /* Length of the output process data buffer */

    EC_T_PBYTE      pbyPDInData;                    /* Pointer to the default input process data buffer */
    EC_T_DWORD      dwPDInDataLength;               /* Length of the input  process data buffer */

#ifdef INCLUDE_MEMORY_PROVIDER
    EC_T_PFMEMREQ   pfPDOutDataReadRequest;         /* This function will be called by the master within the 
                                                     * process data transfer cycle prior to read data.
                                                     */
    EC_T_PFMEMREL   pfPDOutDataReadRelease;         /* This function will be called after all data was read 
                                                     * from output process data buffer.
                                                     */

    EC_T_PFMEMREQ   pfPDInDataWriteRequest;         /* This function will be called by the master within the 
                                                     * process data transfer cycle prior to write data.
                                                     */
    EC_T_PFMEMREL   pfPDInDataWriteRelease;         /* This function will be called after all data was written
                                                     * to input process data buffer.
                                                     */
#endif
} EC_PACKED EC_T_MEMPROV_DESC, *EC_PT_MEMPROV_DESC;

typedef struct _EC_T_SB_SLAVEINFO_DESC
{
    EC_T_DWORD  dwScanBusStatus;                    /* Status during last Bus Scan */
    
    EC_T_DWORD	dwVendorId;                         /* Vendor Identification */
    EC_T_DWORD	dwProductCode;                      /* Product Code */
    EC_T_DWORD	dwRevisionNumber;                   /* Revision Number */
    EC_T_DWORD	dwSerialNumber;                     /* Serial Number */
} EC_PACKED EC_T_SB_SLAVEINFO_DESC, *EC_PT_SB_SLAVEINFO_DESC;

typedef enum _EC_T_eEEPENTRY
{
    eEEP_VendorId	        = ESC_SII_REG_VENDORID,
    eEEP_ProductCode        = ESC_SII_REG_PRODUCTCODE,
    eEEP_RevisionNumber     = ESC_SII_REG_REVISIONNUMBER,
    eEEP_SerialNumber       = ESC_SII_REG_SERIALNUMBER,
    eEEP_BootRcvMbx         = ESC_SII_REG_BOOT_RECV_MBX_OFFSET,
    eEEP_BootSndMbx         = ESC_SII_REG_BOOT_SEND_MBX_OFFSET,
    eEEP_StdRcvMbx          = ESC_SII_REG_STD_RECV_MBX_OFFSET,
    eEEP_StdSndMbx          = ESC_SII_REG_STD_SEND_MBX_OFFSET,
    eEEP_MbxProtocol        = ESC_SII_REG_MBX_PROTOCOL,
    eEEP_AliasAddress       = ESC_SII_REG_ALIASADDRESS,

    /* Borland C++ datatype alignment correction */
    eEEP_BCppDummy          = 0xFFFFFFFF
} EC_T_eEEPENTRY;

#define EEPENTRY_VENDORID         ((EC_T_DWORD)0)
#define EEPENTRY_PRODUCTCODE      ((EC_T_DWORD)1)
#define EEPENTRY_REVISIONNO       ((EC_T_DWORD)2)
#define EEPENTRY_SERIALNO         ((EC_T_DWORD)3)
#define EEPENTRY_BOOTRCVMBX       ((EC_T_DWORD)4) /* LOWORD = offset,   HIWORD = size     */
#define EEPENTRY_BOOTSNDMBX       ((EC_T_DWORD)5) /* LOWORD = offset,   HIWORD = size     */
#define EEPENTRY_STDRCVMBX        ((EC_T_DWORD)6) /* LOWORD = offset,   HIWORD = size     */
#define EEPENTRY_STDSNDMBX        ((EC_T_DWORD)7) /* LOWORD = offset,   HIWORD = size     */
#define EEPENTRY_MBXPROTOCOL      ((EC_T_DWORD)8) /* LOWORD = protocol, HIWORD = reserved */
#define EEPENTRY_ALIASADDRESS     ((EC_T_DWORD)9)
#define EEPENTRY_COUNT            ((EC_T_DWORD)10)

#define BT_CHECK_EEPENTRY_VENDORID      EC_TRUE
#define BT_CHECK_EEPENTRY_PRODUCTCODE   EC_TRUE
#define BT_CHECK_EEPENTRY_REVISIONNO    EC_TRUE
#define BT_CHECK_EEPENTRY_SERIALNO      EC_TRUE

#define EEPENTRYTOINDEX(dwEnum) \
    ((dwEnum==eEEP_VendorId)?EEPENTRY_VENDORID: \
     ((dwEnum==eEEP_ProductCode)?EEPENTRY_PRODUCTCODE: \
      ((dwEnum==eEEP_RevisionNumber)?EEPENTRY_REVISIONNO: \
       ((dwEnum==eEEP_SerialNumber)?EEPENTRY_SERIALNO: \
        ((dwEnum==eEEP_BootRcvMbx)?EEPENTRY_BOOTRCVMBX: \
         ((dwEnum==eEEP_BootSndMbx)?EEPENTRY_BOOTSNDMBX: \
          ((dwEnum==eEEP_StdRcvMbx)?EEPENTRY_STDRCVMBX: \
           ((dwEnum==eEEP_StdSndMbx)?EEPENTRY_STDSNDMBX: \
            ((dwEnum==eEEP_MbxProtocol)?EEPENTRY_MBXPROTOCOL: \
             ((dwEnum==eEEP_AliasAddress)?EEPENTRY_ALIASADDRESS:0 \
    ))))))))))

#define EEPINDEXTOENTRY(dwIndex) \
    ((dwIndex==EEPENTRY_VENDORID)?eEEP_VendorId: \
     ((dwIndex==EEPENTRY_PRODUCTCODE)?eEEP_ProductCode: \
      ((dwIndex==EEPENTRY_REVISIONNO)?eEEP_RevisionNumber: \
       ((dwIndex==EEPENTRY_SERIALNO)?eEEP_SerialNumber: \
        ((dwIndex==EEPENTRY_BOOTRCVMBX)?eEEP_BootRcvMbx: \
         ((dwIndex==EEPENTRY_BOOTSNDMBX)?eEEP_BootSndMbx: \
          ((dwIndex==EEPENTRY_STDRCVMBX)?eEEP_StdRcvMbx: \
           ((dwIndex==EEPENTRY_STDSNDMBX)?eEEP_StdSndMbx: \
            ((dwIndex==EEPENTRY_MBXPROTOCOL)?eEEP_MbxProtocol: \
             ((dwIndex==EEPENTRY_ALIASADDRESS)?eEEP_AliasAddress: eEEP_BCppDummy \
    ))))))))))

typedef struct _EC_T_SCANBUS_PROP_DESC
{
    EC_T_eEEPENTRY  eEEPROMEntry;
    EC_T_DWORD      dwVerify;
} EC_PACKED EC_T_SCANBUS_PROP_DESC, *EC_PT_SCANBUS_PROP_DESC;

typedef enum _EC_T_eSBSlaveInfoType
{
    sbsit_unknown	    = 0,
    sbsit_bustopology   = 1,
    sbsit_configuration = 2,

    /* Borland C++ datatype alignment correction */
    sbsit_BCppDummy     = 0xFFFFFFFF
} EC_T_eSBSlaveInfoType;

typedef struct _EC_T_SB_SLAVEINFO_EEP_REQ_DESC
{
    EC_T_eSBSlaveInfoType       eSbSlaveInfoType;
    EC_T_DWORD                  wAutoIncAddress;
    EC_T_eEEPENTRY              eEEPROMEntry;
} EC_T_SB_SLAVEINFO_EEP_REQ_DESC;

typedef struct _EC_T_SB_SLAVEINFO_EEP_RES_DESC
{
    EC_T_DWORD                  dwScanBusStatus;
    EC_T_eEEPENTRY              eEEPROMEntry;
    EC_T_DWORD                  dwEEPROMValue;
} EC_PACKED EC_T_SB_SLAVEINFO_EEP_RES_DESC;

typedef enum _EC_T_eINFOENTRY
{
    eie_unknown         =  0,       /** nothing / invalid */
    eie_pdoffs_in       =  1,       /** get process data offset of Input data (in Bits) */
    eie_pdsize_in       =  2,       /** get process data size of Input Data (in Bits) */
    eie_pdoffs_out      =  3,       /** get process data offset of Output data (in Bits) */
    eie_pdsize_out      =  4,       /** get process data size of Output Data (in Bits) */
    eie_phys_address    =  5,       /** get slave phys Address */
    eie_portstate       =  6,       /** get port link state (SB Instance) */
    eie_dcsupport       =  7,       /** slave does support DC */
    eie_dc64support     =  8,       /** slave does support 64Bit DC */
    eie_alias_address   =  9,       /** get slave alias address */
    eie_cfgphy_address  = 10,       /** get slave configured phys address */
    eie_device_name     = 11,       /** get slave name from configuration */
    eie_ismailbox_slave = 12,       /** get whether slave support mailboxes */
    eie_pdoffs_in2      = 21,       /** get process data offset of Input data (in Bits) */
    eie_pdsize_in2      = 22,       /** get process data size of Input Data (in Bits) */
    eie_pdoffs_out2     = 23,       /** get process data offset of Output data (in Bits) */
    eie_pdsize_out2     = 24,       /** get process data size of Output Data (in Bits) */
    eie_pdoffs_in3      = 31,       /** get process data offset of Input data (in Bits) */
    eie_pdsize_in3      = 32,       /** get process data size of Input Data (in Bits) */
    eie_pdoffs_out3     = 33,       /** get process data offset of Output data (in Bits) */
    eie_pdsize_out3     = 34,       /** get process data size of Output Data (in Bits) */
    eie_pdoffs_in4      = 41,       /** get process data offset of Input data (in Bits) */
    eie_pdsize_in4      = 42,       /** get process data size of Input Data (in Bits) */
    eie_pdoffs_out4     = 43,       /** get process data offset of Output data (in Bits) */
    eie_pdsize_out4     = 44,       /** get process data size of Output Data (in Bits) */
    
    eie_mbx_outsize     = 45,       /** get out mailbox 1 size */
    eie_mbx_insize      = 46,       /** get in mailbox 1 size */
    eie_mbx_outsize2    = 47,       /** get out mailbox 2 size */
    eie_mbx_insize2     = 48,       /** get in mailbox 2 size */

    eie_isoptional      = 49,       /** is slave optional */
    eie_ispresent       = 50,       /** is slave present on bus */

    eie_esctype         = 51,       /** Type of ESC controller */
    /* Borland C++ datatype alignment correction */
    eie_BCppDummy       = 0xFFFFFFFF
} EC_T_eINFOENTRY;

typedef struct _EC_T_SB_SLAVEINFO_REQ_DESC
{
    EC_T_eINFOENTRY eInfoEntry;
    EC_T_WORD       wAutoIncAddress;
} EC_PACKED EC_T_SB_SLAVEINFO_REQ_DESC;

typedef struct _EC_T_SB_SLAVEINFO_RES_DESC
{
    EC_T_eINFOENTRY eInfoEntry;
    EC_T_DWORD      dwInfoLength;
    EC_T_PBYTE      pbyInfo;
} EC_PACKED EC_T_SB_SLAVEINFO_RES_DESC;

typedef struct _EC_T_DCL_ENABLE_DESC
{
    EC_T_DWORD	dwVerifySyncLatchConfiguration;
    EC_T_DWORD	dwDCLInitTimeout;
} EC_PACKED EC_T_DCL_ENABLE_DESC;


/* descriptor for EC_IOCTL_GET_CYCLIC_CONFIG_INFO call */
typedef struct _EC_T_CYC_CONFIG_DESC
{
    EC_T_DWORD      dwNumCycEntries;    /* total number of cyclic entries */
    EC_T_DWORD      dwTaskId;           /* task id of selected cyclic entry */
    EC_T_DWORD      dwPriority;         /* priority of selected cyclic entry */
    EC_T_DWORD      dwCycleTime;        /* cycle time of selected cyclic entry */
} EC_PACKED EC_T_CYC_CONFIG_DESC;

typedef EC_T_DWORD (*EC_PF_TIMESTAMP)(              EC_T_PVOID      pCallerData,
                                                    EC_T_DWORD*     pdwHostTimeLo   );
                                                    
/* Structure carrying instantaneous values for Master Sync */
typedef struct _EC_T_ACTUALVAL
{
    EC_T_DWORD	dwBeginUpdateCnt;               /*< out call counter incremented each time structure
                                                 *      update is started */
    EC_T_DWORD  dwEndUpdateCnt;                 /*< out call counter incremented each time structure
                                                 *      update is finished */
    EC_T_DWORD	dwBusSyncFrameSendTimeLo;       /*< out Host stamped time (result from pfTimeStamp) LSDW */
    EC_T_DWORD	dwBusTimeHi;                    /*< out Bus time (result from ARMW) MSDW */
    EC_T_DWORD	dwBusTimeLo;                    /*< out Bus time (result from ARMW) LSDW */
    EC_T_DWORD	dwBusSyncFramePostSendTimeLo;   /*< out Host stamped time (result from pfTimeStamp) LSDW */
    EC_T_DWORD	dwSyncPulseGridOffsetHi;        /*< out Sync Pulse Offset (initial grid offset) */
    EC_T_DWORD	dwSyncPulseGridOffsetLo;        /*< out Sync Pulse Offset (initial grid offset) */
    EC_T_BOOL	bDcmCtlInSync;                  /*< in  DCM controller has synchronized with DC ref.clock */
    EC_T_DWORD	dwSyncPeriodLength;             /*< out bus cycle time in nsec */
    EC_T_DWORD	dwRes;                          /*< out reserved */
    EC_T_DWORD  dwLastTimeStampResult;          /*< out Last result of call to registered callback 
                                                 *      function EC_PF_TIMESTAMP */
    EC_T_BOOL   bSlavesInSync;                  /*< out Slaves are in sync = EC_TRUE; out of sync = EC_FALSE; */
    EC_T_BOOL   bResetRequest;                  /*< out If EC_TRUE, master request DCM controller reset (e.g. if reference clock disappears) */
} EC_PACKED EC_T_ACTUALVAL;

/* Controller client register parameters */
typedef struct _EC_T_REGISTER_TSPARMS
{
    EC_T_VOID*	pCallerData;                    /*< in  used by all callback functions */  
    EC_PF_TIMESTAMP	pfTimeStamp;                /*< in  timestamp callback function pointer */
    EC_T_DWORD	dwUpdateMultiplier;             /*< in  Interval multiplier */
    EC_T_DWORD	dwEnableHardRealtime;           /*< in  enable hard real time in link layer */
    EC_T_DWORD	aReserved[10];                  /*< in  reserved for future use */
} EC_PACKED EC_T_REGISTER_TSPARMS;

/* Controller client register result */
typedef struct _EC_T_REGISTER_TSRESULTS
{
    EC_T_DWORD          dwHardRealtimeEnabled;  /*< out Hard real time is used */
    EC_T_ACTUALVAL*     pActualValues;          /*< out pointer to data carrying current data */
} EC_PACKED EC_T_REGISTER_TSRESULTS;

typedef struct _EC_T_LINKLAYER_MODE_DESC
{
    EC_T_LINKMODE	eLinkMode;                  /*< out Linkmode of LL */
    EC_T_LINKMODE	eLinkModeRed;               /*< out Linkmode of RedLL */
} EC_PACKED EC_T_LINKLAYER_MODE_DESC;

typedef struct _EC_T_SLVSTATISTICS_DESC
{
    EC_T_WORD       wRxErrorCounter[ESC_PORT_COUNT];     /*< out RX Error Counters per Slave Port */
    EC_T_BYTE       byFwdRxErrorCounter[ESC_PORT_COUNT]; /*< out Forwarded RX Error Counters per Slave Port */
    EC_T_BYTE       byEcatProcUnitErrorCounter;         /*< out EtherCAT Processing unit error counter */
    EC_T_BYTE       byPDIErrorCounter;                  /*< out PDI Error counter */
    EC_T_WORD       wAlStatusCode;                      /*< out AL status code    */
    EC_T_BYTE       byLostLinkCounter[ESC_PORT_COUNT];   /*< out Lost Link Counter per Slave Port */
} EC_PACKED EC_T_SLVSTATISTICS_DESC;

typedef struct _EC_T_SLAVE_PORT_DESC
{
    EC_T_WORD       wSlaveAddress;
    EC_T_WORD       wPortNumber;
} EC_PACKED EC_T_SLAVE_PORT_DESC;

typedef enum _EC_T_EHOTCONNECTMODE
{
    echm_unknown        = 0x0000,
    echm_manual         = 0x0001,
    echm_automatic      = 0x0002,
    echm_fullmanual     = 0x0004,
    echm_automan_mask   = 0x000f,
    echm_borderclose    = 0x0010,

    echm_BCppDummy =   0xFFFFFFFF
} EC_T_EHOTCONNECTMODE;

/* callback pointer for RX frame */ 
typedef EC_T_VOID (*EC_T_PF_RXFRAME_CB)(EC_T_VOID*);

/* ecatGetCfgSlaveInfo */
typedef struct _EC_T_CFG_SLAVE_INFO
{
    EC_T_DWORD                  dwSlaveId;              /*< out Slave ID */
    EC_T_CHAR                   abyDeviceName[ECAT_DEVICE_NAMESIZE]; /*< out Slave name */
    EC_T_DWORD                  dwHCGroupIdx;           /*< out Index of the hot connect group, 0 for mandatory */
    EC_T_BOOL                   bIsPresent;             /*< out Slave is currently present on bus */
    EC_T_BOOL                   bIsHCGroupPresent;      /*< out Slave the hot connect group of the slave is present */

    EC_T_DWORD                  dwVendorId;             /*< out Vendor identification */
    EC_T_DWORD                  dwProductCode;          /*< out Product code */
    EC_T_DWORD                  dwRevisionNumber;       /*< out Revision number */
    EC_T_DWORD                  dwSerialNumber;         /*< out Serial number */

    EC_T_WORD                   wStationAddress;        /*< out Station address */
    EC_T_WORD                   wAutoIncAddress;        /*< out Auto increment address */

    EC_T_DWORD                  dwPdOffsIn;             /*< out Process data offset of input data (in Bits) */
    EC_T_DWORD                  dwPdSizeIn;             /*< out Process data size of input data (in Bits) */
    EC_T_DWORD                  dwPdOffsOut;            /*< out Process data offset of output data (in Bits) */
    EC_T_DWORD                  dwPdSizeOut;            /*< out Process data size of output data */
    EC_T_DWORD                  dwPdOffsIn2;            /*< out Process data offset of input data (in Bits) */
    EC_T_DWORD                  dwPdSizeIn2;            /*< out Process data size of input data (in Bits) */
    EC_T_DWORD                  dwPdOffsOut2;           /*< out Process data offset of output data (in Bits) */
    EC_T_DWORD                  dwPdSizeOut2;           /*< out Process data size of output data */
    EC_T_DWORD                  dwPdOffsIn3;            /*< out Process data offset of input data (in Bits) */
    EC_T_DWORD                  dwPdSizeIn3;            /*< out Process data size of input data (in Bits) */
    EC_T_DWORD                  dwPdOffsOut3;           /*< out Process data offset of output data (in Bits) */
    EC_T_DWORD                  dwPdSizeOut3;           /*< out Process data size of output data */
    EC_T_DWORD                  dwPdOffsIn4;            /*< out Process data offset of input data (in Bits) */
    EC_T_DWORD                  dwPdSizeIn4;            /*< out Process data size of input data (in Bits) */
    EC_T_DWORD                  dwPdOffsOut4;           /*< out Process data offset of output data (in Bits) */
    EC_T_DWORD                  dwPdSizeOut4;           /*< out Process data size of output data */

    EC_T_DWORD                  dwMbxSupportedProtocols;/*< out Supported mailbox protocols */
    EC_T_DWORD                  dwMbxOutSize;           /*< out Mailbox 1 output size */
    EC_T_DWORD                  dwMbxInSize;            /*< out Mailbox 1 input size */
    EC_T_DWORD                  dwMbxOutSize2;          /*< out Mailbox 2 output size */
    EC_T_DWORD                  dwMbxInSize2;           /*< out Mailbox 2 input size */

    EC_T_BOOL                   bDcSupport;             /*< out Slave supports DC */

    EC_T_WORD                   wNumProcessVarsInp;     /*< out Number of output process data variables*/
    EC_T_WORD                   wNumProcessVarsOutp;    /*< out Number of input process data variables */

    EC_T_WORD                   wPrevStationAddress;    /*< Station address of the previous slave */
    EC_T_WORD                   wPrevPort;              /*< Connected port of the previous slave */

    EC_T_WORD                   wIdentifyAdo;           /*< out ADO used for identification command */
    EC_T_WORD                   wIdentifyData;          /*< out Identification value to be validated */
    EC_T_BYTE                   byPortDescriptor;       /*< out Port descriptor (ESC register  0x0007) */

    EC_T_BYTE                   abyReserved[3];
    EC_T_DWORD                  adwReserved[7];
} EC_PACKED EC_T_CFG_SLAVE_INFO;

/* ecatGetBusSlaveInfo */
typedef struct _EC_T_BUS_SLAVE_INFO
{
    EC_T_DWORD                  dwSlaveId;              /*< out Slave ID */
    EC_T_DWORD                  adwPortSlaveIds[ESC_PORT_COUNT]; /*< out Slave IDs connected to ports */
    EC_T_WORD                   wPortState;             /*< out Port state */
    EC_T_WORD                   wAutoIncAddress;        /*< out Auto increment address */
    EC_T_BOOL                   bDcSupport;             /*< out Slave support DC */
    EC_T_BOOL                   bDc64Support;           /*< out Slave support 64Bit DC */

    EC_T_DWORD                  dwVendorId;             /*< out Vendor identification   (EEPROM offset 0x0008) */
    EC_T_DWORD                  dwProductCode;          /*< out Product code            (EEPROM offset 0x000A) */
    EC_T_DWORD                  dwRevisionNumber;       /*< out Revision number         (EEPROM offset 0x000C) */
    EC_T_DWORD                  dwSerialNumber;         /*< out Serial number           (EEPROM offset 0x000E) */

    EC_T_BYTE                   byESCType;              /*< out ESC type                (ESC register  0x0000) */
    EC_T_BYTE                   byESCRevision;          /*< out ESC revision            (ESC register  0x0001) */
    EC_T_WORD                   wESCBuild;              /*< out ESC build               (ESC register  0x0002) */
    EC_T_BYTE                   byPortDescriptor;       /*< out Port descriptor         (ESC register  0x0007) */
    EC_T_BYTE                   byReserved;
    EC_T_WORD                   wFeaturesSupported;     /*< out Features supported      (ESC register  0x0008) */
    EC_T_WORD                   wStationAddress;        /*< out Station address         (ESC register  0x0010) */
    EC_T_WORD                   wAliasAddress;          /*< out Alias address           (ESC register  0x0012) */
    EC_T_WORD                   wAlStatus;              /*< out AL status               (ESC register  0x0130) */
    EC_T_WORD                   wAlStatusCode;          /*< out AL status code          (ESC register  0x0132) */
    EC_T_DWORD                  dwSystemTimeDifference; /*< out System time difference  (ESC register  0x092C) */
    EC_T_WORD                   wMbxSupportedProtocols; /*< out Support mailbox         (EEPROM offset 0x001C) */
    EC_T_WORD                   wDlStatus;              /*< out DL status               (ESC register  0x0110) */
    EC_T_DWORD                  adwReserved[9];

} EC_PACKED EC_T_BUS_SLAVE_INFO;

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

EC_T_CHAR*  ecatGetTextInt(                EC_T_WORD               wTextId         );

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* INC_ECINTERFACECOMMON */

/*-END OF SOURCE FILE--------------------------------------------------------*/

