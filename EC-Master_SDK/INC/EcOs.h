/*-----------------------------------------------------------------------------
 * EcOs.h                   
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              AT-EM OS-Layer header
 *---------------------------------------------------------------------------*/

#ifndef INC_ECOS
#define INC_ECOS

/*-INCLUDES------------------------------------------------------------------*/
#ifndef INC_ECTYPE
#include "EcType.h"
#endif
#include <EcOsPlatform.h>
/* No OS specific includes files allowed */
#include <limits.h>

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
#if (!defined EC_INT32_MAX) && (defined LONG_MAX)
#define EC_INT32_MAX    LONG_MAX
#endif
#if (!defined EC_INT64_MAX) && (defined LLONG_MAX)
#define EC_INT64_MAX    LLONG_MAX
#endif
#if (!defined EC_INT32_MAX) && (defined _I32_MAX)
#define EC_INT32_MAX    _I32_MAX
#endif
#if (!defined EC_INT64_MAX) && (defined _I64_MAX)
#define EC_INT64_MAX    _I64_MAX
#endif

#if (defined EC_SOCKET_IP_SUPPORTED) || (defined EC_SOCKET_MSGQUEUE_WIN32_SUPPORTED) || (defined EC_SOCKET_MSGQUEUE_RTOSSHM_SUPPORTED) || (defined EC_SOCKET_RTOSLIB_SUPPORTED)
#define EC_SOCKET_SUPPORTED
#endif

#if (defined EC_SOCKET_SUPPORTED)

/* Socket layer internal error code selection */
#if !(defined EC_E_BSD_ENOTCONN) && (defined ENOTCONN)
#define EC_E_BSD_ENOTCONN        ENOTCONN
#endif
#if !(defined EC_E_BSD_ENOTSOCK) && (defined ENOTSOCK)
#define EC_E_BSD_ENOTSOCK        ENOTSOCK
#endif
#if !(defined EC_E_BSD_ESHUTDOWN) && (defined ESHUTDOWN)
#define EC_E_BSD_ESHUTDOWN       ESHUTDOWN
#endif
#if !(defined EC_E_BSD_EHOSTUNREACH) && (defined EHOSTUNREACH)
#define EC_E_BSD_EHOSTUNREACH    EHOSTUNREACH
#endif
#if !(defined EC_E_BSD_EINVAL) && (defined EINVAL)
#define EC_E_BSD_EINVAL          EINVAL
#endif
#if !(defined EC_E_BSD_EMSGSIZE) && (defined EMSGSIZE)
#define EC_E_BSD_EMSGSIZE        EMSGSIZE
#endif
#if !(defined EC_E_BSD_ECONNABORTED) && (defined ECONNABORTED)
#define EC_E_BSD_ECONNABORTED    ECONNABORTED
#endif
#if !(defined EC_E_BSD_ETIMEDOUT) && (defined ETIMEDOUT)
#define EC_E_BSD_ETIMEDOUT       ETIMEDOUT
#endif
#if !(defined EC_E_BSD_ECONNRESET) && (defined ECONNRESET)
#define EC_E_BSD_ECONNRESET      ECONNRESET
#endif

#ifndef EC_INVALID_SOCKET
#define EC_INVALID_SOCKET INVALID_SOCKET
#endif

#ifndef EC_SOCKET_ERROR
#define EC_SOCKET_ERROR SOCKET_ERROR
#endif

#ifndef EC_T_SOCKET
#define EC_T_SOCKET EC_T_INT
#endif

#ifndef EC_SOMAXCONN
#define EC_SOMAXCONN SOMAXCONN
#endif

#endif /* EC_SOCKET_SUPPORTED */


/*-TYPEDEFS------------------------------------------------------------------*/
typedef EC_T_VOID (*EC_PF_OSTIMER)(EC_T_VOID);
typedef EC_T_BOOL (*EC_PF_OSDBGMSGHK)(const EC_T_CHAR* szFormat, EC_T_VALIST vaArgs);
typedef EC_T_VOID (*EC_PF_THREADENTRY)(EC_T_VOID* pvParams);
typedef EC_T_VOID (*EC_PF_LLREGISTER)(EC_T_VOID);

/* SMP support */
#ifndef EC_CPUSET_DEFINED
typedef unsigned long   EC_T_CPUSET;        /* CPU-set for SMP systems */
#endif

typedef EC_T_DWORD (*EC_PF_SYSTIME)(EC_T_UINT64* pqwSystemTime);

typedef struct _EC_T_OS_INIT_DESC
{
    EC_T_DWORD      dwReserved;
    EC_PF_SYSTIME   pfSystemTimeGet;
    EC_T_PVOID      pvPlatformSpecific;
	EC_T_DWORD      dwSupportedFeatures;
} EC_PACKED EC_T_OS_INIT_DESC;

#if (defined EC_SOCKET_SUPPORTED)

#ifndef EC_T_FD_SET
typedef struct fd_set EC_T_FD_SET;
#endif

#ifndef EC_T_TIMEVAL
typedef struct timeval EC_T_TIMEVAL;
#endif

#ifndef EC_T_SOCKADDR
typedef struct sockaddr EC_T_SOCKADDR;
#endif

#ifndef EC_T_SOCKADDR_IN
typedef struct sockaddr_in EC_T_SOCKADDR_IN;
#endif

#endif /* EC_SOCKET_SUPPORTED */

/*-MACROS--------------------------------------------------------------------*/
#define     EC_OFFSETOF(s,m)    ((size_t)&(((s *)0)->m))

#define     EC_INTSIZEOF(n)     ( (sizeof(n) + sizeof(int) - 1) & ~(sizeof(int) - 1) )
#ifndef EC_VASTART
#define     EC_VASTART(ap,v)    ( ap = (EC_T_VALIST)&v + EC_INTSIZEOF(v) )
#endif
#ifndef EC_VAARG
#define     EC_VAARG(ap,t)      ( *(t *)((ap += EC_INTSIZEOF(t)) - EC_INTSIZEOF(t)) )
#endif
#ifndef EC_VAEND
#define     EC_VAEND(ap)        ( ap = (EC_T_VALIST)0 )
#endif

#define     EC_UNREFPARM(p)     {(p)=(p);}
#define     EC_MAX(a,b)         (((a) > (b)) ? (a) : (b))
#define     EC_MIN(a,b)         (((a) < (b)) ? (a) : (b))
#define     EC_ENDOF(p)         ((p)+1)

#define     SafeOsFree(p)       {if (EC_NULL!=(p)) {OsFree    (p); (p) = EC_NULL;}}
#define     SafeDelete(p)       {if (EC_NULL!=(p)) {delete    (p); (p) = EC_NULL;}}
#define     SafeDeleteArray(p)  {if (EC_NULL!=(p)) {delete [] (p); (p) = EC_NULL;}}

/* SMP support */
#ifndef EC_CPUSET_DEFINED
/* This macros will be overloaded by the specific EcOsPlatform.h */
#define     EC_CPUSET_ZERO(CpuSet)          (CpuSet) = 0    /* clear all CPU indexes in the CPU set */
#define     EC_CPUSET_SET(CpuSet,nCpuIndex) (CpuSet) = (1<<(nCpuIndex))    /* set CPU index nCpuIndex (0..x) in the CPU set */
#define     EC_CPUSET_SETALL(CpuSet)        (CpuSet) = 0xFFFFFFFF    /* set all CPU indexes in the CPU set */
#endif

#define EC_BIT_MASK(Bitsize)                            ((1<<(Bitsize))-1)
#define EC_BITFIELD_MASK(Bitpos,Bitsize)                ((EC_BIT_MASK((Bitsize)))<<(Bitpos))
#define EC_RESET_WORD_IN_BITFIELD(wVal,Bitpos,Bitsize)  ((wVal)&(~EC_BITFIELD_MASK((Bitpos),(Bitsize))))
#define EC_SET_WORD_IN_BITFIELD(Bitpos,Bitsize,wVal)    (((wVal)&(EC_BIT_MASK((Bitsize))))<<(Bitpos))
#define EC_GET_WORD_IN_BITFIELD(Bitpos,Bitsize,wVal)    (((wVal)>>(Bitpos))&EC_BIT_MASK((Bitsize)))

#define EC_BYTE0(x)     (EC_T_BYTE)(((x) >>  0) & 0xff)
#define EC_BYTE1(x)     (EC_T_BYTE)(((x) >>  8) & 0xff)
#define EC_BYTE2(x)     (EC_T_BYTE)(((x) >> 16) & 0xff)
#define EC_BYTE3(x)     (EC_T_BYTE)(((x) >> 24) & 0xff)
#define EC_BYTE4(x)     (EC_T_BYTE)(((x) >> 32) & 0xff)
#define EC_BYTE5(x)     (EC_T_BYTE)(((x) >> 40) & 0xff)
#define EC_BYTE6(x)     (EC_T_BYTE)(((x) >> 48) & 0xff)
#define EC_BYTE7(x)     (EC_T_BYTE)(((x) >> 56) & 0xff)

#define EC_BYTEN(ptr, n) \
    ((EC_T_BYTE*)(ptr))[(n)]

#define EC_WORD0(x)     (((x) >>  0) & 0xffff)
#define EC_WORD1(x)     (((x) >> 16) & 0xffff)

#define EC_WORDSWAP(x)  (EC_T_WORD)( \
                        ((  (EC_T_WORD)EC_BYTE0((x))) <<  8) | \
                        ((  (EC_T_WORD)EC_BYTE1((x))) <<  0))

#define EC_DWORDSWAP(x) (EC_T_DWORD)( \
                        (( (EC_T_DWORD)EC_BYTE0((x))) << 24) | \
                        (( (EC_T_DWORD)EC_BYTE1((x))) << 16) | \
                        (( (EC_T_DWORD)EC_BYTE2((x))) <<  8) | \
                        (( (EC_T_DWORD)EC_BYTE3((x))) <<  0))

#define EC_QWORDSWAP(x) (EC_T_UINT64)( \
                        (((EC_T_UINT64)EC_BYTE0((x))) << 56) | \
                        (((EC_T_UINT64)EC_BYTE1((x))) << 48) | \
                        (((EC_T_UINT64)EC_BYTE2((x))) << 40) | \
                        (((EC_T_UINT64)EC_BYTE3((x))) << 32) | \
                        (((EC_T_UINT64)EC_BYTE4((x))) << 24) | \
                        (((EC_T_UINT64)EC_BYTE5((x))) << 16) | \
                        (((EC_T_UINT64)EC_BYTE6((x))) <<  8) | \
                        (((EC_T_UINT64)EC_BYTE7((x))) <<  0))

#if !(defined EC_REALSWAP)
    EC_INLINE(EC_T_REAL EC_REALSWAP_INLINE(EC_T_REAL fSrc)) {
    	 EC_T_VOID* pvSrc = (EC_T_VOID*)&fSrc;
         EC_T_UINT64 qwHelper = EC_QWORDSWAP(*((EC_T_UINT64*)pvSrc));
         EC_T_VOID* pvDest = (EC_T_VOID*)&qwHelper;
         return *((EC_T_REAL*)pvDest);
    }
    #define EC_REALSWAP(fSrc) \
         EC_REALSWAP_INLINE(fSrc)
#endif /* EC_REALSWAP */

/* memory access */
#if (defined WITHALIGNMENT)
    #if !(defined EC_SETWORD)
    #define EC_SETWORD(ptr, val) \
    {   \
        EC_BYTEN((ptr), 0) = EC_BYTE0((val)); \
        EC_BYTEN((ptr), 1) = EC_BYTE1((val)); \
    }
    #endif

    #if !(defined EC_SETDWORD)
    /* little endian! LSB on [0], MSB on [3] */
    #define EC_SETDWORD(ptr, val) \
    {   \
        EC_BYTEN((ptr), 0) = EC_BYTE0((val));  \
        EC_BYTEN((ptr), 1) = EC_BYTE1((val));  \
        EC_BYTEN((ptr), 2) = EC_BYTE2((val));  \
        EC_BYTEN((ptr), 3) = EC_BYTE3((val));  \
    }
    #endif

    #if !(defined EC_SETQWORD)
    #define EC_SETQWORD(ptr, val)   \
    {   \
        EC_BYTEN((ptr), 0) = EC_BYTE0((val));  \
        EC_BYTEN((ptr), 1) = EC_BYTE1((val));  \
        EC_BYTEN((ptr), 2) = EC_BYTE2((val));  \
        EC_BYTEN((ptr), 3) = EC_BYTE3((val));  \
        EC_BYTEN((ptr), 4) = EC_BYTE4((val));  \
        EC_BYTEN((ptr), 5) = EC_BYTE5((val));  \
        EC_BYTEN((ptr), 6) = EC_BYTE6((val));  \
        EC_BYTEN((ptr), 7) = EC_BYTE7((val));  \
    }
    #endif

    #if !(defined EC_GETWORD)
    #define EC_GETWORD(ptr) (EC_T_WORD) \
            ((EC_BYTEN((ptr), 0) <<  0) | \
             (EC_BYTEN((ptr), 1) <<  8)   \
            )
    #endif

    #if !(defined EC_GETDWORD)
    #define EC_GETDWORD(ptr) (EC_T_DWORD) \
            ((EC_BYTEN((ptr), 0) <<  0) | \
             (EC_BYTEN((ptr), 1) <<  8) | \
             (EC_BYTEN((ptr), 2) << 16) | \
             (EC_BYTEN((ptr), 3) << 24)   \
            )
    #endif

    #if !(defined EC_GETQWORD)
    #define EC_GETQWORD(ptr) (EC_T_UINT64) \
            ((((EC_T_UINT64)(EC_BYTEN((ptr), 0))) <<  0) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 1))) <<  8) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 2))) << 16) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 3))) << 24) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 4))) << 32) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 5))) << 40) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 6))) << 48) | \
             (((EC_T_UINT64)(EC_BYTEN((ptr), 7))) << 56)   \
            )
    #endif

#else
    #if !(defined EC_SETWORD)
    #define EC_SETWORD(ptr,  val)   ((*((EC_T_WORD*)(ptr))) = (EC_T_WORD)(val))
    #endif
    #if !(defined EC_SETDWORD)
    #define EC_SETDWORD(ptr, val)   ((*((EC_T_DWORD*)(ptr))) = (EC_T_DWORD)(val))
    #endif
    #if !(defined EC_SETQWORD)
    #define EC_SETQWORD(ptr, val)   ((*((EC_T_UINT64*)(ptr))) = (EC_T_UINT64)(val))
    #endif

    #if !(defined EC_GETWORD)
    #define EC_GETWORD(ptr)         (*((EC_T_WORD*)(ptr)))
    #endif
    #if !(defined EC_GETDWORD)
    #define EC_GETDWORD(ptr)        (*((EC_T_DWORD*)(ptr)))
    #endif
    #if !(defined EC_GETQWORD)
    #define EC_GETQWORD(ptr)        (*((EC_T_UINT64*)(ptr)))
    #endif
#endif

#if !(defined EC_GETBOOL)
#define EC_GETBOOL(ptr)         ((EC_T_BOOL)(EC_GETDWORD(ptr))!=EC_FALSE)
#endif

#if !(defined EC_COPYBITS)
    EC_INLINE(EC_T_VOID EC_COPYBITS_INLINE(EC_T_BYTE* pbyDst, EC_T_INT nDstBitOffs, EC_T_BYTE* pbySrc, EC_T_INT nSrcBitOffs, EC_T_INT nBitSize))
    {
        EC_T_INT    nRemLen = nBitSize;            /* remaining length */
        EC_T_BYTE   byMask;
        EC_T_INT    nNumBits = 0;
        EC_T_BYTE*  pSrcHelp = EC_NULL;
        EC_T_WORD   wSrcWork;

        if (nBitSize == 0)
        {
            return;
        }
        /* how many bits we need for the next "destination" byte */
        nNumBits = EC_MIN( (8 - nDstBitOffs%8), nRemLen);

        byMask   = (EC_T_BYTE)((1<<nNumBits)-1);
        byMask   = (EC_T_BYTE)(byMask<<(nDstBitOffs%8));

        /* copy first byte */
        pSrcHelp = &pbySrc[nSrcBitOffs/8];
        wSrcWork = (EC_T_WORD)((EC_T_WORD)pSrcHelp[0] | (((EC_T_WORD)pSrcHelp[1])<<8)); /* xxxx xxxx xx-- ----  e. g. nSrcBitOffs=6, nDstBitOffs=2 */
        wSrcWork = (EC_T_WORD)(wSrcWork>>(nSrcBitOffs%8));                              /* 0000 00xx xxxx xxxx */
        wSrcWork = (EC_T_WORD)(wSrcWork & ((1<<nNumBits)-1));

        pbyDst   = &pbyDst[nDstBitOffs/8];
        *pbyDst  = (EC_T_BYTE)((*pbyDst & ~byMask) | (wSrcWork<<(nDstBitOffs%8)));
        pbyDst++;
        
        nSrcBitOffs = (EC_T_WORD)(nSrcBitOffs + nNumBits);
        nRemLen -= nNumBits;

        while(nRemLen > 0)
        {
            nNumBits  = EC_MIN( 8, nRemLen);

            byMask    = (EC_T_BYTE)((1<<nNumBits)-1);

            pSrcHelp  = &pbySrc[nSrcBitOffs/8];
            wSrcWork  = (EC_T_WORD)(pSrcHelp[0] | (pSrcHelp[1]<<8));   
            wSrcWork  = (EC_T_WORD)(wSrcWork>>(nSrcBitOffs%8));
            wSrcWork &= byMask;

            *pbyDst = (EC_T_BYTE)((*pbyDst & ~byMask) | (wSrcWork));
            pbyDst++;

            nSrcBitOffs = (EC_T_WORD)(nSrcBitOffs + nNumBits);
            nRemLen -= nNumBits;
        }
    }
    #define EC_COPYBITS(pbyDst, nDstBitOffs, pbySrc, nSrcBitOffs, nBitSize) \
        EC_COPYBITS_INLINE(pbyDst, nDstBitOffs, pbySrc, nSrcBitOffs, nBitSize)
#endif /* EC_COPYBITS */

#if !(defined EC_SETBITS)
    #define EC_SETBITS(pbyBuf, pbyData, nDstBitOffs, nBitSize) \
        EC_COPYBITS(pbyBuf, nDstBitOffs, pbyData, 0, nBitSize)
#endif

#if !(defined EC_GETBITS)
    #define EC_GETBITS(pbyBuf, pbyData, nDstBitOffs, nBitSize) \
        EC_COPYBITS(pbyData, 0, pbyBuf, nDstBitOffs, nBitSize)
#endif

#if !(defined EC_MAKEWORD)
#define EC_MAKEWORD(hi, lo)     ((EC_T_WORD )(((EC_T_BYTE)(lo)) | ((EC_T_WORD )((EC_T_BYTE)(hi))) <<  8))
#endif
#if !(defined EC_MAKEDWORD)
#define EC_MAKEDWORD(hi, lo)    ((EC_T_DWORD)(((EC_T_WORD)(lo)) | ((EC_T_DWORD)((EC_T_WORD)(hi))) << 16))
#endif
#if !(defined EC_MAKEQWORD)
#define EC_MAKEQWORD(hi, lo)    ((EC_T_UINT64)(((EC_T_DWORD)(lo)) | ((EC_T_UINT64)((EC_T_DWORD)(hi))) << 32))
#endif

#if !(defined EC_LODWORD)
#define EC_LODWORD(qw)          ((EC_T_DWORD)((EC_T_UINT64)(qw) & 0xFFFFFFFF))
#endif
#if !(defined EC_HIDWORD)
#define EC_HIDWORD(qw)          ((EC_T_DWORD)(((EC_T_UINT64)(qw) >> 32) & 0xFFFFFFFF))
#endif

#if !(defined EC_LOWORD)
#define EC_LOWORD(dw)           ((EC_T_WORD)((dw) & 0xFFFF))
#endif
#if !(defined EC_HIWORD)
#define EC_HIWORD(dw)           ((EC_T_WORD)(((EC_T_DWORD)(dw) >> 16) & 0xFFFF))
#endif

#if !(defined EC_LOBYTE)
#define EC_LOBYTE(w)            ((EC_T_BYTE)((w) & 0xFF))
#endif
#if !(defined EC_HIBYTE)
#define EC_HIBYTE(w)            ((EC_T_BYTE)(((EC_T_WORD)(w)   >>  8) &   0xFF))
#endif

#ifdef EC_BIG_ENDIAN

#define EC_NO_BITFIELDS /* big endian: do not use bitfields! */

#define EC_GET_FRM_BOOL(ptr)        EC_DWORDSWAP(EC_GETDWORD((ptr)))
#define EC_GET_FRM_WORD(ptr)        EC_WORDSWAP( EC_GETWORD((ptr)))
#define EC_GET_FRM_DWORD(ptr)       EC_DWORDSWAP(EC_GETDWORD((ptr)))
#define EC_GET_FRM_QWORD(ptr)       EC_QWORDSWAP(EC_GETQWORD((ptr)))

#define EC_SET_FRM_BOOL(ptr, dw)    EC_SETDWORD((ptr), EC_DWORDSWAP((dw)))
#define EC_SET_FRM_WORD(ptr, w)     EC_SETWORD((ptr),  EC_WORDSWAP((w)))
#define EC_SET_FRM_DWORD(ptr, dw)   EC_SETDWORD((ptr), EC_DWORDSWAP((dw)))
#define EC_SET_FRM_QWORD(ptr, qw)   EC_SETQWORD((ptr), EC_QWORDSWAP((qw)))

#define EC_GET_FRM_WORD_BITFIELD(Bitpos,Bitsize,wVal)           EC_GET_WORD_IN_BITFIELD((Bitpos),(Bitsize),EC_WORDSWAP((wVal)))
#define EC_SET_FRM_WORD_BITFIELD(wVal,wNewVal,Bitpos,Bitsize)   {(wVal) = ((wVal) & ~EC_WORDSWAP(EC_BITFIELD_MASK((Bitpos),(Bitsize)))) | (EC_WORDSWAP((wNewVal)<<(Bitpos)));}

#define EC_NTOHS(w)                 EC_WORDSWAP((w))
#define EC_NTOHL(dw)                EC_DWORDSWAP((dw))
#define EC_NTOHLL(qw)               EC_QWORDSWAP((qw))

#define EC_HTONS(w)                 EC_WORDSWAP((w))
#define EC_HTONL(dw)                EC_DWORDSWAP((dw))
#define EC_HTONLL(qw)               EC_QWORDSWAP((qw))

#else /* EC_BIG_ENDIAN */

#define EC_GET_FRM_BOOL(ptr)        EC_SETDWORD((ptr))
#define EC_GET_FRM_WORD(ptr)        EC_GETWORD((ptr))
#define EC_GET_FRM_DWORD(ptr)       EC_GETDWORD((ptr))
#define EC_GET_FRM_QWORD(ptr)       EC_GETQWORD((ptr))

#define EC_SET_FRM_BOOL(ptr,   w)   EC_SETDWORD((ptr),  (w))
#define EC_SET_FRM_WORD(ptr,   w)   EC_SETWORD((ptr),   (w))
#define EC_SET_FRM_DWORD(ptr, dw)   EC_SETDWORD((ptr), (dw))
#define EC_SET_FRM_QWORD(ptr, qw)   EC_SETQWORD((ptr), (qw))

#define EC_GET_FRM_WORD_BITFIELD(Bitpos,Bitsize,wVal)           EC_GET_WORD_IN_BITFIELD((Bitpos),(Bitsize),(wVal))
#define EC_SET_FRM_WORD_BITFIELD(wVal,wNewVal,Bitpos,Bitsize)   {(wVal) = (EC_T_WORD)(((wVal) & ~EC_BITFIELD_MASK((Bitpos),(Bitsize))) | ((wNewVal)<<(Bitpos)));}

#define EC_NTOHS(w)       ((EC_T_WORD)(w))
#define EC_NTOHL(dw)    ((EC_T_DWORD)(dw))
#define EC_NTOHLL(qw)  ((EC_T_UINT64)(qw))

#define EC_HTONS(w)       ((EC_T_WORD)(w))
#define EC_HTONL(dw)    ((EC_T_DWORD)(dw))
#define EC_HTONLL(qw)  ((EC_T_UINT64)(qw))

#endif /* EC_BIG_ENDIAN */


/*-MACROS--------------------------------------------------------------------*/


/*-FUNCTION DECLARATION------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif

ATECAT_API EC_T_DWORD  OsInit(EC_T_OS_INIT_DESC* pOsParms);
ATECAT_API EC_T_DWORD  OsDeinit(EC_T_VOID);
ATECAT_API EC_T_VOID   OsAddDbgMsgHook(EC_PF_OSDBGMSGHK pfOsDbgMsgHook);

#ifndef OsMalloc
  #define OsMalloc(nSize)                       malloc((size_t)(nSize))
#endif

#ifndef OsFree
 #define OsFree(pvMem)                          free((void*)(pvMem))
#endif

#ifndef OsRealloc
 #define OsRealloc(pMem,nSize)                  realloc((void*)(pMem),(size_t)(nSize))
#endif

#ifndef OsMemset
#define OsMemset(pDest,nVal,nSize)              memset((void*)(pDest),(int)(nVal),(size_t)(nSize))
#endif

#ifndef OsMemcpy
#define OsMemcpy(pDest,pSrc,nSize)              memcpy((void*)(pDest),(const void*)(pSrc),(size_t)(nSize))
#endif

#ifndef OsMemcpyPdIn
#define OsMemcpyPdIn(pDest,pSrc,nSize)            memcpy((void*)(pDest),(const void*)(pSrc),(size_t)(nSize))
#endif

#ifndef OsMemcpyPdOut
#define OsMemcpyPdOut(pDest,pSrc,nSize)            memcpy((void*)(pDest),(const void*)(pSrc),(size_t)(nSize))
#endif

#ifndef OsMemcmp
#define OsMemcmp(pBuf1,pBuf2,nSize)             memcmp((void*)(pBuf1),(const void*)(pBuf2),(size_t)(nSize))
#endif

#ifndef OsMemmove
#define OsMemmove(pDest,pSrc,nSize)             memmove((void*)(pDest),(const void*)(pSrc),(size_t)(nSize))
#endif

#ifndef OsStrlen
#define OsStrlen(szString)                      strlen((const char*)(szString))
#endif

#ifndef OsStrcpy
#define OsStrcpy(szDest,szSrc)                  strcpy((char*)(szDest),(const char*)(szSrc))
#endif

#ifndef OsStrncpy
#define OsStrncpy(szDest,szSrc,nSize)           strncpy((char*)(szDest),(const char*)(szSrc),(size_t)(nSize))
#endif

#ifndef OsStrcmp
#define OsStrcmp(szStr1,szStr2)                 strcmp((const char*)(szStr1),(const char*)(szStr2))
#endif

#ifndef OsStrncmp
#define OsStrncmp(szStr1,szStr2, nSize)         strncmp((const char*)(szStr1),(const char*)(szStr2), (size_t)(nSize))
#endif

#ifndef OsStricmp
#define OsStricmp(szStr1,szStr2)                stricmp((const char*)(szStr1),(const char*)(szStr2))
#endif

#ifndef OsStrtok
#define OsStrtok(szToken,szDelimit)             strtok((char*)(szToken), (const char*)(szDelimit))
#endif

#ifndef OsStrtol
#define OsStrtol(szToken,ppEnd,nRadix)          strtol((const char*)(szToken), (ppEnd), (nRadix))
#endif

#ifndef OsAtoi
#define OsAtoi(szString)                        atoi((const char*)(szString))
#endif

#ifndef OsStrtoul
#define OsStrtoul(szString,ptr,base)            strtoul((const char*)(szString), (ptr), (base))
#endif

#ifndef OsPrintf
#define OsPrintf                                printf
#endif

#ifndef OsVprintf
#define OsVprintf                               vprintf
#endif

#ifndef OsVsnprintf
#define OsVsnprintf                             EcVsnprintf
#endif

#ifndef OsSnprintf
#define OsSnprintf                              EcSnprintf
#endif

#ifndef OsFopen
#define OsFopen                                 fopen
#endif

#ifndef OsFclose
#define OsFclose                                fclose
#endif

#ifndef OsFwrite
#define OsFwrite                                fwrite
#endif

#ifndef OsFread
#define OsFread                                 fread
#endif

#ifndef OsFflush
#define OsFflush                                fflush
#endif

/* this function currently is only used in the MotionDemo application, see also AtXmlParser.cpp */
/*
#ifndef OsGetFileSize
EC_T_INLINE(EC_T_DWORD OsGetFileSize(FILE* fp)) {long l;fseek((fp),0,SEEK_END);l=ftell((fp));fseek((fp),0,SEEK_SET);return l;}
#endif
*/

#ifndef OsDbgAssert
#define OsDbgAssert                             assert
#endif

#ifndef OsSetLastError
#define OsSetLastError(dwError)                 dwError
#endif

#ifndef OsSetEvent
#define OsSetEvent(hEvent)                      SetEvent((HANDLE)(hEvent))
#endif

#ifndef OsResetEvent
#define OsResetEvent(hEvent)                    ResetEvent(hEvent)
#endif

#ifndef OsMemoryBarrier
#define OsMemoryBarrier()                       {   EC_T_DWORD dwDummy = 0; __asm xchg dwDummy, eax }
#endif

/*! \def OsSleep(dwMsec)
    \brief An macro for platform-abstracted Sleep(). 
    \param dwMsec time to sleep in msecs.
    \return N/A.
    Suspends the execution of the current thread for a specified interval. 
    Aligned to ticks it sleeps until the next dwMsec/(tick period) tick.
*/
#ifndef OsSleep
#error  "OsSleep has to be defined in EcOsPlatform.h" 
#endif

#ifndef OsReleaseLinkLayerRegFunc
#define OsReleaseLinkLayerRegFunc(szDriverIdent)
#endif


#ifndef EC_NEW
#define EC_NEW(x)    new x
#endif

EC_T_VOID*  OsCfgFileOpen(const EC_T_CHAR* szCfgFileName);
EC_T_INT    OsCfgFileClose(EC_T_VOID* pvCfgFile);
EC_T_INT    OsCfgFileRead(EC_T_VOID* pvCfgFile, EC_T_VOID* pvDst, EC_T_INT nLen);
EC_T_INT    OsCfgFileError(EC_T_VOID* pvCfgFile);
EC_T_INT    OsCfgFileEof(EC_T_VOID* pvCfgFile);

/* these functions are actually part of the master core */
ATECAT_API EC_T_INT EcVsnprintf(EC_T_CHAR* szDest, EC_T_INT nMaxSize, const EC_T_CHAR* szFormat, EC_T_VALIST vaList);
ATECAT_API EC_T_INT EcSnprintf(EC_T_CHAR* szDest, EC_T_INT nMaxSize, const EC_T_CHAR* szFormat, ...);

#ifndef OsQueryMsecCount
ATECAT_API EC_T_DWORD  OsQueryMsecCount(EC_T_VOID);
#endif
#ifndef OsSleep
EC_T_VOID   OsSleep(EC_T_DWORD dwMsec);
#endif

ATECAT_API EC_T_VOID   OsDbgMsg(const EC_T_CHAR* szFormat, ...);

/***************************/
/* performance measurement */
/***************************/
/* timestamp counter interface for performance measurements (main functions are part of the master core) */
#ifndef INCLUDE_OS_PLATFORM_TSC_SUPPORT

/* if platform does not have specific TSC support, check for defaults if it is yet available */
#if (defined UNDER_CE && defined _M_IX86) || (defined RTOS_32) || (defined VXWORKS && defined CPU && ((CPU==PENTIUM) || (CPU==PENTIUM4))) || (defined QNX6) || ((defined WIN32) && !(defined UNDER_CE)) || ((defined LINUX) && (defined __i386__)) || defined __INTIME__
#define INCLUDE_OS_PLATFORM_TSC_SUPPORT
#define INCLUDE_PENTIUM_TSC
#endif
#if (defined __RCX__)
#define INCLUDE_ARM_TSC
#endif

#endif

typedef EC_T_VOID (*EC_T_FNMESSAGE) (EC_T_CHAR* szMsg);

/* performance measurement descriptors */
#ifdef  __TMS470__
typedef struct _EC_T_TSC_TIME
{
    EC_T_UINT64             qwStart;            /* start time */
    EC_T_UINT64             qwEnd;              /* end time */
    EC_PACKED EC_T_DWORD    dwCurr;             /* [1/10 usec] */
    EC_PACKED EC_T_DWORD    dwMax;              /* [1/10 usec] */
    EC_PACKED EC_T_DWORD    dwAvg;              /* [1/100 usec] */
    EC_PACKED EC_T_BOOL     bMeasReset;         /* EC_TRUE if measurement values shall be reset */
    EC_PACKED EC_T_INT      nIntLevel;          /* for interrupt lockout handling */
} EC_T_TSC_TIME;
#else
typedef struct _EC_T_TSC_TIME
{
    EC_T_UINT64     qwStart;            /* start time */
    EC_T_UINT64     qwEnd;              /* end time */
    EC_T_DWORD      dwCurr;             /* [1/10 usec] */
    EC_T_DWORD      dwMax;              /* [1/10 usec] */
    EC_T_DWORD      dwAvg;              /* [1/100 usec] */
    EC_T_BOOL       bMeasReset;         /* EC_TRUE if measurement values shall be reset */
    EC_T_INT        nIntLevel;          /* for interrupt lockout handling */
} EC_PACKED EC_T_TSC_TIME;
#endif

typedef struct _EC_T_TSC_MEAS_DESC
{
    EC_T_TSC_TIME*  aTscTime;           /* timestamp info array */
    EC_T_DWORD      dwNumMeas;          /* number of elements in aTscTime */
    EC_T_BOOL       bMeasEnabled;       /* EC_TRUE if measurement is enabled */
    EC_T_VOID*      pPrivateData;       /* OS layer private data */
    EC_T_FNMESSAGE  pfnMessage;         /* Function pointer for message drop */
} EC_PACKED EC_T_TSC_MEAS_DESC;

#ifndef OsTscMeasDisableIrq
/* default macros to enable/disable interrupts */
#  if (defined UNDER_CE) && (defined _M_IX86)
#    define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)  { __asm  cli };
#    define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)   { __asm  sti };
#  elif (defined VXWORKS)
#    include "intLib.h"
     /* Some customers don't define _WRS_VX_SMP, but using an SMP kernel.
      * intCpuLock() should also work with the UP kernel (at least for versions > 6) 
      * and will then fall back to intLock().
      */
#    if ( defined(_WRS_VXWORKS_MAJOR) && ((_WRS_VXWORKS_MAJOR == 6) && (_WRS_VXWORKS_MINOR >= 6)) \
         || (_WRS_VXWORKS_MAJOR > 6) ) || defined(_WRS_VX_SMP)
#      define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)  { pTscMeasDesc->aTscTime[dwIndex].nIntLevel = intCpuLock(); };
#      define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)   { intCpuUnlock(pTscMeasDesc->aTscTime[dwIndex].nIntLevel); };
#    else
#      define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)  { pTscMeasDesc->aTscTime[dwIndex].nIntLevel = intLock(); };
#      define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)   { intUnlock(pTscMeasDesc->aTscTime[dwIndex].nIntLevel); };
#    endif /* if ((defined _WRS_VXWORKS_MAJOR) && (_WRS_VXWORKS_MAJOR >= 6)) */
#  elif (defined QNX6)
#    define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)  { ThreadCtl( _NTO_TCTL_IO, 0 );InterruptDisable(); };
#    define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)   { ThreadCtl( _NTO_TCTL_IO, 0 );InterruptEnable(); };
#  elif (defined RTOS_32)
#    define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)  { __asm  {cli }};
#    define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)   { __asm  {sti }};
#  else
#    define OsTscMeasDisableIrq(pTscMeasDesc,dwIndex)
#    define OsTscMeasEnableIrq(pTscMeasDesc,dwIndex)
#  endif
#endif /* ifndef OsTscMeasDisableIrq */

ATECAT_API EC_T_VOID        OsMeasCalibrate(EC_T_UINT64 dwlFreqSet);
ATECAT_API EC_T_DWORD       OsMeasGet100kHzFrequency(EC_T_VOID);
ATECAT_API EC_T_UINT64      OsMeasGetCounterTicks(EC_T_VOID);

/* optional: redirect trace messages into OS specific function, e.g. to store trace log into file 
 * default: print trace log as debug message
 */
#ifndef OsTrcMsg
#define     OsTrcMsg    OsDbgMsg
#endif

ATECAT_API EC_T_VOID*  OsCreateLock(EC_T_VOID);
ATECAT_API EC_T_VOID*  OsCreateLockTyped(EC_T_OS_LOCK_TYPE   eLockType);
ATECAT_API EC_T_VOID   OsDeleteLock(EC_T_VOID* pvLock);
ATECAT_API EC_T_VOID   OsLock(EC_T_VOID* pvLock);
ATECAT_API EC_T_VOID   OsUnlock(EC_T_VOID* pvLock);

ATECAT_API EC_T_VOID*  OsCreateEvent(EC_T_VOID);
ATECAT_API EC_T_VOID   OsDeleteEvent(EC_T_VOID* pvEvent);
#ifndef OsWaitForEvent
ATECAT_API EC_T_DWORD  OsWaitForEvent(EC_T_VOID* pvEvent, EC_T_DWORD dwTimeout);
#endif
#ifndef OsSetEvent
           EC_T_VOID   OsSetEvent(EC_T_VOID* pvEvent);
#endif

#ifndef OsCreateThread
ATECAT_API EC_T_VOID*  OsCreateThread(EC_T_CHAR* szThreadName, EC_PF_THREADENTRY pfThreadEntry, EC_T_DWORD dwPrio, EC_T_DWORD dwStackSize, EC_T_VOID* pvParams);
#endif
#ifndef OsDeleteThreadHandle
ATECAT_API EC_T_VOID   OsDeleteThreadHandle(EC_T_VOID* pvThreadObject);
#endif
#ifndef OsSetThreadPriority
ATECAT_API EC_T_VOID   OsSetThreadPriority( EC_T_VOID* pvThreadObject, EC_T_DWORD dwPrio);
#endif
#ifndef OsSetThreadAffinity
ATECAT_API EC_T_BOOL   OsSetThreadAffinity( EC_T_VOID* pvThreadObject, EC_T_CPUSET CpuSet);
#endif
#ifndef OsGetThreadAffinity
           EC_T_BOOL   OsGetThreadAffinity( EC_T_VOID* pvThreadObject, EC_T_CPUSET* pCpuSet);
#endif

ATECAT_API EC_PF_LLREGISTER OsGetLinkLayerRegFunc( EC_T_CHAR* szDriverIdent );
ATECAT_API EC_T_DWORD  OsSystemTimeGet(EC_T_UINT64* pqwSystemTime);

#ifndef OsMemoryBarrier
EC_T_VOID   OsMemoryBarrier(EC_T_VOID);
#endif

ATECAT_API EC_PF_OSDBGMSGHK OsGetDbgMsgHook(EC_T_VOID);

#ifndef OsForceThreadPrioNormal
ATECAT_API EC_T_DWORD OsForceThreadPrioNormal(EC_T_VOID);
#endif


#if (defined EC_SOCKET_IP_SUPPORTED)

#ifndef EC_FD_SET
#define EC_FD_SET FD_SET 
#endif

#ifndef EC_FD_ZERO
#define EC_FD_ZERO FD_ZERO
#endif

#ifndef EC_FD_ISSET
#define EC_FD_ISSET FD_ISSET
#endif

ATECAT_API EC_T_DWORD OsSocketInit(EC_T_VOID);
ATECAT_API EC_T_DWORD OsSocketDeInit(EC_T_VOID);

#ifndef  OsSocket
#define OsSocket(nAddrFamily, nSockType, nProtocol) socket(nAddrFamily, nSockType, nProtocol)
#endif

#ifndef  OsSocketBind
#define OsSocketBind(hSockHandle, oSockAddr, nSockAddrLen) bind(hSockHandle, oSockAddr, nSockAddrLen)
#endif

#ifndef  OsSocketListen
#define OsSocketListen(hSockHandle, nBacklog) listen(hSockHandle, nBacklog)
#endif

#ifndef  OsSocketSelect
#define OsSocketSelect(nNfds, poReadFds, poWriteFds, poExceptFds, poTimeout) select(nNfds, poReadFds, poWriteFds, poExceptFds, poTimeout)
#endif

#ifndef  OsSocketAccept
#define OsSocketAccept(hSockHandle, oSockAddr, nSockAddrLen) accept(hSockHandle, oSockAddr, nSockAddrLen)
#endif

#ifndef  OsSocketConnect
#define OsSocketConnect(hSockHandle, oSockAddr, nSockAddrLen) connect(hSockHandle, oSockAddr, nSockAddrLen) 
#endif

#ifndef  OsSocketShutdown
#define OsSocketShutdown(hSockHandle, nFlags) shutdown(hSockHandle, nFlags)
#endif

#ifndef  OsSocketSend
#define OsSocketSend(hSockHandle, pbyBuffer, dwBufferLen, dwFlags) send(hSockHandle, pbyBuffer, dwBufferLen, dwFlags)
#endif

#ifndef  OsSocketSendTo
#define OsSocketSendTo(hSockHandle, pbyBuffer, dwBufferLen, dwFlags, oDstAddr, dwDstAddrLen) sendto(hSockHandle, pbyBuffer, dwBufferLen, dwFlags, oDstAddr, dwDstAddrLen)
#endif

#ifndef  OsSocketRecv
#define OsSocketRecv(hSockHandle, pbyBuffer, dwBufferLen, dwFlags) recv(hSockHandle, pbyBuffer, dwBufferLen, dwFlags)
#endif

#ifndef  OsSocketRecvFrom
#define OsSocketRecvFrom(hSockHandle, pbyBuffer, dwBufferLen, dwFlags, oSrcAddr, dwSrcAddrLen) recvfrom(hSockHandle, pbyBuffer, dwBufferLen, dwFlags, oSrcAddr, dwSrcAddrLen)
#endif

#ifndef  OsSocketCloseSocket
#define OsSocketCloseSocket(hSockHandle) close(hSockHandle)
#endif

#ifndef  OsSocketGetLastError
#error "OsSocketGetLastError has to be defined in EcOsPlatform.h"
#endif

#ifndef  OsSocketSetSockOpt
#define OsSocketSetSockOpt(hSockHandle, nLevel, nOptName, pOptValue, nOptLen ) setsockopt(hSockHandle, nLevel, nOptName, pOptValue, nOptLen)
#endif

#ifndef OsHTONS
#define OsHTONS(wHostVal) htons(wHostVal)
#endif

#endif /* EC_SOCKET_IP_SUPPORTED */

#ifndef OsAuxClkInit
#define OsAuxClkInit(dwCpuIndex, dwFrequencyHz, pvOsEvent) EC_FALSE
#endif

#ifndef OsAuxClkDeinit
#define OsAuxClkDeinit(x)
#endif

#ifndef OsAuxClkInitialCountGet
#define OsAuxClkInitialCountGet(pdwInitialCount) EC_E_NOTSUPPORTED
#endif

#ifndef OsAuxClkInputFrequencyGet
#define OsAuxClkInputFrequencyGet(pdwFrequencyHz) EC_E_NOTSUPPORTED
#endif

#ifndef OsAuxClkOutputFrequencyGet
#define OsAuxClkOutputFrequencyGet(pdwFrequencyHz) EC_E_NOTSUPPORTED
#endif

#ifndef OsAuxClkCorrectionSet
#define OsAuxClkCorrectionSet(dwInitialCountNew, nIncrementDif, dwIntCountSet) EC_E_NOTSUPPORTED
#endif

#ifndef OsAuxClkTickSinceInterrupt
#define OsAuxClkTickSinceInterrupt(pqwTicksSinceInterrupt) EC_E_NOTSUPPORTED
#endif

/* request application to terminate, if not implemented: return FALSE */
#ifndef OsTerminateAppRequest
#define OsTerminateAppRequest() EC_FALSE
#endif

#ifdef __cplusplus
} /* extern "C"*/
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

#undef EC_PACKED
#pragma pack (pop)

#else
#endif

#endif /* INC_ECOS */


/*-END OF SOURCE FILE--------------------------------------------------------*/
