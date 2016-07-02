/*-----------------------------------------------------------------------------
 * DCDemoConfig.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master demo configuration header
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/


/*-DEFINES-------------------------------------------------------------------*/

/*********************/
/* thread priorities */
/*********************/
#if defined WIN32 && !defined UNDER_CE && !defined RTOS_32 && !defined UNDER_RTSS
 /* we need to set all threads to the highest possible priority to avoid errors! */
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)THREAD_PRIORITY_TIME_CRITICAL) /* EtherCAT master trigger thread priority */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)THREAD_PRIORITY_TIME_CRITICAL) /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)THREAD_PRIORITY_TIME_CRITICAL) /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)THREAD_PRIORITY_LOWEST)  /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)THREAD_PRIORITY_NORMAL)
#elif (defined UNDER_RTSS)
 #define TIMER_THREAD_PRIO           (RT_PRIORITY_MAX-0) /* EtherCAT master trigger thread priority */
 #define JOBS_THREAD_PRIO            (RT_PRIORITY_MAX-1) /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            (RT_PRIORITY_MAX-2) /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             (RT_PRIORITY_MIN)   /* EtherCAT message logging thread priority */
 #define LOG_ROLLOVER                ((EC_T_WORD)0)      /* EtherCAT message logging rollover counter limit */
 #define MAIN_THREAD_PRIO            (RT_PRIORITY_MIN)
 #define JOBS_THREAD_STACKSIZE       0x8000
 #define LOG_THREAD_STACKSIZE        0x8000
#elif (defined RTOS_32)
 #define MAIN_THREAD_PRIO_OFFSET     10
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)64)    /* EtherCAT master trigger thread priority */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)63)    /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)62)    /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)1 )    /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)3+MAIN_THREAD_PRIO_OFFSET)        
#elif (defined QNX6)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)100)   /* EtherCAT master trigger thread priority */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)99)    /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)98)    /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)40)    /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)30) 
 #define REMOTE_RECV_THREAD_PRIO     ((EC_T_DWORD)50)    /* slightly higher than logger */
#elif (defined __RCX__)
 #define REALTIME_PRIORITY_OFFSET    TSK_PRIO_2
 #define MAIN_THREAD_PRIO_OFFSET     TSK_PRIO_50
 #define TIMER_THREAD_PRIO           (0+REALTIME_PRIORITY_OFFSET)   /* EtherCAT master trigger thread priority */
 #define JOBS_THREAD_PRIO            (1+REALTIME_PRIORITY_OFFSET)   /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            (2+REALTIME_PRIORITY_OFFSET)   /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             (5+MAIN_THREAD_PRIO_OFFSET)    /* EtherCAT message logging thread priority */
 #define LOG_ROLLOVER                ((EC_T_WORD)0)                 /* EtherCAT message logging rollover counter limit */
 #define MAIN_THREAD_PRIO            (MAIN_THREAD_PRIO_OFFSET) 
#elif (defined RTAI)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)99)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)5)    /* EtherCAT master job thread priority (tEcJobTask) */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)99)   /* EtherCAT master packet receive thread priority (tLOsaL_IST) */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)99)   /* EtherCAT message logging thread priority (tAtEmLog) */
#elif (defined LINUX)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)99)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)99)   /* EtherCAT master job thread priority (tEcJobTask) */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)99)   /* EtherCAT master packet receive thread priority (tLOsaL_IST) */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)19)   /* EtherCAT message logging thread priority (tAtEmLog) */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)39)   /* Main thread */
 #define CONSOLE_THREAD_PRIO         ((EC_T_DWORD)39)   /* Console thread */
 #define SOCKET_THREAD_PRIO          ((EC_T_DWORD)69)   /* Socket thread */
 #define LOGSAVE_THREAD_PRIO         ((EC_T_DWORD)14)   /* PDO save thread */
 #define CALC1_THREAD_PRIO           ((EC_T_DWORD)29)   /* calculate thread(High PRI) */
 #define CALC2_THREAD_PRIO           ((EC_T_DWORD)24)   /* calculate thread(Low PRI) */
 #define CALC_ARY_THREAD_PRIO        ((EC_T_DWORD)90)   /* calculate thread(High PRI) */
 #define JOBS_THREAD_STACKSIZE       0x8000
 #define LOG_THREAD_STACKSIZE        0x8000
 #define CONSOLE_THREAD_STACKSIZE    0x2000
 #define SOCK_THREAD_STACKSIZE       0x8000
#elif (defined __INTEGRITY)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)200)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)199)   /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)198)   /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)40)    /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)40)
#elif (defined __INTIME__)
 /*Priority 0 to 127 Used by the OS for servicing external interrupts. */
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)128)   /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)129)   /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)0)     /* Is not used for INtime. The receive thread priority is defined by the Interrupt Level  */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)200)   /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)130)
#elif (defined __TKERNEL)
 #define TIMER_THREAD_PRIO           ((EC_T_DWORD)1)     /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            ((EC_T_DWORD)2)     /* EtherCAT master job thread priority */
 #define RECV_THREAD_PRIO            ((EC_T_DWORD)3)     /* EtherCAT master interrupt service thread priority */
 #define LOG_THREAD_PRIO             ((EC_T_DWORD)4)     /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            ((EC_T_DWORD)10)
#else

 #ifdef VXWORKS_NORMAL_PRIO
  #define REALTIME_PRIORITY_OFFSET    60
 #else
  #define REALTIME_PRIORITY_OFFSET    2
 #endif

 #define TIMER_THREAD_PRIO           (0+REALTIME_PRIORITY_OFFSET)    /* EtherCAT master timer task (tEcTimingTask) */
 #define JOBS_THREAD_PRIO            (1+REALTIME_PRIORITY_OFFSET)    /* EtherCAT master cyclic packet send thread priority */
 #define RECV_THREAD_PRIO            (2+REALTIME_PRIORITY_OFFSET)    /* EtherCAT master packet receive thread priority */
 #define MBX_THREAD_PRIO             (3+REALTIME_PRIORITY_OFFSET)    /* mailbox demo thread priority */

 #define LOG_THREAD_PRIO             (200)                           /* EtherCAT message logging thread priority */
 #define MAIN_THREAD_PRIO            (4+REALTIME_PRIORITY_OFFSET)
 #define REMOTE_RECV_THREAD_PRIO     0

#endif
#ifndef TIMER_THREAD_STACKSIZE
#define TIMER_THREAD_STACKSIZE       0x1000
#endif
#ifndef JOBS_THREAD_STACKSIZE
#define JOBS_THREAD_STACKSIZE        0x4000
#endif
#ifndef LOG_THREAD_STACKSIZE
#define LOG_THREAD_STACKSIZE		 0x4000
#endif

/******************/
/* timer settings */
/******************/

#define SLAVE_MULTIPLIER    1           /* after SLAVE_MULTIPLIER master ticks the acyclic slave commands will be sent */

#define CYCLE_TIME          1           /* 1 msec */
#define BASE_PERIOD         1000        /* 1000 usec */

/***********************************************/
/* static EtherCAT master configuration values */
/***********************************************/
#define MASTER_CFG_ECAT_CMD_MAX_RETRIES        5    /* maximum retries to send pending ethercat command frames */

#define MASTER_CFG_EOE_TIMEOUT              1000    /* timeout sending EoE frames */
#define MASTER_CFG_FOE_BUSY_TIMEOUT          250    /* timeout FoE busy */

#define MASTER_CFG_MAX_QUEUED_ETH_FRAMES      32    /* max number of queued frames, 127 = the absolut maximum number */
#define MASTER_CFG_MAX_SENT_QUFRM_PER_CYC      2    /* max number of queued frames sent with eUsrJob_SendAcycFrames within one cycle */
#define MASTER_CFG_MAX_SLAVECMD_PER_FRAME     32    /* max number of ecat telegrams per frame (0=maximum possible) */
#define MASTER_CFG_ECAT_MAX_BUS_SLAVES       256    /* max number of pre-allocated bus slave objects */


/* DCM controller set value: Distance between frame send time and sync impulse */
#define DCM_CONTROLLER_SETVAL_NANOSEC    ((BASE_PERIOD/2)*1000)            /* 50 % */

/* maximum controller error in nanoseconds */
#if defined WIN32 && !defined UNDER_CE
#define DCM_MAX_CTL_ERROR_NANOSEC       90000
#else
#define DCM_MAX_CTL_ERROR_NANOSEC       30000
#endif

/* maximum time to get in-sync */
#define DCM_SETVAL_TIMEOUT_MSEC         12000

/*-END OF SOURCE FILE--------------------------------------------------------*/
