/*-----------------------------------------------------------------------------
 * TimeoutHelper.cpp        cpp file
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Andreas Willig
 * Description              Timeout Helper
 * Date                     [8/28/2006]
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/

#include "TimeoutHelper.h"

#ifndef INC_ECOS
#include "EcOs.h"
#endif

/*-DEFINES-------------------------------------------------------------------*/

/*-TYPEDEFS------------------------------------------------------------------*/

/*-CLASS---------------------------------------------------------------------*/

#ifdef DEBUG
//EC_T_BOOL G_bEcatDisableAllTimeouts = EC_TRUE;   /* for debugging purposes, disable all timeouts */
EC_T_BOOL G_bEcatDisableAllTimeouts = EC_FALSE;   /* for debugging purposes, disable all timeouts */
#endif

/********************************************************************************/
/**
\brief  Default Constructor
*/
CTimeoutHelper::CTimeoutHelper()
{
    /* member variables will be reset by memset() from outside! */
    m_dwFlags = 0;
    m_dwStartTime = 0;
    m_dwTimeoutMsec = 0;
    m_dwTimeElapse = 0;
    m_pdwMsecCounter = EC_NULL;
    m_dwFlags = 0;
}

/********************************************************************************/
/**
\brief Constructor
*/
CTimeoutHelper::CTimeoutHelper(EC_T_DWORD dwTimeoutMsec)
{
    ::CTimeoutHelper();
    Start(dwTimeoutMsec, EC_NULL);
}

/********************************************************************************/
/**
\brief 
*/
EC_T_VOID CTimeoutHelper::Stop(EC_T_VOID)
{
    m_dwStartTime   = 0;
    m_dwTimeoutMsec = 0;
    m_dwFlags = EC_TO_HELPER_STOPPED;
}

/********************************************************************************/
/**
\brief 
*/
EC_T_VOID CTimeoutHelper::Start
    (EC_T_DWORD dwTimeoutMsec
    ,EC_T_DWORD* pdwMsecCounter)
{
    m_pdwMsecCounter = pdwMsecCounter;

    if(dwTimeoutMsec & 0x80000000)
    {
        m_dwFlags |= EC_TO_HELPER_DEFAULT;
        dwTimeoutMsec = dwTimeoutMsec & ~0x80000000;
    }
    else
    {
        m_dwFlags &= ~EC_TO_HELPER_DEFAULT;
    }
                      

    if(m_pdwMsecCounter != EC_NULL)
    {
        m_dwStartTime = *m_pdwMsecCounter;
    }
    else
    {
        m_dwStartTime = OsQueryMsecCount();
    }

    m_dwTimeoutMsec = dwTimeoutMsec;
    m_dwTimeElapse = m_dwStartTime + dwTimeoutMsec; /* time when timer will elapse */
    if( m_dwTimeElapse < m_dwStartTime)
    {
        m_dwFlags |= EC_TO_HELPER_ELAPSE_WRAP;
    }
    else
    {
        m_dwFlags &= ~EC_TO_HELPER_ELAPSE_WRAP;
    }

    m_dwFlags |= EC_TO_HELPER_STARTED;
    m_dwFlags &= ~EC_TO_HELPER_STOPPED;
}


/********************************************************************************/
/**
\brief 
*/
EC_T_VOID CTimeoutHelper::Start
    (EC_T_DWORD dwTimeoutMsec)
{
    Start(dwTimeoutMsec, EC_NULL);
}


/********************************************************************************/
/**
\brief 
\return 
*/
EC_T_BOOL CTimeoutHelper::IsElapsed()
{
EC_T_DWORD dwMsecCountCurr = 0;
EC_T_BOOL  bTimeElapsed = EC_FALSE;

#ifdef DEBUG
    if( G_bEcatDisableAllTimeouts )
    {
    static EC_T_BOOL s_bMsgShown = EC_FALSE;
        if( !s_bMsgShown )
        {
            s_bMsgShown = EC_TRUE;
            OsDbgMsg( "*******************************************\n" );
            OsDbgMsg( "*******************************************\n" );
            OsDbgMsg( "DEBUG SETTING: All Timeouts are disabled!!!\n" );
            OsDbgMsg( "*******************************************\n" );
            OsDbgMsg( "*******************************************\n" );
        }
        return EC_FALSE;
    }
#endif
    if(m_pdwMsecCounter != EC_NULL)
    {
        dwMsecCountCurr =  *m_pdwMsecCounter;
    }
    else
    {
        dwMsecCountCurr =  OsQueryMsecCount();
    }
    
    if( m_dwFlags &  EC_TO_HELPER_STARTED)
    {
        if( m_dwFlags & EC_TO_HELPER_ELAPSE_WRAP )
        {
            if( (dwMsecCountCurr < m_dwStartTime) && (dwMsecCountCurr >= m_dwTimeElapse) )
            {
                bTimeElapsed = EC_TRUE;
            }
        }
        else
        {
            if( dwMsecCountCurr < m_dwStartTime )
            {
                bTimeElapsed = EC_TRUE;
            }
            if( dwMsecCountCurr >= m_dwTimeElapse )
            {
                bTimeElapsed = EC_TRUE;
            }
        }
    }
    else
    {
        /* in case the timer never was started it always returns with elapsed state! */
        /* bTimeElapsed = EC_TRUE; */
    }
    return bTimeElapsed;
}


/*-FUNCTION DECLARATION------------------------------------------------------*/

/*-LOCAL VARIABLES-----------------------------------------------------------*/

/*-FUNCTIONS-----------------------------------------------------------------*/



/*-END OF SOURCE FILE--------------------------------------------------------*/

