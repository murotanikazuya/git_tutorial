/*-----------------------------------------------------------------------------
 * TimeoutHelper.h          Header file
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Andreas Willig
 * Description              Timeout Helper
 * Date                     [8/28/2006]
 *---------------------------------------------------------------------------*/

#ifndef __TIMEOUTHELPER_H__
#define __TIMEOUTHELPER_H__ 1

/*-INCLUDES------------------------------------------------------------------*/
#ifndef INC_ECTYPE
#include "EcType.h"
#endif


/*-DEFINES-------------------------------------------------------------------*/

#define EC_TO_HELPER_STARTED        0x0001  /* set, if timer is started */
#define EC_TO_HELPER_STOPPED        0x0002  /* set, if timer is stopped */
#define EC_TO_HELPER_DEFAULT        0x0004  /* Default timeout is set when timeout is started with default value */
#define EC_TO_HELPER_ELAPSE_WRAP    0x0008  /* set, if elapse time is less then start time (if time will wrap) */


/*-TYPEDEFS------------------------------------------------------------------*/

/*-CLASS---------------------------------------------------------------------*/

class   CTimeoutHelper
{
public:
    CTimeoutHelper();
    CTimeoutHelper(EC_T_DWORD dwTimeoutMsec);

    EC_T_VOID  Start(EC_T_DWORD dwTimeoutMsec);
    EC_T_VOID  Start(EC_T_DWORD dwTimeoutMsec, EC_T_DWORD* pdwMsecCounter);
    EC_T_VOID  Stop(EC_T_VOID);
    EC_T_BOOL  IsElapsed();
    EC_T_BOOL  IsStarted() { return (m_dwFlags & EC_TO_HELPER_STARTED); }
    EC_T_BOOL  IsStopped() { return (m_dwFlags & EC_TO_HELPER_STOPPED); }
    EC_T_BOOL  IsDefaultTimeout() { return (m_dwFlags & EC_TO_HELPER_DEFAULT); }
    EC_T_VOID  Restart(){ Start(m_dwTimeoutMsec, m_pdwMsecCounter); }
    EC_T_DWORD GetDuration() { return m_dwTimeoutMsec; }

private:
    EC_T_DWORD m_dwStartTime;       /* millisecond counter when timer was started */
    EC_T_DWORD m_dwTimeoutMsec;     /* timeout value */
    EC_T_DWORD m_dwTimeElapse;      /* millisecond counter when timer will elapse */
    EC_T_DWORD* m_pdwMsecCounter;   /* pointer to msec counter */
    EC_T_DWORD m_dwFlags;           /* Flags */
};

/*-FUNCTION DECLARATION------------------------------------------------------*/

/*-LOCAL VARIABLES-----------------------------------------------------------*/

/*-FUNCTIONS-----------------------------------------------------------------*/

#endif /* __TIMEOUTHELPER_H__ */

/*-END OF SOURCE FILE--------------------------------------------------------*/

