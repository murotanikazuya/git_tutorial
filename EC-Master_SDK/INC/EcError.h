/*-----------------------------------------------------------------------------
 * EcError.h                
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              EtherCAT Master error definitions
 *---------------------------------------------------------------------------*/


#ifndef INC_ECERROR
#define INC_ECERROR

/*-DEFINES-------------------------------------------------------------------*/



/*********************************************************************

  Do not change these offset!
  Insert new codes in the free areas

   Errorcode                     Text-ID
    Start   |      End    | Start   | End    | Size  |  Description
-----------------------------------------------------------------------
0x9811.0000 | 0x9811.013F | 0x0000  | 0x013F | 0x140 | Master Error Code
            |             |         |        |       |
0x9811.0140 | 0x9811.01BF | 0x0140  | 0x01BF | 0x040 | free area 2
            |             |         |        |       |
0x9811.0180 | 0x9811.01BF | 0x0180  | 0x01BF | 0x040 | RAS
            |             |         |        |       |
0x9812.01C0 | 0x9812.01FF | 0x01C0  | 0x01FF | 0x040 | DCM
            |             |         |        |       |
    -       |      -      | 0x0200  | 0x02FF | 0x100 | Application framework
            |             |         |        |       |
    -       |      -      | 0x0300  | 0x03FF | 0x100 | AL Status Codes
            |             |         |        |       |


*********************************************************************/


#define       EC_E_NOERROR                      ((EC_T_DWORD)0x00000000)
#define EC_SZTXT_E_NOERROR                      "No Error"

#define       EC_E_ERROR                        ((EC_T_DWORD)0x98110000)
#define EC_SZTXT_E_ERROR                        "Unspecific Error"

#define       EMRAS_E_ERROR                     ((EC_T_DWORD)0x98110180)
#define EMRAS_SZTXT_E_ERROR                     "Unspecific RAS Error"

#define       DCM_E_ERROR                       ((EC_T_DWORD)0x981201C0)
#define DCM_SZTXT_E_ERROR                       "Unspecific DCM Error"

#define       EC_TEXTBASE                       ((EC_T_WORD)0x0200)
#define EC_SZTXT_TEXTBASE                       "Unknown Text (Base)"

#define       EC_ALSTATEBASE                    ((EC_T_WORD)0x0300)
#define EC_SZTXT_ALSTATEBASE                    "AL Status No Error"


/*********************************************************************/
/* Master Error Strings                                              */
/*********************************************************************/

#define       EC_E_NOTSUPPORTED                 ((EC_T_DWORD)EC_E_ERROR+0x01)
#define EC_SZTXT_E_NOTSUPPORTED                 "ERROR: Feature not supported"

#define       EC_E_INVALIDINDEX                 ((EC_T_DWORD)EC_E_ERROR+0x02)
#define EC_SZTXT_E_INVALIDINDEX                 "ERROR: Invalid index"

#define       EC_E_INVALIDOFFSET                ((EC_T_DWORD)EC_E_ERROR+0x03)
#define EC_SZTXT_E_INVALIDOFFSET                "ERROR: Invalid offset"

#define       EC_E_CANCEL                       ((EC_T_DWORD)EC_E_ERROR+0x04)
#define EC_SZTXT_E_CANCEL                       "ERROR: Cancel"

#define       EC_E_INVALIDSIZE                  ((EC_T_DWORD)EC_E_ERROR+0x05)
#define EC_SZTXT_E_INVALIDSIZE                  "ERROR: Invalid size"

#define       EC_E_INVALIDDATA                  ((EC_T_DWORD)EC_E_ERROR+0x06)
#define EC_SZTXT_E_INVALIDDATA                  "ERROR: Invalid data"

#define       EC_E_NOTREADY                     ((EC_T_DWORD)EC_E_ERROR+0x07)
#define EC_SZTXT_E_NOTREADY                     "ERROR: Not ready"

#define       EC_E_BUSY                         ((EC_T_DWORD)EC_E_ERROR+0x08)
#define EC_SZTXT_E_BUSY                         "ERROR: Busy"

#define       EC_E_ACYC_FRM_FREEQ_EMPTY         ((EC_T_DWORD)EC_E_ERROR+0x09)
#define EC_SZTXT_E_ACYC_FRM_FREEQ_EMPTY         "ERROR: Cannot queue acyclic EtherCAT command (MasterConfig.dwMaxQueuedEthFrames)"

#define       EC_E_NOMEMORY                     ((EC_T_DWORD)EC_E_ERROR+0x0A)
#define EC_SZTXT_E_NOMEMORY                     "ERROR: No memory left"

#define       EC_E_INVALIDPARM                  ((EC_T_DWORD)EC_E_ERROR+0x0B)
#define EC_SZTXT_E_INVALIDPARM                  "ERROR: Invalid parameter"

#define       EC_E_NOTFOUND                     ((EC_T_DWORD)EC_E_ERROR+0x0C)
#define EC_SZTXT_E_NOTFOUND                     "ERROR: Not found"

#define       EC_E_DUPLICATE                    ((EC_T_DWORD)EC_E_ERROR+0x0D)
#define EC_SZTXT_E_DUPLICATE                    "ERROR: Duplicate"

#define       EC_E_INVALIDSTATE                 ((EC_T_DWORD)EC_E_ERROR+0x0E)
#define EC_SZTXT_E_INVALIDSTATE                 "ERROR: Invalid state"

#define       EC_E_TIMER_LIST_FULL              ((EC_T_DWORD)EC_E_ERROR+0x0F)
#define EC_SZTXT_E_TIMER_LIST_FULL              "ERROR: Cannot add slave to timer list"

#define       EC_E_TIMEOUT                      ((EC_T_DWORD)EC_E_ERROR+0x10)
#define EC_SZTXT_E_TIMEOUT                      "ERROR: Time-out"

#define       EC_E_OPENFAILED                   ((EC_T_DWORD)EC_E_ERROR+0x11)
#define EC_SZTXT_E_OPENFAILED                   "ERROR: Open failed"

#define       EC_E_SENDFAILED                   ((EC_T_DWORD)EC_E_ERROR+0x12)
#define EC_SZTXT_E_SENDFAILED                   "ERROR: Send failed"

#define       EC_E_INSERTMAILBOX                ((EC_T_DWORD)EC_E_ERROR+0x13)
#define EC_SZTXT_E_INSERTMAILBOX                "ERROR: Insert mailbox error"

#define       EC_E_INVALIDCMD                   ((EC_T_DWORD)EC_E_ERROR+0x14)
#define EC_SZTXT_E_INVALIDCMD                   "ERROR: Invalid mailbox command"

#define       EC_E_UNKNOWN_MBX_PROTOCOL         ((EC_T_DWORD)EC_E_ERROR+0x15)
#define EC_SZTXT_E_UNKNOWN_MBX_PROTOCOL         "ERROR: Unknown mailbox protocol command"

#define       EC_E_ACCESSDENIED                 ((EC_T_DWORD)EC_E_ERROR+0x16)
#define EC_SZTXT_E_ACCESSDENIED                 "ERROR: Access denied"

#define       EC_E_IDENTIFICATIONFAILED         ((EC_T_DWORD)EC_E_ERROR+0x17)
#define EC_SZTXT_E_IDENTIFICATIONFAILED         "ERROR: Identification failed"

#define       EC_E_PRODKEY_INVALID              ((EC_T_DWORD)EC_E_ERROR+0x1A)
#define EC_SZTXT_E_PRODKEY_INVALID              "ERROR: Invalid product key"

#define       EC_E_WRONG_FORMAT                 ((EC_T_DWORD)EC_E_ERROR+0x1B)
#define EC_SZTXT_E_WRONG_FORMAT                 "ERROR: Wrong format of master XML file"

#define       EC_E_FEATURE_DISABLED             ((EC_T_DWORD)EC_E_ERROR+0x1C)
#define EC_SZTXT_E_FEATURE_DISABLED             "ERROR: Feature disabled"

#define       EC_E_SHADOW_MEMORY                ((EC_T_DWORD)EC_E_ERROR+0x1D)
#define EC_SZTXT_E_SHADOW_MEMORY                "ERROR: Shadow memory requested in wrong mode"

#define       EC_E_BUSCONFIG_MISMATCH           ((EC_T_DWORD)EC_E_ERROR+0x1E)
#define EC_SZTXT_E_BUSCONFIG_MISMATCH           "Bus configuration mismatch"

#define       EC_E_CONFIGDATAREAD               ((EC_T_DWORD)EC_E_ERROR+0x1F)
#define EC_SZTXT_E_CONFIGDATAREAD               "ERROR: Error in reading config file"

#define       EC_E_ENI_NO_SAFEOP_OP_SUPPORT     ((EC_T_DWORD)EC_E_ERROR+0x20)
#define EC_SZTXT_E_ENI_NO_SAFEOP_OP_SUPPORT     "ERROR: Configuration doesn't support SAFEOP and OP requested state"

#define       EC_E_XML_CYCCMDS_MISSING          ((EC_T_DWORD)EC_E_ERROR+0x21)
#define EC_SZTXT_E_XML_CYCCMDS_MISSING          "ERROR: Cyclic commands are missing"

#define       EC_E_XML_ALSTATUS_READ_MISSING    ((EC_T_DWORD)EC_E_ERROR+0x22)
#define EC_SZTXT_E_XML_ALSTATUS_READ_MISSING    "ERROR: AL_STATUS register read missing in XML file for at least one state"

#define       EC_E_MCSM_FATAL_ERROR             ((EC_T_DWORD)EC_E_ERROR+0x23)
#define EC_SZTXT_E_MCSM_FATAL_ERROR             "ERROR: Fatal internal McSm"

#define       EC_E_SLAVE_ERROR                  ((EC_T_DWORD)EC_E_ERROR+0x24)
#define EC_SZTXT_E_SLAVE_ERROR                  "ERROR: Slave error"

#define       EC_E_FRAME_LOST                   ((EC_T_DWORD)EC_E_ERROR+0x25)
#define EC_SZTXT_E_FRAME_LOST                   "ERROR: Frame lost, IDX mismatch"

#define       EC_E_CMD_MISSING                  ((EC_T_DWORD)EC_E_ERROR+0x26)
#define EC_SZTXT_E_CMD_MISSING                  "ERROR: At least one EtherCAT command is missing in the received frame"

#define       EC_E_INVALID_DCL_MODE             ((EC_T_DWORD)EC_E_ERROR+0x28)
#define EC_SZTXT_E_INVALID_DCL_MODE             "ERROR: IOCTL EC_IOCTL_DC_LATCH_REQ_LTIMVALS not possible in DC Latching auto read mode"

#define       EC_E_AI_ADDRESS                   ((EC_T_DWORD)EC_E_ERROR+0x29)
#define EC_SZTXT_E_AI_ADDRESS                   "ERROR: Auto increment address - increment mismatch (slave missing)"

#define       EC_E_INVALID_SLAVE_STATE          ((EC_T_DWORD)EC_E_ERROR+0x2A)
#define EC_SZTXT_E_INVALID_SLAVE_STATE          "ERROR: Slave in invalid state, e.g. not in OP (API not callable in this state)"

#define       EC_E_SLAVE_NOT_ADDRESSABLE        ((EC_T_DWORD)EC_E_ERROR+0x2B)
#define EC_SZTXT_E_SLAVE_NOT_ADDRESSABLE        "ERROR: Station address lost or slave missing - FPRD to AL_STATUS failed"

#define       EC_E_CYC_CMDS_OVERFLOW            ((EC_T_DWORD)EC_E_ERROR+0x2C)
#define EC_SZTXT_E_CYC_CMDS_OVERFLOW            "ERROR: Too many cyclic commands in XML configuration file. (Check EC_T_MASTER_CONFIG.dwMaxQueuedEthFrames)"

#define       EC_E_LINK_DISCONNECTED            ((EC_T_DWORD)EC_E_ERROR+0x2D)
#define EC_SZTXT_E_LINK_DISCONNECTED            "ERROR: Ethernet link cable disconnected"

#define       EC_E_MASTERCORE_INACCESSIBLE      ((EC_T_DWORD)EC_E_ERROR+0x2E)
#define EC_SZTXT_E_MASTERCORE_INACCESSIBLE      "ERROR: Master core not accessible"

#define       EC_E_COE_MBXSND_WKC_ERROR         ((EC_T_DWORD)EC_E_ERROR+0x2F)
#define EC_SZTXT_E_COE_MBXSND_WKC_ERROR         "ERROR CoE: Mailbox send: working counter"

/* deprecated */
#define       EC_E_COE_MBXRCV_WKC_ERROR         ((EC_T_DWORD)EC_E_ERROR+0x30)
#define EC_SZTXT_E_COE_MBXRCV_WKC_ERROR         "ERROR CoE: Mailbox receive: working counter"

#define       EC_E_NO_MBX_SUPPORT               ((EC_T_DWORD)EC_E_ERROR+0x31)
#define EC_SZTXT_E_NO_MBX_SUPPORT               "ERROR: No mailbox support"

#define       EC_E_NO_COE_SUPPORT               ((EC_T_DWORD)EC_E_ERROR+0x32)
#define EC_SZTXT_E_NO_COE_SUPPORT               "ERROR CoE: Protocol not supported"

#define       EC_E_NO_EOE_SUPPORT               ((EC_T_DWORD)EC_E_ERROR+0x33)
#define EC_SZTXT_E_NO_EOE_SUPPORT               "ERROR EoE: Protocol not supported"

#define       EC_E_NO_FOE_SUPPORT               ((EC_T_DWORD)EC_E_ERROR+0x34)
#define EC_SZTXT_E_NO_FOE_SUPPORT               "ERROR FoE: Protocol not supported"

#define       EC_E_NO_SOE_SUPPORT               ((EC_T_DWORD)EC_E_ERROR+0x35)
#define EC_SZTXT_E_NO_SOE_SUPPORT               "ERROR SoE: Protocol not supported"

#define       EC_E_NO_VOE_SUPPORT               ((EC_T_DWORD)EC_E_ERROR+0x36)
#define EC_SZTXT_E_NO_VOE_SUPPORT               "ERROR VoE: Protocol not supported"

#define       EC_E_EVAL_VIOLATION               ((EC_T_DWORD)EC_E_ERROR+0x37)
#define EC_SZTXT_E_EVAL_VIOLATION               "ERROR: Configuration violates evaluation limits"

#define       EC_E_EVAL_EXPIRED                 ((EC_T_DWORD)EC_E_ERROR+0x38)
#define EC_SZTXT_E_EVAL_EXPIRED                 "ERROR: Evaluation time limit reached"
          
#define       EC_E_LICENSE_MISSING              ((EC_T_DWORD)EC_E_ERROR+0x39)
#define EC_SZTXT_E_LICENSE_MISSING              "ERROR: License key invalid or missing"

#define       EC_E_SDO_ABORTCODE_TOGGLE         ((EC_T_DWORD)EC_E_ERROR+0x40)
#define EC_SZTXT_E_SDO_ABORTCODE_TOGGLE         "ERROR SDO: Toggle bit not alternated."

#define       EC_E_SDO_ABORTCODE_TIMEOUT        ((EC_T_DWORD)EC_E_ERROR+0x41)
#define EC_SZTXT_E_SDO_ABORTCODE_TIMEOUT        "ERROR SDO: SDO protocol time-out."

#define       EC_E_SDO_ABORTCODE_CCS_SCS        ((EC_T_DWORD)EC_E_ERROR+0x42)
#define EC_SZTXT_E_SDO_ABORTCODE_CCS_SCS        "ERROR SDO: Client/server command specifier not valid or unknown."

#define       EC_E_SDO_ABORTCODE_BLK_SIZE       ((EC_T_DWORD)EC_E_ERROR+0x43)
#define EC_SZTXT_E_SDO_ABORTCODE_BLK_SIZE       "ERROR SDO: Invalid block size (block mode only)."

#define       EC_E_SDO_ABORTCODE_SEQNO          ((EC_T_DWORD)EC_E_ERROR+0x44)
#define EC_SZTXT_E_SDO_ABORTCODE_SEQNO          "ERROR SDO: Invalid sequence number (block mode only)."

#define       EC_E_SDO_ABORTCODE_CRC            ((EC_T_DWORD)EC_E_ERROR+0x45)
#define EC_SZTXT_E_SDO_ABORTCODE_CRC            "ERROR SDO: CRC error (block mode only)."

#define       EC_E_SDO_ABORTCODE_MEMORY         ((EC_T_DWORD)EC_E_ERROR+0x46)
#define EC_SZTXT_E_SDO_ABORTCODE_MEMORY         "ERROR SDO: Out of memory."

#define       EC_E_SDO_ABORTCODE_ACCESS         ((EC_T_DWORD)EC_E_ERROR+0x47)
#define EC_SZTXT_E_SDO_ABORTCODE_ACCESS         "ERROR SDO: Unsupported access to an object."

#define       EC_E_SDO_ABORTCODE_WRITEONLY      ((EC_T_DWORD)EC_E_ERROR+0x48)
#define EC_SZTXT_E_SDO_ABORTCODE_WRITEONLY      "ERROR SDO: Attempt to read a write only object."

#define       EC_E_SDO_ABORTCODE_READONLY       ((EC_T_DWORD)EC_E_ERROR+0x49)
#define EC_SZTXT_E_SDO_ABORTCODE_READONLY       "ERROR SDO: Attempt to write a read only object."

#define       EC_E_SDO_ABORTCODE_INDEX          ((EC_T_DWORD)EC_E_ERROR+0x4A)
#define EC_SZTXT_E_SDO_ABORTCODE_INDEX          "ERROR SDO: Object does not exist in the object dictionary."

#define       EC_E_SDO_ABORTCODE_PDO_MAP        ((EC_T_DWORD)EC_E_ERROR+0x4B)
#define EC_SZTXT_E_SDO_ABORTCODE_PDO_MAP        "ERROR SDO: Object cannot be mapped to the PDO."

#define       EC_E_SDO_ABORTCODE_PDO_LEN        ((EC_T_DWORD)EC_E_ERROR+0x4C)
#define EC_SZTXT_E_SDO_ABORTCODE_PDO_LEN        "ERROR SDO: Number and length of objects to be mapped exceed PDO length"

#define       EC_E_SDO_ABORTCODE_P_INCOMP       ((EC_T_DWORD)EC_E_ERROR+0x4D)
#define EC_SZTXT_E_SDO_ABORTCODE_P_INCOMP       "ERROR SDO: General parameter incompatibility"

#define       EC_E_SDO_ABORTCODE_I_INCOMP       ((EC_T_DWORD)EC_E_ERROR+0x4E)
#define EC_SZTXT_E_SDO_ABORTCODE_I_INCOMP       "ERROR SDO: General internal incompatibility in the device."

#define       EC_E_SDO_ABORTCODE_HARDWARE       ((EC_T_DWORD)EC_E_ERROR+0x4F)
#define EC_SZTXT_E_SDO_ABORTCODE_HARDWARE       "ERROR SDO: Access failed due to an hardware error."

#define       EC_E_SDO_ABORTCODE_DATA_SIZE      ((EC_T_DWORD)EC_E_ERROR+0x50)
#define EC_SZTXT_E_SDO_ABORTCODE_DATA_SIZE      "ERROR SDO: Data type does not match, length of service parameter does not match"

#define       EC_E_SDO_ABORTCODE_DATA_SIZE1     ((EC_T_DWORD)EC_E_ERROR+0x51)
#define EC_SZTXT_E_SDO_ABORTCODE_DATA_SIZE1     "ERROR SDO: Data type does not match, service parameter too long"

#define       EC_E_SDO_ABORTCODE_DATA_SIZE2     ((EC_T_DWORD)EC_E_ERROR+0x52)
#define EC_SZTXT_E_SDO_ABORTCODE_DATA_SIZE2     "ERROR SDO: Data type does not match, service parameter too short"

#define       EC_E_SDO_ABORTCODE_OFFSET         ((EC_T_DWORD)EC_E_ERROR+0x53)
#define EC_SZTXT_E_SDO_ABORTCODE_OFFSET         "ERROR SDO: Sub-index does not exist."

#define       EC_E_SDO_ABORTCODE_DATA_RANGE     ((EC_T_DWORD)EC_E_ERROR+0x54)
#define EC_SZTXT_E_SDO_ABORTCODE_DATA_RANGE     "ERROR SDO: Write access - Parameter value out of range"

#define       EC_E_SDO_ABORTCODE_DATA_RANGE1    ((EC_T_DWORD)EC_E_ERROR+0x55)
#define EC_SZTXT_E_SDO_ABORTCODE_DATA_RANGE1    "ERROR SDO: Write access - Parameter value out of high limit"

#define       EC_E_SDO_ABORTCODE_DATA_RANGE2    ((EC_T_DWORD)EC_E_ERROR+0x56)
#define EC_SZTXT_E_SDO_ABORTCODE_DATA_RANGE2    "ERROR SDO: Write access - Parameter value out of low limit"

#define       EC_E_SDO_ABORTCODE_MINMAX         ((EC_T_DWORD)EC_E_ERROR+0x57)
#define EC_SZTXT_E_SDO_ABORTCODE_MINMAX         "ERROR SDO: Maximum value is less than minimum value."

#define       EC_E_SDO_ABORTCODE_GENERAL        ((EC_T_DWORD)EC_E_ERROR+0x58)
#define EC_SZTXT_E_SDO_ABORTCODE_GENERAL        "ERROR SDO: General error"

#define       EC_E_SDO_ABORTCODE_TRANSFER       ((EC_T_DWORD)EC_E_ERROR+0x59)
#define EC_SZTXT_E_SDO_ABORTCODE_TRANSFER       "ERROR SDO: Unable to transfer or store data to the application"

#define       EC_E_SDO_ABORTCODE_TRANSFER1      ((EC_T_DWORD)EC_E_ERROR+0x5A)
#define EC_SZTXT_E_SDO_ABORTCODE_TRANSFER1      "ERROR SDO: Unable to transfer or store data to the application because of local control"

#define       EC_E_SDO_ABORTCODE_TRANSFER2      ((EC_T_DWORD)EC_E_ERROR+0x5B)
#define EC_SZTXT_E_SDO_ABORTCODE_TRANSFER2      "ERROR SDO: Unable to transfer or store data to the application because of the present device state"

#define       EC_E_SDO_ABORTCODE_DICTIONARY     ((EC_T_DWORD)EC_E_ERROR+0x5C)
#define EC_SZTXT_E_SDO_ABORTCODE_DICTIONARY     "ERROR SDO: Dynamic generation of object dictionary failed or missing object dictionary"
        
#define       EC_E_SDO_ABORTCODE_UNKNOWN        ((EC_T_DWORD)EC_E_ERROR+0x5D)
#define EC_SZTXT_E_SDO_ABORTCODE_UNKNOWN        "ERROR SDO: Unknown code."
                                                                        
#define       EC_E_FOE_ERRCODE_NOTDEFINED       ((EC_T_DWORD)EC_E_ERROR+0x60)
#define EC_SZTXT_E_FOE_ERRCODE_NOTDEFINED       "ERROR FoE: Vendor specific FoE error"

#define       EC_E_FOE_ERRCODE_NOTFOUND         ((EC_T_DWORD)EC_E_ERROR+0x61)
#define EC_SZTXT_E_FOE_ERRCODE_NOTFOUND         "ERROR FoE: Not found"

#define       EC_E_FOE_ERRCODE_ACCESS           ((EC_T_DWORD)EC_E_ERROR+0x62)
#define EC_SZTXT_E_FOE_ERRCODE_ACCESS           "ERROR FoE: Access denied"

#define       EC_E_FOE_ERRCODE_DISKFULL         ((EC_T_DWORD)EC_E_ERROR+0x63)
#define EC_SZTXT_E_FOE_ERRCODE_DISKFULL         "ERROR FoE: Disk full"

#define       EC_E_FOE_ERRCODE_ILLEGAL         ((EC_T_DWORD)EC_E_ERROR+0x64)
#define EC_SZTXT_E_FOE_ERRCODE_ILLEGAL         "ERROR FoE: Illegal"

#define       EC_E_FOE_ERRCODE_PACKENO          ((EC_T_DWORD)EC_E_ERROR+0x65)
#define EC_SZTXT_E_FOE_ERRCODE_PACKENO          "ERROR FoE: Wrong packet number"

#define       EC_E_FOE_ERRCODE_EXISTS           ((EC_T_DWORD)EC_E_ERROR+0x66)
#define EC_SZTXT_E_FOE_ERRCODE_EXISTS           "ERROR FoE: Already exists"

#define       EC_E_FOE_ERRCODE_NOUSER           ((EC_T_DWORD)EC_E_ERROR+0x67)
#define EC_SZTXT_E_FOE_ERRCODE_NOUSER           "ERROR FoE: User missing"

#define       EC_E_FOE_ERRCODE_BOOTSTRAPONLY    ((EC_T_DWORD)EC_E_ERROR+0x68)
#define EC_SZTXT_E_FOE_ERRCODE_BOOTSTRAPONLY    "ERROR FoE: Bootstrap only"

#define       EC_E_FOE_ERRCODE_NOTINBOOTSTRAP   ((EC_T_DWORD)EC_E_ERROR+0x69)
#define EC_SZTXT_E_FOE_ERRCODE_NOTINBOOTSTRAP   "ERROR FoE: Not bootstrap"

#define       EC_E_FOE_ERRCODE_INVALIDPASSWORD  ((EC_T_DWORD)EC_E_ERROR+0x6A)
#define EC_SZTXT_E_FOE_ERRCODE_INVALIDPASSWORD  "ERROR FoE: No rights"

#define       EC_E_FOE_ERRCODE_PROGERROR        ((EC_T_DWORD)EC_E_ERROR+0x6B)
#define EC_SZTXT_E_FOE_ERRCODE_PROGERROR        "ERROR FoE: Program error"

#define       EC_E_CFGFILENOTFOUND              ((EC_T_DWORD)EC_E_ERROR+0x70)
#define EC_SZTXT_E_CFGFILENOTFOUND              "ERROR: Master configuration not found"

#define       EC_E_EEPROMREADERROR              ((EC_T_DWORD)EC_E_ERROR+0x71)
#define EC_SZTXT_E_EEPROMREADERROR              "ERROR: Command error while EEPROM upload"

#define       EC_E_EEPROMWRITEERROR             ((EC_T_DWORD)EC_E_ERROR+0x72)
#define EC_SZTXT_E_EEPROMWRITEERROR             "ERROR: Command error while EEPROM download"

#define       EC_E_XML_CYCCMDS_SIZEMISMATCH     ((EC_T_DWORD)EC_E_ERROR+0x73)
#define EC_SZTXT_E_XML_CYCCMDS_SIZEMISMATCH     "ERROR: Cyclic command wrong size (too long)"

#define       EC_E_XML_INVALID_INP_OFF          ((EC_T_DWORD)EC_E_ERROR+0x74)
#define EC_SZTXT_E_XML_INVALID_INP_OFF          "ERROR: Invalid input offset in cyc cmd, please check InputOffs"

#define       EC_E_XML_INVALID_OUT_OFF          ((EC_T_DWORD)EC_E_ERROR+0x75)
#define EC_SZTXT_E_XML_INVALID_OUT_OFF          "ERROR: Invalid output offset in cyc cmd, please check OutputOffs"

#define       EC_E_PORTCLOSE                    ((EC_T_DWORD)EC_E_ERROR+0x76)
#define EC_SZTXT_E_PORTCLOSE                    "ERROR: Port Close failed"

#define       EC_E_PORTOPEN                     ((EC_T_DWORD)EC_E_ERROR+0x77)
#define EC_SZTXT_E_PORTOPEN                     "ERROR: Port Open failed"

#define       EC_E_SOE_ERRORCODE_INVALID_ACCESS ((EC_T_DWORD)EC_E_ERROR+0x78)
#define EC_SZTXT_E_SOE_ERRORCODE_INVALID_ACCESS "ERROR SoE: Invalid access to element 0" 

#define       EC_E_SOE_ERRORCODE_NOT_EXIST      ((EC_T_DWORD)EC_E_ERROR+0x79)
#define EC_SZTXT_E_SOE_ERRORCODE_NOT_EXIST      "ERROR SoE: Does not exist"
 
#define       EC_E_SOE_ERRORCODE_INVL_ACC_ELEM1 ((EC_T_DWORD)EC_E_ERROR+0x7a)
#define EC_SZTXT_E_SOE_ERRORCODE_INVL_ACC_ELEM1 "ERROR SoE: Invalid access to element 1" 

#define       EC_E_SOE_ERRORCODE_NAME_NOT_EXIST ((EC_T_DWORD)EC_E_ERROR+0x7b)
#define EC_SZTXT_E_SOE_ERRORCODE_NAME_NOT_EXIST "ERROR SoE: Name does not exist" 

#define       EC_E_SOE_ERRORCODE_NAME_UNDERSIZE ((EC_T_DWORD)EC_E_ERROR+0x7c)
#define EC_SZTXT_E_SOE_ERRORCODE_NAME_UNDERSIZE "ERROR SoE: Name undersize in transmission" 

#define       EC_E_SOE_ERRORCODE_NAME_OVERSIZE  ((EC_T_DWORD)EC_E_ERROR+0x7d)
#define EC_SZTXT_E_SOE_ERRORCODE_NAME_OVERSIZE  "ERROR SoE: Name oversize in transmission" 

#define       EC_E_SOE_ERRORCODE_NAME_UNCHANGE  ((EC_T_DWORD)EC_E_ERROR+0x7e)
#define EC_SZTXT_E_SOE_ERRORCODE_NAME_UNCHANGE  "ERROR SoE: Name unchangeable" 

#define       EC_E_SOE_ERRORCODE_NAME_WR_PROT   ((EC_T_DWORD)EC_E_ERROR+0x7f)
#define EC_SZTXT_E_SOE_ERRORCODE_NAME_WR_PROT   "ERROR SoE: Name currently write-protected" 

#define       EC_E_SOE_ERRORCODE_UNDERS_TRANS   ((EC_T_DWORD)EC_E_ERROR+0x80)
#define EC_SZTXT_E_SOE_ERRORCODE_UNDERS_TRANS   "ERROR SoE: Attribute undersize in transmission"
              
#define       EC_E_SOE_ERRORCODE_OVERS_TRANS    ((EC_T_DWORD)EC_E_ERROR+0x81)
#define EC_SZTXT_E_SOE_ERRORCODE_OVERS_TRANS    "ERROR SoE: Attribute oversize in transmission" 

#define       EC_E_SOE_ERRORCODE_ATTR_UNCHANGE  ((EC_T_DWORD)EC_E_ERROR+0x82)
#define EC_SZTXT_E_SOE_ERRORCODE_ATTR_UNCHANGE  "ERROR SoE: Attribute unchangeable" 

#define       EC_E_SOE_ERRORCODE_ATTR_WR_PROT   ((EC_T_DWORD)EC_E_ERROR+0x83)
#define EC_SZTXT_E_SOE_ERRORCODE_ATTR_WR_PROT   "ERROR SoE: Attribute currently write-protected" 

#define       EC_E_SOE_ERRORCODE_UNIT_NOT_EXIST ((EC_T_DWORD)EC_E_ERROR+0x84)
#define EC_SZTXT_E_SOE_ERRORCODE_UNIT_NOT_EXIST "ERROR SoE: Unit does not exist" 

#define       EC_E_SOE_ERRORCODE_UNIT_UNDERSIZE ((EC_T_DWORD)EC_E_ERROR+0x85)
#define EC_SZTXT_E_SOE_ERRORCODE_UNIT_UNDERSIZE "ERROR SoE: Unit undersize in transmission" 

#define       EC_E_SOE_ERRORCODE_UNIT_OVERSIZE  ((EC_T_DWORD)EC_E_ERROR+0x86)
#define EC_SZTXT_E_SOE_ERRORCODE_UNIT_OVERSIZE  "ERROR SoE: Unit oversize in transmission" 

#define       EC_E_SOE_ERRORCODE_UNIT_UNCHANGE  ((EC_T_DWORD)EC_E_ERROR+0x87)
#define EC_SZTXT_E_SOE_ERRORCODE_UNIT_UNCHANGE  "ERROR SoE: Unit unchangeable" 

#define       EC_E_SOE_ERRORCODE_UNIT_WR_PROT   ((EC_T_DWORD)EC_E_ERROR+0x88)
#define EC_SZTXT_E_SOE_ERRORCODE_UNIT_WR_PROT   "ERROR SoE: Unit currently write-protected" 

#define       EC_E_SOE_ERRORCODE_MIN_NOT_EXIST  ((EC_T_DWORD)EC_E_ERROR+0x89)
#define EC_SZTXT_E_SOE_ERRORCODE_MIN_NOT_EXIST  "ERROR SoE: Minimum input value does not exist" 

#define       EC_E_SOE_ERRORCODE_MIN_UNDERSIZE  ((EC_T_DWORD)EC_E_ERROR+0x8a)
#define EC_SZTXT_E_SOE_ERRORCODE_MIN_UNDERSIZE  "ERROR SoE: Minimum input value undersize in transmission" 

#define       EC_E_SOE_ERRORCODE_MIN_OVERSIZE   ((EC_T_DWORD)EC_E_ERROR+0x8b)
#define EC_SZTXT_E_SOE_ERRORCODE_MIN_OVERSIZE   "ERROR SoE: Minimum input value oversize in transmission" 

#define       EC_E_SOE_ERRORCODE_MIN_UNCHANGE   ((EC_T_DWORD)EC_E_ERROR+0x8c)
#define EC_SZTXT_E_SOE_ERRORCODE_MIN_UNCHANGE   "ERROR SoE: Minimum input value unchangeable" 

#define       EC_E_SOE_ERRORCODE_MIN_WR_PROT    ((EC_T_DWORD)EC_E_ERROR+0x8d)
#define EC_SZTXT_E_SOE_ERRORCODE_MIN_WR_PROT    "ERROR SoE: Minimum input value currently write-protected" 

#define       EC_E_SOE_ERRORCODE_MAX_NOT_EXIST  ((EC_T_DWORD)EC_E_ERROR+0x8e)
#define EC_SZTXT_E_SOE_ERRORCODE_MAX_NOT_EXIST  "ERROR SoE: Maximum input value does not exist" 

#define       EC_E_SOE_ERRORCODE_MAX_UNDERSIZE  ((EC_T_DWORD)EC_E_ERROR+0x8f)
#define EC_SZTXT_E_SOE_ERRORCODE_MAX_UNDERSIZE  "ERROR SoE: Maximum input value undersize in transmission" 

#define       EC_E_SOE_ERRORCODE_MAX_OVERSIZE   ((EC_T_DWORD)EC_E_ERROR+0x90)
#define EC_SZTXT_E_SOE_ERRORCODE_MAX_OVERSIZE   "ERROR SoE: Maximum input value oversize in transmission" 

#define       EC_E_SOE_ERRORCODE_MAX_UNCHANGE   ((EC_T_DWORD)EC_E_ERROR+0x91)
#define EC_SZTXT_E_SOE_ERRORCODE_MAX_UNCHANGE   "ERROR SoE: Maximum input value unchangeable" 

#define       EC_E_SOE_ERRORCODE_MAX_WR_PROT    ((EC_T_DWORD)EC_E_ERROR+0x92)
#define EC_SZTXT_E_SOE_ERRORCODE_MAX_WR_PROT    "ERROR SoE: Maximum input value currently write-protected" 

#define       EC_E_SOE_ERRORCODE_DATA_NOT_EXIST ((EC_T_DWORD)EC_E_ERROR+0x93)
#define EC_SZTXT_E_SOE_ERRORCODE_DATA_NOT_EXIST "ERROR SoE: Data item does not exist" 

#define       EC_E_SOE_ERRORCODE_DATA_UNDERSIZE ((EC_T_DWORD)EC_E_ERROR+0x94)
#define EC_SZTXT_E_SOE_ERRORCODE_DATA_UNDERSIZE "ERROR SoE: Data item undersize in transmission" 

#define       EC_E_SOE_ERRORCODE_DATA_OVERSIZE  ((EC_T_DWORD)EC_E_ERROR+0x95)
#define EC_SZTXT_E_SOE_ERRORCODE_DATA_OVERSIZE  "ERROR SoE: Data item oversize in transmission" 

#define       EC_E_SOE_ERRORCODE_DATA_UNCHANGE  ((EC_T_DWORD)EC_E_ERROR+0x96)
#define EC_SZTXT_E_SOE_ERRORCODE_DATA_UNCHANGE  "ERROR SoE: Data item unchangeable" 

#define       EC_E_SOE_ERRORCODE_DATA_WR_PROT   ((EC_T_DWORD)EC_E_ERROR+0x97)
#define EC_SZTXT_E_SOE_ERRORCODE_DATA_WR_PROT   "ERROR SoE: Data item currently write-protected" 

#define       EC_E_SOE_ERRORCODE_DATA_MIN_LIMIT ((EC_T_DWORD)EC_E_ERROR+0x98)
#define EC_SZTXT_E_SOE_ERRORCODE_DATA_MIN_LIMIT "ERROR SoE: Data item less than minimum input value limit"

#define       EC_E_SOE_ERRORCODE_DATA_MAX_LIMIT ((EC_T_DWORD)EC_E_ERROR+0x99)
#define EC_SZTXT_E_SOE_ERRORCODE_DATA_MAX_LIMIT "ERROR SoE: Data item exceeds maximum input value limit"

#define       EC_E_SOE_ERRORCODE_DATA_INCOR     ((EC_T_DWORD)EC_E_ERROR+0x9a)
#define EC_SZTXT_E_SOE_ERRORCODE_DATA_INCOR     "ERROR SoE: Data item is incorrect" 

#define       EC_E_SOE_ERRORCODE_PASWD_PROT     ((EC_T_DWORD)EC_E_ERROR+0x9b)
#define EC_SZTXT_E_SOE_ERRORCODE_PASWD_PROT     "ERROR SoE: Data item is protected by password" 

#define       EC_E_SOE_ERRORCODE_TEMP_UNCHANGE  ((EC_T_DWORD)EC_E_ERROR+0x9c)
#define EC_SZTXT_E_SOE_ERRORCODE_TEMP_UNCHANGE  "ERROR SoE: Data item temporary unchangeable (in AT or MDT)" 

#define       EC_E_SOE_ERRORCODE_INVL_INDIRECT  ((EC_T_DWORD)EC_E_ERROR+0x9d)
#define EC_SZTXT_E_SOE_ERRORCODE_INVL_INDIRECT  "ERROR SoE: Invalid indirect" 

#define       EC_E_SOE_ERRORCODE_TEMP_UNCHANGE1 ((EC_T_DWORD)EC_E_ERROR+0x9e)
#define EC_SZTXT_E_SOE_ERRORCODE_TEMP_UNCHANGE1 "ERROR SoE: Data item temporary unchangeable (parameter or opmode...)" 

#define       EC_E_SOE_ERRORCODE_ALREADY_ACTIVE ((EC_T_DWORD)EC_E_ERROR+0x9f)
#define EC_SZTXT_E_SOE_ERRORCODE_ALREADY_ACTIVE "ERROR SoE: Command already active" 

#define       EC_E_SOE_ERRORCODE_NOT_INTERRUPT  ((EC_T_DWORD)EC_E_ERROR+0x100)
#define EC_SZTXT_E_SOE_ERRORCODE_NOT_INTERRUPT  "ERROR SoE: Command not interruptable" 

#define       EC_E_SOE_ERRORCODE_CMD_NOT_AVAIL  ((EC_T_DWORD)EC_E_ERROR+0x101)
#define EC_SZTXT_E_SOE_ERRORCODE_CMD_NOT_AVAIL  "ERROR SoE: Command not available (in this phase)" 

#define       EC_E_SOE_ERRORCODE_CMD_NOT_AVAIL1 ((EC_T_DWORD)EC_E_ERROR+0x102)
#define EC_SZTXT_E_SOE_ERRORCODE_CMD_NOT_AVAIL1 "ERROR SoE: Command not available (invalid parameter...)" 

#define       EC_E_SOE_ERRORCODE_DRIVE_NO       ((EC_T_DWORD)EC_E_ERROR+0x103)
#define EC_SZTXT_E_SOE_ERRORCODE_DRIVE_NO       "ERROR SoE: Response drive number not identical with the requested drive number"

#define       EC_E_SOE_ERRORCODE_IDN            ((EC_T_DWORD)EC_E_ERROR+0x104)
#define EC_SZTXT_E_SOE_ERRORCODE_IDN            "ERROR SoE: Response IDN not identical with the requested IDN"

#define       EC_E_SOE_ERRORCODE_FRAGMENT_LOST  ((EC_T_DWORD)EC_E_ERROR+0x105)
#define EC_SZTEC_E_SOE_ERRORCODE_FRAGMENT_LOST  "ERROR SoE: At least one fragment lost"

#define       EC_E_SOE_ERRORCODE_BUFFER_FULL    ((EC_T_DWORD)EC_E_ERROR+0x106)
#define EC_SZTEC_E_SOE_ERRORCODE_BUFFER_FULL    "ERROR SoE: RX buffer is full (ecat call with to small data-buffer)"

#define       EC_E_SOE_ERRORCODE_NO_DATA        ((EC_T_DWORD)EC_E_ERROR+0x107)    
#define EC_SZTXT_E_SOE_ERRORCODE_NO_DATA        "ERROR SoE: No data state."

#define       EC_E_SOE_ERRORCODE_NO_DEFAULT_VALUE  ((EC_T_DWORD)EC_E_ERROR+0x108)
#define EC_SZTXT_E_SOE_ERRORCODE_NO_DEFAULT_VALUE  "ERROR SoE: No default value."

#define       EC_E_SOE_ERRORCODE_DEFAULT_LONG   ((EC_T_DWORD)EC_E_ERROR+0x109)    
#define EC_SZTXT_E_SOE_ERRORCODE_DEFAULT_LONG   "ERROR SoE: Default value transmission too long."

#define       EC_E_SOE_ERRORCODE_DEFAULT_WP     ((EC_T_DWORD)EC_E_ERROR+0x10a) 
#define EC_SZTXT_E_SOE_ERRORCODE_DEFAULT_WP     "ERROR SoE: Default value cannot be changed, read only." 

#define       EC_E_SOE_ERRORCODE_INVL_DRIVE_NO  ((EC_T_DWORD)EC_E_ERROR+0x10b)
#define EC_SZTXT_E_SOE_ERRORCODE_INVL_DRIVE_NO  "ERROR SoE: Invalid drive number." 

#define       EC_E_SOE_ERRORCODE_GENERAL_ERROR  ((EC_T_DWORD)EC_E_ERROR+0x10c)
#define EC_SZTXT_E_SOE_ERRORCODE_GENERAL_ERROR  "ERROR SoE: General error"

#define       EC_E_SOE_ERRCODE_NO_ELEM_ADR      ((EC_T_DWORD)EC_E_ERROR+0x10d)
#define EC_SZTXT_E_SOE_ERRCODE_NO_ELEM_ADR      "ERROR SoE: No element addressed." 

#define       EC_E_SLAVE_NOT_PRESENT            ((EC_T_DWORD)EC_E_ERROR+0x10e)
#define EC_SZTXT_E_SLAVE_NOT_PRESENT            "Command not executed. Slave is not present on Bus" 

#define       EC_E_NO_FOE_SUPPORT_BS            ((EC_T_DWORD)EC_E_ERROR+0x10f)
#define EC_SZTXT_E_NO_FOE_SUPPORT_BS            "ERROR FoE: Protocol not supported in boot strap"

#define       EC_E_EEPROMRELOADERROR            ((EC_T_DWORD)EC_E_ERROR+0x110)
#define EC_SZTXT_E_EEPROMRELOADERROR            "ERROR: command error while EEPROM reload"

#define       EC_E_SLAVECTRLRESETERROR          ((EC_T_DWORD)EC_E_ERROR+0x111)
#define EC_SZTXT_E_SLAVECTRLRESETERROR          "ERROR: command error while Reset Slave Controller"

#define       EC_E_SYSDRIVERMISSING             ((EC_T_DWORD)EC_E_ERROR+0x112)
#define EC_SZTXT_E_SYSDRIVERMISSING             "ERROR: Cannot open system driver ect.sys"

#define       EC_E_BUSCONFIG_TOPOCHANGE         ((EC_T_DWORD)EC_E_ERROR+0x11E)
#define EC_SZTXT_E_BUSCONFIG_TOPOCHANGE         "Bus configuration not detected, Topology changed"

#define       EC_E_EOE_MBX_WKC_ERROR            ((EC_T_DWORD)EC_E_ERROR+0x11F)
#define EC_SZTXT_E_EOE_MBX_WKC_ERROR            "ERROR EoE: Mailbox receive: working counter"
                                                
#define       EC_E_FOE_MBX_WKC_ERROR            ((EC_T_DWORD)EC_E_ERROR+0x120)
#define EC_SZTXT_E_FOE_MBX_WKC_ERROR            "ERROR FoE: Mailbox receive: working counter"
                                                
#define       EC_E_SOE_MBX_WKC_ERROR            ((EC_T_DWORD)EC_E_ERROR+0x121)
#define EC_SZTXT_E_SOE_MBX_WKC_ERROR            "ERROR SoE: mailbox receive: working counter"
                                                
#define       EC_E_AOE_MBX_WKC_ERROR            ((EC_T_DWORD)EC_E_ERROR+0x122)
#define EC_SZTXT_E_AOE_MBX_WKC_ERROR            "ERROR AoE: Mailbox receive: working counter"

#define       EC_E_VOE_MBX_WKC_ERROR            ((EC_T_DWORD)EC_E_ERROR+0x123)
#define EC_SZTXT_E_VOE_MBX_WKC_ERROR            "ERROR VoE: Mailbox receive: working counter"

#define       EC_E_EEPROMASSIGNERROR            ((EC_T_DWORD)EC_E_ERROR+0x124)
#define EC_SZTXT_E_EEPROMASSIGNERROR            "ERROR: EEPROM assignment failed"

#define       EC_E_MBX_ERROR_TYPE               ((EC_T_DWORD)EC_E_ERROR+0x125)
#define EC_SZTXT_E_MBX_ERROR_TYPE               "ERROR: Error mailbox received"

#define       EC_E_REDLINEBREAK                 ((EC_T_DWORD)EC_E_ERROR+0x126)
#define EC_SZTXT_E_REDLINEBREAK                 "ERROR: Redundancy line break"

#define       EC_E_XML_INVALID_CMD_WITH_RED     ((EC_T_DWORD)EC_E_ERROR+0x127)
#define EC_SZTXT_E_XML_INVALID_CMD_WITH_RED     "ERROR: Invalid EtherCAT cmd in cyclic frame with redundancy"

#define       EC_E_XML_PREV_PORT_MISSING        ((EC_T_DWORD)EC_E_ERROR+0x128)
#define EC_SZTXT_E_XML_PREV_PORT_MISSING        "ERROR: <PreviousPort>-tag is missing!"

#define       EC_E_XML_DC_CYCCMDS_MISSING       ((EC_T_DWORD)EC_E_ERROR+0x129)
#define EC_SZTXT_E_XML_DC_CYCCMDS_MISSING       "ERROR: DC is enabled and DC cyclic commands are missing (e.g. access to 0x900)!"

#define       EC_E_DLSTATUS_IRQ_TOPOCHANGED     ((EC_T_DWORD)EC_E_ERROR+0x130)
#define EC_SZTXT_E_DLSTATUS_IRQ_TOPOCHANGED     "ERROR: DL Status Interrupt because of changed Topology"

#define       EC_E_PTS_IS_NOT_RUNNING           ((EC_T_DWORD)EC_E_ERROR+0x131)
#define EC_SZTXT_E_PTS_IS_NOT_RUNNING           "ERROR: The Pass Through Server is not running!"

#define       EC_E_ADS_IS_RUNNING               ((EC_T_DWORD)EC_E_ERROR+0x132)
#define       EC_E_PTS_IS_RUNNING               EC_E_ADS_IS_RUNNING
#define EC_SZTXT_E_PTS_IS_RUNNING               "ERROR: The ADS adapter (Pass Through Server) is running!"

#define       EC_E_PTS_THREAD_CREATE_FAILED     ((EC_T_DWORD)EC_E_ERROR+0x133)
#define EC_SZTXT_E_PTS_THREAD_CREATE_FAILED     "ERROR: Could not start the Pass Through Server!"

#define       EC_E_PTS_SOCK_BIND_FAILED         ((EC_T_DWORD)EC_E_ERROR+0x134)
#define EC_SZTXT_E_PTS_SOCK_BIND_FAILED         "ERROR: The Pass Through Server could not bind the IP address with a socket!"

#define          EC_E_PTS_NOT_ENABLED           ((EC_T_DWORD)EC_E_ERROR+0x135)
#define EC_SZTXT_EC_E_PTS_NOT_ENABLED           "The Pass Through Server is running but not enabled"

#define          EC_E_PTS_LL_MODE_NOT_SUPPORTED ((EC_T_DWORD)EC_E_ERROR+0x136)
#define EC_SZTXT_EC_E_PTS_LL_MODE_NOT_SUPPORTED "ERROR: This LinkLayer mode is not supported by the Pass Through Server!"

#define          EC_E_VOE_NO_MBX_RECEIVED       ((EC_T_DWORD)EC_E_ERROR+0x137)
#define EC_SZTXT_EC_E_VOE_NO_MBX_RECEIVED       "ERROR VoE: No VoE mailbox received!"

#define          EC_E_DC_REF_CLOCK_SYNC_OUT_UNIT_DISABLED ((EC_T_DWORD)EC_E_ERROR+0x138)
#define EC_SZTXT_EC_E_DC_REF_CLOCK_SYNC_OUT_UNIT_DISABLED "ERROR: SYNC out unit of reference clock is disabled!"

#define          EC_E_DC_REF_CLOCK_NOT_FOUND    ((EC_T_DWORD)EC_E_ERROR+0x139)
#define EC_SZTXT_EC_E_DC_REF_CLOCK_NOT_FOUND    "ERROR: Reference clock not found!"

#define       EC_E_MBX_CMD_WKC_ERROR            ((EC_T_DWORD)EC_E_ERROR+0x13B)
#define EC_SZTXT_E_MBX_CMD_WKC_ERROR            "ERROR: Mailbox command working counter error!"

#define          EC_E_NO_AOE_SUPPORT            ((EC_T_DWORD)EC_E_ERROR+0x13C)
#define EC_SZTXT_E_NO_AOE_SUPPORT               "ERROR AoE: Protocol not supported"

#define          EC_E_AOE_INV_RESPONSE_SIZE     ((EC_T_DWORD)EC_E_ERROR+0x13D)
#define EC_SZTXT_EC_E_AOE_INV_RESPONSE_SIZE     "ERROR AoE: Invalid AoE response received!"

#define       EC_E_AOE_ERROR                    ((EC_T_DWORD)EC_E_ERROR+0x13E)
#define EC_SZTXT_E_AOE_ERROR                    "ERROR AoE: Common AoE device error"

#define       EC_E_AOE_SRVNOTSUPP               ((EC_T_DWORD)EC_E_ERROR+0x13F)
#define EC_SZTXT_E_AOE_SRVNOTSUPP               "ERROR AoE: Service is not supported by server"

#define       EC_E_AOE_INVALIDGRP               ((EC_T_DWORD)EC_E_ERROR+0x140)
#define EC_SZTXT_E_AOE_INVALIDGRP               "ERROR AoE: Invalid index group"

#define       EC_E_AOE_INVALIDOFFSET            ((EC_T_DWORD)EC_E_ERROR+0x141)
#define EC_SZTXT_E_AOE_INVALIDOFFSET            "ERROR AoE: Invalid index offset"

#define       EC_E_AOE_INVALIDACCESS            ((EC_T_DWORD)EC_E_ERROR+0x142)
#define EC_SZTXT_E_AOE_INVALIDACCESS            "ERROR AoE: Reading/writing not permitted"

#define       EC_E_AOE_INVALIDSIZE              ((EC_T_DWORD)EC_E_ERROR+0x143)
#define EC_SZTXT_E_AOE_INVALIDSIZE              "ERROR AoE: Parameter size not correct"

#define       EC_E_AOE_INVALIDDATA              ((EC_T_DWORD)EC_E_ERROR+0x144)
#define EC_SZTXT_E_AOE_INVALIDDATA              "ERROR AoE: Invalid parameter value(s)"

#define       EC_E_AOE_NOTREADY                 ((EC_T_DWORD)EC_E_ERROR+0x145)
#define EC_SZTXT_E_AOE_NOTREADY                 "ERROR AoE: Device is not in a ready state"

#define       EC_E_AOE_BUSY                     ((EC_T_DWORD)EC_E_ERROR+0x146)
#define EC_SZTXT_E_AOE_BUSY                     "ERROR AoE: Device is busy"

#define       EC_E_AOE_INVALIDCONTEXT           ((EC_T_DWORD)EC_E_ERROR+0x147)
#define EC_SZTXT_E_AOE_INVALIDCONTEXT           "ERROR AoE: Invalid context"

#define       EC_E_AOE_NOMEMORY                 ((EC_T_DWORD)EC_E_ERROR+0x148)
#define EC_SZTXT_E_AOE_NOMEMORY                 "ERROR AoE: Out of memory"

#define       EC_E_AOE_INVALIDPARM              ((EC_T_DWORD)EC_E_ERROR+0x149)
#define EC_SZTXT_E_AOE_INVALIDPARM              "ERROR AoE: Invalid parameter value(s)"

#define       EC_E_AOE_NOTFOUND                 ((EC_T_DWORD)EC_E_ERROR+0x14A)
#define EC_SZTXT_E_AOE_NOTFOUND                 "ERROR AoE: Not found"

#define       EC_E_AOE_SYNTAX                   ((EC_T_DWORD)EC_E_ERROR+0x14B)
#define EC_SZTXT_E_AOE_SYNTAX                   "ERROR AoE: Syntax error in comand or file"

#define       EC_E_AOE_INCOMPATIBLE             ((EC_T_DWORD)EC_E_ERROR+0x14C)
#define EC_SZTXT_E_AOE_INCOMPATIBLE             "ERROR AoE: Objects do not match"

#define       EC_E_AOE_EXISTS                   ((EC_T_DWORD)EC_E_ERROR+0x14D)
#define EC_SZTXT_E_AOE_EXISTS                   "ERROR AoE: Object already exists"

#define       EC_E_AOE_SYMBOLNOTFOUND           ((EC_T_DWORD)EC_E_ERROR+0x14E)
#define EC_SZTXT_E_AOE_SYMBOLNOTFOUND           "ERROR AoE: Symbol not found"

#define       EC_E_AOE_SYMBOLVERSIONINVALID     ((EC_T_DWORD)EC_E_ERROR+0x14F)
#define EC_SZTXT_E_AOE_SYMBOLVERSIONINVALID     "ERROR AoE: Symbol version invalid"

#define       EC_E_AOE_INVALIDSTATE             ((EC_T_DWORD)EC_E_ERROR+0x150)
#define EC_SZTXT_E_AOE_INVALIDSTATE             "ERROR AoE: Server is in invalid state"

#define       EC_E_AOE_TRANSMODENOTSUPP         ((EC_T_DWORD)EC_E_ERROR+0x151)
#define EC_SZTXT_E_AOE_TRANSMODENOTSUPP         "ERROR AoE: AdsTransMode not supported"

#define       EC_E_AOE_NOTIFYHNDINVALID         ((EC_T_DWORD)EC_E_ERROR+0x152)
#define EC_SZTXT_E_AOE_NOTIFYHNDINVALID         "ERROR AoE: Notification handle is invalid"

#define       EC_E_AOE_CLIENTUNKNOWN            ((EC_T_DWORD)EC_E_ERROR+0x153)
#define EC_SZTXT_E_AOE_CLIENTUNKNOWN            "ERROR AoE: Notification client not registered"

#define       EC_E_AOE_NOMOREHDLS               ((EC_T_DWORD)EC_E_ERROR+0x154)
#define EC_SZTXT_E_AOE_NOMOREHDLS               "ERROR AoE: No more notification handles"

#define       EC_E_AOE_INVALIDWATCHSIZE         ((EC_T_DWORD)EC_E_ERROR+0x155)
#define EC_SZTXT_E_AOE_INVALIDWATCHSIZE         "ERROR AoE: Size for watch to big"

#define       EC_E_AOE_NOTINIT                  ((EC_T_DWORD)EC_E_ERROR+0x156)
#define EC_SZTXT_E_AOE_NOTINIT                  "ERROR AoE: Device not initialized"

#define       EC_E_AOE_TIMEOUT                  ((EC_T_DWORD)EC_E_ERROR+0x157)
#define EC_SZTXT_E_AOE_TIMEOUT                  "ERROR AoE: Device has a timeout"

#define       EC_E_AOE_NOINTERFACE              ((EC_T_DWORD)EC_E_ERROR+0x158)
#define EC_SZTXT_E_AOE_NOINTERFACE              "ERROR AoE: Query interface failed"

#define       EC_E_AOE_INVALIDINTERFACE         ((EC_T_DWORD)EC_E_ERROR+0x159)
#define EC_SZTXT_E_AOE_INVALIDINTERFACE         "ERROR AoE: Wrong interface required"

#define       EC_E_AOE_INVALIDCLSID             ((EC_T_DWORD)EC_E_ERROR+0x15A)
#define EC_SZTXT_E_AOE_INVALIDCLSID             "ERROR AoE: Class ID is invalid"

#define       EC_E_AOE_INVALIDOBJID             ((EC_T_DWORD)EC_E_ERROR+0x15B)
#define EC_SZTXT_E_AOE_INVALIDOBJID             "ERROR AoE: Object ID is invalid"

#define       EC_E_AOE_PENDING                  ((EC_T_DWORD)EC_E_ERROR+0x15C)
#define EC_SZTXT_E_AOE_PENDING                  "ERROR AoE: Request is pending"

#define       EC_E_AOE_ABORTED                  ((EC_T_DWORD)EC_E_ERROR+0x15D)
#define EC_SZTXT_E_AOE_ABORTED                  "ERROR AoE: Request is aborted"

#define       EC_E_AOE_WARNING                  ((EC_T_DWORD)EC_E_ERROR+0x15E)
#define EC_SZTXT_E_AOE_WARNING                  "ERROR AoE: Signal warning"

#define       EC_E_AOE_INVALIDARRAYIDX          ((EC_T_DWORD)EC_E_ERROR+0x15F)
#define EC_SZTXT_E_AOE_INVALIDARRAYIDX          "ERROR AoE: Invalid array index"

#define       EC_E_AOE_SYMBOLNOTACTIVE          ((EC_T_DWORD)EC_E_ERROR+0x160)
#define EC_SZTXT_E_AOE_SYMBOLNOTACTIVE          "ERROR AoE: Symbol not active -> release handle and try again"

#define       EC_E_AOE_ACCESSDENIED             ((EC_T_DWORD)EC_E_ERROR+0x161)
#define EC_SZTXT_E_AOE_ACCESSDENIED             "ERROR AoE: Access denied"

#define       EC_E_AOE_INTERNAL                 ((EC_T_DWORD)EC_E_ERROR+0x162)    
#define EC_SZTXT_E_AOE_INTERNAL                 "ERROR AoE: Internal error"

#define       EC_E_AOE_TARGET_PORT_NOT_FOUND    ((EC_T_DWORD)EC_E_ERROR+0x163)        
#define EC_SZTXT_E_AOE_TARGET_PORT_NOT_FOUND    "ERROR AoE: Target port not found"

#define       EC_E_AOE_TARGET_MACHINE_NOT_FOUND ((EC_T_DWORD)EC_E_ERROR+0x164)        
#define EC_SZTXT_E_AOE_TARGET_MACHINE_NOT_FOUND "ERROR AoE: Target machine not found"

#define       EC_E_AOE_UNKNOWN_CMD_ID           ((EC_T_DWORD)EC_E_ERROR+0x165)        
#define EC_SZTXT_E_AOE_UNKNOWN_CMD_ID           "ERROR AoE: Unknown command ID"

#define       EC_E_AOE_PORT_NOT_CONNECTED       ((EC_T_DWORD)EC_E_ERROR+0x166)        
#define EC_SZTXT_E_AOE_PORT_NOT_CONNECTED       "ERROR AoE: Port not connected"

#define       EC_E_AOE_INVALID_AMS_LENGTH       ((EC_T_DWORD)EC_E_ERROR+0x167)        
#define EC_SZTXT_E_AOE_INVALID_AMS_LENGTH       "ERROR AoE: Invalid AMS length"

#define       EC_E_AOE_INVALID_AMS_ID           ((EC_T_DWORD)EC_E_ERROR+0x168)        
#define EC_SZTXT_E_AOE_INVALID_AMS_ID           "ERROR AoE: invalid AMS Net ID"

#define       EC_E_AOE_PORT_DISABLED            ((EC_T_DWORD)EC_E_ERROR+0x169)        
#define EC_SZTXT_E_AOE_PORT_DISABLED            "ERROR AoE: Port disabled"

#define       EC_E_AOE_PORT_CONNECTED           ((EC_T_DWORD)EC_E_ERROR+0x16A)        
#define EC_SZTXT_E_AOE_PORT_CONNECTED           "ERROR AoE: Port already connected"

#define       EC_E_AOE_INVALID_AMS_PORT         ((EC_T_DWORD)EC_E_ERROR+0x16B)        
#define EC_SZTXT_E_AOE_INVALID_AMS_PORT         "ERROR AoE: Invalid AMS port!"

#define       EC_E_AOE_NO_MEMORY                ((EC_T_DWORD)EC_E_ERROR+0x16C)        
#define EC_SZTXT_E_AOE_NO_MEMORY                "ERROR AoE: No memory!"

#define       EC_E_AOE_VENDOR_SPECIFIC          ((EC_T_DWORD)EC_E_ERROR+0x16D)
#define EC_SZTXT_E_AOE_VENDOR_SPECIFIC          "ERROR AoE: Vendor specific AoE device error"

#define       EC_E_XML_AOE_NETID_INVALID        ((EC_T_DWORD)EC_E_ERROR+0x16E)
#define EC_SZTXT_E_XML_AOE_NETID_INVALID        "ERROR: Invalid AoE NetID!"

#define       EC_E_MAX_BUS_SLAVES_EXCEEDED      ((EC_T_DWORD)EC_E_ERROR+0x16F)
#define EC_SZTXT_E_MAX_BUS_SLAVES_EXCEEDED      "ERROR: Maximum number of bus slave has been exceeded!"

#define       EC_E_MBXERR_SYNTAX                ((EC_T_DWORD)EC_E_ERROR+0x170)
#define EC_SZTXT_E_MBXERR_SYNTAX                "ERROR Mailbox: Syntax of 6 octet Mailbox header is wrong!"

#define       EC_E_MBXERR_UNSUPPORTEDPROTOCOL   ((EC_T_DWORD)EC_E_ERROR+0x171)
#define EC_SZTXT_E_MBXERR_UNSUPPORTEDPROTOCOL   "ERROR Mailbox: The Mailbox protocol is not supported!"

#define       EC_E_MBXERR_INVALIDCHANNEL        ((EC_T_DWORD)EC_E_ERROR+0x172)
#define EC_SZTXT_E_MBXERR_INVALIDCHANNEL        "ERROR Mailbox: Field contains wrong value!"

#define       EC_E_MBXERR_SERVICENOTSUPPORTED   ((EC_T_DWORD)EC_E_ERROR+0x173)
#define EC_SZTXT_E_MBXERR_SERVICENOTSUPPORTED   "ERROR Mailbox: The service in the Mailbox protocol is not supported!"

#define       EC_E_MBXERR_INVALIDHEADER         ((EC_T_DWORD)EC_E_ERROR+0x174)
#define EC_SZTXT_E_MBXERR_INVALIDHEADER         "ERROR Mailbox: The mailbox protocol header of the mailbox protocol is wrong!"

#define       EC_E_MBXERR_SIZETOOSHORT          ((EC_T_DWORD)EC_E_ERROR+0x175)
#define EC_SZTXT_E_MBXERR_SIZETOOSHORT          "ERROR Mailbox: Length of received mailbox data is too short!"

#define       EC_E_MBXERR_NOMOREMEMORY          ((EC_T_DWORD)EC_E_ERROR+0x176)
#define EC_SZTXT_E_MBXERR_NOMOREMEMORY          "ERROR Mailbox: Mailbox protocol can not be processed because of limited ressources!"

#define       EC_E_MBXERR_INVALIDSIZE           ((EC_T_DWORD)EC_E_ERROR+0x177)
#define EC_SZTXT_E_MBXERR_INVALIDSIZE           "ERROR Mailbox: The length of data is inconsistent!"

#define       EC_E_DC_SLAVES_BEFORE_REF_CLOCK   ((EC_T_DWORD)EC_E_ERROR+0x178)
#define EC_SZTXT_E_DC_SLAVES_BEFORE_REF_CLOCK   "ERROR: Slaves with DC configured are present on bus before the reference clock!"

#define       EC_E_DATA_TYPE_CONVERSION_FAILED  ((EC_T_DWORD)EC_E_ERROR+0x179)
#define EC_SZTXT_E_DATA_TYPE_CONVERSION_FAILED  "ERROR: Data type conversion failed!"

#define       EC_E_FOE_ERRCODE_MAX_FILE_SIZE    ((EC_T_DWORD)EC_E_ERROR+0x17A)
#define EC_SZTXT_E_FOE_ERRCODE_MAX_FILE_SIZE    "ERROR FoE: File is bigger than max file size."

#define       EC_E_LINE_CROSSED                 ((EC_T_DWORD)EC_E_ERROR+0x17B)
#define EC_SZTXT_E_LINE_CROSSED                 "ERROR: Line crossed."

#define       EC_E_LINE_CROSSED_SLAVE_INFO      ((EC_T_DWORD)EC_E_ERROR+0x17C)
#define EC_SZTXT_E_LINE_CROSSED_SLAVE_INFO      "ERROR: Line crossed at slave \"%s\", EtherCAT auto-increment address=%d, station address=%d. Error at port %d."

#define       EC_E_SOCKET_DISCONNECTED          ((EC_T_DWORD)EC_E_ERROR+0x17D)
#define EC_SZTXT_E_SOCKET_DISCONNECTED          "ERROR: Socket disconnected"

#define       EC_E_ERROR_LAST                   ((EC_T_DWORD)EC_E_ERROR+0x17D)
/* warning!   EMRAS_E_ERROR                     ((EC_T_DWORD)EC_E_ERROR+0x180) */

/*********************************************************************/
/* DCM Error Codes                                                   */
/*********************************************************************/

#define       DCM_E_NOTINITIALIZED              ((EC_T_DWORD)DCM_E_ERROR+0x01)  
#define DCM_SZTXT_E_NOTINITIALIZED              "ERROR: init function not called or not successful"

#define       DCM_E_MAX_CTL_ERROR_EXCEED        ((EC_T_DWORD)DCM_E_ERROR+0x02)  
#define DCM_SZTXT_E_MAX_CTL_ERROR_EXCEED        "ERROR: controller error - synchronisation out of limit"

#define       DCM_E_NOMEMORY                    ((EC_T_DWORD)DCM_E_ERROR+0x03)  
#define DCM_SZTXT_E_NOMEMORY                    "ERROR: not enough memory"

#define       DCM_E_INVALID_HWLAYER             ((EC_T_DWORD)DCM_E_ERROR+0x04)  
#define DCM_SZTXT_E_INVALID_HWLAYER             "ERROR: hardware layer - (BSP) invalid"

#define       DCM_E_TIMER_MODIFY_ERROR          ((EC_T_DWORD)DCM_E_ERROR+0x05)  
#define DCM_SZTXT_E_TIMER_MODIFY_ERROR          "ERROR: hardware layer - error modifying the timer"

#define       DCM_E_TIMER_NOT_RUNNING           ((EC_T_DWORD)DCM_E_ERROR+0x06)  
#define DCM_SZTXT_E_TIMER_NOT_RUNNING           "ERROR: hardware layer - timer is not running"

#define       DCM_E_WRONG_CPU                   ((EC_T_DWORD)DCM_E_ERROR+0x07)  
#define DCM_SZTXT_E_WRONG_CPU                   "ERROR: hardware layer - function is called on wrong CPU"

#define       DCM_E_INVALID_SYNC_PERIOD         ((EC_T_DWORD)DCM_E_ERROR+0x08)  
#define DCM_SZTXT_E_INVALID_SYNC_PERIOD         "ERROR: invalid DC sync period length (invalid clock master?)"

#define       DCM_E_INVALID_SETVAL              ((EC_T_DWORD)DCM_E_ERROR+0x09)  
#define DCM_SZTXT_E_INVALID_SETVAL              "ERROR: Error DCM Controller SetVal is to small."

#define       DCM_E_DRIFT_TO_HIGH               ((EC_T_DWORD)DCM_E_ERROR+0x0A)  
#define DCM_SZTXT_E_DRIFT_TO_HIGH               "ERROR: Error DCM Controller - Drift between local timer and ref clock to high."

#define       DCM_E_BUS_CYCLE_WRONG             ((EC_T_DWORD)DCM_E_ERROR+0x0B)  
#define DCM_SZTXT_E_BUS_CYCLE_WRONG              "ERROR: Error DCM Controller - Bus cycle time (dwBusCycleTimeUsec) doesn't match real cycle"

#define       DCM_E_ERROR_LAST                  ((EC_T_DWORD)0x981201FF)

/*********************************************************************/
/* RAS Error Codes                                                   */
/*********************************************************************/

#define EMRAS_SZTXT_EMRAS_E_ERROR               "RAS Error"

#define       EMRAS_E_INVALIDCOOKIE             ((EC_T_DWORD)EMRAS_E_ERROR+0x01)
#define EMRAS_SZTXT_E_INVALIDCOOKIE             "ERROR: Invalid cookie"

#define       EMRAS_E_MULSRVDISMULCON           ((EC_T_DWORD)EMRAS_E_ERROR+0x03)
#define EMRAS_SZTXT_E_MULSRVDISMULCON           "ERROR: Connecting 2nd server denied because Multi Server support is disabled"

#define       EMRAS_E_LOGONCANCELLED            ((EC_T_DWORD)EMRAS_E_ERROR+0x04)
#define EMRAS_SZTXT_E_LOGONCANCELLED            "ERROR: Logon cancelled"

#define       EMRAS_E_INVALIDVERSION            ((EC_T_DWORD)EMRAS_E_ERROR+0x06)
#define EMRAS_SZTXT_E_INVALIDVERSION            "ERROR: Invalid version"

#define       EMRAS_E_INVALIDACCESSCONFIG       ((EC_T_DWORD)EMRAS_E_ERROR+0x07)
#define EMRAS_SZTXT_E_INVALIDACCESSCONFIG       "ERROR: Access configuration is invalid"

#define       EMRAS_E_ACCESSLESS                ((EC_T_DWORD)EMRAS_E_ERROR+0x08)
#define EMRAS_SZTXT_E_ACCESSLESS                "ERROR: No access to this call at this accesslevel"

#define       EMRAS_E_INVALIDDATARECEIVED       ((EC_T_DWORD)EMRAS_E_ERROR+0x09)
#define EMRAS_SZTXT_E_INVALIDDATARECEIVED       "ERROR: Invalid data received"

#define       EMRAS_EVT_SERVERSTOPPED           ((EC_T_DWORD)EMRAS_E_ERROR+0x11)
#define EMRAS_SZTXT_EVT_SERVERSTOPPED           "Server stopped"

#define       EMRAS_EVT_WDEXPIRED               ((EC_T_DWORD)EMRAS_E_ERROR+0x12)
#define EMRAS_SZTXT_EVT_WDEXPIRED               "Watchdog expired"

#define       EMRAS_EVT_RECONEXPIRED            ((EC_T_DWORD)EMRAS_E_ERROR+0x13)
#define EMRAS_SZTXT_EVT_RECONEXPIRED            "Reconnect expired"

#define       EMRAS_EVT_CLIENTLOGON             ((EC_T_DWORD)EMRAS_E_ERROR+0x14)
#define EMRAS_SZTXT_EVT_CLIENTLOGON             "Client logged on"

#define       EMRAS_EVT_RECONNECT               ((EC_T_DWORD)EMRAS_E_ERROR+0x15)            /* Client reconnect */
#define EMRAS_SZTXT_EVT_RECONNECT               "Client reconnected"

#define       EMRAS_EVT_SOCKCHANGE              ((EC_T_DWORD)EMRAS_E_ERROR+0x16)            /* Socket exchanged after Reconnect */
#define EMRAS_SZTXT_EVT_SOCKCHANGE              "Socket exchanged after reconnect"
                                   
#define       EMRAS_EVT_CLNTDISC                ((EC_T_DWORD)EMRAS_E_ERROR+0x17)            /* Client disconnected */
#define EMRAS_SZTXT_EVT_CLNTDISC                "Client disconnected"

#define       EMRAS_E_ERROR_LAST                ((EC_T_DWORD)0x981101BF)
/* warning!   DCM_E_ERROR                       ((EC_T_DWORD)EMRAS_E_ERROR+0x10040) */ 


/*********************************************************************/
/* Application Framework Strings                                     */
/*********************************************************************/

#define       EC_TXT_MASTER_STATE_CHANGE        ((EC_T_WORD)(EC_TEXTBASE+0x0001))
#define EC_SZTXT_TXT_MASTER_STATE_CHANGE        "Master state changed from <%s> to <%s>"

#define       EC_TXT_SB_RESULT_OK               ((EC_T_WORD)(EC_TEXTBASE+0x0002))
#define EC_SZTXT_TXT_SB_RESULT_OK               "Bus scan successful - %d slaves found"

#define       EC_TXT_SB_RESULT_ERROR            ((EC_T_WORD)(EC_TEXTBASE+0x0003))
#define EC_SZTXT_TXT_SB_RESULT_ERROR            "Bus scan error '%s (0x%x)', %d slaves found"

#define       EC_TXT_DC_STATUS                  ((EC_T_WORD)(EC_TEXTBASE+0x0004))
#define EC_SZTXT_TXT_DC_STATUS                  "Distributed clocks - status %s (0x%x)"

#define       EC_TXT_DCL_STATUS                 ((EC_T_WORD)(EC_TEXTBASE+0x0005))
#define EC_SZTXT_TXT_DCL_STATUS                 "Distributed clocks - latching status %s (0x%x)"

#define       EC_TXT_COE_SDO_DNLD_ERROR         ((EC_T_WORD)(EC_TEXTBASE+0x0006))
#define EC_SZTXT_TXT_COE_SDO_DNLD_ERROR         "CoE - SDO download failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_COE_SDO_UPLD_ERROR         ((EC_T_WORD)(EC_TEXTBASE+0x0007))
#define EC_SZTXT_TXT_COE_SDO_UPLD_ERROR         "CoE - SDO upload failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_COE_GETODL_ERROR           ((EC_T_WORD)(EC_TEXTBASE+0x0008))
#define EC_SZTXT_TXT_COE_GETODL_ERROR           "CoE - OD-list upload failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_COE_GETOBDESC_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x0009))
#define EC_SZTXT_TXT_COE_GETOBDESC_ERROR        "CoE - object description upload failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_COE_GETENTRYDESC_ERROR     ((EC_T_WORD)(EC_TEXTBASE+0x000A))
#define EC_SZTXT_TXT_COE_GETENTRYDESC_ERROR     "CoE - object entry description upload failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_COE_EMRG_TFER_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x000B))
#define EC_SZTXT_TXT_COE_EMRG_TFER_ERROR        "CoE - emergency transfer failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_COE_EMRG                   ((EC_T_WORD)(EC_TEXTBASE+0x000C))
#define EC_SZTXT_TXT_COE_EMRG                   "CoE - emergency request, id =0x%x, len=%d ==> slave address=%d, ErrCode=0x%x, ErrReg=0x%x, data: '%02x %02x %02x %02x %02x'."

#define       EC_TXT_CYCCMD_WKC_ERROR           ((EC_T_WORD)(EC_TEXTBASE+0x000D))
#define EC_SZTXT_TXT_CYCCMD_WKC_ERROR           "Cyclic command WKC error on %s - Address: 0x%x - WKC act/set=%d/%d"

#define       EC_TXT_MASTINITCMD_WKC_ERROR      ((EC_T_WORD)(EC_TEXTBASE+0x000E))
#define EC_SZTXT_TXT_MASTINITCMD_WKC_ERROR      "Master init command working counter error - Command: %s - Logical/Physical address: 0x%x, WKC act/set=%d/%d"

#define       EC_TXT_SLVINITCMD_WKC_ERROR       ((EC_T_WORD)(EC_TEXTBASE+0x000F))
#define EC_SZTXT_TXT_SLVINITCMD_WKC_ERROR       "Slave init command working counter error - Slave  \"%s\": - EtherCAT address=%d - Command: %s - Logical/Physical address: 0x%x, WKC act/set=%d/%d"

#define       EC_TXT_EOEMBXRCV_WKC_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x0010))
#define EC_SZTXT_TXT_EOEMBXRCV_WKC_ERROR        "EoE mbox receive working counter error - Slave  \"%s\": - EtherCAT address=%d - Command: %s - Logical/Physical address: 0x%x, WKC act/set=%d/%d"

#define       EC_TXT_COEMBXRCV_WKC_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x0011))
#define EC_SZTXT_TXT_COEMBXRCV_WKC_ERROR        "CoE mbox receive working counter error - Slave  \"%s\": - EtherCAT address=%d - Command: %s - Logical/Physical address: 0x%x, WKC act/set=%d/%d"

#define       EC_TXT_FOEMBXRCV_WKC_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x0012))
#define EC_SZTXT_TXT_FOEMBXRCV_WKC_ERROR        "FoE mbox receive working counter error - Slave  \"%s\": - EtherCAT address=%d - Command: %s - Logical/Physical address: 0x%x, WKC act/set=%d/%d"

#define       EC_TXT_SOEMBXRCV_WKC_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x0013))
#define EC_SZTXT_TXT_SOEMBXRCV_WKC_ERROR        "SoE mbox receive working counter error - Slave  \"%s\": - EtherCAT address=%d - Command: %s - Logical/Physical address: 0x%x, WKC act/set=%d/%d"

#define       EC_TXT_EOEMBXSND_WKC_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x0014))
#define EC_SZTXT_TXT_EOEMBXSND_WKC_ERROR        "EoE mbox send working counter error - Slave  \"%s\": - EtherCAT address=%d - Command: %s - Logical/Physical address: 0x%x, WKC act/set=%d/%d"

#define       EC_TXT_COEMBXSND_WKC_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x0015))
#define EC_SZTXT_TXT_COEMBXSND_WKC_ERROR        "CoE mbox send working counter error - Slave  \"%s\": - EtherCAT address=%d - Command: %s - Logical/Physical address: 0x%x, WKC act/set=%d/%d"

#define       EC_TXT_FOEMBXSND_WKC_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x0016))
#define EC_SZTXT_TXT_FOEMBXSND_WKC_ERROR        "FoE mbox send working counter error - Slave  \"%s\": - EtherCAT address=%d - Command: %s - Logical/Physical address: 0x%x, WKC act/set=%d/%d"

#define       EC_TXT_SOEMBXSND_WKC_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x0017))
#define EC_SZTXT_TXT_SOEMBXSND_WKC_ERROR        "SoE mbox send working counter error - Slave  \"%s\": - EtherCAT address=%d - Command: %s - Logical/Physical address: 0x%x, WKC act/set=%d/%d"

#define       EC_TXT_FRMRESP_RETRY              ((EC_T_WORD)(EC_TEXTBASE+0x0018))
#define EC_SZTXT_TXT_FRMRESP_RETRY              "Retry sending a %s frame due to %s"

#define       EC_TXT_FRMRESP_NORETRY            ((EC_T_WORD)(EC_TEXTBASE+0x0019))
#define EC_SZTXT_TXT_FRMRESP_NORETRY            "%s response on %s Ethernet frame"

#define       EC_TXT_ADDERRINFO                 ((EC_T_WORD)(EC_TEXTBASE+0x001A))
#define EC_SZTXT_TXT_ADDERRINFO                 "Additional error information: %s"

#define       EC_TXT_CMDIDXACTVAL               ((EC_T_WORD)(EC_TEXTBASE+0x001B))
#define EC_SZTXT_TXT_CMDIDXACTVAL               "EtherCAT command IDX act value=0x%x"

#define       EC_TXT_CMDIDXSETVAL               ((EC_T_WORD)(EC_TEXTBASE+0x001C))
#define EC_SZTXT_TXT_CMDIDXSETVAL               "EtherCAT command IDX set value=0x%x"

#define       EC_TXT_SLVINITCMDRSPERR_NR        ((EC_T_WORD)(EC_TEXTBASE+0x001D))
#define EC_SZTXT_TXT_SLVINITCMDRSPERR_NR        "Slave init command response error - Slave  \"%s\": - EtherCAT address=%d - Current State change of slave=\"%s\" No Response."

#define       EC_TXT_SLVINITCMDRSPERR_VE        ((EC_T_WORD)(EC_TEXTBASE+0x001E))
#define EC_SZTXT_TXT_SLVINITCMDRSPERR_VE        "Slave init command response error - Slave  \"%s\": - EtherCAT address=%d - Current State change of slave=\"%s\" Validation error."

#define       EC_TXT_SLVINITCMDRSPERR_FLD       ((EC_T_WORD)(EC_TEXTBASE+0x001F))
#define EC_SZTXT_TXT_SLVINITCMDRSPERR_FLD       "Slave init command response error - Slave  \"%s\": - EtherCAT address=%d - Current State change of slave=\"%s\" target state could not be reached."

#define       EC_TXT_MASTINITCMDRSPERR_NR       ((EC_T_WORD)(EC_TEXTBASE+0x0020))
#define EC_SZTXT_TXT_MASTINITCMDRSPERR_NR       "Master init command response error - Current State change of master=\"%s\" No Response, is there anything connected?"

#define       EC_TXT_MASTINITCMDRSPERR_VE       ((EC_T_WORD)(EC_TEXTBASE+0x0021))
#define EC_SZTXT_TXT_MASTINITCMDRSPERR_VE       "Master init command response error - Current State change of master=\"%s\" Validation error, are the correct slaves connected?"
                 
#define       EC_TXT_CMD_MISSING                ((EC_T_WORD)(EC_TEXTBASE+0x0022))
#define EC_SZTXT_TXT_CMD_MISSING                "Missing EtherCAT %s command in Ethernet frame - Slave  \"%s\": - EtherCAT address=%d - Index of missing command in the Ethernet frame=%d"

#define       EC_TXT_MBSLV_INITCMDTO            ((EC_T_WORD)(EC_TEXTBASE+0x0023))
#define EC_SZTXT_TXT_MBSLV_INITCMDTO            "\"Mailbox init command\" time-out - Slave  \"%s\": - EtherCAT address=%d - Current State change of slave=\"%s\""

#define       EC_TXT_NOT_ALL_DEVS_OP            ((EC_T_WORD)(EC_TEXTBASE+0x0024))
#define EC_SZTXT_TXT_NOT_ALL_DEVS_OP            "Not all EtherCAT slave devices are in operational state"

#define       EC_TXT_CABLE_CONNECTED            ((EC_T_WORD)(EC_TEXTBASE+0x0025))
#define EC_SZTXT_TXT_CABLE_CONNECTED            "Ethernet cable connected"

#define       EC_TXT_CABLE_NOT_CONNECTED        ((EC_T_WORD)(EC_TEXTBASE+0x0026))
#define EC_SZTXT_TXT_CABLE_NOT_CONNECTED        "Ethernet cable not connected"

#define       EC_TXT_CYCCMD_TIMEOUT             ((EC_T_WORD)(EC_TEXTBASE+0x0027))
#define EC_SZTXT_TXT_CYCCMD_TIMEOUT             "Cyclic command time-out: Time between sending cyclic commands too long"

#define       EC_TXT_REDLINEBREAK               ((EC_T_WORD)(EC_TEXTBASE+0x0028))
#define EC_SZTXT_TXT_REDLINEBREAK               "ERROR: Redundancy Line break between slave %d and %d"

#define       EC_TXT_SLVERR_DETECTED            ((EC_T_WORD)(EC_TEXTBASE+0x0029))
#define EC_SZTXT_TXT_SLVERR_DETECTED            "ERROR: At least one slave is in error status!"

#define       EC_TXT_SLVERR_INFO                ((EC_T_WORD)(EC_TEXTBASE+0x002A))
#define EC_SZTXT_TXT_SLVERR_INFO                "Slave error \"%s\": - EtherCAT address=%d - State <%s%s>(0x%x), control status <%s>(0x%x)"

#define       EC_TXT_SLV_NOT_ADDRABLE           ((EC_T_WORD)(EC_TEXTBASE+0x002B))
#define EC_SZTXT_TXT_SLV_NOT_ADDRABLE           "Unable to address slave \"%s\": - EtherCAT address=%d"

#define       EC_TXT_MBSLV_SDO_ABORT            ((EC_T_WORD)(EC_TEXTBASE+0x002C))
#define EC_SZTXT_TXT_MBSLV_SDO_ABORT            "SDO abort - Slave  \"%s\": - EtherCAT address=%d - %s (0x%x) - Index=0x%x SubIndex=0x%x"

#define       EC_TXT_DCSLVSYNC_INSYNC           ((EC_T_WORD)(EC_TEXTBASE+0x002D))
#define EC_SZTXT_TXT_DCSLVSYNC_INSYNC           "DC slaves are \"in-sync\" - Deviation: 0x%x"

#define       EC_TXT_DCSLVSYNC_OUTOFSYNC        ((EC_T_WORD)(EC_TEXTBASE+0x002E))
#define EC_SZTXT_TXT_DCSLVSYNC_OUTOFSYNC        "DC slaves are \"out-of-sync\" - Deviation: 0x%x"

#define       EC_TXT_DCL_SINGLE_LATCH           ((EC_T_WORD)(EC_TEXTBASE+0x002F))
#define EC_SZTXT_TXT_DCL_SINGLE_LATCH           "DC single latch on slave 0x%x Ident 0x%x"

#define       EC_TXT_FOE_UPLD_ERROR             ((EC_T_WORD)(EC_TEXTBASE+0x0030))
#define EC_SZTXT_TXT_FOE_UPLD_ERROR             "MbxTferRcv - file upload failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_FOE_DNLD_ERROR             ((EC_T_WORD)(EC_TEXTBASE+0x0031))
#define EC_SZTXT_TXT_FOE_DNLD_ERROR             "MbxTferRcv - file download failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_CLNTREGDROP                ((EC_T_WORD)(EC_TEXTBASE+0x0032))
#define EC_SZTXT_TXT_CLNTREGDROP                "Client registration lost!"

#define       EC_TXT_REDLINEFIXED               ((EC_T_WORD)(EC_TEXTBASE+0x0033))
#define EC_SZTXT_TXT_REDLINEFIXED               "Redundancy - Line is fixed"

#define       EC_TXT_SOEMBX_WRITE_ERROR         ((EC_T_WORD)(EC_TEXTBASE+0x0034))
#define EC_SZTXT_TXT_SOEMBX_WRITE_ERROR         "SoE init command mbox write error - Slave  \"%s\": - EtherCAT address=%d - Current State change of slave=\"%s\""

#define       EC_TXT_DCM_INSYNC                 ((EC_T_WORD)(EC_TEXTBASE+0x0035))
#define EC_SZTXT_TXT_DCM_INSYNC                 "DCM is in sync Cur=\"%7d\", Avg=\"%7d\", Max=\"%7d\""

#define       EC_TXT_DCM_OUTOFSYNC              ((EC_T_WORD)(EC_TEXTBASE+0x0036))
#define EC_SZTXT_TXT_DCM_OUTOFSYNC              "DCM is out of sync Cur=\"%7d\", Avg=\"%7d\", Max=\"%7d\""

#define       EC_TXT_SLAVE_STATECHANGED         ((EC_T_WORD)(EC_TEXTBASE+0x0037))
#define EC_SZTXT_TXT_SLAVE_STATECHANGED         "Slave %d: state changed to <%s>"

#define       EC_TXT_STATUSCODE_NOERROR         ((EC_T_WORD)(EC_ALSTATEBASE+0x0000))
#define EC_SZTXT_TXT_STATUSCODE_NOERROR         "No error"

#define       EC_TXT_STATUSCODE_ERROR           ((EC_T_WORD)(EC_ALSTATEBASE+0x0001))
#define EC_SZTXT_TXT_STATUSCODE_ERROR           "Unspecified error"

#define       EC_TXT_STATUSCODE_NO_MEMORY       ((EC_T_WORD)(EC_ALSTATEBASE+0x0002))
#define EC_SZTXT_TXT_STATUSCODE_NO_MEMORY       "No Memory"
                                                
#define       EC_TXT_STATUSCODE_INVREQSTATECNG  ((EC_T_WORD)(EC_ALSTATEBASE+0x0011))
#define EC_SZTXT_TXT_STATUSCODE_INVREQSTATECNG  "Invalid requested state change"

#define       EC_TXT_STATUSCODE_UNKREQSTATE     ((EC_T_WORD)(EC_ALSTATEBASE+0x0012))
#define EC_SZTXT_TXT_STATUSCODE_UNKREQSTATE     "Unknown requested state"

#define       EC_TXT_STATUSCODE_BOOTSTRAPNSUPP  ((EC_T_WORD)(EC_ALSTATEBASE+0x0013))
#define EC_SZTXT_TXT_STATUSCODE_BOOTSTRAPNSUPP  "Bootstrap not supported"

#define       EC_TXT_STATUSCODE_NOVALIDFW       ((EC_T_WORD)(EC_ALSTATEBASE+0x0014))
#define EC_SZTXT_TXT_STATUSCODE_NOVALIDFW       "No valid firmware"

#define       EC_TXT_STATUSCODE_INVALIDMBXCNF1  ((EC_T_WORD)(EC_ALSTATEBASE+0x0015))
#define EC_SZTXT_TXT_STATUSCODE_INVALIDMBXCNF1  "Invalid mailbox configuration in BOOT"

#define       EC_TXT_STATUSCODE_INVALIDMBXCNF2  ((EC_T_WORD)(EC_ALSTATEBASE+0x0016))
#define EC_SZTXT_TXT_STATUSCODE_INVALIDMBXCNF2  "Invalid mailbox configuration in PREOP"

#define       EC_TXT_STATUSCODE_INVALIDSMCNF    ((EC_T_WORD)(EC_ALSTATEBASE+0x0017))
#define EC_SZTXT_TXT_STATUSCODE_INVALIDSMCNF    "Invalid sync manager configuration"

#define       EC_TXT_STATUSCODE_NOVALIDIN       ((EC_T_WORD)(EC_ALSTATEBASE+0x0018))
#define EC_SZTXT_TXT_STATUSCODE_NOVALIDIN       "No valid inputs available"

#define       EC_TXT_STATUSCODE_NOVALIDOUT      ((EC_T_WORD)(EC_ALSTATEBASE+0x0019))
#define EC_SZTXT_TXT_STATUSCODE_NOVALIDOUT      "No valid outputs available"

#define       EC_TXT_STATUSCODE_SYNCERROR       ((EC_T_WORD)(EC_ALSTATEBASE+0x001A))
#define EC_SZTXT_TXT_STATUSCODE_SYNCERROR       "Synchronization error"

#define       EC_TXT_STATUSCODE_SMWATCHDOG      ((EC_T_WORD)(EC_ALSTATEBASE+0x001B))
#define EC_SZTXT_TXT_STATUSCODE_SMWATCHDOG      "Sync manager watchdog"

#define       EC_TXT_STATUSCODE_INVSMTYPES      ((EC_T_WORD)(EC_ALSTATEBASE+0x001C))
#define EC_SZTXT_TXT_STATUSCODE_INVSMTYPES      "Invalid Sync Manager Types"

#define       EC_TXT_STATUSCODE_INVOUTCONFIG    ((EC_T_WORD)(EC_ALSTATEBASE+0x001D))
#define EC_SZTXT_TXT_STATUSCODE_INVOUTCONFIG    "Invalid Output Configuration"

#define       EC_TXT_STATUSCODE_INVINCONFIG     ((EC_T_WORD)(EC_ALSTATEBASE+0x001E))
#define EC_SZTXT_TXT_STATUSCODE_INVINCONFIG     "Invalid Input Configuration"

#define       EC_TXT_STATUSCODE_INVWDCONFIG     ((EC_T_WORD)(EC_ALSTATEBASE+0x001F))
#define EC_SZTXT_TXT_STATUSCODE_INVWDCONFIG     "Invalid Watchdog Configuration"

#define       EC_TXT_STATUSCODE_SLVNEEDCOLDRS   ((EC_T_WORD)(EC_ALSTATEBASE+0x0020))
#define EC_SZTXT_TXT_STATUSCODE_SLVNEEDCOLDRS   "Slave needs cold restart"

#define       EC_TXT_STATUSCODE_SLVNEEDINIT     ((EC_T_WORD)(EC_ALSTATEBASE+0x0021))
#define EC_SZTXT_TXT_STATUSCODE_SLVNEEDINIT     "Slave needs INIT"

#define       EC_TXT_STATUSCODE_SLVNEEDPREOP    ((EC_T_WORD)(EC_ALSTATEBASE+0x0022))
#define EC_SZTXT_TXT_STATUSCODE_SLVNEEDPREOP    "Slave needs PREOP"

#define       EC_TXT_STATUSCODE_SLVNEEDSAFEOP   ((EC_T_WORD)(EC_ALSTATEBASE+0x0023))
#define EC_SZTXT_TXT_STATUSCODE_SLVNEEDSAFEOP   "Slave needs SAFEOP"

#define       EC_TXT_STATUSCODE_INVALID_INPUT_MAPPING         ((EC_T_WORD)(EC_ALSTATEBASE+0x0024))
#define EC_SZTXT_TXT_STATUSCODE_INVALID_INPUT_MAPPING         "Invalid Input Mapping"

#define       EC_TXT_STATUSCODE_INVALID_OUTPUT_MAPPING        ((EC_T_WORD)(EC_ALSTATEBASE+0x0025))
#define EC_SZTXT_TXT_STATUSCODE_INVALID_OUTPUT_MAPPING        "Invalid Output Mapping"

#define       EC_TXT_STATUSCODE_INCONSISTENT_SETTINGS         ((EC_T_WORD)(EC_ALSTATEBASE+0x0026))
#define EC_SZTXT_TXT_STATUSCODE_INCONSISTENT_SETTINGS         "Inconsistent Settings"

#define       EC_TXT_STATUSCODE_FREERUN_NOT_SUPPORTED         ((EC_T_WORD)(EC_ALSTATEBASE+0x0027))
#define EC_SZTXT_TXT_STATUSCODE_FREERUN_NOT_SUPPORTED         "FreeRun not supported"

#define       EC_TXT_STATUSCODE_SYNCMODE_NOT_SUPPORTED        ((EC_T_WORD)(EC_ALSTATEBASE+0x0028))
#define EC_SZTXT_TXT_STATUSCODE_SYNCMODE_NOT_SUPPORTED        "SyncMode not supported"

#define       EC_TXT_STATUSCODE_FREERUN_NEEDS_THREEBUFFER_MODE ((EC_T_WORD)(EC_ALSTATEBASE+0x0029))
#define EC_SZTXT_TXT_STATUSCODE_FREERUN_NEEDS_THREEBUFFER_MODE "FreeRun needs 3Buffer Mode"

#define       EC_TXT_STATUSCODE_BACKGROUND_WATCHDOG           ((EC_T_WORD)(EC_ALSTATEBASE+0x002A))
#define EC_SZTXT_TXT_STATUSCODE_BACKGROUND_WATCHDOG           "Background Watchdog"

#define       EC_TXT_STATUSCODE_NO_VALID_INPUTS_AND_OUTPUTS   ((EC_T_WORD)(EC_ALSTATEBASE+0x002B))
#define EC_SZTXT_TXT_STATUSCODE_NO_VALID_INPUTS_AND_OUTPUTS   "No Valid Inputs and Outputs"

#define       EC_TXT_STATUSCODE_FATAL_SYNC_ERROR              ((EC_T_WORD)(EC_ALSTATEBASE+0x002C))
#define EC_SZTXT_TXT_STATUSCODE_FATAL_SYNC_ERROR              "Fatal Sync Error"

#define       EC_TXT_STATUSCODE_INVOUTFMMUCNFG  ((EC_T_WORD)(EC_ALSTATEBASE+0x002D))
#define EC_SZTXT_TXT_STATUSCODE_INVOUTFMMUCNFG  "Invalid Output FMMU Configuration"

#define       EC_TXT_STATUSCODE_INVINFMMUCNFG   ((EC_T_WORD)(EC_ALSTATEBASE+0x002E))
#define EC_SZTXT_TXT_STATUSCODE_INVINFMMUCNFG   "Invalid Input FMMU Configuration"
                                                
#define       EC_TXT_STATUSCODE_INVDCSYNCCNFG   ((EC_T_WORD)(EC_ALSTATEBASE+0x0030))
#define EC_SZTXT_TXT_STATUSCODE_INVDCSYNCCNFG   "Invalid DC SYNCH Configuration"

#define       EC_TXT_STATUSCODE_INVDCLATCHCNFG  ((EC_T_WORD)(EC_ALSTATEBASE+0x0031))
#define EC_SZTXT_TXT_STATUSCODE_INVDCLATCHCNFG  "Invalid DC Latch Configuration"

#define       EC_TXT_STATUSCODE_PLLERROR        ((EC_T_WORD)(EC_ALSTATEBASE+0x0032))
#define EC_SZTXT_TXT_STATUSCODE_PLLERROR        "PLL Error"

#define       EC_TXT_STATUSCODE_INVDCIOERROR    ((EC_T_WORD)(EC_ALSTATEBASE+0x0033))
#define EC_SZTXT_TXT_STATUSCODE_INVDCIOERROR    "Invalid DC IO Error"

#define       EC_TXT_STATUSCODE_INVDCTOERROR    ((EC_T_WORD)(EC_ALSTATEBASE+0x0034))
#define EC_SZTXT_TXT_STATUSCODE_INVDCTOERROR    "Invalid DC Timeout Error"

#define       EC_TXT_STATUSCODE_DC_INVALID_SYNC_CYCLE_TIME    ((EC_T_WORD)(EC_ALSTATEBASE+0x0035))
#define EC_SZTXT_TXT_STATUSCODE_DC_INVALID_SYNC_CYCLE_TIME    "DC Invalid Sync Cycle Time"

#define       EC_TXT_STATUSCODE_DC_SYNC0_CYCLE_TIME           ((EC_T_WORD)(EC_ALSTATEBASE+0x0036))
#define EC_SZTXT_TXT_STATUSCODE_DC_SYNC0_CYCLE_TIME           "DC Sync0 Cycle Time"

#define       EC_TXT_STATUSCODE_DC_SYNC1_CYCLE_TIME           ((EC_T_WORD)(EC_ALSTATEBASE+0x0037))
#define EC_SZTXT_TXT_STATUSCODE_DC_SYNC1_CYCLE_TIME           "DC Sync1 Cycle Time"

#define       EC_TXT_STATUSCODE_MBX_AOE                       ((EC_T_WORD)(EC_ALSTATEBASE+0x0041))
#define EC_SZTXT_TXT_STATUSCODE_MBX_AOE                       "MBX AOE"
                                                
#define       EC_TXT_STATUSCODE_MBX_EOE         ((EC_T_WORD)(EC_ALSTATEBASE+0x0042))
#define EC_SZTXT_TXT_STATUSCODE_MBX_EOE         "MBX EOE"

#define       EC_TXT_STATUSCODE_MBX_COE         ((EC_T_WORD)(EC_ALSTATEBASE+0x0043))
#define EC_SZTXT_TXT_STATUSCODE_MBX_COE         "MBX COE"

#define       EC_TXT_STATUSCODE_MBX_FOE         ((EC_T_WORD)(EC_ALSTATEBASE+0x0044))
#define EC_SZTXT_TXT_STATUSCODE_MBX_FOE         "MBX FOE"

#define       EC_TXT_STATUSCODE_MBX_SOE         ((EC_T_WORD)(EC_ALSTATEBASE+0x0045))
#define EC_SZTXT_TXT_STATUSCODE_MBX_SOE         "MBX SOE"
                                                
#define       EC_TXT_STATUSCODE_MBX_VOE         ((EC_T_WORD)(EC_ALSTATEBASE+0x004F))
#define EC_SZTXT_TXT_STATUSCODE_MBX_VOE         "MBX VOE"

#define       EC_TXT_STATUSCODE_EEPROM_NO_ACCESS              ((EC_T_WORD)(EC_ALSTATEBASE+0x0050))
#define EC_SZTXT_TXT_STATUSCODE_EEPROM_NO_ACCESS              "EEPROM no access"

#define       EC_TXT_STATUSCODE_EEPROM_ERROR                  ((EC_T_WORD)(EC_ALSTATEBASE+0x0051))
#define EC_SZTXT_TXT_STATUSCODE_EEPROM_ERROR                  "EEPROM Error"

#define       EC_TXT_STATUSCODE_SLAVE_RESTARTED_LOCALLY       ((EC_T_WORD)(EC_ALSTATEBASE+0x0060))
#define EC_SZTXT_TXT_STATUSCODE_SLAVE_RESTARTED_LOCALLY       "Slave restarted locally"

#define       EC_ALSTATEBASE_LAST               ((EC_T_WORD)0x03FF)

#define       EC_TXT_HC_DETAGRESULT_OK          ((EC_T_WORD)(EC_TEXTBASE+0x0050))
#define EC_SZTXT_TXT_HC_DETAGRESULT_OK          "Hot Connect detect all groups successful - %d groups found, %d groups are present, Mask: 0x%x"

#define       EC_TXT_HC_DETAGRESULT_ERROR       ((EC_T_WORD)(EC_TEXTBASE+0x0051))
#define EC_SZTXT_TXT_HC_DETAGRESULT_ERROR       "Hot Connect detect all groups error '%s (0x%x)', %d groups found"

#define       EC_TXT_SLAVE_DISAPPEARS           ((EC_T_WORD)(EC_TEXTBASE+0x0052))
#define       EC_TXT_HC_SLAVEPART EC_TXT_SLAVE_DISAPPEARS
#define EC_SZTXT_TXT_SLAVE_DISAPPEARS           "Slave %s with station address %d removed from Bus"
#define EC_SZTXT_TXT_HC_SLAVEPART EC_SZTXT_TXT_SLAVE_DISAPPEARS

#define       EC_TXT_SLAVE_APPEARS              ((EC_T_WORD)(EC_TEXTBASE+0x0053))
#define       EC_TXT_HC_SLAVEJOIN EC_TXT_SLAVE_APPEARS
#define EC_SZTXT_TXT_SLAVE_APPEARS              "Slave %s with station address %d added to Bus"
#define EC_SZTXT_TXT_HC_SLAVEJOIN EC_SZTXT_TXT_SLAVE_APPEARS

#define       EC_TXT_HC_TOPOCHGDONE             ((EC_T_WORD)(EC_TEXTBASE+0x0054))
#define EC_SZTXT_TXT_HC_TOPOCHGDONE             "Topology Change Performed %s (0x%x)"

#define       EC_TXT_HC_BORDERCLOSE             ((EC_T_WORD)(EC_TEXTBASE+0x0055))
#define EC_SZTXT_TXT_HC_BORDERCLOSE             "Border Close Performed on Port %x:%x (%s) with Result : %s (0x%x)"

#define       EC_TXT_SOE_WRITE_ERROR            ((EC_T_WORD)(EC_TEXTBASE+0x0056))
#define EC_SZTXT_TXT_SOE_WRITE_ERROR            "MbxTferRcv - Write IDN failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_SOE_READ_ERROR             ((EC_T_WORD)(EC_TEXTBASE+0x0057))
#define EC_SZTXT_TXT_SOE_READ_ERROR             "MbxTferRcv - Read IDN failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_SOE_EMRG_TFER_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x0058))
#define EC_SZTXT_TXT_SOE_EMRG_TFER_ERROR        "SoE - emergency transfer failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_SOE_EMRG                   ((EC_T_WORD)(EC_TEXTBASE+0x0059))
#define EC_SZTXT_TXT_SOE_EMRG                   "SoE - emergency request, id =0x%x, len=%d ==> slave address=%d, ErrCode=0x%x, data: '%02x %02x %02x %02x %02x'."

#define       EC_TXT_SOE_NOTIFIC_TFER_ERROR     ((EC_T_WORD)(EC_TEXTBASE+0x005a))
#define EC_SZTXT_TXT_SOE_NOTIFIC_TFER_ERROR     "SoE - notification transfer failure, statVal=%d, errCode=0x%x (%s)"

#define       EC_TXT_SOE_NOTIFICATION           ((EC_T_WORD)(EC_TEXTBASE+0x005b))
#define EC_SZTXT_TXT_SOE_NOTIFICATION           "SoE - notification request, id =0x%x, len=%d ==> slave address=%d, Header=0x%x, IDN=%d, data: '%02x %02x %02x %02x %02x'."

#define       EC_TXT_MBSLV_FOE_ABORT            ((EC_T_WORD)(EC_TEXTBASE+0x005C))
#define EC_SZTXT_TXT_MBSLV_FOE_ABORT            "FoE abort - Slave  \"%s\": - EtherCAT address=%d - %s (0x%x)"

#define       EC_TXT_MBXRCV_INVALID_DATA        ((EC_T_WORD)(EC_TEXTBASE+0x005D))
#define EC_SZTXT_TXT_MBXRCV_INVALID_DATA        "Invalid mailbox data received - Slave  \"%s\": - EtherCAT address=%d"

#define       EC_TXT_PDIWATCHDOG                ((EC_T_WORD)(EC_TEXTBASE+0x005E))
#define EC_SZTXT_TXT_PDIWATCHDOG                "PDI Watchdog expired - Slave  \"%s\": - EtherCAT address=%d"

#define       EC_TXT_SLAVE_NOTSUPPORTED         ((EC_T_WORD)(EC_TEXTBASE+0x005F))
#define EC_SZTXT_TXT_SLAVE_NOTSUPPORTED         "Slave not supported - Slave  \"%s\": - EtherCAT address=%d"

#define       EC_TXT_SLAVE_UNEXPECTED_STATE     ((EC_T_WORD)(EC_TEXTBASE+0x0060))
#define EC_SZTXT_TXT_SLAVE_UNEXPECTED_STATE     "Unexpected state - Slave  \"%s\": - EtherCAT address=%d - Current state=<%s> - Expected=<%s>"

#define       EC_TXT_ALL_DEVS_OP                ((EC_T_WORD)(EC_TEXTBASE+0x0061))
#define EC_SZTXT_TXT_ALL_DEVS_OP                "All EtherCAT slave devices are in operational state"

#define       EC_TXT_VOE_DNLD_ERROR             ((EC_T_WORD)(EC_TEXTBASE+0x0062))
#define EC_SZTXT_TXT_VOE_DNLD_ERROR             "VoE write failed, eTferStatus = %d, error = 0x%x (%s)"

#define       EC_TXT_VOE_UPLD_ERROR             ((EC_T_WORD)(EC_TEXTBASE+0x0063))
#define EC_SZTXT_TXT_VOE_UPLD_ERROR             "VoE read failed, eTferStatus = %d, error = 0x%x (%s)"

#define       EC_TXT_VOEMBXSND_WKC_ERROR        ((EC_T_WORD)(EC_TEXTBASE+0x0064))
#define EC_SZTXT_TXT_VOEMBXSND_WKC_ERROR        "VoE mbox send working counter error - Slave  \"%s\": - EtherCAT address=%d - Command: %s - Logical/Physical address: 0x%x, WKC act/set=%d/%d"

#define       EC_TXT_AOE_CMD_ERROR              ((EC_T_WORD)(EC_TEXTBASE+0x0065))
#define EC_SZTXT_TXT_AOE_CMD_ERROR              "AoE command response error - error = 0x%x (%s) -  device error: 0x%x - command error: 0x%x"
    
#define       EC_TXT_EEPROM_CHECKSUM_ERROR      ((EC_T_WORD)(EC_TEXTBASE+0x0066))
#define EC_SZTXT_TXT_EEPROM_CHECKSUM_ERROR      "EEPROM checksum error - Slave  \"%s\": - EtherCAT address=%d"

#define       EC_TXT_JUNCTION_RED_CHANGE        ((EC_T_WORD)(EC_TEXTBASE+0x0067))
#define EC_SZTXT_TXT_JUNCTION_RED_CHANGE        "Junction redundancy change (Line break = %d) - Slave  \"%s\": - EtherCAT address=%d"

#define       EC_TEXTBASE_LAST                  ((EC_T_WORD)0x02FF)

/*-MACROS--------------------------------------------------------------------*/

#endif /* INC_ECERROR */


/*-END OF SOURCE FILE--------------------------------------------------------*/
