/*-----------------------------------------------------------------------------
 * EcObjDef.h	        file
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Willig, Andreas
 * Description              description of file
 * Date                     2007/10/16::10:03
 *---------------------------------------------------------------------------*/

#if !(defined __ECOBJDEF_H__)
#define __ECOBJDEF_H__      1

/*-INCLUDES------------------------------------------------------------------*/
#ifndef INC_ECOS
#include <EcOs.h>
#endif
#ifndef INC_ECTYPE
#include <EcType.h>
#endif

/*-DEFINES-------------------------------------------------------------------*/

/* configuration for OD */
#define HISTORY_OBJECT_DIAGELE_SIZE     ((EC_T_WORD)0x0100)     /* size in bytes */

#define MAX_SLAVE_DEVICENAME            ((EC_T_BYTE)0x50)       /* 0x3xxx Slave Device name len */

#define EC_OD_MAX_DRIVER_IDENT_LEN     40                      /* maximum length of link layer name */

/*EC_T_SDOINFOOBJDESC*/
#define	OBJFLAGS_MAXSUBINDEXMASK      0x00FF			
#define	OBJFLAGS_MAXSUBINDEXSHIFT     0			
#define	OBJFLAGS_OBJCODEMASK          0x0F00			
#define	OBJFLAGS_OBJCODESHIFT         8			
#define	OBJCODE_VAR                   0x07
#define	OBJCODE_ARR                   0x08
#define	OBJCODE_REC                   0x09

/*EC_T_SDOINFOENTRYDESC*/
#define	ACCESS_READWRITE              0x003F
#define	ACCESS_READ                   0x0007
#define	ACCESS_READ_PREOP             0x0001
#define	ACCESS_READ_SAFEOP            0x0002
#define	ACCESS_READ_OP                0x0004
#define	ACCESS_WRITE                  0x0038
#define	ACCESS_WRITE_PREOP            0x0008
#define	ACCESS_WRITE_SAFEOP           0x0010
#define	ACCESS_WRITE_OP               0x0020
#define	OBJACCESS_NOPDOMAPPING        0x0000
#define	OBJACCESS_RXPDOMAPPING        0x0040
#define	OBJACCESS_TXPDOMAPPING        0x0080
#define	OBJACCESS_BACKUP              0x0100
#define	OBJACCESS_SETTINGS            0x0200
#define	OBJACCESS_FORCE               0x8000
                                    
#define   DEFTYPE_BUSDIAGNOSTIC     0x0040
#define   DEFTYPE_MACADDRESS        0x0041
#define   DEFTYPE_MASTERINITPARM    0x0042
#define   DEFTYPE_SLAVECFGINFO      0x0043
#define   DEFTYPE_BUSLOADBASE       0x0044
#define   DEFTYPE_SLAVECFG          0x0045
#define   DEFTYPE_SLAVEINF          0x0046
#define   DEFTYPE_SLAVEDIAG         0x0047
#define   DEFTYPE_DEVICEPROFILE     0x0048
#define   DEFTYPE_DETECTMODCMD      0x0049
#define   DEFTYPE_CONFADDRLIST      0x0050
#define   DEFTYPE_CONNADDRLIST      0x0051

#define COEOBJID_0x800              ((EC_T_WORD)0x0800)
#define COEOBJID_DEVICE_TYPE        ((EC_T_WORD)0x1000)
#define COEOBJID_MANF_DEVICE_NAME   ((EC_T_WORD)0x1008)
#define COEOBJID_MANF_HW_VERSION    ((EC_T_WORD)0x1009)
#define COEOBJID_MANF_SW_VERSION    ((EC_T_WORD)0x100A)
#define COEOBJID_IDENTITY_OBJECT    ((EC_T_WORD)0x1018)
#define COEOBJID_HISTORY_OBJECT     ((EC_T_WORD)0x10F3)

#define COEOBJID_MAST_STATECHNG     ((EC_T_WORD)0x2000)
#define COEOBJID_MAST_STATESUMMARY  ((EC_T_WORD)0x2001)
#define COEOBJID_BUS_DIAGNOSIS      ((EC_T_WORD)0x2002)
#define COEOBJID_MAC_ADDRESS        ((EC_T_WORD)0x2005)
#define COEOBJID_DEBUG_REGISTER     ((EC_T_WORD)0x2010)
#define COEOBJID_MASTER_INIT_PARM   ((EC_T_WORD)0x2020)
#define COEOBJID_DC_DEVIATION_LIMIT ((EC_T_WORD)0x2100)
#define COEOBJID_DC_CURDEVIATION    ((EC_T_WORD)0x2101)


#define COEOBJID_BUSLOAD_BASE       ((EC_T_WORD)0x2200)
#define COEOBJID_BUSLOAD_HIST       ((EC_T_WORD)0x2201)

#define COEOBJID_SLAVECFGINFOBASE   ((EC_T_WORD)0x3000)

#define COEOBJID_SLAVECFGBASE       ((EC_T_WORD)0x8000) /* configured slave -> "Modular Device Profiles" */
#define COEOBJID_SLAVEINFBASE       ((EC_T_WORD)0x9000) /* connected slave -> "Modular Device Profiles" */
#define COEOBJID_SLAVEDIAGBASE      ((EC_T_WORD)0xA000) /* diagnosis data -> "Modular Device Profiles" */
#define COEOBJID_DEVICEPROFILE      ((EC_T_WORD)0xF000) /* modular device profile -> "Modular Device Profiles" */
#define COEOBJID_DETECTMODCMD       ((EC_T_WORD)0xF002) /* detect modules command -> "Modular Device Profiles" */
#define COEOBJID_CONFADDRLISTBASE   ((EC_T_WORD)0xF020) /* configured address list -> "Modular Device Profiles" */
#define COEOBJID_CONNADDRLISTBASE   ((EC_T_WORD)0xF040) /* detected address list -> "Modular Device Profiles" */

#define COEOBJID_ENDLIST            ((EC_T_WORD)0xFFFF)

#define DEVICETYPE_ETHERNET_GATEWAY     ((EC_T_WORD)1000)
#define DEVICETYPE_ETHERCAT_MASTER      ((EC_T_WORD)1100)
#define DEVICETYPE_ETHERCAT_SLAVE       ((EC_T_WORD)1110)
#define DEVICETYPE_KBUS_MASTER          ((EC_T_WORD)1120)
#define DEVICETYPE_PROFIBUS_MASTER      ((EC_T_WORD)3100)
#define DEVICETYPE_PROFIBUS_SLAVE       ((EC_T_WORD)3110)
#define DEVICETYPE_INTERBUS_MASTER      ((EC_T_WORD)4100)
#define DEVICETYPE_INTERBUS_SLAVE       ((EC_T_WORD)4110)
#define DEVICETYPE_CANOPEN_MASTER       ((EC_T_WORD)5100)
#define DEVICETYPE_CANOPEN_SLAVE        ((EC_T_WORD)5110)
#define DEVICETYPE_DEVICENET_MASTER     ((EC_T_WORD)5200)
#define DEVICETYPE_DEVICENET_SLAVE      ((EC_T_WORD)5210)
#define DEVICETYPE_ASI_MASTER           ((EC_T_WORD)6200)
#define DEVICETYPE_IOLINK_MASTER        ((EC_T_WORD)6220)

/*-PACK SETTINGS-------------------------------------------------------------*/
#if defined  __GNUC__   /* GNU */

#define EC_PACKED1 __attribute__((aligned(1), packed))

#if (defined __RCX__)
#pragma pack (1)
#endif

#elif defined _DIAB_TOOL

#define EC_PACKED1 __attribute__((aligned(1), packed))

#elif defined _MSC_VER  /* MICROSOFT */

#define EC_PACKED1
#pragma pack (push, 1)
#pragma warning(disable:4514)

#elif defined __MET__

#define EC_PACKED1
/*#pragma pack (push, 1)*/
#pragma Push_align_members(1)

#elif defined __ghs

#define EC_PACKED1 __attribute__((aligned(1), packed))
#pragma pack (1)

#elif defined(__BORLANDC__)

#pragma pack (push, 1)
#define EC_PACKED1
	
#else
#error Please check pack settings for this compiler!!!
#endif

/*-TYPEDEFS------------------------------------------------------------------*/


typedef struct _EC_T_OBJ1018
{
    EC_T_WORD   wSubIndex0;         /* Subindex 000 */
    EC_T_DWORD  dwVendorID;         /* Subindex 001 */
    EC_T_DWORD  dwProductcode;      /* Subindex 002 */
    EC_T_DWORD  dwRevision;         /* Subindex 003 */
    EC_T_DWORD  dwSerialnumber;     /* Subindex 004 */
} EC_PACKED1 EC_T_OBJ1018;

#define SETDIAGNUMBER(number) \
    ((EC_T_DWORD)(((((EC_T_DWORD)(number))&0xFFF)|(0xE<<12)|((((((EC_T_DWORD)(number))>>12)==0)?1:((((EC_T_DWORD)(number))>>12)==0xFFFF)?0xFFFE:(((EC_T_DWORD)(number))>>12))<<16))))

#define DIAGFLAGINFO            ((EC_T_WORD)0)
#define DIAGFLAGWARN            ((EC_T_WORD)1)
#define DIAGFLAGERROR           ((EC_T_WORD)2)

#define DIAGPARMTYPEDATATYPE    ((EC_T_WORD)(0<<12))
#define DIAGPARMTYPEBYTEARRAY   ((EC_T_WORD)(1<<12))
#define DIAGPARMTYPEASCIISTRG   ((EC_T_WORD)(2<<12))
#define DIAGPARMTYPEUNICODESTRG ((EC_T_WORD)(3<<12))
#define DIAGPARMTYPETEXTID      ((EC_T_WORD)(4<<12))

#define OBJ10F3DGMSG_TIMSTAMP_OFFSET    0x08

typedef struct _EC_T_OBJ10F3_DIAGMSG
{
    EC_T_DWORD  dwDiagNumber;                           /* 0x00 */
    EC_T_WORD   wFlags;                                 /* 0x04 */
    EC_T_WORD   wTextId;                                /* 0x06 */
#ifdef  __TMS470__
    EC_T_DWORD  dwTimeStampLo;                          /* 0x08 */
    EC_T_DWORD  dwTimeStampHi;                          /* 0x08 */
#else
    EC_T_UINT64 qwTimeStamp;                            /* 0x08 */
#endif
    EC_T_BYTE   byParams[HISTORY_OBJECT_DIAGELE_SIZE];  /* structure data of type:
                                                         * 16 bit flags, byte stream, 16 bit flags, .... 
                                                         */
} EC_PACKED1 EC_T_OBJ10F3_DIAGMSG;

typedef struct _EC_T_OBJ10F3
{
    EC_T_WORD               wSubIndex0;                 /* Subindex 000 */
    EC_T_BYTE               byMaxDiagMessages;          /* Subindex 001 */
    EC_T_BYTE               bySubIndexMessageNew;       /* Subindex 002 */
    EC_T_BYTE               bySubIndexMessageAck;       /* Subindex 003 */
    EC_T_BOOL               bNewDiagMessages;           /* Subindex 004 */
    EC_T_WORD               wFlags;                     /* Subindex 005 */
    EC_T_OBJ10F3_DIAGMSG    aoDiagMessages[249];        /* Subindex 006 - 255 data appended */
} EC_PACKED1 EC_T_OBJ10F3;


typedef enum _EC_T_OBJ2000_VALUES
{
    eMastStChng_init        = 1,
    eMastStChng_preop       = 2,
    eMastStChng_safeop      = 4,
    eMastStChng_op          = 8,

    /* Borland C++ datatype alignment correction */
    eMastStChng_BCppDummy   = 0xFFFFFFFF
} EC_T_OBJ2000_VALUES;

/* 0x2001: Master State Summary (EC_T_DWORD) */
#define OBJ2001_STATE_SUM_MASTER_OK    0x00000001      /* Bit 0: = 1 Master o.k. */
                                                       /* Bit 1-3: Reserved */
#define OBJ2001_STATE_SUM_MASTER_STATE 0x000000F0      /* Bit 4-7: Master State */
#define OBJ2001_STATE_SUM_SLAVE_REQ    0x00000100      /* Bit 8: Slaves in requested State */
#define OBJ2001_STATE_SUM_MASTER_REQ   0x00000200      /* Bit 9: Master in requested State */
#define OBJ2001_STATE_SUM_BUS_MATCH    0x00000400      /* Bit 10: Bus Scan Match */
#define OBJ2001_STATE_SUM_RES11        0x00000800      /* Bit 11: Reserved */
#define OBJ2001_STATE_SUM_DC_ENA       0x00001000      /* Bit 12: DC is enabled */
#define OBJ2001_STATE_SUM_DC_SYNC      0x00002000      /* Bit 13: DC In-Sync */
#define OBJ2001_STATE_SUM_DC_BUSY      0x00004000      /* Bit 14: DC Busy */
#define OBJ2001_STATE_SUM_RES15        0x00008000      /* Bit 15: Reserved */
#define OBJ2001_STATE_SUM_LINK_UP      0x00010000      /* Bit 16: Link Up  */
                                                       /* Bit 17-31: Reserved */

                                                             /* dword : .... .... .... ...L  .bID .BMS ssss ...O */
#define OBJ2001_STATE_SUM_MASK1  ((EC_T_DWORD)0x000107f0)    /* mask  : 0000 0000 0000 0001  0000 0111 1111 0000 */
#define OBJ2001_STATE_SUM_VALUE1 ((EC_T_DWORD)0x00010780)    /* value : 0000 0000 0000 0001  0000 0111 1000 0000 */
#define OBJ2001_STATE_SUM_MASK2  ((EC_T_DWORD)0x00007000)    /* mask2 : 0000 0000 0000 0000  0111 0000 0000 0000 */
#define OBJ2001_STATE_SUM_VALUE2 ((EC_T_DWORD)0x00003000)    /* value : 0000 0000 0000 0000  0011 0000 0000 0000 */

/* 0x2002: Bus Diagnosis Object */
#define OBJ2002_TXFRM_OFFSET    0x18
#define OBJ2002_RXFRM_OFFSET    0x1C
#define OBJ2002_LOSFRM_OFFSET   0x20
#define OBJ2002_CYCFRM_OFFSET   0x24
#define OBJ2002_CYCDGR_OFFSET   0x28
#define OBJ2002_ACYCFRM_OFFSET  0x2C
#define OBJ2002_ACYCDGR_OFFSET  0x30

typedef struct _EC_T_OBJ2002
{
    EC_T_WORD               wSubIndex0;             /* 0x00 */  /* Subindex 000 */
    EC_T_WORD               wReserved;              /* 0x02 */  /* Subindex 001 */
    EC_T_DWORD              dwCRC32ConfigCheckSum;  /* 0x04 */  /* Subindex 002 */
    EC_T_DWORD              dwNumSlavesFound;       /* 0x08 */  /* Subindex 003 */
    EC_T_DWORD              dwNumDCSlavesFound;     /* 0x0C */  /* Subindex 004 */
    EC_T_DWORD              dwNumCfgSlaves;         /* 0x10 */  /* Subindex 005 */
    EC_T_DWORD              dwNumMbxSlaves;         /* 0x14 */  /* Subindex 006 */
    
    EC_T_DWORD              dwTXFrames;             /* 0x18 */  /* Subindex 007 */
    EC_T_DWORD              dwRXFrames;             /* 0x1C */  /* Subindex 008 */
    EC_T_DWORD              dwLostFrames;           /* 0x20 */  /* Subindex 009 */
    
    EC_T_DWORD              dwCyclicFrames;         /* 0x24 */  /* Subindex 010 */
    EC_T_DWORD              dwCyclicDatagrams;      /* 0x28 */  /* Subindex 011 */
    EC_T_DWORD              dwAcyclicFrames;        /* 0x2C */  /* Subindex 012 */
    EC_T_DWORD              dwAcyclicDatagrams;     /* 0x30 */  /* Subindex 013 */
    EC_T_DWORD              dwClearCounters;        /* 0x34 */  /* Subindex 014 */
} EC_PACKED1 EC_T_OBJ2002;

typedef struct _EC_T_OBJ2005
{
    EC_T_WORD               wSubIndex0;             /* Subindex 000 */
    EC_T_BYTE               abyHardware[6];         /* Subindex 001 */
    EC_T_BYTE               abyRedHardware[6];      /* Subindex 002 */
    EC_T_BYTE               abyCfgSource[6];        /* Subindex 003 */
    EC_T_BYTE               abyCfgDestination[6];   /* Subindex 004 */
} EC_PACKED1 EC_T_OBJ2005;

typedef struct _EC_T_OBJ2020
{
    EC_T_WORD               wSubIndex0;             /* Subindex 000 */
    EC_T_DWORD              dwApplicationVersion;   /* Subindex 001 */
    EC_T_DWORD              dwMasterVersion;        /* Subindex 002 */
    EC_T_DWORD              dwSlaveMultiplier;      /* Subindex 003 */
    EC_T_DWORD              dwEcatCmdTimeout;       /* Subindex 004 */
    EC_T_DWORD              dwEcatCmdMaxRetries;    /* Subindex 005 */
    EC_T_DWORD              dwBusCycleTimeUsec;     /* Subindex 006 */
    EC_T_DWORD              dwEoeTimeout;           /* Subindex 007 */
    EC_T_DWORD              dwFoeBusyTimeout;       /* Subindex 008 */
    EC_T_DWORD              dwMaxQueuedEthFrames;   /* Subindex 009 */
    EC_T_DWORD              dwMaxSlaveCmdPerFrame;  /* Subindex 010 */
    EC_T_DWORD              dwReserved1;            /* Subindex 011 */
    EC_T_DWORD              dwReserved2;            /* Subindex 012 */
    EC_T_DWORD              dwStateChangeDebug;     /* Subindex 013 */
    EC_T_CHAR               szDriverIdent[EC_OD_MAX_DRIVER_IDENT_LEN];
                                                    /* Subindex 014 */
    EC_T_BOOL               bPollingModeActive;     /* Subindex 015 */
    EC_T_BOOL               bAllocSendFrameActive;  /* Subindex 016 */
} EC_PACKED1 EC_T_OBJ2020;

typedef struct _EC_T_OBJ2200
{
    EC_T_WORD               wSubIndex0;             /* Subindex 000 */

    EC_T_DWORD              dwBytesPerSecondAct;    /* Subindex 001: TX bytes/second actual value */
    EC_T_DWORD              dwBytesPerSecondMin;    /* Subindex 002: TX bytes/second min. value   */
    EC_T_DWORD              dwBytesPerSecondMax;    /* Subindex 003: TX bytes/second max. value   */

    EC_T_DWORD              dwBytesPerCycleAct;     /* Subindex 004: TX bytes/cycle actual value  */
    EC_T_DWORD              dwBytesPerCycleMin;     /* Subindex 005: TX bytes/cycle min. value    */
    EC_T_DWORD              dwBytesPerCycleMax;     /* Subindex 006: TX bytes/cycle max. value    */
} EC_PACKED1 EC_T_OBJ2200;

typedef struct _EC_T_OBJ3XXX
{
    EC_T_WORD               wSubIndex0;                         /* Subindex 000 */
    EC_T_BOOL               bEntryValid;                        /* Subindex 001 */
    EC_T_DWORD              dwVendorID;                         /* Subindex 002 */
    EC_T_DWORD              dwProductCode;                      /* Subindex 003 */
    EC_T_DWORD              dwRevisionNo;                       /* Subindex 004 */
    EC_T_DWORD              dwSerialNo;                         /* Subindex 005 */

    EC_T_CHAR               szDeviceName[MAX_SLAVE_DEVICENAME]; /* Subindex 006 */
    EC_T_WORD               wAutoIncAddr;                       /* Subindex 007 */
    EC_T_WORD               wPhysAddr;                          /* Subindex 008 */
    EC_T_WORD               wConfigPhysAddr;                    /* Subindex 009 */
    EC_T_WORD               wAliasAddr;                         /* Subindex 010 */
    EC_T_WORD               wPortState;                         /* Subindex 011 */
    EC_T_BOOL               bDCSupport;                         /* Subindex 012 */
    EC_T_BOOL               bDC64Support;                       /* Subindex 013 */
    EC_T_BOOL               bMailboxSupport;                    /* Subindex 014 */
    EC_T_DWORD              dwReqState;                         /* Subindex 015 */
    EC_T_DWORD              dwCurState;                         /* Subindex 016 */
    EC_T_BOOL               bErrFlagSet;                        /* Subindex 017 */
    EC_T_BOOL               bEnableLinkMsgs;                    /* Subindex 018 */
    EC_T_DWORD              dwErrorCode;                        /* Subindex 019 */
    EC_T_BOOL               bSyncPulseActive;                   /* Subindex 020 */
    EC_T_DWORD              dwDCSync0Period;                    /* Subindex 021 */
    EC_T_DWORD              dwDCSync1Period;                    /* Subindex 022 */
    EC_T_DWORD              dwSBErrorCode;                      /* Subindex 023 */

    EC_T_WORD               wRxErrorCounter0;                   /* Subindex 024 */
    EC_T_WORD               wRxErrorCounter1;                   /* Subindex 025 */
    EC_T_WORD               wRxErrorCounter2;                   /* Subindex 026 */
    EC_T_WORD               wRxErrorCounter3;                   /* Subindex 027 */
    EC_T_BYTE               byFwdRxErrorCounter0;               /* Subindex 028 */
    EC_T_BYTE               byFwdRxErrorCounter1;               /* Subindex 029 */
    EC_T_BYTE               byFwdRxErrorCounter2;               /* Subindex 030 */
    EC_T_BYTE               byFwdRxErrorCounter3;               /* Subindex 031 */
    
    EC_T_BYTE               byEcatProcUnitErrorCounter;         /* Subindex 032 */
    EC_T_BYTE               byPDIErrorCounter;                  /* Subindex 033 */
    EC_T_WORD               wMbxSupportedProtocols;             /* Subindex 034 */
    EC_T_BYTE               byLostLinkCounter0;                 /* Subindex 035 */
    EC_T_BYTE               byLostLinkCounter1;                 /* Subindex 036 */
    EC_T_BYTE               byLostLinkCounter2;                 /* Subindex 037 */
    EC_T_BYTE               byLostLinkCounter3;                 /* Subindex 038 */
    EC_T_BYTE               byFmmusSupported;                   /* Subindex 039 */
    EC_T_BYTE               bySyncManagersSupported;            /* Subindex 040 */
    EC_T_BYTE               byRamSizeKb;                        /* Subindex 041 */
    EC_T_BYTE               byPortDescriptor;                   /* Subindex 042 */

    EC_T_BYTE               byESCType;                          /* Subindex 043 */
    
    EC_T_BOOL               bSlaveIsOptional;                   /* Subindex 044 */
    EC_T_BOOL               bSlaveIsPresent;                    /* Subindex 045 */
    EC_T_DWORD              dwHotConnectGroupId;                /* Subindex 046 */
    EC_T_DWORD              dwSystemTimeDifference;             /* Subindex 047 */
} EC_PACKED1 EC_T_OBJ3XXX;

/* Modular Device Profiles: EtherCAT Master  - internal slave object element (configured slaves ) */
typedef struct _EC_T_OBJ8XXX
{
    EC_T_WORD               wSubIndex0;                         /*   0 */       /* Subindex 000 */
    EC_T_WORD               wFixedStationAddr;                  /*   2 */       /* Subindex 001: Station Address of the first EtherCAT Slave (same value as 0xF020:01) */
    EC_T_CHAR               szType[64];                         /*   4 */       /* Subindex 002: Type of the first EtherCAT Slave configured */
    EC_T_CHAR               szName[64];                         /*  68 */       /* Subindex 003: Name of the first EtherCAT Slave configured (object 0x1008 of the EtherCAT slave) */
    EC_T_DWORD              dwDeviceType;                       /* 132 */       /* Subindex 004: Device Type of the first EtherCAT Slave configured (object 0x1000 of the EtherCAT slave) */
    EC_T_DWORD              dwVendorID;                         /* 136 */       /* Subindex 005: Vendor ID of the first EtherCAT Slave configured (entry 0x1018:01 of the EtherCAT slave) */
    EC_T_DWORD              dwProductCode;                      /* 140 */       /* Subindex 006: Product Code of the first EtherCAT Slave configured (entry 0x1018:02 of the EtherCAT slave) */
    EC_T_DWORD              dwRevision;                         /* 144 */       /* Subindex 007: Revision of the first EtherCAT Slave configured (entry 0x1018:03 of the EtherCAT slave) */
    EC_T_DWORD              dwSerial;                           /* 148 */       /* Subindex 008: Serial No of the first EtherCAT Slave configured (entry 0x1018:04 of the EtherCAT slave) */
    EC_T_WORD               wMailboxOutSize;                    /* 152 */       /* Subindex 033: Mailbox Write Size (SM0) of the first EtherCAT Slave configured */
    EC_T_WORD               wMailboxInSize;                     /* 154 */       /* Subindex 034: Mailbox Read Size (SM1) of the first EtherCAT Slave configured */
} EC_PACKED1 EC_T_OBJ8XXX;              

/* Modular Device Profiles: EtherCAT Master  - internal slave object element (connected slaves) */
typedef struct _EC_T_OBJ9XXX
{
    EC_T_WORD               wSubIndex0;                         /*   0 */       /* Subindex 000 */
    EC_T_WORD               wFixedStationAddr;                  /*   2 */       /* Subindex 001: Fixed Station Address of the first EtherCAT Slave (same value as 0xF020:01) */
    EC_T_DWORD              dwVendorID;                         /*   4 */       /* Subindex 005: Vendor ID of the first EtherCAT Slave configured (entry 0x1018:01 of the EtherCAT slave) */
    EC_T_DWORD              dwProductCode;                      /*   8 */       /* Subindex 006: Product Code of the first EtherCAT Slave configured (entry 0x1018:02 of the EtherCAT slave) */
    EC_T_DWORD              dwRevision;                         /*  12 */       /* Subindex 007: Revision of the first EtherCAT Slave configured (entry 0x1018:03 of the EtherCAT slave) */
    EC_T_DWORD              dwSerial;                           /*  16 */       /* Subindex 008: Serial No. of the first EtherCAT Slave configured (entry 0x1018:04 of the EtherCAT slave) */
    EC_T_WORD               wDLStatus;                          /*  20 */       /* Subindex 032: DL Status (Register 0x110-0x111) of the first EtherCAT found */
} EC_PACKED1 EC_T_OBJ9XXX;              

/* Modular Device Profiles: EtherCAT Master  - internal slave object element (slave diagnosis) */
typedef struct _EC_T_OBJAXXX
{
    EC_T_WORD               wSubIndex0;                         /*   0 */       /* Subindex 000 */
    EC_T_WORD               wALStatus;                          /*   2 */       /* Subindex 001: AL Status (Register 0x130-0x131) of the first EtherCAT slave configured */
    EC_T_WORD               wALControl;                         /*   4 */       /* Subindex 002: AL Control (Register 0x120-0x121) of the first EtherCAT slave configured */
} EC_PACKED1 EC_T_OBJAXXX;              

/* Modular Device Profiles: EtherCAT Master  - modular device profile */
typedef struct _EC_T_OBJF000
{
    EC_T_WORD               wSubIndex0;                         /*   0 */       /* Subindex 000 */
    EC_T_WORD               wIndexDistance;                     /*   2 */       /* Subindex 001: Index distance between two modules = 0x01 */
    EC_T_WORD               wMaxModuleCnt;                      /*   4 */       /* Subindex 002: Maximum number of EtherCAT Slaves connected to the EtherCAT Master = 4080 */
    EC_T_DWORD              dwGeneralCfg;                       /*   8 */       /* Subindex 003: Available entries in objects 0x8nn0 = 0x000000FF */
    EC_T_DWORD              dwGeneralInfo;                      /*  12 */       /* Subindex 004: Available entries in objects 0x9nn0 = 0x000000F1 (if information data supported) or 0x00000000 (if information data not supported) */
} EC_PACKED1 EC_T_OBJF000;              

/* Modular Device Profiles: EtherCAT Master  - detect modules command */
typedef struct _EC_T_OBJF002
{
    EC_T_WORD               wSubIndex0;                         /*   0 */       /* Subindex 000 */
    EC_T_BYTE               abyCmdRequest[2];                   /*   1 */       /* Subindex 001: When this subindex is written, the EtherCAT Master shall scan the EtherCAT bus and update the objects 0xF04x and 0x9nnn */
    EC_T_BYTE               byCmdStatus;                        /*   3 */       /* Subindex 002: 1: command is finished, no error
                                                                                                 3: command is finished, error
                                                                                           100-199: 0-99% of the command is done
                                                                                               255: command is executing */
    EC_T_BYTE               abyCmdResponse[6];                  /*   4 */       /* Subindex 003: Byte 0: like Subindex 2
                                                                                                 Byte 1: always 0
                                                                                                 Byte 2-3: 0: no error, > 0: vendor specific error code
                                                                                                 Byte 4-5: number of EtherCAT slaves found */
} EC_PACKED1 EC_T_OBJF002;              

/* Modular Device Profiles: EtherCAT Master  - configured address list */
typedef struct _EC_T_OBJF02X
{
    EC_T_WORD               wSubIndex0;                         /*   0 */       /* Subindex 000 */
    EC_T_WORD               wStationAddr[255];                  /*   2 */       /* Subindex 001 - 255 */
} EC_PACKED1 EC_T_OBJF02X;              

/* Modular Device Profiles: EtherCAT Master  - detected address list */
typedef struct _EC_T_OBJF04X
{
    EC_T_WORD               wSubIndex0;                         /*   0 */       /* Subindex 000 */
    EC_T_WORD               wStationAddr[255];                  /*   2 */       /* Subindex 001 - 255 */
} EC_PACKED1 EC_T_OBJF04X;  


/*-PACK SETTINGS-------------------------------------------------------------*/
#if defined  __GNUC__   /* GNU */

#undef  EC_PACKED1

#if (defined __RCX__)
#pragma pack ()
#endif

#elif defined _MSC_VER  /* MICROSOFT */

#undef  EC_PACKED1
#pragma pack (pop)

#elif defined __MET__

#undef EC_PACKED1
/*#pragma pack (pop)*/
#pragma Pop_align_members()

#elif defined __ghs

#undef EC_PACKED
#pragma pack()
		
#elif defined(__BORLANDC__)

#pragma pack (pop) 
#undef EC_PACKED1
	
#else
#endif

/*-CLASS---------------------------------------------------------------------*/

/*-FUNCTION DECLARATION------------------------------------------------------*/

/*-LOCAL VARIABLES-----------------------------------------------------------*/

/*-FUNCTIONS-----------------------------------------------------------------*/


#endif /* __ECOBJDEF_H__ */ 

/*-END OF SOURCE FILE--------------------------------------------------------*/
