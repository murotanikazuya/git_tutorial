/*-----------------------------------------------------------------------------
 * EcCommon.h               
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              Common header shared by all AT-EM layers.
 *---------------------------------------------------------------------------*/

#ifndef INC_ECCOMMON
#define INC_ECCOMMON

/*-INCLUDES------------------------------------------------------------------*/
#ifndef INC_ECTYPE
  #include "EcType.h"
#endif
#ifndef INC_ECERROR
  #include "EcError.h"
#endif
#ifndef INC_ECOS
  #include "EcOs.h"
#endif
#include "EcVersion.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*-DEFINES-------------------------------------------------------------------*/

/* debug tracing */
#define     EC_TRACE_OS                     0x00000001  /* ecat os layer */
#define     EC_TRACE_CONFIG                 0x00000002  /* ecat config layer */
#define     EC_TRACE_LINK                   0x00000004  /* ecat link layer */
#define     EC_TRACE_CORE                   0x00000008  /* ecat master core layer, general */
#define     EC_TRACE_CORE_QUECMD_SEQ        0x00000010  /* ecat core layer, queued command sequence */
#define     EC_TRACE_CORE_SND_CYCFRAMES     0x00000020  /* ecat core layer, sent cyclic frames */
#define     EC_TRACE_CORE_SND_QUEFRAMES     0x00000040  /* ecat core layer, sent queued frames */
#define     EC_TRACE_CORE_RCV_CYCFRAMES     0x00000080  /* ecat core layer, received cyclic frames */
#define     EC_TRACE_CORE_RCV_QUEFRAMES     0x00000100  /* ecat core layer, received queued frames */
#define     EC_TRACE_CORE_SEQUENCE          0x00000200  /* ecat core layer, general sequence for timing */
#define     EC_TRACE_INTERFACE              0x00000400  /* interface layer */
#define     EC_TRACE_INITCMDS               0x00000800  /* trace ecat init commands */
#define     EC_TRACE_CYCCMD_HASH            0x00001000  /* ecat master core layer, hashing of cyclic commands */
#define     EC_TRACE_CREATE_FRAMES          0x00002000  /* ecat master core layer, creation of frames */

#define     EC_TRACE_CORE_ALL_FRAMES (EC_TRACE_CORE_SND_CYCFRAMES|EC_TRACE_CORE_SND_QUEFRAMES|EC_TRACE_CORE_RCV_CYCFRAMES|EC_TRACE_CORE_RCV_QUEFRAMES)

/* memory usage tracing */
#define     EC_MEMTRACE_STRING          0x00000001  /* memory tracing CString class (config layer) */
#define     EC_MEMTRACE_CONFIG          0x00000002  /* ecat config layer, general */
#define     EC_MEMTRACE_CONFIG_STR      0x00000004  /* ecat config layer, string objects */
#define     EC_MEMTRACE_CONFIG_NODE     0x00000008  /* ecat config layer, xml nodes */
#define     EC_MEMTRACE_CONFIG_DATA     0x00000010  /* ecat config layer, xml data */
#define     EC_MEMTRACE_CONFIG_CMDDESC  0x00000020  
#define     EC_MEMTRACE_XMLPARSER       0x00000040  /* ecat xml parser */
#define     EC_MEMTRACE_CORE_FACTORY    0x00000080  /* ecat core layer, device factory */
#define     EC_MEMTRACE_CORE_DEVICE     0x00000100  /* ecat core layer, device */
#define     EC_MEMTRACE_CORE_MASTER     0x00000200  /* ecat core layer, master */
#define     EC_MEMTRACE_CORE_SLAVE      0x00000400  /* ecat core layer, slaves */
#define     EC_MEMTRACE_CORE_SWITCH     0x00000800  /* ecat core layer, switch */
#define     EC_MEMTRACE_CORE_MBX        0x00001000  /* ecat core layer, mailbox */
#define     EC_MEMTRACE_CORE_FRAMES     0x00002000  /* ecat core layer, sent frames */
#define     EC_MEMTRACE_INTERFACE       0x00008000  /* ecat interface layer */
#define     EC_MEMTRACE_APP             0x00010000  /* application */



#define     EC_ERRORMSG(Msg)  OsDbgMsg Msg; OsDbgAssert(EC_FALSE)   /* print an error message and jump into debugger */
#define     EC_ERRORMSGC(Msg) OsDbgMsg Msg                          /* print an error message and continue */
#define     EC_ERRORMSGL(Msg) m_poEcDevice->LinkErrorMsg Msg       /* send an error message to the link layer for debugging purposes */
#define     EC_ERRORMSGC_L(Msg) EC_ERRORMSGL(Msg); EC_ERRORMSGC(Msg)
#define     EC_ERRORMSG_L(Msg)  EC_ERRORMSGL(Msg); EC_ERRORMSG(Msg)

/*/////////////////////////////////////////////////////////////////////////////////////////
//
// Standard Types
*/

#define	DEFTYPE_NULL              ((EC_T_WORD)0x0000)       
#define	DEFTYPE_BOOLEAN           ((EC_T_WORD)0x0001)       /* bit size: 1 */
#define	DEFTYPE_INTEGER8          ((EC_T_WORD)0x0002)       /* bit size: 8 */
#define	DEFTYPE_INTEGER16         ((EC_T_WORD)0x0003)       /* bit size: 16 */
#define	DEFTYPE_INTEGER32         ((EC_T_WORD)0x0004)       /* bit size: 32 */
#define	DEFTYPE_UNSIGNED8         ((EC_T_WORD)0x0005)       /* bit size: 8 */
#define	DEFTYPE_UNSIGNED16        ((EC_T_WORD)0x0006)       /* bit size: 16 */
#define	DEFTYPE_UNSIGNED32        ((EC_T_WORD)0x0007)       /* bit size: 32 */
#define	DEFTYPE_REAL32            ((EC_T_WORD)0x0008)       /* bit size: 32 */
#define	DEFTYPE_VISIBLESTRING     ((EC_T_WORD)0x0009)       /* bit size: 8*n */
#define	DEFTYPE_OCTETSTRING       ((EC_T_WORD)0x000A)       /* bit size: 8*(n+1) */
#define	DEFTYPE_UNICODESTRING     ((EC_T_WORD)0x000B)       /* bit size: 16*(n+1) */
#define	DEFTYPE_TIMEOFDAY         ((EC_T_WORD)0x000C)
#define	DEFTYPE_TIMEDIFFERENCE    ((EC_T_WORD)0x000D)
#define	DEFTYPE_DOMAIN            ((EC_T_WORD)0x000F)
#define	DEFTYPE_INTEGER24         ((EC_T_WORD)0x0010)       /* bit size: 24 */
#define	DEFTYPE_REAL64            ((EC_T_WORD)0x0011)       /* bit size: 64 */
#define	DEFTYPE_INTEGER40         ((EC_T_WORD)0x0012)       /* bit size: 40 */
#define	DEFTYPE_INTEGER48         ((EC_T_WORD)0x0013)       /* bit size: 48 */
#define	DEFTYPE_INTEGER56         ((EC_T_WORD)0x0014)       /* bit size: 56 */
#define	DEFTYPE_INTEGER64         ((EC_T_WORD)0x0015)       /* bit size: 64 */
#define	DEFTYPE_UNSIGNED24        ((EC_T_WORD)0x0016)       /* bit size: 24 */
#define	DEFTYPE_UNSIGNED40        ((EC_T_WORD)0x0018)       /* bit size: 40 */
#define	DEFTYPE_UNSIGNED48        ((EC_T_WORD)0x0019)       /* bit size: 48 */
#define	DEFTYPE_UNSIGNED56        ((EC_T_WORD)0x001A)       /* bit size: 56 */
#define	DEFTYPE_UNSIGNED64        ((EC_T_WORD)0x001B)       /* bit size: 64 */

#define	DEFTYPE_GUID              ((EC_T_WORD)0x001D)       /* bit size: 128 */
#define	DEFTYPE_BYTE              ((EC_T_WORD)0x001E)       /* bit size: 8 */
#define	DEFTYPE_WORD              ((EC_T_WORD)0x001F)       /* bit size: 16 */
#define	DEFTYPE_DWORD             ((EC_T_WORD)0x0020)       /* bit size: 32 */
#define	DEFTYPE_PDOMAPPING        ((EC_T_WORD)0x0021)

#define	DEFTYPE_IDENTITY          ((EC_T_WORD)0x0023)

#define	DEFTYPE_COMMAND           ((EC_T_WORD)0x0025)
#define	DEFTYPE_PDOCOMPAR         ((EC_T_WORD)0x0027)
#define	DEFTYPE_ENUM              ((EC_T_WORD)0x0028)
#define	DEFTYPE_SMPAR             ((EC_T_WORD)0x0029)
#define	DEFTYPE_RECORD            ((EC_T_WORD)0x002A)
#define	DEFTYPE_BACKUP_PARAMETER  ((EC_T_WORD)0x002B)
#define	DEFTYPE_MODULAR_DEVICE_PROFILE  ((EC_T_WORD)0x002C)
#define	DEFTYPE_BITARR8           ((EC_T_WORD)0x002D)       /* bit size: 8 */
#define	DEFTYPE_BITARR16          ((EC_T_WORD)0x002E)       /* bit size: 16 */
#define	DEFTYPE_BITARR32          ((EC_T_WORD)0x002F)       /* bit size: 32 */
#define	DEFTYPE_BIT1              ((EC_T_WORD)0x0030)       /* bit size: 1 */
#define	DEFTYPE_BIT2              ((EC_T_WORD)0x0031)       /* bit size: 1 */
#define	DEFTYPE_BIT3              ((EC_T_WORD)0x0032)       /* bit size: 1 */
#define	DEFTYPE_BIT4              ((EC_T_WORD)0x0033)       /* bit size: 1 */
#define	DEFTYPE_BIT5              ((EC_T_WORD)0x0034)       /* bit size: 1 */
#define	DEFTYPE_BIT6              ((EC_T_WORD)0x0035)       /* bit size: 1 */
#define	DEFTYPE_BIT7              ((EC_T_WORD)0x0036)       /* bit size: 1 */
#define	DEFTYPE_BIT8              ((EC_T_WORD)0x0037)       /* bit size: 1 */

/* 0x40-0x5F Manufacturer Specific Complex Data Type */

#define DEFTYPE_ARRAY_OF_INT      ((EC_T_WORD)0x0260)       /* ARRAY [0..n] OF INT  16*(n+1) */
#define DEFTYPE_ARRAY_OF_SINT     ((EC_T_WORD)0x0261)       /* ARRAY [0..n] OF SINT  8*(n+1) */
#define DEFTYPE_ARRAY_OF_DINT     ((EC_T_WORD)0x0262)       /* ARRAY [0..n] OF DINT  32*(n+1) */
#define DEFTYPE_ARRAY_OF_UDINT    ((EC_T_WORD)0x0263)       /* ARRAY [0..n] OF UDINT 32*(n+1) */

#define DEFTYPE_ERROR_SETTING     ((EC_T_WORD)0x0281)
#define DEFTYPE_HISTORY           ((EC_T_WORD)0x0282)       /* Object 0x10F3 */
#define DEFTYPE_DIAGNOSIS_OBJECT  ((EC_T_WORD)0x0282)       /* Object 0x10F3 */

#define DEFTYPE_EXTERNAL_SYNC_STATUS    ((EC_T_WORD)0x0283)       /* Object 0x10F4 */
#define DEFTYPE_EXTERNAL_SYNC_SETTINGS  ((EC_T_WORD)0x0284)       /* Object 0x10F5 */
#define DEFTYPE_FSOEFRAME               ((EC_T_WORD)0x0285)
#define DEFTYPE_FSOECOMMPAR             ((EC_T_WORD)0x0286)


/*-FORWARD DECLARATIONS------------------------------------------------------*/
/* these functions are only available in the DEBUG version */
ATECAT_API EC_T_DWORD ecatGetTraceMask(EC_T_VOID);
ATECAT_API EC_T_DWORD ecatGetMemTraceMask(EC_T_VOID);
ATECAT_API EC_T_VOID  ecatSetTraceMask(EC_T_DWORD dwTraceMask );
ATECAT_API EC_T_VOID  ecatSetMemTraceMask(EC_T_DWORD dwMemTraceMask );


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* INC_ECCOMMON */


/*-END OF SOURCE FILE--------------------------------------------------------*/
