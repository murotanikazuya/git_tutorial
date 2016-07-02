/*-----------------------------------------------------------------------------
 * AtEmRasError.h           file
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Willig, Andreas
 * Description              description of file
 * Date                     2007/5/7::7:00
 *---------------------------------------------------------------------------*/

#ifndef  __ATEMRASERROR_H__
#define  __ATEMRASERROR_H__     1

/*-INCLUDES------------------------------------------------------------------*/

#include <EcOs.h>
#include <EcError.h>

/*-DEFINES-------------------------------------------------------------------*/

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

#elif defined __ghs

#define EC_PACKED __attribute__((aligned(4), packed))
#pragma pack (4)

#elif defined(__BORLANDC__)

#pragma pack (push, 4)
#define EC_PACKED	
	
#else
#error Please check pack settings for this compiler!!!
#endif

typedef struct _ATEMRAS_T_CONNOTIFYDESC
{
    EC_T_DWORD      dwCause;    /**< [in]   Connection change type */
    EC_T_DWORD      dwCookie;   /**< [in]   Cookie of connection */
} EC_PACKED ATEMRAS_T_CONNOTIFYDESC, *ATEMRAS_PT_CONNOTIFYDESC;

typedef struct _ATEMRAS_T_REGNOTIFYDESC
{
    EC_T_DWORD      dwCookie;       /**< [in]   Cookie of connection */
    EC_T_DWORD      dwResult;       /**< [in]   Result of call */
    EC_T_DWORD      dwInstanceId;   /**< [in]   ID master instance */
    EC_T_DWORD      dwClientId;     /**< [in]   ID of un- / registered client */
} EC_PACKED ATEMRAS_T_REGNOTIFYDESC, *ATEMRAS_PT_REGNOTIFYDESC;

typedef struct _ATEMRAS_T_MARSHALERRORDESC
{
    EC_T_DWORD      dwCookie;       /**< [in]   Cookie of faulting connection */
    EC_T_DWORD      dwCause;        /**< [in]   Cause of fault */
    EC_T_DWORD      dwLenStatCmd;   /**< [in]   Faulty Protocol Header */
    EC_T_DWORD      dwCommandCode;  /**< [in]   Faulting command code */
} EC_PACKED ATEMRAS_T_MARSHALERRORDESC, *ATEMRAS_PT_MARSHALERRORDESC;

typedef struct _ATEMRAS_T_NONOTIFYMEMORYDESC
{
    EC_T_DWORD      dwCookie;       /**< [in]   Cookie of faulting connection */
    EC_T_DWORD      dwCode;         /**< [in]   Fault causing notification code */
} EC_PACKED ATEMRAS_T_NONOTIFYMEMORYDESC, *ATEMRAS_PT_NONOTIFYMEMORYDESC;

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

/*-CLASS---------------------------------------------------------------------*/

/*-FUNCTION DECLARATION------------------------------------------------------*/

/*-LOCAL VARIABLES-----------------------------------------------------------*/

/*-FUNCTIONS-----------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif
ATECAT_API EC_T_CHAR*  emRasErrorText(EC_T_DWORD dwError);
ATECAT_API EC_T_CHAR*  emRasEventText(EC_T_DWORD dwEvent);
#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* __ATEMRASERROR_H__ */

/*-END OF SOURCE FILE--------------------------------------------------------*/
