/*-----------------------------------------------------------------------------
 * EcLink.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master link layer interface
 *---------------------------------------------------------------------------*/

#ifndef INC_ECLINK
#define INC_ECLINK

/*-INCLUDES------------------------------------------------------------------*/

#ifndef INC_ECTYPE
#include <EcType.h>
#endif
#ifndef INC_ECOS
#include <EcOs.h>
#endif
#ifndef INC_ECINTERFACECOMMON
#include "EcInterfaceCommon.h"
#endif

#ifndef ATEMLL_API
#define ATEMLL_API
#endif

#ifdef __cplusplus
extern "C" {
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


/*-DEFINES-------------------------------------------------------------------*/

#define LINK_LAYER_DRV_DESC_PATTERN         0x11aaddaa
#define LINK_LAYER_DRV_DESC_VERSION         0x00020200      /* version 2.2 */

#define LINK_LAYER_DEV_PARAM_PATTERN        0x11aaaaaa
#define LINK_LAYER_DEV_PARAM_VERSION        0x00020000      /* version 2.0 */

#define MAX_DRIVER_IDENT_LEN                40              /* maximum length for a driver ident string */

#define EC_LINKUNIT_PCILOCATION             0x01000000      /* if the MSB of dwUnit is set to this value, the 3LSB mean Bus-Dev-Func */

#define EC_LINKNOTIFY_GENERIC               0x00000000
#define EC_LINKNOTIFY_LINKSTATECHGD         (EC_LINKNOTIFY_GENERIC | 0x1)   /* not used within AT-EM */
#define EC_LINKNOTIFY_TXMEMORYCHGD          (EC_LINKNOTIFY_GENERIC | 0x2)   /* not used within AT-EM */

#define EC_LINKIOCTL_GENERIC                0x00000000
#define EC_LINKIOCTL_INTERRUPTENABLE        (EC_LINKIOCTL_GENERIC  | 0x1)   /* not used within AT-EM */
#define EC_LINKIOCTL_PROMISCOUSMODE         (EC_LINKIOCTL_GENERIC  | 0x2)   /* not used within AT-EM */
#define EC_LINKIOCTL_SETMULTICASTADDR       (EC_LINKIOCTL_GENERIC  | 0x3)   /* not used within AT-EM */
#define EC_LINKIOCTL_CLRMULTICASTADDR       (EC_LINKIOCTL_GENERIC  | 0x4)   /* not used within AT-EM */
#define EC_LINKIOCTL_SETLINKMODE            (EC_LINKIOCTL_GENERIC  | 0x5)   /* not used within AT-EM */
#define EC_LINKIOCTL_GETLINKMODE            (EC_LINKIOCTL_GENERIC  | 0x6)   /* not used within AT-EM */
#define EC_LINKIOCTL_RESTART                (EC_LINKIOCTL_GENERIC  | 0x7)   /* not used within AT-EM */
#define EC_LINKIOCTL_SET_LINKMODE           (EC_LINKIOCTL_GENERIC  | 0x8)   /* not used within AT-EM: set mode to polling or interrupt */
#define EC_LINKIOCTL_SET_ALLOC_SENDFRAME    (EC_LINKIOCTL_GENERIC  | 0x9)   /* not used within AT-EM: for debug purposes: enable/disable EcLinkAllocSendFrame() support */
#define EC_LINKIOCTL_FORCELINKMODE          (EC_LINKIOCTL_GENERIC  | 0xA)   /* not used within AT-EM: Bit 0=Autoneg.;1=FullDuplex;16-31=LinkSpeed 1=10,2=100,3=1000MBit */
#define EC_LINKIOCTL_GETINFOLIST            (EC_LINKIOCTL_GENERIC  | 0xB)   /* not used within AT-EM: query EC_T_LINK_INFOLIST */

/* The following IO-Controls are mandatory for LinkLayer in interrupt mode */
#define EC_LINKIOCTL_REGISTER_FRAME_CLBK    (EC_LINKIOCTL_GENERIC  | 0x10)  /* registers the receive frame callback */
#define EC_LINKIOCTL_UNREGISTER_FRAME_CLBK  (EC_LINKIOCTL_GENERIC  | 0x11)  /* unregisters the receive frame callback */

#define EC_LINKIOCTL_INTSTATUS_READ         (EC_LINKIOCTL_GENERIC  | 0x12)  /* not used within AT-EM: read card's interrupt status register */
#define EC_LINKIOCTL_INTSTATUS_WRITE        (EC_LINKIOCTL_GENERIC  | 0x13)  /* not used within AT-EM: write card's interrupt status register */
#define EC_LINKIOCTL_FEATURECONTROL         (EC_LINKIOCTL_GENERIC  | 0x14)  /* not used within AT-EM: Bit 0=JumboFrame */
#define EC_LINKIOCTL_UPDATE_LINKSTATUS      (EC_LINKIOCTL_GENERIC  | 0x15)  /* not used within AT-EM: Update link status (eTSEC only) */
   
/*-TYPEDEFS------------------------------------------------------------------*/

typedef enum _EC_T_LINKSTATUS
{
    eLinkStatus_UNDEFINED = 0,
    eLinkStatus_OK,
    eLinkStatus_DISCONNECTED,
    eLinkStatus_HALFDUPLEX,

    /* Borland C++ datatype alignment correction */
    eLinkStatus_BCppDummy   = 0xFFFFFFFF
} EC_T_LINKSTATUS;

typedef enum _EC_T_ALLOCSENDFRAME_MODE
{
    EcLinkAllocSendFrameMode_UNDEFINED = 0,
    EcLinkAllocSendFrameMode_ENABLED,
    EcLinkAllocSendFrameMode_DISABLED,
    EcLinkAllocSendFrameMode_RANDOM,              /* for debug purposes only! */

    /* Borland C++ datatype alignment correction */
    EcLinkAllocSendFrameMode_BCppDummy   = 0xFFFFFFFF
} EC_T_ALLOCSENDFRAMEMODE;

/* PHY Interface */
typedef enum _EC_T_PHY_INTERFACE
{
    ePHY_UNDEFINED = 0,
    ePHY_FIXED_LINK = 1 << 0,  /* No PHY access at all */
    ePHY_MII = 1 << 1,         /* 10 / 100 MBit */
    ePHY_RMII = 1 << 2,        /* Reduced MII, 10 / 100 MBit */
    ePHY_GMII = 1 << 3,        /* Gigabit MII, 10, 100, 1000 MBit */  
    ePHY_SGMII = 1 << 4,       /* Serial (SERDES) Gigabit MII, 10, 100, 1000 MBit */
    ePHY_RGMII = 1 << 5        /* Reduced Gigabit MII, 10, 100, 1000 MBit */
} EC_T_PHY_INTERFACE;

struct _EC_T_LINK_NOTIFYPARMS;
struct _EC_T_LINK_FRAMEDESC;

typedef EC_T_DWORD (*EC_T_RECEIVEFRAMECALLBACK)(EC_T_VOID* pvContext, struct _EC_T_LINK_FRAMEDESC* pLinkFrameDesc, EC_T_BOOL* pbFrameProcessed);
typedef EC_T_DWORD (*EC_T_LINK_NOTIFY)(EC_T_DWORD dwCode, struct _EC_T_LINK_NOTIFYPARMS* pParms);

/* EtherCAT notify parameters */

typedef struct _EC_T_LINK_NOTIFYPARMS
{
    EC_T_VOID*      pvContext;			/*< in  context */
    EC_T_BYTE*      pbyInBuf;            /*< in  input data buffer */
    EC_T_DWORD      dwInBufSize;        /*< in  size of input data buffer in byte */
    EC_T_BYTE*      pbyOutBuf;          /*< out output data buffer */
    EC_T_DWORD      dwOutBufSize;       /*< in  size of output data buffer in byte */
    EC_T_DWORD*     pdwNumOutData;      /*< out number of output data bytes stored in output data buffer */
} EC_PACKED EC_T_LINK_NOTIFYPARMS;

/* EtherCAT link frame descriptor */

typedef struct _EC_T_LINK_FRAMEDESC
{
    EC_T_VOID*          pvContext;          /*< link layer context */
    EC_T_BYTE*          pbyFrame;           /*< frame data buffer */
    EC_T_DWORD          dwSize;             /*< size of the frame buffer */
    EC_T_BOOL           bBuffersFollow;     /*< if EC_TRUE try to queue next frame in link layer,
                                                if EC_FALSE fill up DMA descriptors to force immediate send */

    EC_T_DWORD*         pdwTimeStampLo;     /*< data store to store timestamp result to */
    EC_T_DWORD*         pdwTimeStampPostLo; /*< data store to store timestamp result to */
    EC_T_PVOID          pvTimeStampCtxt;    /*< context for pfnTimeStamp */
    EC_PF_TIMESTAMP     pfnTimeStamp;       /*< function if not EC_NULL called to do timestamping */
    EC_T_DWORD*         pdwLastTSResult;    /*< result code store of last time stamp call */
} EC_PACKED EC_T_LINK_FRAMEDESC;


/* Structure to register the Link Layer receive callback */
typedef struct _EC_T_LINK_FRM_RECV_CLBK
{
    EC_T_RECEIVEFRAMECALLBACK   pfFrameReceiveCallback;
    EC_T_VOID*                  pvDevice;
} EC_PACKED EC_T_LINK_FRM_RECV_CLBK;


typedef struct _EC_T_LINK_IOCTLPARMS
{
    EC_T_PBYTE      pbyInBuf;           /*< in  input data buffer */
    EC_T_DWORD      dwInBufSize;        /*< in  size of input data buffer in byte */
    EC_T_PBYTE      pbyOutBuf;          /*< out output data buffer */
    EC_T_DWORD      dwOutBufSize;       /*< in  size of output data buffer in byte */
    EC_T_DWORD*     pdwNumOutData;      /*< out number of output data bytes stored in output data buffer */
} EC_PACKED EC_T_LINK_IOCTLPARMS;

typedef EC_T_DWORD (*PF_EcLinkOpen)(EC_T_VOID* pvLinkParms, EC_T_RECEIVEFRAMECALLBACK pfReceiveFrameCallback, EC_T_LINK_NOTIFY pfLinkNotifyCallback, EC_T_VOID* pvContext, EC_T_VOID** ppvInstance );
typedef EC_T_DWORD (*PF_EcLinkClose)(EC_T_VOID* pvInstance);
typedef EC_T_DWORD (*PF_EcLinkSendFrame)(EC_T_VOID* pvInstance, EC_T_LINK_FRAMEDESC* pLinkFrameDesc);
typedef EC_T_DWORD (*PF_EcLinkSendAndFreeFrame)(EC_T_VOID* pvInstance, EC_T_LINK_FRAMEDESC* pLinkFrameDesc);
typedef EC_T_DWORD (*PF_EcLinkRecvFrame)(EC_T_VOID* pvInstance, EC_T_LINK_FRAMEDESC* pLinkFrameDesc);
typedef EC_T_DWORD (*PF_EcLinkAllocSendFrame)(EC_T_VOID* pvInstance, EC_T_LINK_FRAMEDESC* pLinkFrameDesc, EC_T_DWORD dwSize);
typedef EC_T_VOID  (*PF_EcLinkFreeSendFrame )(EC_T_VOID* pvInstance, EC_T_LINK_FRAMEDESC* pLinkFrameDesc);
typedef EC_T_VOID  (*PF_EcLinkFreeRecvFrame )(EC_T_VOID* pvInstance, EC_T_LINK_FRAMEDESC* pLinkFrameDesc);
typedef EC_T_DWORD      (*PF_EcLinkGetEthernetAddress)(EC_T_VOID* pvInstance, EC_T_BYTE* pbyEthernetAddress);
typedef EC_T_LINKSTATUS (*PF_EcLinkGetStatus)(EC_T_VOID* pvInstance);
typedef EC_T_DWORD      (*PF_EcLinkGetSpeed)(EC_T_VOID* pvInstance);
typedef EC_T_LINKMODE   (*PF_EcLinkGetMode)(EC_T_VOID* pvInstance);
typedef EC_T_DWORD      (*PF_EcLinkIoctl)(EC_T_VOID* pvInstance, EC_T_DWORD dwCode, EC_T_LINK_IOCTLPARMS* pParms );

typedef struct _EC_T_LINK_DEV_PARAM
{
    EC_T_DWORD                  dwValidationPattern;
    EC_T_DWORD                  dwDevParamVersion;
    EC_T_CHAR                   szDriverIdent[MAX_DRIVER_IDENT_LEN];    /* driver identification string (zero terminated) */
    EC_T_LINKMODE               eLinkMode;                              /* operation mode: polling or interrupt */
    EC_T_BOOL                   bForceAllocSendFrame;                   /* obsolete */
    EC_T_PVOID                  pvDrvSpecificParam;                     /* pointer to additional driver specific parameters */
} EC_PACKED EC_T_LINK_DEV_PARAM;


typedef struct _EC_T_LINK_DRV_DESC
{
    EC_T_DWORD                  dwValidationPattern;
    EC_T_DWORD                  dwInterfaceVersion;
    EC_PF_OSDBGMSGHK            pfOsDbgMsgHook;
    PF_EcLinkOpen               pfEcLinkOpen;
    PF_EcLinkClose              pfEcLinkClose;

    /* Standard send functions */
    PF_EcLinkSendFrame          pfEcLinkSendFrame;
    PF_EcLinkSendAndFreeFrame   pfEcLinkSendAndFreeFrame;

    /* Timestamping time optimized send functions */
    PF_EcLinkSendFrame          pfEcLinkRes1;
    PF_EcLinkSendAndFreeFrame   pfEcLinkRes2;

    PF_EcLinkRecvFrame          pfEcLinkRecvFrame;
    PF_EcLinkAllocSendFrame     pfEcLinkAllocSendFrame;
    PF_EcLinkFreeSendFrame      pfEcLinkFreeSendFrame ;
    PF_EcLinkFreeRecvFrame      pfEcLinkFreeRecvFrame ;
    PF_EcLinkGetEthernetAddress pfEcLinkGetEthernetAddress;
    PF_EcLinkGetStatus          pfEcLinkGetStatus;
    PF_EcLinkGetSpeed           pfEcLinkGetSpeed;
    PF_EcLinkGetMode            pfEcLinkGetMode;
    PF_EcLinkIoctl              pfEcLinkIoctl;

    EC_T_VOID*                  pvLinkInstance;
} EC_PACKED EC_T_LINK_DRV_DESC;

typedef EC_T_DWORD (*PF_emllRegister)(EC_T_LINK_DRV_DESC* pLinkDrvDesc, EC_T_DWORD dwLinkDrvDescSize);

typedef EC_T_VOID (*PF_emllISRCallback)(EC_T_VOID* pvInstance, EC_T_VOID* pvContext);

typedef EC_T_DWORD (*PF_DOINT_HDL)(EC_T_PVOID pvLinkParms);


#define EC_LINK_INFO_DESCRIPTION_SIZE_MAX   63
typedef enum _EC_T_LINK_INFO_DATATYPE
{
    eLinkInfoDataType_bool = 0,
    eLinkInfoDataType_byte,
    eLinkInfoDataType_word,
    eLinkInfoDataType_dword,
    eLinkInfoDataType_qword,

    /* Borland C++ datatype alignment correction */
    eLinkInfoDataType_BCppDummy   = 0xFFFFFFFF
} EC_T_LINK_INFO_DATATYPE;

#ifndef  __TMS470__
typedef struct _EC_T_LINK_INFO
{
    EC_T_CHAR               szDescription[EC_LINK_INFO_DESCRIPTION_SIZE_MAX];  /*< Description text of the value */
    EC_T_LINK_INFO_DATATYPE eDataType;          /*< Datatype */
    union _EC_T_LINK_INFO_DATA
    {
        EC_T_BOOL           bData;              /*< Data as boolean */
        EC_T_BYTE           byData;             /*< Data as byte */
        EC_T_WORD           wData;              /*< Data as word */
        EC_T_DWORD          dwData;             /*< Data as dword */
        EC_T_UINT64         qwData;             /*< Data as qword */
    } EC_PACKED u;
} EC_PACKED EC_T_LINK_INFO;

typedef struct _EC_T_LINK_INFOLIST
{
    EC_T_DWORD          dwInfoCntMax;       /*< Total number of info elements available */
    EC_T_DWORD          dwInfoCntUsed;      /*< Number of info elements used */
    EC_T_LINK_INFO      aInfo[1];
} EC_PACKED EC_T_LINK_INFOLIST;
#endif

/*****************************************************************************/
/* Hardware specific link layer parameters                                   */
/*****************************************************************************/

/* WinPcap Windows */
/* =============== */

#define MAX_LEN_WINPCAP_ADAPTER_ID  ((EC_T_DWORD)39)

typedef struct _EC_T_LINK_OPENPARMS_WINPCAP
{
    EC_T_BYTE                   arrIpAddress[4];
    EC_T_LINKMODE               eLinkMode;       /* only for compatibility reason */
    EC_T_CHAR                   szAdapterId[MAX_LEN_WINPCAP_ADAPTER_ID]; /* {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} */
} EC_PACKED EC_T_LINK_OPENPARMS_WINPCAP;


/* SNARF VxWorks */
/* ============= */

#define MAX_LEN_SNARF_ADAPTER_NAME  ((EC_T_DWORD)64)

typedef struct _EC_T_LINK_OPENPARMS_SNARF
{
    EC_T_WCHAR  aszAdapterName[MAX_LEN_SNARF_ADAPTER_NAME];
} EC_PACKED EC_T_LINK_OPENPARMS_SNARF;


/* Intel Pro 100 family */
/* ==================== */

#define EC_LINK_TYPE_I8255x     0xA1ECA108

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_I8255x
{
    EC_T_DWORD                  dwType;
    EC_T_DWORD                  dwUnit;
    EC_T_DWORD                  dwIstPriority;
    EC_T_LINKMODE               eLinkMode;          /* only for compatibility reason */
    PF_DOINT_HDL                pfDoIntHandling;    /* [out]    interrupt processing routine for ext IntHandling */

    PF_emllISRCallback          pfIsrCallback;		/* [in]		optional called by ISR */
    EC_T_PVOID		            pvIsrCBContext;		/* [in]		ISR callback context */
} EC_PACKED EC_T_LINK_OPENPARMS_I8255x;


/* Intel Pro 1000 family */
/* ==================== */

#define EC_LINK_TYPE_I8254x     0xA1ECA10B

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_I8254x
{
    EC_T_DWORD                  dwType;         /* 0xA1ECA10B */
    EC_T_DWORD                  dwUnit;

    EC_T_DWORD                  dwIstPriority;
    EC_T_LINKMODE               eLinkMode;              /* only for compatibility reason */
    PF_DOINT_HDL                pfDoIntHandling;        /* [out]    interrupt processing routine for ext IntHandling */

    EC_T_DWORD                  dwRxBuffers;            /* [in]     Low word : Number of receive buffers
                                                                    High word: Receive buffer size (single Ethernet frame).
                                                                               \sa EC_T_LINK_I8254X_BUFFERSIZE.
                                                                               Default is 0 -> buffer optimized for standard Ethernet frame.
                                                         */

    EC_T_DWORD                  dwTxBuffers;            /* [in]     Low word : Number of transmit buffers
                                                                    High word: Transmit buffer size (single Ethernet frame).
                                                                               \sa EC_T_LINK_I8254X_BUFFERSIZE.
                                                                               Default is 0 -> buffer optimized for standard Ethernet frame.
                                                         */

    PF_emllISRCallback          pfIsrCallback;          /* [in]     optional called by ISR */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]     ISR callback context */
    EC_T_BOOL                   bDisableLocks;          /* [in]     Locks in LL Disabled */
} EC_PACKED EC_T_LINK_OPENPARMS_I8254x;


/* Realtek RTL8139      */
/* ==================== */

#define EC_LINK_TYPE_RTL8139     0xA1ECA109

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_RTL8139
{
    EC_T_DWORD                  dwType;         /* 0xA1ECA109 */
    EC_T_DWORD                  dwUnit;

    EC_T_DWORD                  dwIstPriority;
    EC_T_LINKMODE               eLinkMode;              /* only for compatibility reason */
    PF_DOINT_HDL                pfDoIntHandling;        /* [out]    interrupt processing routine for ext IntHandling */

    EC_T_DWORD                  dwRxBuffers;            /* [in]     amount of Receive  Buffers */
    EC_T_DWORD                  dwTxBuffers;            /* [in]     amount of Transmit Buffers (Frames) */

    PF_emllISRCallback          pfIsrCallback;          /* [in]     optional called by ISR */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]     ISR callback context */
} EC_PACKED EC_T_LINK_OPENPARMS_RTL8139;


/* Realtek RTL8169      */
/* ==================== */

#define EC_LINK_TYPE_RTL8169     0xA1ECA10C

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_RTL8169
{
    EC_T_DWORD                  dwType;         /* 0xA1ECA10C */
    EC_T_DWORD                  dwUnit;

    EC_T_DWORD                  dwIstPriority;
    EC_T_LINKMODE               eLinkMode;              /* only for compatibility reason */
    PF_DOINT_HDL                pfDoIntHandling;        /* [out]    interrupt processing routine for ext IntHandling */

    PF_emllISRCallback          pfIsrCallback;          /* [in]     optional called by ISR */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]     ISR callback context */
} EC_PACKED EC_T_LINK_OPENPARMS_RTL8169;


/* FreeScale FEC */
/* ============= */

#define EC_LINK_TYPE_FSLFEC     0xA1ECA110

/* FEC type */
typedef enum _EC_T_FEC_TYPE
{
    eFEC_UNDEFINED,
    eFEC_IMX25,     /* MAC on Freescale i.MX25 (ARM9; ARMv5) */
    eFEC_IMX28,     /* MAC on Freescale i.MX28 (ARM9; ARMv5) */
    eFEC_IMX53,     /* MAC on Freescale i.MX53 (ARM Cortex-A8; ARMv7-a) */
    eFEC_IMX6,      /* MAC on Freescale i.MX6 (ARM Cortex-A9 Single/Dual/Quad; ARMv7-a) */
} EC_T_FEC_TYPE;

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_FSLFEC
{
    EC_T_DWORD                  dwType;                 /* 0xA1ECA110 */
    EC_T_DWORD                  dwUnit;

    EC_T_DWORD                  dwIstPriority;
    PF_DOINT_HDL                pfDoIntHandling;        /* [out]    interrupt processing routine for ext IntHandling */

    EC_T_DWORD                  dwRxBuffers;            /* [in]     Low word : Number of receive buffers */
    EC_T_DWORD                  dwTxBuffers;            /* [in]     Low word : Number of transmit buffers */

    PF_emllISRCallback          pfIsrCallback;          /* [in]     optional called by ISR */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]     ISR callback context */

    EC_T_FEC_TYPE               eFecType;
    EC_T_PHY_INTERFACE          ePhyInterface;
} EC_PACKED EC_T_LINK_OPENPARMS_FSLFEC;


/* FreeScale TSEC/eTSEC */
/* ==================== */

#define EC_LINK_TYPE_ETSEC     0xA1ECA10D

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_ETSEC
{
    EC_T_DWORD                  dwType;                 /* EC_LINK_TYPE_ETSEC */
    EC_T_DWORD                  dwRegisterBase;         /* [in] Physical base address of register block (4k) */
    EC_T_DWORD                  dwLocalMdioBase;        /* [in] Physical base address of local MDIO register block (4k). 
                                                                For the eTSEC V1 or TSEC this is the same as dwRegisterBase, for
                                                                the eTSEC V2 it's not. */
    EC_T_DWORD                  dwPhyMdioBase;          /* [in] Physical base address of MDIO register block (4k). 
                                                                This is the MDIO base of the (e)TSEC where the PHY (MII bus)
                                                                is physically connected to (MII interface is shared by (e)TSEC's). */
    
#define ETSEC_FIXED_LINK  0xFFFFFFFF
    
    EC_T_DWORD                  dwPhyAddr;              /* [in] PHY address on MII bus. ETSEC_FIXED_LINK if fixed link configuration */
    EC_T_DWORD                  dwTbiPhyAddr;           /* [in] Address of internal TBI phy. Any address from [0..31] can be used here,
                                                                but the address shouldn't collide with any external PHY connected to
                                                                the external MII bus */

#define ETSEC_LINKFLAG_LINKOK                (1 << 0)
#define ETSEC_LINKFLAG_1000baseT_Full        (1 << 1)
#define ETSEC_LINKFLAG_1000baseT_Half        (1 << 2)
#define ETSEC_LINKFLAG_100baseT_Full         (1 << 3)
#define ETSEC_LINKFLAG_100baseT_Half         (1 << 4)
#define ETSEC_LINKFLAG_10baseT_Full          (1 << 5)
#define ETSEC_LINKFLAG_10baseT_Half          (1 << 6)
    
    EC_T_DWORD                  dwFixedLinkVal;         /* [in]  Only evaluated if dwPhyAddr == FIXED_LINK. Set to one of the
                                                                 ETSEC_LINKFLAG_* macros. I.e. ETSEC_LINKFLAG_1000baseT_Full */
    EC_T_BYTE                   abyStationAddress[6];   /* [in]  MAC station address */
    EC_T_BYTE                   byRes[2];
    EC_T_VOID*                  oMiiBusMtx;             /* [in]  This mutex protect the access to the (shared) MII bus. Set to 0 if mutex shouldn't be used.
                                                                 The MII bus is shared between eTSEC instances. So this mutex should be created once and 
                                                                 assigned here for all Linklayer instances */
    
    /* Interrupt handling */
    EC_T_DWORD                  dwRxInterrupt;          /* [in]  Receive interrupt number (IRQ) */
    EC_T_DWORD                  dwIstPriority;          /* [in]  Priority of interrupt service thread (1 .. 99) */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]  ISR callback context */
    
    /* legacy stuff */
    EC_T_LINKMODE               eLinkMode;              /* only for compatibility reason */    
    PF_DOINT_HDL                pfDoIntHandling;        /* [out] Interrupt processing routine for ext IntHandling */
    PF_emllISRCallback          pfIsrCallback;          /* [in]  Optional called by ISR */

} EC_PACKED EC_T_LINK_OPENPARMS_ETSEC;


/* NDISUIO Windows CE   */
/* ==================== */

#define EC_LINK_TYPE_NDISUIO    0xA1ECA101
#define ECAT_NDISUIO_DEVNAME    TEXT("ECT1:")

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_NDISUIO
{
    EC_T_DWORD      dwType;
    EC_T_DWORD      dwRcvThreadPriority;
    EC_T_WCHAR*     szNetworkAdapterName;
} EC_PACKED EC_T_LINK_OPENPARMS_NDISUIO;


/* Davicom DM9000       */
/* ==================== */

#define EC_LINK_TYPE_DM9000    0xA1ECA104

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_DM9000
{
    EC_T_DWORD      dwType;
    EC_T_DWORD      dwRcvThreadPriority;
    EC_T_DWORD      dwBaseAddress;
    EC_T_DWORD      dwBaseIO;
    EC_T_LINKMODE   eLinkMode;          /* only for compatibility reason */
    EC_T_DWORD      dwIrqNum;
    EC_T_DWORD      dwSysIntr;

    EC_T_DWORD      dwFrameBufferLen;

    PF_DOINT_HDL	pfDoIntHandling;	/* [out]	interrupt processing routine for ext IntHandling */
} EC_PACKED EC_T_LINK_OPENPARMS_DM9000;

#define EC_LINK_TYPE_EOE    0xA1ECA201

/* Link parameters */
/*****************************************************************************/
/** \brief  Variables to identify the EOE link layer driver/instance which shall
*           be opened with the ``EoELinkOpen()`` call.
*
*           Attention: Parameters not used for identification of the link layer
*                      (or master stack  instance) must be cleared to 0.
*/
typedef struct _EC_T_LINK_OPENPARMS_EOE
{
    /*! \var    dwEmInstanceID
        \brief  Instance ID (or instance number), identical to the instance ID
                of the EtherCAT master which is the master of the network we want
                to open.
	*/
    EC_T_DWORD dwEmInstanceID;

    /*! \var    pvUplinkInstance
        \brief  Pointer to the ``CEcEoEUplink`` instance/object (if available).
	*/
    EC_T_PVOID pvUplinkInstance;

    /*! \var    dwEthAddress
        \brief  Ethernet address of the driver/adapter we want to open.
	*/
    EC_T_BYTE abyEthAddress[6];

    /*! \var    abyIpAdress
        \brief  IP address of the driver/adapter we want to open.
	*/
    EC_T_BYTE abyIpAdress[4];

    /*! \var    szEoEDrvIdent
        \brief  Name of the driver/adapter we want to open (e.g. "eth0").
	*/
    EC_T_CHAR szEoEDrvIdent[MAX_DRIVER_IDENT_LEN];
} EC_PACKED EC_T_LINK_OPENPARMS_EOE;


/* Marvell PXA MFU */
/* =============== */

#define EC_LINK_TYPE_PXAMFU     0xA1ECA111

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_PXAMFU
{
    EC_T_DWORD                  dwType;                 /* EC_LINK_TYPE_PXAMFU */
    EC_T_DWORD                  dwUnit;

    EC_T_DWORD                  dwIstPriority;
    EC_T_LINKMODE               eLinkMode;              /* only for compatibility reason */
    PF_DOINT_HDL                pfDoIntHandling;        /* [out]    interrupt processing routine for ext IntHandling */

    PF_emllISRCallback          pfIsrCallback;          /* [in]     optional called by ISR */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]     ISR callback context */
} EC_PACKED EC_T_LINK_OPENPARMS_PXAMFU;


/* Beckhoff CCAT */
/* =============== */

#define EC_LINK_TYPE_CCAT       0xA1ECA112

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_CCAT
{
    EC_T_DWORD                  dwType;                 /* EC_LINK_TYPE_CCAT */
    EC_T_DWORD                  dwUnit;

    EC_T_DWORD                  dwIstPriority;
    EC_T_LINKMODE               eLinkMode;              /* only for compatibility reason */
    PF_DOINT_HDL                pfDoIntHandling;        /* [out]    interrupt processing routine for ext IntHandling */

    PF_emllISRCallback          pfIsrCallback;          /* [in]     optional called by ISR */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]     ISR callback context */
} EC_PACKED EC_T_LINK_OPENPARMS_CCAT;


/* Texas Instruments CPSW */
/* ====================== */

#define EC_LINK_TYPE_CPSW_V1    0xA1ECA113
#define EC_LINK_TYPE_CPSW_V2    0xA1ECA118
#define EC_LINK_TYPE_CPSW       EC_LINK_TYPE_CPSW_V2

/* Needed for IOCTL */
#define CPSW_LINKFLAG_LINKOK                (1 << 0)
#define CPSW_LINKFLAG_1000baseT_Full        (1 << 1)
#define CPSW_LINKFLAG_1000baseT_Half        (1 << 2)
#define CPSW_LINKFLAG_100baseT_Full         (1 << 3)
#define CPSW_LINKFLAG_100baseT_Half         (1 << 4)
#define CPSW_LINKFLAG_10baseT_Full          (1 << 5)
#define CPSW_LINKFLAG_10baseT_Half          (1 << 6)

#define CPSW_SLAVECNT 2

typedef struct _EC_T_LINK_SLAVEDATA_CPSW
{
   EC_T_DWORD dwCpswSlaveOffs;
   EC_T_DWORD dwCpswSliverOffs;
} EC_T_LINK_SLAVEDATA_CPSW;

typedef struct _EC_T_LINK_PLATFORMDATA_CPSW
{
   EC_T_DWORD dwRegisterBase; /* Register base of CPSW (0x4A100000) */
   EC_T_DWORD dwHostPortOffs; /* Port 0 registers */
   EC_T_DWORD dwCpswAleOffs;
   EC_T_DWORD dwCpswCpdmaOffs;
   EC_T_DWORD dwCpdmaStateramOffs;
   EC_T_DWORD dwCpswSsOffs; /* AM387x: CPSW_3GSS, AM33xx: CPSW_WR */
   EC_T_DWORD dwMdioOffs;
   EC_T_DWORD dwBdRamOffs; /* 8k SRAM for buffer descriptors */
   
   EC_T_LINK_SLAVEDATA_CPSW slave[CPSW_SLAVECNT];
} EC_T_LINK_PLATFORMDATA_CPSW;

/* AM33xx (e.g. Beaglebone) */
const static EC_T_LINK_PLATFORMDATA_CPSW AM33xx_pdata =
{
   0x4A100000, /* dwRegisterBase */
   0x108, /* dwHostPortOffs */
   0xd00, /* dwCpswAleOffs */
   0x800, /* dwCpswCpdmaOffs */
   0xA00,  /* dwCpdmaStateramOffs */
   0x1200, /* dwCpswSsOffs */
   0x1000, /* dwMdioOffs */
   0x2000, /* dwBdRamOffs */
   {
      /* Port 1 */
      {
         0x208, /* dwCpswSlaveOffs */
         0xd80, /* dwCpswSliverOffs */
      },
      /* Port 2 */
      {
         0x308, /* dwCpswSlaveOffs */
         0xdc0, /* dwCpswSliverOffs */
      }
   }
};

/* DM814x/AM387x (e.g. Mistral/TI 814X/387X BASE EVM) */
const static EC_T_LINK_PLATFORMDATA_CPSW AM387x_pdata =
{
   0x4A100000, /* dwRegisterBase */
   0x28, /* dwHostPortOffs */
   0x600, /* dwCpswAleOffs */
   0x100, /* dwCpswCpdmaOffs */
   0x200,  /* dwCpdmaStateramOffs */
   0x900, /* dwCpswSsOffs */
   0x800, /* dwMdioOffs */
   0x2000, /* dwBdRamOffs */
   {
      /* Port 1 */
      {
         0x50, /* dwCpswSlaveOffs */
         0x700, /* dwCpswSliverOffs */
      },
      /* Port 2 */
      {
         0x90, /* dwCpswSlaveOffs */
         0x740, /* dwCpswSliverOffs */
      }
   }
};

/* Select the connection type of PHY chip */
typedef enum _EC_T_CPSW_PHYCONNECTION
{
    eCpswPhySource_GMII = 0,
    eCpswPhySource_RGMII
} EC_T_CPSW_PHYCONNECTION;

/* Link parameters Version 1 */
typedef struct _EC_T_LINK_OPENPARMS_CPSW_V1
{
    EC_T_DWORD                  dwType;                 /* EC_LINK_TYPE_CPSW_V1 */
    EC_T_LINK_PLATFORMDATA_CPSW oPlatCfg;
    EC_T_BYTE                   abyStationAddress[6];   /* [in]  MAC station address */
    EC_T_DWORD                  dwPhyAddr;              /* [in]  PHY address (0 .. 31) on MII bus */
    EC_T_DWORD                  dwPortIdx;              /* [in]  0 := first port, 1 := 2nd port */
    EC_T_DWORD                  dwPortPrio;             /* [in]  0 (lowest), 1 (highest) */
    EC_T_BOOL                   bMaster;                /* [in]  TRUE := Initialize MAC */
    EC_T_BOOL                   bPhyRestartAutoNegotiation;

    /* Interrupt handling */
    EC_T_DWORD                  dwRxInterrupt;          /* [in]  Receive interrupt number (IRQ) */
    EC_T_DWORD                  dwIstPriority;          /* [in]  Priority of interrupt service thread (1 .. 99) */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]  ISR callback context */

    /* legacy stuff */
    EC_T_LINKMODE               eLinkMode;              /* only for compatibility reason */    
    PF_DOINT_HDL                pfDoIntHandling;        /* [out] Interrupt processing routine for ext IntHandling */
    PF_emllISRCallback          pfIsrCallback;          /* [in]  Optional called by ISR */
} EC_T_LINK_OPENPARMS_CPSW_V1;

/* Link parameters Version 2
 * The structure layout is binary compatible with version 1 */
typedef struct _EC_T_LINK_OPENPARMS_CPSW_V2
{
    EC_T_DWORD                  dwType;                 /* EC_LINK_TYPE_CPSW */
    EC_T_LINK_PLATFORMDATA_CPSW oPlatCfg;
    EC_T_BYTE                   abyStationAddress[6];   /* [in]  MAC station address */
    EC_T_DWORD                  dwPhyAddr;              /* [in]  PHY address (0 .. 31) on MII bus */
    EC_T_DWORD                  dwPortIdx;              /* [in]  0 := first port, 1 := 2nd port */
    EC_T_DWORD                  dwPortPrio;             /* [in]  0 (lowest), 1 (highest) */
    EC_T_BOOL                   bMaster;                /* [in]  TRUE := Initialize MAC */
    EC_T_BOOL                   bPhyRestartAutoNegotiation;

    /* Interrupt handling */
    EC_T_DWORD                  dwRxInterrupt;          /* [in]  Receive interrupt number (IRQ) */
    EC_T_DWORD                  dwIstPriority;          /* [in]  Priority of interrupt service thread (1 .. 99) */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]  ISR callback context */

    /* legacy stuff */
    EC_T_LINKMODE               eLinkMode;              /* only for compatibility reason */
    PF_DOINT_HDL                pfDoIntHandling;        /* [out] Interrupt processing routine for ext IntHandling */
    PF_emllISRCallback          pfIsrCallback;          /* [in]  Optional called by ISR */

    EC_T_CPSW_PHYCONNECTION     ePhyConnection;         /* [in]  PHY connection type (GMII, RGMII) */

    /* for future extension */
    EC_T_DWORD					dwReserved[4];			/* [in]  Must be 0 */
} EC_T_LINK_OPENPARMS_CPSW_V2;

typedef struct _EC_T_LINK_OPENPARMS_CPSW_V2 EC_T_LINK_OPENPARMS_CPSW;

/* Xilinx GEM */
/* ====================== */

#define EC_LINK_TYPE_GEM       0xA1ECA114

/* Needed for IOCTL */
#define GEM_LINKFLAG_LINKOK                (1 << 0)
#define GEM_LINKFLAG_1000baseT_Full        (1 << 1)
#define GEM_LINKFLAG_100baseT_Full         (1 << 3)

#define GEM_SLAVECNT 2

/* Select the source of the Rx clock, control and data signals */
typedef enum _EC_T_GEM_RXSOURCE
{
    eGemRxSource_MIO = 0,
    eGemRxSource_EMIO
} EC_T_GEM_RXSOURCE;

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_GEM
{
    EC_T_DWORD                  dwType;                 /* EC_LINK_TYPE_GEM */

    EC_T_DWORD                  dwPortIdx;              /* [in]  0 := first port GEM0, 1 := 2nd port GEM1*/
    EC_T_GEM_RXSOURCE           eRxSource;              /* [in]  Select the source of the Rx clock, control and data signals */

    EC_T_BYTE                   byRes[8];               /* reserved for binary compatibilit< with prev version  */
    EC_T_DWORD                  dwPhyAddr;              /* [in]  PHY address (0 .. 31) on MII bus */

    /* next version */
    /* Interrupt handling */
    EC_T_DWORD                  dwRxInterrupt;          /* [in]  Receive interrupt number (IRQ) */
    EC_T_DWORD                  dwIstPriority;          /* [in]  Priority of interrupt service thread (1 .. 99) */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]  ISR callback context */

    /* legacy stuff */
    EC_T_LINKMODE               eLinkMode;              /* only for compatibility reason. Must be equal to EC_T_LINK_DEV_PARAM.eLinkMode */
    PF_DOINT_HDL                pfDoIntHandling;        /* [out] Interrupt processing routine for ext IntHandling */
    PF_emllISRCallback          pfIsrCallback;          /* [in]  Optional called by ISR */
} EC_T_LINK_OPENPARMS_GEM;


/* SMSC LAN9218i */
/* ============= */

#define EC_LINK_TYPE_L9218i		0xA1ECA10A

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_L9218i
{
    EC_T_DWORD                  dwType;         		/* 0xA1ECA10A */

    EC_T_DWORD                  dwIstPriority;
    EC_T_LINKMODE               eLinkMode;
    PF_DOINT_HDL                pfDoIntHandling;        /* [out]    interrupt processing routine for ext IntHandling */

    EC_T_DWORD                  dwTxBuffers;            /* [in]     amount of Transmit Buffers (Frames) */

    PF_emllISRCallback          pfIsrCallback;		    /* [in]		optional called by ISR */
    EC_T_PVOID		            pvIsrCBContext;		    /* [in]		ISR callback context */
} EC_T_LINK_OPENPARMS_L9218i;


/* Xilinx LogiCORE IP XPS Ethernet Lite Media Access Controller */
/* ============================================================ */

#define EC_LINK_TYPE_EMAC     0xA1ECA114

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_EMAC
{
    EC_T_DWORD                  dwType;                 /* EC_LINK_TYPE_EMAC */

    EC_T_DWORD                  dwRegisterBase;         /* [in] Physical base address of register block (4k) */
    EC_T_DWORD                  dwPhyAddr;              /* [in] PHY address on MII bus */

    /* Interrupt handling */
    EC_T_DWORD                  dwRxInterrupt;          /* [in]  Receive interrupt number (IRQ) */
    EC_T_DWORD                  dwIstPriority;          /* [in]  Priority of interrupt service thread */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]  ISR callback context */

    /* legacy stuff */
    EC_T_LINKMODE               eLinkMode;              /* only for compatibility reason */    
    PF_DOINT_HDL                pfDoIntHandling;        /* [out] Interrupt processing routine for ext IntHandling */
    PF_emllISRCallback          pfIsrCallback;          /* [in]  Optional called by ISR */
} EC_PACKED EC_T_LINK_OPENPARMS_EMAC;


/* Intel EG20T MAC */
/* =============== */

#define EC_LINK_TYPE_EG20T     0xA1ECA115

/* Needed for IOCTL */
#define EG20T_LINKFLAG_LINKOK                (1 << 0)
#define EG20T_LINKFLAG_1000baseT_Full        (1 << 1)
#define EG20T_LINKFLAG_1000baseT_Half        (1 << 2)
#define EG20T_LINKFLAG_100baseT_Full         (1 << 3)
#define EG20T_LINKFLAG_100baseT_Half         (1 << 4)
#define EG20T_LINKFLAG_10baseT_Full          (1 << 5)
#define EG20T_LINKFLAG_10baseT_Half          (1 << 6)

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_EG20T
{
    EC_T_DWORD                  dwType;         /* 0xA1ECA115 */
    EC_T_DWORD                  dwUnit;

    EC_T_DWORD                  dwIstPriority;
    EC_T_LINKMODE               eLinkMode;              /* only for compatibility reason */
    PF_DOINT_HDL                pfDoIntHandling;        /* [out]    interrupt processing routine for ext IntHandling */

    PF_emllISRCallback          pfIsrCallback;          /* [in]     optional called by ISR */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]     ISR callback context */

    EC_T_BOOL                   bPhyRestartAutoNeg;     /* [in]     If true, auto negotiation is restarted on initialization */
} EC_PACKED EC_T_LINK_OPENPARMS_EG20T;


/*****************************************************************************/
/* PMX1000-R6040                                                             */
/*****************************************************************************/

#define EC_LINK_TYPE_R6040      0xA1ECA117

/* Link parameters */
typedef struct _EC_T_LINK_OPENPARMS_R6040
{
    EC_T_DWORD                  dwType;                 /* EC_LINK_TYPE_R6040 */
    EC_T_DWORD                  dwUnit;

    EC_T_DWORD                  dwIstPriority;
    PF_DOINT_HDL                pfDoIntHandling;        /* [out]    interrupt processing routine for ext IntHandling */

    PF_emllISRCallback          pfIsrCallback;          /* [in]     optional called by ISR */
    EC_T_PVOID                  pvIsrCBContext;         /* [in]     ISR callback context */
} EC_PACKED EC_T_LINK_OPENPARMS_R6040;


/*****************************************************************************/
/** \brief  Structure to open the link layer. This struct contains of two
*           parts. A common part that is equal for all Link Layer and an
*           driver specific part.
*/

#define MAX_SIZE_DRV_SPECIFIC_PARM  ((EC_T_INT)256)
#define MAX_SIZE_LINK_OPEN_PARM     (sizeof(EC_T_LINK_DEV_OPEN_PARAM))

typedef struct _EC_T_LINK_DEV_OPEN_PARAM
{
    EC_T_LINK_DEV_PARAM oLinkDevParam;

    union _EC_T_LINK_OPEN_PARMS
    {
        EC_T_LINK_OPENPARMS_I8255x  oI8255x;
        EC_T_LINK_OPENPARMS_I8254x  oI8254x;
        EC_T_LINK_OPENPARMS_RTL8139 oRtl8139;
        EC_T_LINK_OPENPARMS_RTL8169 oRtl8169;
        EC_T_LINK_OPENPARMS_NDISUIO oNdisUio;
        EC_T_LINK_OPENPARMS_DM9000  oDm9000;
        EC_T_LINK_OPENPARMS_WINPCAP oWinPcap;
        EC_T_LINK_OPENPARMS_SNARF   oSnarf;
        EC_T_LINK_OPENPARMS_EOE     oEoE;
        EC_T_LINK_OPENPARMS_FSLFEC  oFslFec;
        EC_T_LINK_OPENPARMS_ETSEC   oFslEtsec;
        EC_T_LINK_OPENPARMS_PXAMFU  oPxaMfu;
        EC_T_LINK_OPENPARMS_CCAT    oCCAT;
        EC_T_LINK_OPENPARMS_CPSW    oCPSW;
        EC_T_LINK_OPENPARMS_L9218i  oL9218i;
        EC_T_LINK_OPENPARMS_EMAC    oEMAC;
        EC_T_LINK_OPENPARMS_GEM     oGEM;
        EC_T_LINK_OPENPARMS_EG20T   oEG20T;
        EC_T_LINK_OPENPARMS_R6040   oR6040;
        EC_T_BYTE                   abyOpenParam[MAX_SIZE_DRV_SPECIFIC_PARM];
    } linkOpenParms;
} EC_PACKED EC_T_LINK_DEV_OPEN_PARAM;



/*-FUNCTIONS DECLARATION-----------------------------------------------------*/

ATEMLL_API EC_T_DWORD emllRegisterI8255x
    (EC_T_LINK_DRV_DESC*  pLinkDrvDesc        /* [in,out] link layer driver descriptor */
    ,EC_T_DWORD         dwLinkDrvDescSize);   /* [in]     size in bytes of link layer driver descriptor */

ATEMLL_API EC_T_DWORD emllRegisterI8254x
    (EC_T_LINK_DRV_DESC*  pLinkDrvDesc        /* [in,out] link layer driver descriptor */
    ,EC_T_DWORD         dwLinkDrvDescSize);   /* [in]     size in bytes of link layer driver descriptor */

ATEMLL_API EC_T_DWORD emllRegisterFslFec
(EC_T_LINK_DRV_DESC*  pLinkDrvDesc        /* [in,out] link layer driver descriptor */
 ,EC_T_DWORD         dwLinkDrvDescSize);   /* [in]     size in bytes of link layer driver descriptor */

ATEMLL_API EC_T_DWORD emllRegisterRTL8139
    (EC_T_LINK_DRV_DESC*  pLinkDrvDesc        /* [in,out] link layer driver descriptor */
    ,EC_T_DWORD         dwLinkDrvDescSize);   /* [in]     size in bytes of link layer driver descriptor */
    
ATEMLL_API EC_T_DWORD emllRegisterRTL8169
    (EC_T_LINK_DRV_DESC*  pLinkDrvDesc        /* [in,out] link layer driver descriptor */
    ,EC_T_DWORD         dwLinkDrvDescSize);   /* [in]     size in bytes of link layer driver descriptor */

ATEMLL_API EC_T_DWORD emllRegisterCCAT
    (EC_T_LINK_DRV_DESC*  pLinkDrvDesc        /* [in,out] link layer driver descriptor */
    ,EC_T_DWORD         dwLinkDrvDescSize);   /* [in]     size in bytes of link layer driver descriptor */

ATEMLL_API EC_T_DWORD emllRegisterNdisUio
    (EC_T_LINK_DRV_DESC*  pLinkDrvDesc        /* [in,out] link layer driver descriptor */
    ,EC_T_DWORD         dwLinkDrvDescSize);   /* [in]     size in bytes of link layer driver descriptor */

ATEMLL_API EC_T_DWORD emllRegisterR6040
    (EC_T_LINK_DRV_DESC*  pLinkDrvDesc        /* [in,out] link layer driver descriptor */
    ,EC_T_DWORD         dwLinkDrvDescSize);   /* [in]     size in bytes of link layer driver descriptor */

/*-COMPILER SETTINGS---------------------------------------------------------*/
#ifdef __cplusplus
}
#endif


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

#endif /* INC_ECLINK */


/*-END OF SOURCE FILE--------------------------------------------------------*/
