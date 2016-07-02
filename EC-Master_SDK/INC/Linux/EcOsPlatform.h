/*-----------------------------------------------------------------------------
 * EcOsPlatform.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              AT-EM OS-Layer header file for Linux platform
 *----------------------------------------------------------------------------*/

#ifndef INC_ECOSPLATFORM
#define INC_ECOSPLATFORM

/*-SUPPORT-SELECTION----------------------------------------------------------*/
#define EC_SOCKET_IP_SUPPORTED

/*-INCLUDES-------------------------------------------------------------------*/
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#include <EcType.h>

#ifdef EC_SOCKET_IP_SUPPORTED
#  include <sys/types.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#endif

/*-DEFINES--------------------------------------------------------------------*/
#define ATECAT_API

#ifdef __arm__
#  define WITHALIGNMENT   /* --> for ARM etc. */
#endif

#ifdef _ARCH_PPC
#  define EC_BIG_ENDIAN 1
#endif

#ifdef EC_SOCKET_IP_SUPPORTED
#  ifndef SD_BOTH
#    define SD_BOTH     ((EC_T_INT)2)
#  endif
#endif

#undef DISABLE_LOCK

/* using the DEBUG definition: ASSERTions are activated */
/* using the ASSERT_SUSPEND definition: the asserting task will be suspended */
#if 0
#   define ASSERT_SUSPEND
#endif

/*-TYPEDEFS-------------------------------------------------------------------*/
typedef struct _OS_LOCK_DESC
{
    EC_T_OS_LOCK_TYPE   eLockType;            /* type of lock e. g. Default, SpinLock, Semaphore */
    pthread_mutex_t     Mutex;
    int                 nLockCnt;
    pthread_t           pThread;
} OS_LOCK_DESC;

typedef va_list             EC_T_VALIST;

typedef enum
{
    eSLEEP_USLEEP,
    eSLEEP_NANOSLEEP,
    eSLEEP_CLOCK_NANOSLEEP
} E_SLEEP;

typedef unsigned long long  EC_T_UINT64;
typedef signed long long    EC_T_INT64;
typedef int EC_T_CPUSET;                           /* CPU-set for SMP systems */

/*-MACROS---------------------------------------------------------------------*/

#define EC_CPUSET_DEFINED
#define EC_CPUSET_ZERO(CpuSet)          (CpuSet) = 0                /* clear all CPU indexes in the CPU set */
#define EC_CPUSET_SET(CpuSet,nCpuIndex) (CpuSet) = (1<<nCpuIndex)   /* set CPU index nCpuIndex (0..x) in the CPU set */
#define EC_CPUSET_SETALL(CpuSet)        (CpuSet) = 0xFFFF           /* set all CPU indexes in the CPU set */
#define EC_T_FD_SET  fd_set
#define EC_VASTART   va_start
#define EC_VAEND     va_end
#define EC_VAARG     va_arg

#define EC_INLINEATTRIBUTE
/* ## needs a param before issuing string so "generate" one */
#define EC_INLINEINT(dummy, functiondecl)     \
    __inline dummy ##functiondecl __attribute__((always_inline)); \
    __inline dummy ##functiondecl
#define EC_INLINE(funcdecl) EC_INLINEINT(,funcdecl)

/* use macros for the most important OS layer routines */
#define OsSetLastError(dwError)        dwError
#define OsSleep(dwMsec)                LinuxSleep(dwMsec)
#define OsSetEvent(pvEvent)            LinuxSetEvent((sem_t*)(pvEvent))
#define OsResetEvent(pvEvent)          sem_trywait((sem_t*)(pvEvent))
#define OsMemoryBarrier()
#define OsStricmp strcasecmp

#ifdef EC_SOCKET_IP_SUPPORTED
#  define INVALID_SOCKET (-1)
#  define SOCKET_ERROR (-1)
#  define OsSocketAccept(hSockHandle, oSockAddr, nSockAddrLen) \
     accept(hSockHandle, oSockAddr, (socklen_t *)(nSockAddrLen))
#  define OsSocketRecvFrom(hSockHandle, pbyBuffer, dwBufferLen, dwFlags, oSrcAddr, dwSrcAddrLen) \
      recvfrom(hSockHandle, pbyBuffer, dwBufferLen, dwFlags, oSrcAddr, (socklen_t *) (dwSrcAddrLen))
#  define OsSocketGetLastError() 0
#endif /* ifdef EC_SOCKET_IP_SUPPORTED */

#ifdef DEBUG
#  ifdef ASSERT_SUSPEND
#    define OsDbgAssert(bCond)                      OsDbgAssertFunc((bCond),__FILE__,__LINE__)
     EC_T_VOID OsDbgAssertFunc(EC_T_BOOL bAssertCondition, EC_T_CHAR* szFile, EC_T_DWORD dwLine);
#  else
#    define OsDbgAssert                             assert
#  endif /* ifdef ASSERT_SUSPEND */
#endif /* ifdef DEBUG */

#ifndef OsDbgAssert
#  define OsDbgAssert(x)
#endif

/* define here if tracing shall be enabled
 * #define DEBUGTRACE
 */
#ifdef DEBUGTRACE
   /* optional: redirect trace messages into OS specific function, e.g. to
    * store trace log into file
    * default: print trace log as debug message
    */
#  define OsTrcMsg OsTrcMsg
   ATECAT_API  EC_T_VOID   OsTrcMsg(const EC_T_CHAR* szFormat, ...);
#endif /* ifdef DEBUGTRACE */


/*-PROTOTYPES-----------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

EC_T_VOID LinuxSleep(EC_T_DWORD dwMsec);

/* determine how OsSleep is implemented */
ATECAT_API EC_T_VOID OsSleepSetType(E_SLEEP ESleep);

EC_T_VOID LinuxSetEvent(sem_t* pEvent);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* INC_ECOSPLATFORM */

/*-END OF SOURCE FILE---------------------------------------------------------*/
