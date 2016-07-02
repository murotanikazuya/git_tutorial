/*-----------------------------------------------------------------------------
 * EcType.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master type definitions
 *---------------------------------------------------------------------------*/

#ifndef INC_ECTYPE
#define INC_ECTYPE


/*-TYPEDEFS------------------------------------------------------------------*/
#define EC_T_VOID void
typedef void           *EC_T_PVOID;

typedef int             EC_T_BOOL;

typedef char            EC_T_CHAR;
typedef unsigned short  EC_T_WCHAR;

typedef unsigned char   EC_T_BYTE, *EC_T_PBYTE;
typedef unsigned short  EC_T_WORD;
typedef unsigned int    EC_T_DWORD;

typedef signed char	    EC_T_SBYTE;
typedef signed short    EC_T_SWORD;
typedef signed int      EC_T_SDWORD;

typedef int             EC_T_INT;
typedef unsigned int    EC_T_UINT;

typedef short           EC_T_SHORT;
typedef unsigned short  EC_T_USHORT;

typedef double			EC_T_REAL;

typedef void*			EC_T_ADDRESS;

/* type of lock */
typedef enum
{
    eLockType_DEFAULT= 1,                           /*< Default mutex           */
    eLockType_SPIN,                                 /*< only jobs --> spin lock */
    eLockType_INTERFACE,                            /*< interface and jobs      */

    /* Borland C++ datatype alignment correction */
    eLockType_BCppDummy   = 0xFFFFFFFF
} EC_T_OS_LOCK_TYPE;                                       


/*-MACROS--------------------------------------------------------------------*/
#define EC_FALSE            0
#define EC_TRUE             1
#define EC_NULL             0

#define EC_NOWAIT           ((EC_T_DWORD)0x00000000)
#define EC_WAITINFINITE     ((EC_T_DWORD)0xFFFFFFFF)


/*-MASTER FEATURES-----------------------------------------------------------*/

/* object dictionary */
#define EC_MASTER_DEVICE_NAME       "EC-Master"         /* Index: 0x1008 */
#define EC_MASTER_HW_VERSION        "V xx.xx.xx.xx"     /* Index: 0x1009 */
#define EC_MASTER_SW_VERSION        "V xx.xx.xx.xx"     /* Index: 0x100A */
#define EC_MASTER_VENDOR_ID         0x00004154          /* Index: 0x1018 Subindex 1 */
#define EC_MASTER_PRODUCT_CODE      0x6d657461          /* Index: 0x1018 Subindex 2 */

/* Class A */
#define EC_MASTER_CLASS_A
#ifdef  EC_MASTER_CLASS_A
#define INCLUDE_DC_SUPPORT          /* distributed clock */
#define INCLUDE_RED_DEVICE          /* redundancy */
#define INCLUDE_MASTER_OBD          /* master object dictionary */
#define INCLUDE_VOE_SUPPORT         /* vendor specific access over EtherCAT */ 
#define INCLUDE_FOE_SUPPORT         /* file access over EtherCAT */
#define INCLUDE_AOE_SUPPORT         /* ADS over EtherCAT */ 
#define INCLUDE_MULTIPLE_CYC_ENTRIES
#endif

/* Class B */
#define EC_MASTER_CLASS_B
#ifdef  EC_MASTER_CLASS_B
#define INCLUDE_SOE_SUPPORT         /* servoDrive over EtherCAT */
#define INCLUDE_EOE_SUPPORT         /* mailbox protocol EoE */
#define INCLUDE_SLAVE_STATISTICS    /* cyclic reading of slave error registers */
#define INCLUDE_VARREAD             /* read PD variable tags from XML */
#define INCLUDE_MEMORY_PROVIDER
#define INCLUDE_FRAME_LOGGING       /* callback interface to log frames */
#define FRAME_LOSS_SIMULATION
#endif

/* feature packs */
#define INCLUDE_HOTCONNECT
#define INCLUDE_PASS_THROUGH_SERVER
#define INCLUDE_EOE_ENDPOINT        /* ethernet over EtherCAT end point */
#define INCLUDE_ADS_ADAPTER

/* miscellaneous */
#undef  VLAN_FRAME_SUPPORT                      /* VLAN frame support */
#undef  INCLUDE_COE_PDO_SUPPORT                 /* currently not supported */
#undef  INCLUDE_FOE_LOGICAL_STATE_MBOX_POLLING  /* currently not supported */
#undef  INCLUDE_EC_INTERNAL_TSC_MEASUREMENT

/* RAS */
#undef  INCLUDE_RAS_SPOCSUPPORT

#if (defined __RCX__)
#undef INCLUDE_MASTER_OBD
#undef VLAN_FRAME_SUPPORT
#undef INCLUDE_VARREAD
#undef FRAME_LOSS_SIMULATION
#endif

#if (defined __MET__)
#undef INCLUDE_MASTER_OBD
#undef INCLUDE_FOE_SUPPORT
#undef VLAN_FRAME_SUPPORT
#undef INCLUDE_VARREAD
#undef FRAME_LOSS_SIMULATION
#undef INCLUDE_VOE_SUPPORT
#endif

#if (defined RTAI)
#undef INCLUDE_EOE_ENDPOINT
#undef INCLUDE_FOE_SUPPORT
#undef FRAME_LOSS_SIMULATION
#undef INCLUDE_MASTER_OBD
#undef VLAN_FRAME_SUPPORT
#undef INCLUDE_VOE_SUPPORT
#endif

/* bit mask for supported features (used for license check) */
#define EC_FEATURES_MASTER_MASK     0x00000001
#define EC_FEATURES_RED_MASK        0x00000002

#endif /* INC_ECTYPE */

/*-END OF SOURCE FILE--------------------------------------------------------*/
