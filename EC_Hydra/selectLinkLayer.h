/*-----------------------------------------------------------------------------
 * selectLinkLayer.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Andreas Willig
 * Description              EtherCAT master link layer selection
 *---------------------------------------------------------------------------*/

#ifndef __SELECTLINKAYER_H__
#define __SELECTLINKAYER_H__    1

/*-INCLUDES------------------------------------------------------------------*/

#ifndef INC_ATETHERCAT
#endif
#ifndef INC_ECCOMMON
#include "EcCommon.h"
#endif
#ifndef INC_ECOS
#include "EcOs.h"
#endif
#include "stdio.h"
#include "stdlib.h"
#ifndef INC_ECLINK
#include "EcLink.h"
#endif

#ifdef VXWORKS
#include "vxWorks.h"
#include "sysLib.h"
#include "tickLib.h"
#if ( ((defined _WRS_VXWORKS_MAJOR) && (_WRS_VXWORKS_MAJOR >= 6)) && ((defined _WRS_VXWORKS_MINOR) && (_WRS_VXWORKS_MINOR >= 5)) ) || (defined IFCONFIG_SUPPORT)
#include <inetLib.h>
#include <wrn/coreip/net/utils/ifconfig.h>
#else
#include <ifLib.h>
#endif
#endif

#if (defined _WRS_VXWORKS_MAJOR) && (defined VX_CPU_FAMILY) && (VX_CPU_FAMILY==arm)
#define INCLUDE_HNX
#endif

#if ((defined __RCX__) || (defined INCLUDE_HNX) )
#define LINK_HNX

#if (defined __RCX__)
#define BOOTLOADER
#endif
#define INCLUDE_NETX_DB500SYS
#endif


#ifdef LINK_HNX
#include "LinkHnx.h"
#endif
#ifdef __INTIME__
#include <stdio.h>
#elif _MSC_VER
#include "warn_dis.h"
#include <windows.h>
#include <tchar.h>
#define TCHAR_DEFINED
#include <stdio.h>
#include "warn_ena.h"
#endif

/*-DEFINES-------------------------------------------------------------------*/

#ifdef VXWORKS
/* these prototypes are locally defined to avoid warnings caused by VxWorks header files */
#ifdef __cplusplus
extern "C" {
#endif
    typedef int END_OBJ;
    typedef END_OBJ* (*END_LOAD_FUNC) (char *, EC_T_VOID*);
    extern int      ioTaskStdGet (int taskId, int stdFd);
    extern STATUS   ioGlobalStdSet (int stdFd, int newFd);
#if ((_WRS_VXWORKS_MAJOR == 6) && (_WRS_VXWORKS_MINOR < 9)) || ! defined(_WRS_VXWORKS_MAJOR) /* VxWorks 5.x */
    extern STATUS   taskDelay   (int ticks);
#endif
    extern int      taskNameToId    (char * name);
    extern STATUS   taskPrioritySet (int tid, int newPriority);
    extern STATUS   taskPriorityGet (int tid, int * pPriority);
    extern END_OBJ* endFindByName(char* pName, int unit);
    extern void *   muxDevLoad(int unit, END_OBJ* (*endLoad) (char *, void*),char *initString,BOOL loaning, void*pBSP);
    extern STATUS   muxDevStart(void* pCookie);
    int ipAttach(int unit, char* pDevice);
#ifdef __cplusplus
}
#endif



/* network adapter definitions */
#define BUFF_LOAN       EC_TRUE                   /* enable buffer loaning */
#define SNARF_IPADDR  "172.6.6.101"
#define SNARF1_IPADDR "172.7.7.102"
#ifndef NETMASK
#define NETMASK         0xffff0000
#endif

#endif /* #ifdef VXWORKS */

/*-TYPEDEFS------------------------------------------------------------------*/
#ifndef TCHAR_DEFINED
typedef char TCHAR;
#endif

/*-MACROS--------------------------------------------------------------------*/

/*-CLASS---------------------------------------------------------------------*/

/*-FUNCTION DECLARATION------------------------------------------------------*/

/*-LOCAL VARIABLES-----------------------------------------------------------*/

/*-FUNCTIONS-----------------------------------------------------------------*/

EC_T_BOOL ParseIpAddress(EC_T_CHAR* ptcWord, EC_T_BYTE* pbyIpAddress);

EC_T_CHAR* GetNextWord(EC_T_CHAR **ppCmdLine, EC_T_CHAR *pStorage);

EC_T_BOOL DoSelectLinkLayer(    EC_T_CHAR** ptcWord, 
                                EC_T_CHAR** lpCmdLine,
                                EC_T_CHAR*  tcStorage, 
                                EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
                                EC_T_INT*   pRetVal, 
                                EC_T_DWORD  dwRecvPriority,
                                EC_T_BOOL*  bIsPrimaryLinkLayer     );

EC_T_VOID ShowLinkLayerSyntax1(EC_T_VOID);
EC_T_VOID ShowLinkLayerSyntax2(EC_T_VOID);




#endif /*__SELECTLINKAYER_H__*/
 
/*-END OF SOURCE FILE--------------------------------------------------------*/
