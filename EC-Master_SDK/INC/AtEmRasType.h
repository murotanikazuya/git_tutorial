/*-----------------------------------------------------------------------------
 * AtEmRasType.h            file
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Willig, Andreas
 * Description              description of file
 * Date                     2007/5/4::13:58
 *---------------------------------------------------------------------------*/

#ifndef __ATEMRASTYPE_H__
#define __ATEMRASTYPE_H__   1

/*-INCLUDES------------------------------------------------------------------*/
#include <EcType.h>

/*-DEFINES-------------------------------------------------------------------*/

/*-MACROS--------------------------------------------------------------------*/
#define EC_TRACE_ADDMEM(Mask, szLoc, dwAddress, nSize)
#define EC_TRACE_SUBMEM(Mask, szLoc, dwAddress)

/*-TYPEDEFS------------------------------------------------------------------*/

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

typedef union _ATEMRAS_T_IPADDR
{
    EC_T_DWORD  dwAddr;
    struct /*_t_sAddr*/
    {
        EC_T_BYTE by[4];
    } sAddr;
} EC_PACKED ATEMRAS_T_IPADDR, *ATEMRAS_PT_IPADDR;

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

extern EC_T_DWORD G_dwVerbosity;

#define VERBOSEOUT(lvl, msg) \
    if( G_dwVerbosity >= (EC_T_DWORD)(lvl) ){ \
    OsDbgMsg("(%08X)%s:%d::", OsQueryMsecCount(), __FILE__,__LINE__); \
    OsDbgMsg msg; \
    }

/*-CLASS---------------------------------------------------------------------*/

/*-FUNCTION DECLARATION------------------------------------------------------*/

/*-LOCAL VARIABLES-----------------------------------------------------------*/

/*-FUNCTIONS-----------------------------------------------------------------*/

#undef INCLUDE_RAS_TRACESUPPORT

#endif /* __ATEMRASTYPE_H__ */ 

/*-END OF SOURCE FILE--------------------------------------------------------*/
