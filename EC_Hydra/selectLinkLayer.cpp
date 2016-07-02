/*-----------------------------------------------------------------------------
 * selectLinkLayer.cpp
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Andreas Willig
 * Description              EtherCAT master link layer selection
 *---------------------------------------------------------------------------*/

/*-INCLUDES------------------------------------------------------------------*/
#include "selectLinkLayer.h"
#include "EcLink.h"
#include "ecatDemoCommon.h"

/*-DEFINES-------------------------------------------------------------------*/

/*-TYPEDEFS------------------------------------------------------------------*/

/*-CLASS---------------------------------------------------------------------*/

/*-FUNCTION DECLARATION------------------------------------------------------*/

/*-LOCAL VARIABLES-----------------------------------------------------------*/

/*-FUNCTIONS-----------------------------------------------------------------*/

/* 
 * This function is referencing symbols that may not be available in the BSP.
 * Disable this function by default (not referenced anyway).
 */
#if defined(VXWORKS) && 0 
/********************************************************************************/
/** \brief  Load VxWorks network driver.
*
* \return  EC_TRUE in case of success, EC_FALSE otherwise.
*/
EC_T_BOOL vxNetDrvLoad( char* szDevName, int nDevUnit, char* szDevFullName, END_LOAD_FUNC pfEndLoad, char* szIpAddr )
{
END_OBJ* pCookie   = 0;
END_OBJ* pEnd      = 0;
int      nUnit     = nDevUnit;
EC_T_BOOL bOk      = EC_FALSE;
#if ( ((defined _WRS_VXWORKS_MAJOR) && (_WRS_VXWORKS_MAJOR >= 6)) && ((defined _WRS_VXWORKS_MINOR) && (_WRS_VXWORKS_MINOR >= 5)) ) || (defined IFCONFIG_SUPPORT)
char     szEndConfig[255] = {0};
struct in_addr  inetAddress;
char     szMask[INET_ADDRSTRLEN];
#endif /* _WRS_VXWORKS_... */

    OsDbgMsg("Attaching interface %s...", szDevFullName);

    pEnd = endFindByName(szDevName, nUnit);
    if( pEnd == EC_NULL )
    {
        /* load device */
        pCookie = (END_OBJ*)muxDevLoad(nUnit, (END_LOAD_FUNC)pfEndLoad, "", BUFF_LOAN, 0);
        OsDbgMsg ("\nmuxDevLoad return cookie = 0x%08x\n", (unsigned long)pCookie);
        if (pCookie == EC_NULL)
        {
            OsDbgMsg("\nmuxDevLoad failed! (errno=0x%08X)\n", errno);
            goto Exit;
        }
        else
        {
            if (muxDevStart(pCookie) == ERROR)
            {
                OsDbgMsg("\nmuxDevStart failed! (errno=0x%08X)\n", errno);
                goto Exit;
            }
        }
    }
    /* find the END_OBJ */
    pEnd = endFindByName(szDevName, nUnit);
    if (pEnd == EC_NULL)
    {
        OsDbgMsg("\nCould not find device %s%d!\n",szDevName, nUnit);
        goto Exit;
    }
    else
    {
        if (ipAttach(nUnit, szDevName) != OK)
        {
            OsDbgMsg("\nFailed to attach to device %s\n", szDevName);
            OsDbgMsg("possible reason: VxWorks parameter 'IP_MAX_UNITS' is too small!\n");
            goto Exit;
        }
#if ( ((defined _WRS_VXWORKS_MAJOR) && (_WRS_VXWORKS_MAJOR >= 6)) && ((defined _WRS_VXWORKS_MINOR) && (_WRS_VXWORKS_MINOR >= 5)) ) || (defined IFCONFIG_SUPPORT)
        inetAddress.s_addr = htonl(NETMASK);
        inet_ntop (AF_INET, (void *) &inetAddress, szMask, INET_ADDRSTRLEN);
        sprintf(szEndConfig, "%s%d inet %s netmask %s up",szDevName, nUnit, szIpAddr, szMask);
        if (ifconfig(szEndConfig) != OK)
        {
            OsDbgMsg("\nError setting inet address of %s to %s, (errno=0x%08X)\n", szDevFullName, szIpAddr, errno);
            goto Exit;
        }
#else
        /* set subnet mask, if any specified */
        ifMaskSet(szDevFullName, NETMASK);

        /* set inet address */
        if (ifAddrSet(szDevFullName, szIpAddr) != OK)
        {
            OsDbgMsg("\nError setting inet address of %s to %s, (errno=0x%08X)\n", szDevFullName, szIpAddr, errno);
            goto Exit;
        }
#endif
        OsDbgMsg("done\n");
    }
    bOk = EC_TRUE;

Exit:
    return bOk;
}
#endif /* if defined(VXWORKS) && 0 */




/********************************************************************************/
/** \brief  Parse for the ip-address and return an byte array (EC_T_BYTE[4])
*
* \return  EC_TRUE in case of success, EC_FALSE in case of an invalid parameter or
*          SYNTAX_ERROR (-2) .
*/

EC_T_BOOL ParseIpAddress
(
	EC_T_CHAR* ptcWord,
    EC_T_BYTE* pbyIpAddress)
{
    EC_T_CHAR* ptcTmp   = EC_NULL;
    EC_T_INT   nCnt     = 0;
    EC_T_DWORD bRetVal  = EC_TRUE;

    if(EC_NULL == pbyIpAddress )
    {
        bRetVal = EC_FALSE;
        goto Exit;
    }

    /* Get IP address */
    ptcTmp = ptcWord;
    for (nCnt = 0; nCnt < 4; nCnt++)
    {
        if (ptcTmp == EC_NULL)
        {
            bRetVal = (EC_T_DWORD)SYNTAX_ERROR;
            goto Exit;
        }
        pbyIpAddress[nCnt] = (EC_T_BYTE)OsStrtol(ptcTmp, EC_NULL, 10);
        if(nCnt < 2)
        {
            ptcTmp = OsStrtok( EC_NULL, ".");
        }
        else if(nCnt < 3)
        {
            ptcTmp = OsStrtok( EC_NULL, " ");
        }
    }

    Exit:

    return bRetVal;
}


/********************************************************************************/
/** Parse next command line argument
*
* Return: pointer to the next argument.
*/
EC_T_CHAR* GetNextWord(EC_T_CHAR **ppCmdLine, EC_T_CHAR *pStorage)
{
    EC_T_CHAR *pWord;

    EC_UNREFPARM(ppCmdLine);
    EC_UNREFPARM(pStorage);

    pWord = (EC_T_CHAR *)OsStrtok(NULL, " ");

    return pWord;
}


#if !(defined INCLUDE_ATEMRAS) && ((defined UNDER_CE) || (defined LINUX) || (defined INCLUDE_DM9000) || (defined LINK_HNX) || (defined __INTEGRITY))

/***************************************************************************************************/
/**
\brief  Select Link Layer.

This function checks whether parameter portion is a LinkLayer information and processes it
\return EC_TRUE if parameter is LinkLayer Portion, EC_FALSE otherwise.
*/
static EC_T_VOID InitLinkLayerParam
(   EC_T_BYTE**             ppbyLinkLayer,
    EC_T_LINK_DEV_PARAM*    pLinkDevParam,
    EC_T_CHAR*              szIdent
)
{
    pLinkDevParam->dwValidationPattern = LINK_LAYER_DEV_PARAM_PATTERN;
    pLinkDevParam->dwDevParamVersion = LINK_LAYER_DEV_PARAM_VERSION;
    OsStrncpy( pLinkDevParam->szDriverIdent, szIdent, sizeof(pLinkDevParam->szDriverIdent) - 1);
    pLinkDevParam->bForceAllocSendFrame = EC_FALSE;
    pLinkDevParam->pvDrvSpecificParam = (EC_T_BYTE*)pLinkDevParam + sizeof(EC_T_LINK_DEV_PARAM);  /* append parameters at the end */
    *ppbyLinkLayer = (EC_T_BYTE*)pLinkDevParam->pvDrvSpecificParam;
}
#endif

/***************************************************************************************************/
/**
\brief  Tries to create NIOS link layer if parameters correspond to NIOS.

\return EC_TRUE if NIOS layer specified and it was created, EC_FALSE otherwise.
*/
static inline EC_T_BOOL CreateNiosLayer
(
    EC_T_CHAR**     ptcWord,
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam
)
{
#if !(defined EC_VERSION_NIOS)
    EC_UNREFPARM(ptcWord);
    EC_UNREFPARM(poLinkDevOpenParam);

    return EC_FALSE;
#else
    EC_T_BOOL bResult = EC_FALSE;
    if (OsStricmp( (*ptcWord), "-nios") == 0)
    {
        CreateLinkParaNios(poLinkDevOpenParam);

        bResult = EC_TRUE;
    }
    return bResult;
#endif /* !(defined EC_VERSION_NIOS) */
}

/***************************************************************************************************/
/**
\brief  Select Link Layer.

This function checks whether parameter portion is a LinkLayer information and processes it
\return EC_TRUE if parameter is LinkLayer Portion, EC_FALSE otherwise.
*/
EC_T_BOOL DoSelectLinkLayer
(   EC_T_CHAR**     ptcWord,
    EC_T_CHAR**     lpCmdLine,
    EC_T_CHAR*      tcStorage,
    EC_T_LINK_DEV_OPEN_PARAM* poLinkDevOpenParam,
    EC_T_INT*       pRetVal,
    EC_T_DWORD      dwRecvPriority,
    EC_T_BOOL*      pbIsPrimaryLinkLayer /* [out]  Redundancy support: EC_TRUE if it is the primary link layer, EC_FALSE otherwise */
)
{
    EC_T_BOOL                   bIsLinkLayerEntry   = EC_FALSE;
    EC_T_DWORD                  dwCopyLen           = 0;
    EC_T_DWORD                  dwMaxParamSize = sizeof(EC_T_LINK_DEV_OPEN_PARAM);


    bIsLinkLayerEntry  = EC_FALSE;

    if(pbIsPrimaryLinkLayer != EC_NULL)
    {
        (*pbIsPrimaryLinkLayer) = EC_TRUE;
    }

#if !(defined INCLUDE_ATEMRAS) && ((defined UNDER_CE) || (defined LINUX) || (defined INCLUDE_DM9000) || (defined LINK_HNX) || (defined __INTEGRITY))
    EC_T_LINK_DEV_PARAM*    pLinkDevParam       = (EC_T_LINK_DEV_PARAM*)&poLinkDevOpenParam->oLinkDevParam;
    EC_T_BYTE*              pbyLinkLayer        = (EC_T_BYTE*)&poLinkDevOpenParam->oLinkDevParam;
#endif

    EC_UNREFPARM(dwRecvPriority);
    EC_UNREFPARM(pRetVal);
    EC_UNREFPARM(tcStorage);
    EC_UNREFPARM(lpCmdLine);
    EC_UNREFPARM(dwCopyLen);
    EC_UNREFPARM(dwMaxParamSize);

	/* Parameter check */
    if(poLinkDevOpenParam == EC_NULL)
    {
        goto Exit;
    }

    if( '-' != (*ptcWord)[0] )
    {
        goto Exit;
    }

#if (defined WIN32) && !(defined UNDER_CE) && !(defined RTOS_32)
    if ( OsStricmp( (*ptcWord), "-WinPcap") == 0 || OsStricmp( (*ptcWord), "-WinPcap2") == 0 )
    {
        EC_T_BOOL       bRet              = EC_FALSE;
        EC_T_BYTE       abyIpAddress[4]   = {0};
        EC_T_INT        nMode             = 0;
        EC_T_LINKMODE   eLinkMode         = EcLinkMode_POLLING;

        /* Get next word */
        (*ptcWord) = OsStrtok(EC_NULL, ".");

        /* No IP-Address specified.*/
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            /* WinPcap link layer uses the IP address to select the appropriate network interface */
            OsDbgMsg( "Use the first network adapter found in the adapter list to send EtherCAT telegrams\n" );
            goto Exit;
        }

        /* Parse the specified IP-Address  */
        bRet = ParseIpAddress(*ptcWord, abyIpAddress);
        if(!bRet || bRet == SYNTAX_ERROR)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        else
        {
            nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

            if(nMode == 0)
            {
                eLinkMode = EcLinkMode_INTERRUPT;
            }
            else if(nMode == 1)
            {
                eLinkMode = EcLinkMode_POLLING;
            }
            else
            {
                OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n", nMode );
                *pRetVal = SYNTAX_ERROR;
                goto Exit;
            }
        }

        /* Create the link layer parameter */
        bRet = CreateLinkParaWinPcap(poLinkDevOpenParam, abyIpAddress, eLinkMode);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }


        if((pbIsPrimaryLinkLayer != EC_NULL) && (OsStricmp( (*ptcWord), "-WinPcap2") == 0))
        {
            if(pbIsPrimaryLinkLayer != EC_NULL) *pbIsPrimaryLinkLayer = EC_FALSE;
        }

        bIsLinkLayerEntry = EC_TRUE;
    }

#endif
#if (defined UNDER_CE) && !(defined INCLUDE_ATEMRAS)
    else if (OsStricmp( (*ptcWord), "-NdisUio") == 0)
    {
#define NDISUIO_DEVNAME   TEXT("ECT1:")
#define NDISUIO_DRIVERKEY TEXT("Drivers\\BuiltIn\\ECAT")
        HANDLE                      hNdisUioDevice  = EC_NULL;
        HANDLE                      hNdisUioDriver  = EC_NULL;
        EC_T_LINK_OPENPARMS_NDISUIO oLinkOpenParms  = {0};

        EC_T_WCHAR*                 pszAdapterName  = EC_NULL;

        InitLinkLayerParam( &pbyLinkLayer, pLinkDevParam, "NdisUio" );

        /* get next word */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        /* check if NDISUIO driver started */
        OsDbgMsg("Check if ECAT driver is started...");
        hNdisUioDevice = CreateFile( NDISUIO_DEVNAME, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
        if ((hNdisUioDevice != EC_NULL) && (hNdisUioDevice != INVALID_HANDLE_VALUE))
        {
            OsDbgMsg("Ok!\n");
        }
        else
        {
            OsDbgMsg("Failed!\n");
            hNdisUioDevice = EC_NULL;
        }
        /* try to load driver if not already loaded */
        if (hNdisUioDevice == EC_NULL)
        {
            OsDbgMsg("Try to load ECAT driver...");
            hNdisUioDriver = ActivateDeviceEx (NDISUIO_DRIVERKEY, 0,  0, 0);
            if ((hNdisUioDriver != EC_NULL) && (hNdisUioDriver != INVALID_HANDLE_VALUE))
            {
                OsDbgMsg( "Ok!\n" );
            }
            else
            {
                OsDbgMsg( "Failed!\n" );
                hNdisUioDriver = EC_NULL;
                goto Exit;
            }
        }
        /* check if driver is available */
        if ((hNdisUioDevice == EC_NULL) && (hNdisUioDriver == EC_NULL))
        {
            OsDbgMsg("No NDISUIO ECAT driver available!!!\n" );
            goto Exit;
        }
        else if (hNdisUioDevice != EC_NULL)
        {
            /* close handle, it was just for the check */
            CloseHandle(hNdisUioDevice);
            hNdisUioDevice = EC_NULL;
        }
        /* NdisUio uses the network adapter name to select the appropriate network interface */
        oLinkOpenParms.dwType               = EC_LINK_TYPE_NDISUIO;
        pszAdapterName = (EC_T_WCHAR*)&pbyLinkLayer[sizeof(oLinkOpenParms)];

#ifdef  UNICODE
        _snwprintf((wchar_t*)pszAdapterName, 64, L"%S", (*ptcWord));
#else
        _snwprintf(pszAdapterName, 64, L"%s", (*ptcWord));
#endif

        oLinkOpenParms.szNetworkAdapterName = pszAdapterName;
        oLinkOpenParms.dwRcvThreadPriority  = dwRecvPriority;
        pLinkDevParam->eLinkMode            = EcLinkMode_POLLING;


        dwCopyLen = EC_MIN(sizeof(oLinkOpenParms), dwMaxParamSize);
        OsMemcpy(pbyLinkLayer, &oLinkOpenParms, dwCopyLen);
        bIsLinkLayerEntry = EC_TRUE;
    }
#if (defined INCLUDE_DM9000)
    else if (OsStricmp( (*ptcWord), "-dm9000") == 0)
    {
        EC_T_LINK_OPENPARMS_DM9000  oLinkOpenParmsDM9000    = {0};

        InitLinkLayerParam( &pbyLinkLayer, pLinkDevParam, "DM9000" );

        oLinkOpenParmsDM9000.dwType                 = EC_LINK_TYPE_DM9000;
        oLinkOpenParmsDM9000.dwRcvThreadPriority    = dwRecvPriority;
        oLinkOpenParmsDM9000.dwBaseAddress          = 0x08000300;   /* CS2 phys */
        oLinkOpenParmsDM9000.dwBaseIO               = 0x320;        /* IO base address when IO is used */
        oLinkOpenParmsDM9000.dwIrqNum               = 0x3;          /* DM9000 ISA Evaluation Board */   /* Bosch ATMO IRQ 0x01000065; */
        oLinkOpenParmsDM9000.dwFrameBufferLen       = 0x20;

        oLinkOpenParmsDM9000.eLinkMode              = EcLinkMode_INTERRUPT;

        oLinkOpenParmsDM9000.pfDoIntHandling        = EC_NULL;

        dwCopyLen = EC_MIN(sizeof(oLinkOpenParmsDM9000), dwMaxParamSize);

        OsMemcpy(pbyLinkLayer, &oLinkOpenParmsDM9000, dwCopyLen);

        bIsLinkLayerEntry = EC_TRUE;
    }
#endif
#endif /* #ifdef UNDER_CE */
#if (defined VXWORKS)
    else if (OsStrcmp( (*ptcWord), "-snarf") == 0)
    {
        EC_T_BOOL   bRet                                        = EC_FALSE;
        EC_T_WCHAR  aszAdapterName[MAX_LEN_SNARF_ADAPTER_NAME]  = {0};

        /* Get next word: adapter name */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (  OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        OsStrcpy((char*)aszAdapterName, (char*)(*ptcWord));

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaSnarf(poLinkDevOpenParam, aszAdapterName);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        bIsLinkLayerEntry = EC_TRUE;
    }
#if (defined INCLUDE_ETHERLIB)
    else if (OsStrcmp( (*ptcWord), "-etherlib") == 0)
    {
        EC_T_BOOL   bRet                                        = EC_FALSE;
        EC_T_WCHAR  aszAdapterName[MAX_LEN_SNARF_ADAPTER_NAME]  = {0};

        /* Get next word: adapter name */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (  OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        OsStrcpy((char*)aszAdapterName, (char*)(*ptcWord));

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaEtherlib(poLinkDevOpenParam, aszAdapterName);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        bIsLinkLayerEntry = EC_TRUE;
    }
#endif /* (defined INCLUDE_ETHERLIB) */
#endif /* #ifdef VXWORKS */
#if (defined LINUX)
    else if (OsStrcmp( (*ptcWord), "-SockRaw") == 0)
    {
        EC_T_WCHAR  aszAdapterName[64]  = {0};

        InitLinkLayerParam( &pbyLinkLayer, pLinkDevParam, (EC_T_CHAR*)"SockRaw" );

        /* undef also SOCK_RAW_POLLING_MODE in EcDeviceSockRaw.cpp if interrupt mode should be used */
        pLinkDevParam->eLinkMode = EcLinkMode_POLLING;

        /* get next word */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (  OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        OsStrncpy((char*)aszAdapterName, (char*)(*ptcWord), sizeof(aszAdapterName) - 1);

        dwCopyLen = EC_MIN(sizeof(aszAdapterName), dwMaxParamSize);
        OsMemcpy(pbyLinkLayer, aszAdapterName, dwCopyLen);

        bIsLinkLayerEntry = EC_TRUE;
    }
#endif /* #ifdef LINUX */
#if (defined __INTEGRITY)
    else if (OsStrcmp( (*ptcWord), "-GhEth") == 0)
    {
        EC_T_WCHAR  aszAdapterName[64]  = {0};

        InitLinkLayerParam( &pbyLinkLayer, pLinkDevParam, "GhEth" );

        /* get next word */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (  OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        OsStrcpy( (char*)aszAdapterName, (char*)(*ptcWord) );

        dwCopyLen = EC_MIN(sizeof(aszAdapterName), dwMaxParamSize);
        OsMemcpy(pbyLinkLayer, aszAdapterName, dwCopyLen);

        bIsLinkLayerEntry = EC_TRUE;
    }
#endif /* #ifdef __INTEGRITY */
#ifdef LINK_HNX
    else if (OsStricmp( (*ptcWord), "-hnx") == 0)
    {
        EC_T_DWORD                      dwInstanceNr    = 0;
        static EC_T_LINK_OPENPARMS_HNX  oParm0          = HNX0_ARRAYIDX_INIT;
        static EC_T_LINK_OPENPARMS_HNX  oParm1          = HNX1_ARRAYIDX_INIT;

        EC_T_BYTE                       byMCast[6]      = {0x01,0x01,0x05,0x01,0x00,0x00};

        InitLinkLayerParam( &pbyLinkLayer, pLinkDevParam, (EC_T_CHAR*)"Hnx" );

        oParm0.dwSendBuffers = 10;
        oParm1.dwSendBuffers = 10;

        oParm0.eLinkMode  = EcLinkMode_POLLING;
        oParm1.eLinkMode  = EcLinkMode_POLLING;


        oParm0.bPromiscuousMode = EC_TRUE;
        oParm1.bPromiscuousMode = EC_TRUE;

        oParm0.bSetMulticast    = EC_TRUE;
        oParm1.bSetMulticast    = EC_TRUE;

        OsMemcpy(oParm0.byMCMacAddr, byMCast, 6);
        OsMemcpy(oParm1.byMCMacAddr, byMCast, 6);

        /* get next word */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (
            ((*ptcWord) == EC_NULL)
         || (OsStrncmp( (*ptcWord), "-", 1) == 0)
           )
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        dwInstanceNr = OsStrtol((*ptcWord), EC_NULL, 10);

        OsDbgMsg("Hilscher NetX raw driver selected instance %d\n", dwInstanceNr);

        switch(dwInstanceNr)
            {
            case HNX0_ARRAYIDX:
                dwCopyLen = EC_MIN(sizeof(EC_T_LINK_OPENPARMS_HNX), dwMaxParamSize);
                OsMemcpy(pbyLinkLayer, &oParm0, dwCopyLen);

                bIsLinkLayerEntry = EC_TRUE;
                break;
            case HNX1_ARRAYIDX:
                dwCopyLen = EC_MIN(sizeof(EC_T_LINK_OPENPARMS_HNX), dwMaxParamSize);
                OsMemcpy(pbyLinkLayer, &oParm1, dwCopyLen);

                bIsLinkLayerEntry = EC_TRUE;
                break;
            default:
                bIsLinkLayerEntry = EC_FALSE;
                break;
            };
    }
#endif /* #ifdef LINK_HNX */
#if ((defined UNDER_CE) || (defined VXWORKS) || (defined RTOS_32) || (defined UNDER_RTSS) || (defined QNX6) || (defined LINUX) || (defined __INTEGRITY) || (defined __INTIME__) || (defined __TKERNEL)) && !(defined INCLUDE_ATEMRAS)
    else if (OsStricmp( (*ptcWord), "-i8255x") == 0)
    {
        EC_T_BOOL     bRet       = EC_FALSE;
        EC_T_INT      nMode      = 0;
        EC_T_LINKMODE eLinkMode  = EcLinkMode_UNDEFINED;
        EC_T_DWORD    dwInstance = 0;

        /* Get next word: instance */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        dwInstance = (EC_T_DWORD)OsStrtol((*ptcWord), NULL, 0);

        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n", nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaI8255x(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        bIsLinkLayerEntry = EC_TRUE;

    }
    else if (OsStricmp( (*ptcWord), "-i8254x") == 0)
    {
        EC_T_BOOL     bRet       = EC_FALSE;
        EC_T_INT      nMode      = 0;
        EC_T_LINKMODE eLinkMode  = EcLinkMode_UNDEFINED;
        EC_T_DWORD    dwInstance = 0;

        /* Get next word: instance */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        dwInstance = (EC_T_DWORD)OsStrtol((*ptcWord), NULL, 0);

        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n",nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaI8254x(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        bIsLinkLayerEntry = EC_TRUE;

    }
    else if (OsStricmp( (*ptcWord), "-fslfec") == 0)
    {
        EC_T_BOOL     bRet       = EC_FALSE;
        EC_T_INT      nMode      = 0;
        EC_T_LINKMODE eLinkMode  = EcLinkMode_UNDEFINED;
        EC_T_DWORD    dwInstance = 0;

        /* Get next word: instance */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        dwInstance = (EC_T_DWORD)OsStrtol((*ptcWord), NULL, 0);

        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n",nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaFslFec(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        bIsLinkLayerEntry = EC_TRUE;

    }
    else if (OsStricmp( (*ptcWord), "-rtl8139") == 0)
    {
        EC_T_BOOL     bRet       = EC_FALSE;
        EC_T_INT      nMode      = 0;
        EC_T_LINKMODE eLinkMode  = EcLinkMode_UNDEFINED;
        EC_T_DWORD    dwInstance = 0;

        /* Get next word: instance */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        dwInstance = (EC_T_DWORD)OsStrtol((*ptcWord), NULL, 0);

        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)",nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaRTL8139(poLinkDevOpenParam, dwInstance, (EC_T_LINKMODE)eLinkMode, dwRecvPriority);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        bIsLinkLayerEntry = EC_TRUE;
    }
    else if (OsStricmp( (*ptcWord), "-rtl8169") == 0)
    {
      EC_T_BOOL     bRet       = EC_FALSE;
      EC_T_INT      nMode      = 0;
      EC_T_LINKMODE eLinkMode  = EcLinkMode_UNDEFINED;
      EC_T_DWORD    dwInstance = 0;

      /* Get next word: instance */
      (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
      if (((*ptcWord) == NULL) || (OsStrncmp((*ptcWord), "-", 1) == 0))
      {
         *pRetVal = SYNTAX_ERROR;
         goto Exit;
      }
      dwInstance = (EC_T_DWORD)OsStrtol((*ptcWord), NULL, 0);

      /* Get next word: mode */
      (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
      if (((*ptcWord) == NULL) || (OsStrncmp((*ptcWord), "-", 1) == 0))
      {
         *pRetVal = SYNTAX_ERROR;
         goto Exit;
      }
      nMode = (EC_T_INT) OsStrtol((*ptcWord), NULL, 10);

      if (nMode == 0)
      {
         eLinkMode = EcLinkMode_INTERRUPT;
      }
      else if (nMode == 1)
      {
         eLinkMode = EcLinkMode_POLLING;
      }
      else
      {
         OsDbgMsg("Invalid Link Layer Mode (mode == %d)", nMode);
         *pRetVal = SYNTAX_ERROR;
         goto Exit;
      }

      /* Fill Link Layer parameter structure */
      bRet = CreateLinkParaRTL8169(poLinkDevOpenParam, dwInstance, (EC_T_LINKMODE) eLinkMode, dwRecvPriority);
      if (!bRet)
      {
         *pRetVal = SYNTAX_ERROR;
         goto Exit;

      }

      bIsLinkLayerEntry = EC_TRUE;
   }
#endif
#if (defined(LINUX) && defined(_ARCH_PPC)) || (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC32))
   else if (OsStricmp( (*ptcWord), "-fsletsec") == 0)
   {
       EC_T_BOOL     bRet       = EC_FALSE;
       EC_T_INT      nMode      = 0;
       EC_T_LINKMODE eLinkMode  = EcLinkMode_UNDEFINED;
       EC_T_DWORD    dwInstance = 0;

       /* Get next word: instance */
       (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
       if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
       {
           *pRetVal = SYNTAX_ERROR;
           goto Exit;
       }
       dwInstance = (EC_T_DWORD)OsStrtol((*ptcWord), NULL, 0);

       /* Get next word: mode */
       (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
       if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
       {
           *pRetVal = SYNTAX_ERROR;
           goto Exit;
       }
       nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

       if(nMode == 0)
       {
           eLinkMode = EcLinkMode_INTERRUPT;
       }
       else if(nMode == 1)
       {
           eLinkMode = EcLinkMode_POLLING;
       }
       else
       {
           OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n",nMode );
           *pRetVal = SYNTAX_ERROR;
           goto Exit;
       }

       /* Fill Link Layer parameter structure */
       bRet = CreateLinkParaFslEtsec(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
       if(!bRet)
       {
           *pRetVal = SYNTAX_ERROR;
           goto Exit;

       }

       bIsLinkLayerEntry = EC_TRUE;
   }
#endif /* if (defined(LINUX) && defined(_ARCH_PPC)) || (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC32)) */
#if (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC440))
    else if (OsStricmp( (*ptcWord), "-emac") == 0)
    {
        EC_T_BOOL     bRet       = EC_FALSE;
        EC_T_INT      nMode      = 0;
        EC_T_LINKMODE eLinkMode  = EcLinkMode_UNDEFINED;
        EC_T_DWORD    dwInstance = 0;

        /* Get next word: instance */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        dwInstance = (EC_T_DWORD)OsStrtol((*ptcWord), NULL, 0);

        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n",nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaEmac(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        bIsLinkLayerEntry = EC_TRUE;
    } 
#endif /* (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC440)) */
#if (defined INCLUDE_PXAMFU)
    else if (OsStricmp( (*ptcWord), "-pxamfu") == 0)
    {
        EC_T_BOOL     bRet       = EC_FALSE;
        EC_T_INT      nMode      = 0;
        EC_T_LINKMODE eLinkMode  = EcLinkMode_UNDEFINED;
        EC_T_DWORD    dwInstance = 0;

        /* Get next word: instance */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        dwInstance = (EC_T_DWORD)OsStrtol((*ptcWord), NULL, 0);

        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n",nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaPxaMfu(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        bIsLinkLayerEntry = EC_TRUE;
    }
#endif
    else if (OsStricmp( (*ptcWord), "-CCAT") == 0)
    {
        EC_T_BOOL     bRet       = EC_FALSE;
        EC_T_INT      nMode      = 0;
        EC_T_LINKMODE eLinkMode  = EcLinkMode_UNDEFINED;
        EC_T_DWORD    dwInstance = 0;

        /* Get next word: instance */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        dwInstance = (EC_T_DWORD)OsStrtol((*ptcWord), NULL, 0);

        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n",nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaCCAT(poLinkDevOpenParam, dwInstance, eLinkMode, dwRecvPriority);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        bIsLinkLayerEntry = EC_TRUE;

    }
#ifdef STARTERWARE_NOOS
    else if (OsStricmp( (*ptcWord), "-swcpsw") == 0)
    {
        EC_T_BOOL bRet          = EC_FALSE;
        EC_T_INT nMode          = 0;
        EC_T_BOOL bMaster = EC_FALSE;
        EC_T_LINKMODE eLinkMode = EcLinkMode_UNDEFINED;
        EC_T_INT nPort      = 0;
        EC_T_INT nPortPrio      = 0;

        /* Get next word: port */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nPort = (EC_T_BYTE)OsStrtol((*ptcWord), NULL, 10);

        /* Get next word: port priority */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nPortPrio = (EC_T_BYTE)OsStrtol((*ptcWord), NULL, 10);

        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n",nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

         /* Get next word: Master flag */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        bMaster = (*ptcWord)[0] == 'm';

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaCPSW(poLinkDevOpenParam,
           nPort, eLinkMode, dwRecvPriority, nPortPrio, bMaster, EC_TRUE);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        bIsLinkLayerEntry = EC_TRUE;
    }
#endif
#if (defined INCLUDE_CPSW)
    else if (OsStricmp( (*ptcWord), "-cpsw") == 0)
    {
        EC_T_BOOL bRet          = EC_FALSE;
        EC_T_INT nMode          = 0;
        EC_T_BOOL bMaster = EC_FALSE;
        EC_T_LINKMODE eLinkMode = EcLinkMode_UNDEFINED;
        EC_T_INT nPort      = 0;
        EC_T_INT nPortPrio      = 0;

        /* Get next word: port */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nPort = (EC_T_BYTE)OsStrtol((*ptcWord), NULL, 10);

        /* Get next word: port priority */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nPortPrio = (EC_T_BYTE)OsStrtol((*ptcWord), NULL, 10);

        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n",nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

         /* Get next word: Master flag */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        bMaster = (*ptcWord)[0] == 'm';

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaCPSW(poLinkDevOpenParam,
           nPort, eLinkMode, dwRecvPriority, nPortPrio, bMaster, EC_FALSE);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        bIsLinkLayerEntry = EC_TRUE;

    }
#endif /* if (defined INCLUDE_CPSW) */
#if (defined INCLUDE_SMSC)
    else if (OsStricmp( (*ptcWord), "-smsc") == 0)
    {
        EC_T_INT nMode          = 0;
        EC_T_LINKMODE eLinkMode = EcLinkMode_UNDEFINED;

        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n",nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n",nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        /* Fill Link Layer parameter structure */
        EC_T_BOOL bRet = CreateLinkParaSmscL9218i(poLinkDevOpenParam, eLinkMode, dwRecvPriority);
        if (!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        bIsLinkLayerEntry = EC_TRUE;
    }
#endif /* if (defined INCLUDE_SMSC) */
#if (defined INCLUDE_GEM)
    else if (OsStricmp( (*ptcWord), "-gem") == 0)
    {
        EC_T_BOOL bRet          = EC_FALSE;
        EC_T_INT nMode          = 0;
        EC_T_LINKMODE eLinkMode = EcLinkMode_UNDEFINED;
        EC_T_DWORD nPort        = 0;

        /* Get next word: port */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nPort = (EC_T_BYTE)OsStrtol((*ptcWord), NULL, 10);
        
        /* Get next word: mode */
        (*ptcWord) = GetNextWord(lpCmdLine, tcStorage);
        if (((*ptcWord) == EC_NULL) || (OsStrncmp( (*ptcWord), "-", 1) == 0))
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }
        nMode = (EC_T_INT)OsStrtol((*ptcWord), NULL, 10);

        if(nMode == 0)
        {
            eLinkMode = EcLinkMode_INTERRUPT;
        }
        else if(nMode == 1)
        {
            eLinkMode = EcLinkMode_POLLING;
        }
        else
        {
            OsDbgMsg( "Invalid Link Layer Mode (mode == %d)\n",nMode );
            *pRetVal = SYNTAX_ERROR;
            goto Exit;
        }

        /* Fill Link Layer parameter structure */
        bRet = CreateLinkParaGem(poLinkDevOpenParam, eLinkMode, nPort);
        if(!bRet)
        {
            *pRetVal = SYNTAX_ERROR;
            goto Exit;

        }

        bIsLinkLayerEntry = EC_TRUE;
    }
#endif /* if (defined INCLUDE_GEM) */
    else if ( CreateNiosLayer(ptcWord, poLinkDevOpenParam) )
    {
        bIsLinkLayerEntry = EC_TRUE;
    }

Exit:
    return bIsLinkLayerEntry;
}

EC_T_VOID ShowLinkLayerSyntax1(EC_T_VOID)
{
    OsDbgMsg (""
#if (defined WIN32) && !(defined UNDER_CE) && !(defined RTOS_32) && !(defined UNDER_RTSS)
    " [-winpcap IpAddress Mode] [-winpcap2 IpAddress]"
#endif
#if (defined RTOS_32) || (defined QNX6) || (defined UNDER_CE) || (defined VXWORKS) || (defined LINUX) || (defined UNDER_RTSS) || (defined __INTEGRITY) || (defined __INTIME__) || (defined __TKERNEL)
    " [-i8255x Instance Mode] [-i8254x Instance Mode] [-rtl8139 Instance Mode] [-rtl8169 Instance Mode]"
#endif
#if (defined(__arm__) && defined(LINUX)) || (defined(_VX_CPU_FAMILY) && (_VX_CPU_FAMILY==_VX_ARM)) || (defined(UNDER_CE) && defined(ARM))
    " [-fslfec Instance Mode]"
#endif
#if (defined(LINUX) && defined(_ARCH_PPC)) || (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC32))
    " [-fsletsec Instance Mode]"
#endif
#if (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC440))
    " [-emac Instance Mode]"
#endif          
#if (defined UNDER_CE)
    " [-ndisuio Adapter]"
#   if (defined INCLUDE_DM9000)
    " [-dm9000]"
#   endif /* INCLUDE_DM9000 */
#endif
#if (defined VXWORKS)
    " [-snarf AdapterName]"
#   if (defined INCLUDE_ETHERLIB)
    " [-etherlib AdapterName]"
#   endif /* INCLUDE_ETHERLIB */
#endif
#if (defined VXWORKS) || (defined __RCX__)
    " [-hnx Instance]"
#endif /* VXWORKS || defined __RCX__ */
#if (defined LINUX)
    " [-SockRaw device]"
#endif /* defined LINUX */
#if (defined __INTEGRITY)
    " [-GhEth device]"
#endif
#if (defined INCLUDE_PXAMFU)
    " [-pxamfu Instance Mode]"
#endif
#if (defined INCLUDE_SMSC)
    " [-smsc Mode]"
#endif
#if (defined INCLUDE_CPSW)
    " [-cpsw Port Mode PortPriority MasterFlag]"
#endif
#if (defined INCLUDE_GEM)
    " [-gem Mode Port]"
#endif
#if (defined EC_VERSION_NIOS)
    "[-nios] "
#endif
    "\n"); /* OsDbgMsg */
}

EC_T_VOID ShowLinkLayerSyntax2(EC_T_VOID)
{
#if (defined WIN32) && !(defined UNDER_CE) && !(defined RTOS_32)  && !(defined UNDER_RTSS)
    OsDbgMsg("   -winpcap          Link layer = WinPcap/NPF\n");
    OsDbgMsg("     IpAddress       IP address of network adapter card, ex. 192.168.157.2\n");
    OsDbgMsg("                     NPF only: 255.255.255.x, x = network adapter number (1,2,...)\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1) mode\n");
#endif
#if (defined RTOS_32) || (defined QNX6) || (defined UNDER_CE) || (defined VXWORKS) || (defined LINUX) || (defined UNDER_RTSS) || (defined __INTIME__) || (defined __TKERNEL)
    OsDbgMsg("   -i8255x           Link layer = Intel 8255x\n");
    OsDbgMsg("     Instance        Device instance (1=first), ex. 1\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1) mode\n");
    OsDbgMsg("   -i8254x           Link layer = Intel 8254x\n");
    OsDbgMsg("     Instance        Device instance (1=first), ex. 1\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1) mode\n");
    OsDbgMsg("   -rtl8139          Link layer = Realtek RTL8139\n");
    OsDbgMsg("     Instance        Device instance (1=first), ex. 1\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1) mode\n");
    OsDbgMsg("   -rtl8169          Link layer = Realtek RTL8169 / RTL8168 / RTL8111\n");
    OsDbgMsg("     Instance        Device instance (1=first), ex. 1\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1) mode\n");
#endif /* RTOS_32 || defined QNX6 */
#if (defined(__arm__) && defined(LINUX)) || (defined(_VX_CPU_FAMILY) && (_VX_CPU_FAMILY==_VX_ARM)) || (defined(UNDER_CE) && defined(ARM))
    OsDbgMsg("   -fslfec           Link layer = Freescale FEC\n");
    OsDbgMsg("     Instance        Device instance (1=first), ex. 1\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1) mode\n");
#endif
#if (defined(LINUX) && defined(_ARCH_PPC)) || (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC32))
    OsDbgMsg("   -fsletsec         Link layer = Freescale TSEC / eTSEC V1 / eTSEC V2 (VeTSEC)\n");
    OsDbgMsg("     Instance        Device instance (1=first), ex. 1\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1) mode\n");
#endif
#if (defined(VXWORKS) && defined(_VX_CPU) && (_VX_CPU==_VX_PPC440))
    OsDbgMsg("   -emac             Link layer = Xilinx LogiCORE IP XPS EMAC\n");
    OsDbgMsg("     Instance        Device instance (must be 1)\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1) mode\n");
#endif          
#if (defined UNDER_CE)
    OsDbgMsg("   -ndisuio          Link layer = NdisUio\n");
    OsDbgMsg("     Adapter         Device name (registry), ex. PCI\\RTL81391\n");
#   if (defined INCLUDE_DM9000)
    OsDbgMsg("   -dm9000           Link layer = Davicom DM9000 on Keith&Koep Triceps IV\n");
#   endif /* INCLUDE_DM9000 */
#endif /* UNDER_CE */
#if (defined VXWORKS)
    OsDbgMsg("   -snarf            Link layer = SNARF link layer device statically loaded\n");
    OsDbgMsg("    AdapterName      Adapter name, ex. fei0\n");
#   if (defined INCLUDE_ETHERLIB)
    OsDbgMsg("   -etherlib         Link layer = EtherLib link layer device statically loaded\n");
    OsDbgMsg("    AdapterName      Adapter name, ex. fei0\n");
#   endif /* INCLUDE_ETHERLIB */
#endif /* VXWORKS */
#if (defined VXWORKS) || (defined __RCX__)
    OsDbgMsg("   -hnx              Link layer = Hilscher NetX XPEC Builtin PHY raw driver\n");
    OsDbgMsg("     Instance        Device instance (0=first), ex. 0\n");
#endif /* VXWORKS || defined __RCX__ */
#if (defined LINUX)
    OsDbgMsg("   -SockRaw          Link layer = raw socket\n");
    OsDbgMsg("     device          network device (e.g. eth1)\n");
#endif /* LINUX */
#if (defined __INTEGRITY)
    OsDbgMsg("   -GhEth            Link layer = GHETH\n");
    OsDbgMsg("     device          network device (e.g. eth1)\n");
#endif /* defined __INTEGRITY */
#ifdef STARTERWARE_NOOS
    OsDbgMsg("   -swcpsw           Link layer = Texas Instruments Common Platform Switch (CPSW) using Starter Ware driver\n");
    OsDbgMsg("     Port            Switch Port: 1 P1, 2 P2\n");
    OsDbgMsg("     PortPriority    Low priority (0) or high priority (1)\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1)\n");
    OsDbgMsg("     MasterFlag      (m) Master (Initialize Switch), (s) Slave\n");
#endif
#if (defined INCLUDE_CPSW)
    OsDbgMsg("   -cpsw             Link layer = Texas Instruments Common Platform Switch (CPSW)\n");
    OsDbgMsg("     Port            Switch Port: 1 P1, 2 P2\n");
    OsDbgMsg("     PortPriority    Low priority (0) or high priority (1)\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1)\n");
    OsDbgMsg("     MasterFlag      (m) Master (Initialize Switch), (s) Slave\n");
#endif /* defined INCLUDE_SMSC */
#if (defined INCLUDE_SMSC)
    OsDbgMsg("   -smsc             Link layer = SMSC LAN9218i/LAN9221\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1)\n");
#endif /* defined INCLUDE_SMSC */
#if (defined INCLUDE_GEM)
    OsDbgMsg("   -gem             Link layer = Xilinx Zynq-7000 (GEM)\n");    
    OsDbgMsg("     Port            GEM0 (0) or GEM1 (1)\n");
    OsDbgMsg("     Mode            Interrupt (0) or Polling (1)\n");
#endif /* defined INCLUDE_SMSC */
#if (defined EC_VERSION_NIOS)
    OsDbgMsg("   -nios            Link layer = NIOS II\n");
#endif /* defined INCLUDE_SMSC */
}




/*-END OF SOURCE FILE--------------------------------------------------------*/
