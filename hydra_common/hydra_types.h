#ifndef _HYDRA_TYPES_H_
#define _HYDRA_TYPES_H_

#ifndef INC_ECTYPE
#include "EcType.h"
#endif
#include "AtEthercat.h"

typedef union {
    struct {
        int    id;
        int    enabled;
        double pos_act;
        double vel_act;
        double tau_act;
        double pos_cmd;
        double vel_cmd;
        double tau_cmd;
    } DATA;
    unsigned char BYTE[56];
} joint_state_t;

typedef union
{
    struct {
        int    id;
        int    enable;
        double pos_ref;
        double vel_ref;
        double tau_ref;
    } DATA;
    unsigned char BYTE[32];
} joint_cmd_t;

typedef union
{
    struct {
        unsigned int   stsword;
        double         pos_act;
        unsigned int   rawpos_act;
        double         vel_act;
        double         tau_act;
        double         tau2_act;
        double         tau3_act; //currently Ko want to input temperature(ADC6)
    } DATA;
    unsigned char BYTE[48];
} eha_state_t;

typedef union
{
    struct {
        unsigned int   ctlword;
        double         pos_ref;
        unsigned int   rawpos_ref;
        double         vel_ref;
        double         tau_ref;
    } DATA;
    unsigned char BYTE[32];
} eha_cmd_t;

typedef struct
{
    unsigned int stsword;
    double wrench[6];
} ft_sensor_state_t;

typedef struct
{
    unsigned int stsword;
    double  gyro[3];
    double  acc[3];
    double  Quarternion[4];
    double  Omega_c[3];
    double  Accel[3];
    double  VelWorld[3];
} imu_sensor_state_t;

typedef union
{
    struct {
        ft_sensor_state_t  ft_sensor[4]; //(48+4)x4
        imu_sensor_state_t imu[1]; //(19x8+4)
    } DATA;
    unsigned char BYTE[384];
} sensor_state_t;

typedef struct
{
    unsigned int ctlword;
} ft_sensor_cmd_t;

typedef struct
{
    unsigned int ctlword;
} imu_sensor_cmd_t;

typedef union
{
    struct {
        ft_sensor_cmd_t  ft_sensor[4]; //(48+4)x4
        imu_sensor_cmd_t imu[1]; //(19x8+4)
    } DATA;
    unsigned char BYTE[136];
} sensor_cmd_t;

#define SLV_NUM_MD4KW_3M		16	
#define SLV_NUM_MD4KW_2MFS		2	
#define SLV_NUM_MD4KW_HAND		2	
#define SLV_NUM_MD4KW_IMU		1	
#define SLAVE_NOT_FOUND         0xFFFF

#define SLAVE_NUM_MAX			100

#ifndef SHM_IN_NAME
#define SHM_IN_NAME             ("/tmp/shm/HydraIn")
#endif

#ifndef SHM_OUT_NAME
#define SHM_OUT_NAME            ("/tmp/shm/HydraOut")
#endif

#ifndef SEM_ECATSYNC_NAME
#define SEM_ECATSYNC_NAME ("/HydraSync")
#endif

//#define SHM_ACCNUM_MAX  3
#define SHM_ACCNUM_MAX  100
#define SDO_PARAM_MAX           1000

#define EHA_MAX             47
#define HYDRA_JNT_MAX       41
#define HYDRA_HAND_JNT_MAX  26

// 共有メモリを経由したSDO送受信マクロ関数群
#define	SDOREQ(ReqNo)						pShmOut_MD4KW->SdoReq[ReqNo]
#define	SDOREQ_NO							pShmOut_MD4KW->SdoReqNo
#define	SDORES_NO							pShmIn_MD4KW->SdoResNo
#define SDONO_UPDATE(ReqNo)					ReqNo = (ReqNo+1) % SDO_PARAM_MAX;
#define	SDORES(ResNo)						pShmIn_MD4KW->SdoRes[ResNo]
#define	SDORES_RET(ResNo)					pShmIn_MD4KW->SdoResRet[ResNo]

// SDO要求時のヘッダ設定(共通)
#define SDOREQ_HDR(Type, SlvNo, Index, SubIndex, Size)	\
                                            { SDOREQ(SDOREQ_NO).Md4kwType = Type; \
                                              SDOREQ(SDOREQ_NO).SlaveNo   = SlvNo; \
                                              SDOREQ(SDOREQ_NO).ObIndex   = Index; \
                                              SDOREQ(SDOREQ_NO).ObSubIndex= SubIndex; \
                                              SDOREQ(SDOREQ_NO).DataSize  = Size; }

// SDO要求時のデータ設定（型毎にマクロを切り替える)
#define SHM_SDOREQ_SET_FLOAT(ReqNo, Type, SlvNo, Index, SubIndex, Size, value)	\
                                            { ReqNo = SDOREQ_NO; \
                                              SDOREQ(SDOREQ_NO).SetCmdFlg = true; \
                                              SDOREQ_HDR(Type, SlvNo, Index, SubIndex, Size); \
                                              memset(SDOREQ(SDOREQ_NO).Data.byte , 0, sizeof(SDOREQ(SDOREQ_NO).Data.byte )); \
                                              SDOREQ(SDOREQ_NO).Data.f = value; \
                                              SDONO_UPDATE(SDOREQ_NO); }

#define SHM_SDOREQ_SET_INT(ReqNo, Type, SlvNo, Index, SubIndex, Size, value)	\
                                            { ReqNo = SDOREQ_NO; \
                                              SDOREQ(SDOREQ_NO).SetCmdFlg = true; \
                                              SDOREQ_HDR(Type, SlvNo, Index, SubIndex, Size); \
                                              memset(SDOREQ(SDOREQ_NO).Data.byte , 0, sizeof(SDOREQ(SDOREQ_NO).Data.byte )); \
                                              if(SDOREQ(SDOREQ_NO).DataSize == 1) SDOREQ(SDOREQ_NO).Data.sc = value; \
                                              else if(SDOREQ(SDOREQ_NO).DataSize == 2) SDOREQ(SDOREQ_NO).Data.ss = value; \
                                              else SDOREQ(SDOREQ_NO).Data.si = value; \
                                              SDONO_UPDATE(SDOREQ_NO); }

#define SHM_SDOREQ_SET_UINT(ReqNo, Type, SlvNo, Index, SubIndex, Size, value)	\
                                            { ReqNo = SDOREQ_NO; \
                                              SDOREQ(SDOREQ_NO).SetCmdFlg = true; \
                                              SDOREQ_HDR(Type, SlvNo, Index, SubIndex, Size); \
                                              memset(SDOREQ(SDOREQ_NO).Data.byte , 0, sizeof(SDOREQ(SDOREQ_NO).Data.byte )); \
                                              if(SDOREQ(SDOREQ_NO).DataSize == 1) SDOREQ(SDOREQ_NO).Data.uc = value; \
                                              else if(SDOREQ(SDOREQ_NO).DataSize == 2) SDOREQ(SDOREQ_NO).Data.us = value; \
                                              else SDOREQ(SDOREQ_NO).Data.ui = value; \
                                              SDONO_UPDATE(SDOREQ_NO); }

// SDO要求時のヘッダ設定(取得コマンド用)
#define SHM_SDOREQ_GET(ReqNo, Type, SlvNo, Index, SubIndex, Size)	\
                                            { ReqNo = SDOREQ_NO; \
                                              SDOREQ(SDOREQ_NO).SetCmdFlg = false; \
                                              SDOREQ_HDR(Type, SlvNo, Index, SubIndex, Size); \
                                              memset(SDOREQ(SDOREQ_NO).Data.byte , 0, sizeof(SDOREQ(SDOREQ_NO).Data.byte )); \
                                              SDONO_UPDATE(SDOREQ_NO); }

#define ACCIN_MD4KW(AccIdx)   pShmIn_MD4KW->Acc[(pShmIn_MD4KW->Idx + AccIdx) % SHM_ACCNUM_MAX]
#define	ACCOUT_MD4KW(AccIdx)  pShmOut_MD4KW->Acc[(pShmOut_MD4KW->Idx + AccIdx) % SHM_ACCNUM_MAX]

// 共通
#define	SHM_MSEC(AccIdx) 		ACCIN_MD4KW(AccIdx).iMsec


// MD4KWドライバ用アクセス関数群
#define SHM_MD4KW_3M_INP(AccIdx, SlvIdx)             ACCIN_MD4KW(AccIdx).SlvMD4KW_3M[SlvIdx].MD4KW_3M.tInpOrg
#define SHM_MD4KW_2MFS_INP(AccIdx, SlvIdx)           ACCIN_MD4KW(AccIdx).SlvMD4KW_2MFS[SlvIdx].MD4KW_2MFS.tInpOrg
#define SHM_MD4KW_HAND_INP(AccIdx, SlvIdx)           ACCIN_MD4KW(AccIdx).SlvMD4KW_Hand[SlvIdx].MD4KW_HAND.tInpOrg
#define SHM_MD4KW_IMU_INP(AccIdx, SlvIdx)            ACCIN_MD4KW(AccIdx).SlvMD4KW_IMU[SlvIdx].MD4KW_IMU.tInpOrg

#define SHM_HYDRA_EHA_STATE_IN(AccIdx, JntIdx)       ACCIN_MD4KW(AccIdx).Actuators[(JntIdx)]
#define SHM_HYDRA_EHA_POS_IN(AccIdx, JntIdx)         ACCIN_MD4KW(AccIdx).Actuators[(JntIdx)].EHA_pos
#define SHM_HYDRA_EHA_RAWPOS_IN(AccIdx, JntIdx)         ACCIN_MD4KW(AccIdx).Actuators[(JntIdx)].EHA_rawpos
#define SHM_HYDRA_EHA_VEL_IN(AccIdx, JntIdx)         ACCIN_MD4KW(AccIdx).Actuators[(JntIdx)].EHA_vel
#define SHM_HYDRA_EHA_TAU_IN(AccIdx, JntIdx)         ACCIN_MD4KW(AccIdx).Actuators[(JntIdx)].EHA_tau
#define SHM_HYDRA_EHA_TAU2_IN(AccIdx, JntIdx)        ACCIN_MD4KW(AccIdx).Actuators[(JntIdx)].EHA_tau2
#define SHM_HYDRA_EHA_TAU3_IN(AccIdx, JntIdx)        ACCIN_MD4KW(AccIdx).Actuators[(JntIdx)].EHA_tau3
#define SHM_HYDRA_EHA_STATUS_IN(AccIdx, JntIdx)      ACCIN_MD4KW(AccIdx).Actuators[(JntIdx)].EHA_status

#define SHM_HYDRA_JOINT_POS_IN(AccIdx, JntIdx)       ACCIN_MD4KW(AccIdx).Joints[(JntIdx)].joint_pos
#define SHM_HYDRA_JOINT_VEL_IN(AccIdx, JntIdx)       ACCIN_MD4KW(AccIdx).Joints[(JntIdx)].joint_vel
#define SHM_HYDRA_JOINT_TAU_IN(AccIdx, JntIdx)       ACCIN_MD4KW(AccIdx).Joints[(JntIdx)].joint_tau
#define SHM_HYDRA_JOINT_STATUS_IN(AccIdx, JntIdx)    ACCIN_MD4KW(AccIdx).Joints[(JntIdx)].joint_status

#define SHM_HYDRA_HAND_POS_IN(AccIdx, JntIdx)       ACCIN_MD4KW(AccIdx).Hands[(JntIdx)].joint_pos
#define SHM_HYDRA_HAND_VEL_IN(AccIdx, JntIdx)       ACCIN_MD4KW(AccIdx).Hands[(JntIdx)].joint_vel

#define SHM_HYDRA_IMU_GYROX_IN(AccIdx)         ACCIN_MD4KW(AccIdx).IMU.gyro_X
#define SHM_HYDRA_IMU_GYROY_IN(AccIdx)         ACCIN_MD4KW(AccIdx).IMU.gyro_Y
#define SHM_HYDRA_IMU_GYROZ_IN(AccIdx)         ACCIN_MD4KW(AccIdx).IMU.gyro_Z
#define SHM_HYDRA_IMU_ACCX_IN(AccIdx)          ACCIN_MD4KW(AccIdx).IMU.acc_X
#define SHM_HYDRA_IMU_ACCY_IN(AccIdx)          ACCIN_MD4KW(AccIdx).IMU.acc_Y
#define SHM_HYDRA_IMU_ACCZ_IN(AccIdx)          ACCIN_MD4KW(AccIdx).IMU.acc_Z
#define SHM_HYDRA_IMU_QUART_IN(AccIdx, Idx)    ACCIN_MD4KW(AccIdx).IMU.Quarternion[(Idx)]
#define SHM_HYDRA_IMU_OMEGAC_IN(AccIdx, Idx)   ACCIN_MD4KW(AccIdx).IMU.Omega_c[(Idx)]
#define SHM_HYDRA_IMU_ACCEL_IN(AccIdx, Idx)    ACCIN_MD4KW(AccIdx).IMU.Accel[(Idx)]
#define SHM_HYDRA_IMU_VELWORLD_IN(AccIdx, Idx) ACCIN_MD4KW(AccIdx).IMU.VelWorld[(Idx)]

#define SHM_HYDRA_FS_FX_IN(AccIdx, Idx)        ACCIN_MD4KW(AccIdx).FS[Idx].F_X
#define SHM_HYDRA_FS_FY_IN(AccIdx, Idx)        ACCIN_MD4KW(AccIdx).FS[Idx].F_Y
#define SHM_HYDRA_FS_FZ_IN(AccIdx, Idx)        ACCIN_MD4KW(AccIdx).FS[Idx].F_Z
#define SHM_HYDRA_FS_MX_IN(AccIdx, Idx)        ACCIN_MD4KW(AccIdx).FS[Idx].M_X
#define SHM_HYDRA_FS_MY_IN(AccIdx, Idx)        ACCIN_MD4KW(AccIdx).FS[Idx].M_Y
#define SHM_HYDRA_FS_MZ_IN(AccIdx, Idx)        ACCIN_MD4KW(AccIdx).FS[Idx].M_Z
#define SHM_HYDRA_FS_FX_RAW_IN(AccIdx, Idx)    ACCIN_MD4KW(AccIdx).FS[Idx].F_X_raw
#define SHM_HYDRA_FS_FY_RAW_IN(AccIdx, Idx)    ACCIN_MD4KW(AccIdx).FS[Idx].F_Y_raw
#define SHM_HYDRA_FS_FZ_RAW_IN(AccIdx, Idx)    ACCIN_MD4KW(AccIdx).FS[Idx].F_Z_raw
#define SHM_HYDRA_FS_MX_RAW_IN(AccIdx, Idx)    ACCIN_MD4KW(AccIdx).FS[Idx].M_X_raw
#define SHM_HYDRA_FS_MY_RAW_IN(AccIdx, Idx)    ACCIN_MD4KW(AccIdx).FS[Idx].M_Y_raw
#define SHM_HYDRA_FS_MZ_RAW_IN(AccIdx, Idx)    ACCIN_MD4KW(AccIdx).FS[Idx].M_Z_raw

#define SHM_MD4KW_3M_OUT(AccIdx, SlvIdx)             ACCOUT_MD4KW(AccIdx).SlvMD4KW_3M[TM_OFFS+SlvIdx].MD4KW_3M.tOutOrg
#define SHM_MD4KW_2MFS_OUT(AccIdx, SlvIdx)           ACCOUT_MD4KW(AccIdx).SlvMD4KW_2MFS[TM_OFFS+SlvIdx].MD4KW_2MFS.tOutOrg
#define SHM_MD4KW_HAND_OUT(AccIdx, SlvIdx)           ACCOUT_MD4KW(AccIdx).SlvMD4KW_Hand[TM_OFFS+SlvIdx].MD4KW_HAND.tOutOrg
#define SHM_MD4KW_IMU_OUT(AccIdx, SlvIdx)            ACCOUT_MD4KW(AccIdx).SlvMD4KW_IMU[TM_OFFS+SlvIdx].MD4KW_IMU.tOutOrg

#define SHM_HYDRA_EHA_CTRLWORD_OUT(AccIdx, JntIdx)   ACCOUT_MD4KW(AccIdx).Actuators[(JntIdx)].EHA_ctrlword
#define SHM_HYDRA_ENABLE_OUT(AccIdx)                 ACCOUT_MD4KW(AccIdx).EnableAll
#define SHM_HYDRA_JOINT_CTRLMODE_OUT(AccIdx, JntIdx)   ACCOUT_MD4KW(AccIdx).Joints[(JntIdx)].ctrl_mode
#define SHM_HYDRA_JOINT_REFPOS_OUT(AccIdx, JntIdx)   ACCOUT_MD4KW(AccIdx).Joints[(JntIdx)].joint_pos
#define SHM_HYDRA_JOINT_REFVEL_OUT(AccIdx, JntIdx)   ACCOUT_MD4KW(AccIdx).Joints[(JntIdx)].joint_vel
#define SHM_HYDRA_JOINT_REFTAU_OUT(AccIdx, JntIdx)   ACCOUT_MD4KW(AccIdx).Joints[(JntIdx)].joint_tau
#define SHM_HYDRA_FS_CTRLWORD_OUT(AccIdx, SlvIdx)   ACCOUT_MD4KW(AccIdx).FS[(SlvIdx)].ctrlword
#define SHM_HYDRA_IMU_CTRLWORD_OUT(AccIdx, SlvIdx)   ACCOUT_MD4KW(AccIdx).IMU[(SlvIdx)].ctrlword

/*-PACK SETTINGS-------------------------------------------------------------*/
#if defined  __GNUC__   /* GNU */

#define EC_PACKED __attribute__((aligned(1), packed))

#if (defined __RCX__)
#pragma pack (1)
#endif

#elif defined _DIAB_TOOL

#define EC_PACKED __attribute__((aligned(1), packed))

#elif defined _MSC_VER  /* MICROSOFT */

#define EC_PACKED
#pragma pack (push, 1)
#pragma warning(disable:4514)

#elif defined __MET__

#define EC_PACKED
#pragma Push_align_members(1)

#elif defined __ghs

#define EC_PACKED __attribute__((aligned(1), packed))
#pragma pack (1)

#else
#error Please check pack settings for this compiler!!!
#endif

typedef struct _T_MD4KW_3M_RXPDO_BASIC {
    EC_T_WORD  Axis0_CtrlWordOut; // 0x7000
    EC_T_DWORD Axis0_PosRefOut;   // 0x7001
    EC_T_WORD  Axis0_VelRefOut;   // 0x7002
    EC_T_WORD  Axis0_TauRefOut;   // 0x7003
    
    EC_T_WORD  Axis1_CtrlWordOut; // 0x7100
    EC_T_DWORD Axis1_PosRefOut;   // 0x7101
    EC_T_WORD  Axis1_VelRefOut;   // 0x7102
    EC_T_WORD  Axis1_TauRefOut;   // 0x7103

    EC_T_WORD  Axis2_CtrlWordOut; // 0x7200
    EC_T_DWORD Axis2_PosRefOut;   // 0x7201
    EC_T_WORD  Axis2_VelRefOut;   // 0x7202
    EC_T_WORD  Axis2_TauRefOut;   // 0x7203

} EC_PACKED T_MD4KW_3M_RXPDO_BASIC;

typedef struct _T_MD4KW_2MFS_RXPDO_BASIC {
    EC_T_WORD  Axis0_CtrlWordOut; // 0x7000
    EC_T_DWORD Axis0_PosRefOut;   // 0x7001
    EC_T_WORD  Axis0_VelRefOut;   // 0x7002
    EC_T_WORD  Axis0_TauRefOut;   // 0x7003

    EC_T_WORD  Axis1_CtrlWordOut; // 0x7100
    EC_T_DWORD Axis1_PosRefOut;   // 0x7101
    EC_T_WORD  Axis1_VelRefOut;   // 0x7102
    EC_T_WORD  Axis1_TauRefOut;   // 0x7103

  EC_T_WORD FS_CtrlWordOut; //0x7600

} EC_PACKED T_MD4KW_2MFS_RXPDO_BASIC;

typedef struct _T_MD4KW_HAND_RXPDO_BASIC {
    EC_T_WORD  Axis0_CtrlWordOut; // 0x7000
    EC_T_DWORD Axis0_PosRefOut;   // 0x7001
    EC_T_WORD  Axis0_VelRefOut;   // 0x7002
    EC_T_WORD  Axis0_TauRefOut;   // 0x7003
    
    EC_T_WORD  Axis1_CtrlWordOut; // 0x7100
    EC_T_DWORD Axis1_PosRefOut;   // 0x7101
    EC_T_WORD  Axis1_VelRefOut;   // 0x7102
    EC_T_WORD  Axis1_TauRefOut;   // 0x7103
    
    EC_T_WORD  Axis2_CtrlWordOut; // 0x7200
    EC_T_DWORD Axis2_PosRefOut;   // 0x7201
    EC_T_WORD  Axis2_VelRefOut;   // 0x7202
    EC_T_WORD  Axis2_TauRefOut;   // 0x7203

    EC_T_WORD  Axis3_CtrlWordOut; // 0x7300
    EC_T_DWORD Axis3_PosRefOut;   // 0x7301
    EC_T_WORD  Axis3_VelRefOut;   // 0x7302
    EC_T_WORD  Axis3_TauRefOut;   // 0x7303
    
    EC_T_WORD  Axis4_CtrlWordOut; // 0x7400
    EC_T_DWORD Axis4_PosRefOut;   // 0x7401
    EC_T_WORD  Axis4_VelRefOut;   // 0x7402
    EC_T_WORD  Axis4_TauRefOut;   // 0x7403
    
} EC_PACKED T_MD4KW_HAND_RXPDO_BASIC;

typedef struct _T_MD4KW_IMU_RXPDO_BASIC {
    EC_T_WORD CtrlWordOut; // 0x7000
} EC_PACKED T_MD4KW_IMU_RXPDO_BASIC;

typedef struct _T_MD4KW_3M_TXPDO_BASIC {
    EC_T_WORD  Axis0_StsWrdActIn;// 0x6000
    EC_T_DWORD Axis0_PosActIn;  // 0x6001
    EC_T_WORD  Axis0_VelActIn;  // 0x6002
    EC_T_WORD  Axis0_TauActIn;  // 0x6003
    EC_T_WORD  Axis0_Vel2ActIn; // 0x6005
    EC_T_WORD  Axis0_Tau2ActIn; // 0x6006
    EC_T_WORD  Axis0_Tau3ActIn; // 0x6007

    EC_T_WORD  Axis1_StsWrdActIn;// 0x6100
    EC_T_DWORD Axis1_PosActIn;  // 0x6101
    EC_T_WORD  Axis1_VelActIn;  // 0x6102
    EC_T_WORD  Axis1_TauActIn;  // 0x6103
    EC_T_WORD  Axis1_Vel2ActIn; // 0x6105
    EC_T_WORD  Axis1_Tau2ActIn; // 0x6106
    EC_T_WORD  Axis1_Tau3ActIn; // 0x6107

    EC_T_WORD  Axis2_StsWrdActIn;// 0x6200
    EC_T_DWORD Axis2_PosActIn;  // 0x6201
    EC_T_WORD  Axis2_VelActIn;  // 0x6202
    EC_T_WORD  Axis2_TauActIn;  // 0x6203
    EC_T_WORD  Axis2_Vel2ActIn; // 0x6205
    EC_T_WORD  Axis2_Tau2ActIn; // 0x6206
    EC_T_WORD  Axis2_Tau3ActIn; // 0x6207

  EC_T_WORD Tick;

} EC_PACKED T_MD4KW_3M_TXPDO_BASIC;

typedef struct _T_MD4KW_2MFS_TXPDO_BASIC
{
    EC_T_WORD  Axis0_StsWrdActIn;// 0x6000
    EC_T_DWORD Axis0_PosActIn;  // 0x6001
    EC_T_WORD  Axis0_VelActIn;  // 0x6002
    EC_T_WORD  Axis0_TauActIn;  // 0x6003
    EC_T_WORD  Axis0_Vel2ActIn; // 0x6005
    EC_T_WORD  Axis0_Tau2ActIn; // 0x6006
    EC_T_WORD  Axis0_Tau3ActIn; // 0x6007
    
    EC_T_WORD  Axis1_StsWrdActIn;// 0x6100
    EC_T_DWORD Axis1_PosActIn;  // 0x6101
    EC_T_WORD  Axis1_VelActIn;  // 0x6102
    EC_T_WORD  Axis1_TauActIn;  // 0x6103
    EC_T_WORD  Axis1_Vel2ActIn; // 0x6105
    EC_T_WORD  Axis1_Tau2ActIn; // 0x6106
    EC_T_WORD  Axis1_Tau3ActIn; // 0x6107

    EC_T_WORD StsActFs;// 0x6600
    EC_T_WORD FS_Fx; // 0x6601
    EC_T_WORD FS_Fy; // 0x6602
    EC_T_WORD FS_Fz; // 0x6603
    EC_T_WORD FS_Mx; // 0x6604
    EC_T_WORD FS_My; // 0x6605
    EC_T_WORD FS_Mz; // 0x6606

  EC_T_WORD Tick;

} EC_PACKED T_MD4KW_2MFS_TXPDO_BASIC;

typedef struct _T_MD4KW_HAND_TXPDO_BASIC {
    EC_T_WORD  Axis0_StsWrdActIn;	//obj6000_StatusWord
    EC_T_DWORD Axis0_PosActIn;      //obj6001_PosActIn
    EC_T_WORD  Axis0_TauActIn;      //obj6003_TauActIn
    EC_T_WORD  Axis0_Vel2ActIn;     //obj6005_Vel2ActIn

    EC_T_WORD  Axis1_StsWrdActIn;	//obj6100_StatusWord
    EC_T_DWORD Axis1_PosActIn;      //obj6101_PosActIn
    EC_T_WORD  Axis1_TauActIn;      //obj6103_TauActIn
    EC_T_WORD  Axis1_Vel2ActIn;     //obj6105_Vel2ActIn

    EC_T_WORD  Axis2_StsWrdActIn;	//obj6200_StatusWord
    EC_T_DWORD Axis2_PosActIn;      //obj6201_PosActIn
	EC_T_WORD  Axis2_TauActIn;      //obj6203_TauActIn
    EC_T_WORD  Axis2_Vel2ActIn;     //obj6205_Vel2ActIn

    EC_T_WORD  Axis3_StsWrdActIn;	//obj6300_StatusWord
    EC_T_DWORD Axis3_PosActIn;      //obj6301_PosActIn
    EC_T_WORD  Axis3_TauActIn;      //obj6303_TauActIn
    EC_T_WORD  Axis3_Vel2ActIn;     //obj6305_Vel2ActIn

    EC_T_WORD  Axis4_StsWrdActIn;	//obj6400_StatusWord
    EC_T_DWORD Axis4_PosActIn;      //obj6401_PosActIn
    EC_T_WORD  Axis4_TauActIn;      //obj6403_TauActIn
    EC_T_WORD  Axis4_Vel2ActIn;     //obj6405_Vel2ActIn

	EC_T_WORD Tick01msIn;    //obj6501_Tick01msIn

	EC_T_WORD ThumbMP_AAIn;  //obj6510_encIn
    EC_T_WORD ThumbMP_FEIn;  //obj6511_encIn
    EC_T_WORD ThumbIPIn;     //obj6512_encIn
    EC_T_WORD ThumbDIPIn;    //obj6513_encIn

	EC_T_WORD IndexMP_AAIn;  //obj6514_encIn
    EC_T_WORD IndexMP_FEIn;  //obj6515_encIn
    EC_T_WORD IndexPIPIn;    //obj6516_encIn
    EC_T_WORD IndexDIPIn;    //obj6517_encIn

	EC_T_WORD MiddleMP_AAIn; //obj6518_encIn
    EC_T_WORD MiddleMP_FEIn; //obj6519_encIn
    EC_T_WORD MiddlePIPIn;   //obj651a_encIn
    EC_T_WORD MiddleDIPIn;   //obj651b_encIn

	EC_T_WORD RingMP_AAIn;   //obj651c_encIn
    EC_T_WORD RingMP_FEIn;   //obj651d_encIn
    EC_T_WORD RingPIPIn;     //obj651e_encIn
    EC_T_WORD RingDIPIn;     //obj651f_encIn

} EC_PACKED T_MD4KW_HAND_TXPDO_BASIC;

typedef struct _T_MD4KW_IMU_TXPDO_BASIC {
    EC_T_WORD	StsWrdActIn;	// obj6000_StatusWord
	EC_T_DWORD  GyroXIn;        // obj6701_GyroXIn
	EC_T_DWORD  GyroYIn;        // obj6703_GyroYIn
	EC_T_DWORD  GyroZIn;        // obj6705_GyroZIn
    EC_T_WORD   AccXIn;         // obj6711_AccXIn
    EC_T_WORD   AccYIn;         // obj6712_AccYIn
    EC_T_WORD   AccZIn;         // obj6713_AccZIn
    EC_T_WORD   Tick01msIn;     // obj6501_Tick01msIn
	EC_T_DWORD  Quarternion0In; // obj6801_Quarternion0In
	EC_T_DWORD  Quarternion1In; // obj6802_Quarternion1In
	EC_T_DWORD  Quarternion2In; // obj6803_Quarternion2In
	EC_T_DWORD  Quarternion3In; // obj6804_Quarternion3In
	EC_T_DWORD  Omega_c0In;     // obj6805_Omega_c0In
	EC_T_DWORD  Omega_c1In;     // obj6806_Omega_c1In
	EC_T_DWORD  Omega_c2In;     // obj6807_Omega_c2In
	EC_T_DWORD  Accel0In;       // obj6808_Accel0In
	EC_T_DWORD  Accel1In;       // obj6809_Accel1In
	EC_T_DWORD  Accel2In;       // obj680a_Accel2In
	EC_T_DWORD  VelWorld0In;    // obj680b_VelWorld0In
	EC_T_DWORD  VelWorld1In;    // obj680c_VelWorld1In
	EC_T_DWORD  VelWorld2In;    // obj680d_VelWorld2In
} EC_PACKED T_MD4KW_IMU_TXPDO_BASIC;

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

#else
#endif

typedef struct _T_MD4KW_3M_PDODATA
{
    T_MD4KW_3M_TXPDO_BASIC		tInp;
    T_MD4KW_3M_RXPDO_BASIC		tOut;
}T_MD4KW_3M_PDODATA;

typedef struct _T_MD4KW_2MFS_PDODATA
{
    T_MD4KW_2MFS_TXPDO_BASIC		tInp;
    T_MD4KW_2MFS_RXPDO_BASIC		tOut;
}T_MD4KW_2MFS_PDODATA;

typedef struct _T_MD4KW_HAND_PDODATA
{
    T_MD4KW_HAND_TXPDO_BASIC		tInp;
    T_MD4KW_HAND_RXPDO_BASIC		tOut;
}T_MD4KW_HAND_PDODATA;

typedef struct _T_MD4KW_IMU_PDODATA
{
    T_MD4KW_IMU_TXPDO_BASIC		tInp;
    T_MD4KW_IMU_RXPDO_BASIC		tOut;
}T_MD4KW_IMU_PDODATA;

typedef struct _T_MD4KW_3M_INFO
{
    EC_T_GET_SLAVE_INFO		Info;
    T_MD4KW_3M_PDODATA		Pdo;			// 最新のPDOを格納しておく
}T_MD4KW_3M_INFO;

typedef struct _T_MD4KW_2MFS_INFO
{
    EC_T_GET_SLAVE_INFO		Info;
    T_MD4KW_2MFS_PDODATA	Pdo;			// 最新のPDOを格納しておく
}T_MD4KW_2MFS_INFO;

typedef struct _T_MD4KW_HAND_INFO
{
    EC_T_GET_SLAVE_INFO		Info;
    T_MD4KW_HAND_PDODATA	Pdo;			// 最新のPDOを格納しておく
}T_MD4KW_HAND_INFO;

typedef struct _T_MD4KW_IMU_INFO
{
    EC_T_GET_SLAVE_INFO		Info;
    T_MD4KW_IMU_PDODATA		Pdo;			// 最新のPDOを格納しておく
}T_MD4KW_IMU_INFO;

typedef struct _T_ALL_SLAVE_INFO {
    /* YNL Motor Driver Data */
    EC_T_DWORD MD4KW_3MSlaves;
    T_MD4KW_3M_INFO MD4KW_3M[SLV_NUM_MD4KW_3M];

    EC_T_DWORD MD4KW_2MFSSlaves;
    T_MD4KW_2MFS_INFO MD4KW_2MFS[SLV_NUM_MD4KW_2MFS];

    EC_T_DWORD MD4KW_HandSlaves;
    T_MD4KW_HAND_INFO MD4KW_Hand[SLV_NUM_MD4KW_HAND];

    EC_T_DWORD MD4KW_IMUSlaves;
    T_MD4KW_IMU_INFO MD4KW_IMU[SLV_NUM_MD4KW_IMU];

}T_ALL_SLAVE_INFO;

union U_SHM_INPUT_ACC_MD4KW_3M_SLV 
{
    struct {
        T_MD4KW_3M_TXPDO_BASIC		tInpOrg;
    } MD4KW_3M;
};

typedef struct _S_SHM_INPUT_ACC_EHA_STATE
{
  EC_T_REAL  EHA_pos;
  EC_T_DWORD EHA_rawpos;
  EC_T_REAL  EHA_vel;
  EC_T_REAL  EHA_tau;
  EC_T_REAL  EHA_tau2;
  EC_T_REAL  EHA_tau3;
  EC_T_BYTE  EHA_status;
} S_SHM_INPUT_ACC_EHA_STATE;

typedef struct _S_SHM_INPUT_ACC_JOINT_STATE
{
    EC_T_REAL  joint_pos;
    EC_T_REAL  joint_vel;
    EC_T_REAL  joint_tau;
    EC_T_BYTE  joint_status;
} S_SHM_INPUT_ACC_JOINT_STATE;

typedef struct _S_SHM_INPUT_ACC_HAND_STATE
{
    EC_T_REAL  joint_pos;
    EC_T_REAL  joint_vel;
} S_SHM_INPUT_ACC_HAND_STATE;

typedef struct _S_SHM_INPUT_ACC_IMU_STATE
{
    double  gyro_X;
    double  gyro_Y;
    double  gyro_Z;
    double  acc_X;
    double  acc_Y;
    double  acc_Z;
	double  Quarternion[4];
	double  Omega_c[3];
	double  Accel[3];
	double  VelWorld[3];
} S_SHM_INPUT_ACC_IMU_STATE;

typedef struct _S_SHM_INPUT_ACC_FS_STATE
{
    double  F_X;
    double  F_Y;
    double  F_Z;
    double  M_X;
    double  M_Y;
    double  M_Z;
    EC_T_WORD F_X_raw;
    EC_T_WORD F_Y_raw;
    EC_T_WORD F_Z_raw;
    EC_T_WORD M_X_raw;
    EC_T_WORD M_Y_raw;
    EC_T_WORD M_Z_raw;
} S_SHM_INPUT_ACC_FS_STATE;

union U_SHM_INPUT_ACC_MD4KW_2MFS_SLV 
{
    struct {
        T_MD4KW_2MFS_TXPDO_BASIC		tInpOrg;
    } MD4KW_2MFS;
};
union U_SHM_INPUT_ACC_MD4KW_HAND_SLV 
{
    struct {
        T_MD4KW_HAND_TXPDO_BASIC		tInpOrg;
    } MD4KW_HAND;
};
union U_SHM_INPUT_ACC_MD4KW_IMU_SLV 
{
    struct {
        T_MD4KW_IMU_TXPDO_BASIC		tInpOrg;
    } MD4KW_IMU;
};

struct T_SHM_INPUT_ACC_MD4KW
{
    EC_T_INT				        iMsec;
    U_SHM_INPUT_ACC_MD4KW_3M_SLV	SlvMD4KW_3M[64];
    U_SHM_INPUT_ACC_MD4KW_2MFS_SLV	SlvMD4KW_2MFS[2];//SLAVE_NUM_MAX
    U_SHM_INPUT_ACC_MD4KW_HAND_SLV	SlvMD4KW_Hand[2];
    U_SHM_INPUT_ACC_MD4KW_IMU_SLV	SlvMD4KW_IMU[1];
    S_SHM_INPUT_ACC_EHA_STATE       Actuators[64];
    S_SHM_INPUT_ACC_JOINT_STATE     Joints[64];
    S_SHM_INPUT_ACC_HAND_STATE      Hands[26];
    S_SHM_INPUT_ACC_IMU_STATE       IMU;
	S_SHM_INPUT_ACC_FS_STATE        FS[2];
};

union U_SET_VALUE
{
	unsigned  char      byte[4];
	signed    char      sc;
	unsigned  char      uc;
	signed    short     ss;
	unsigned  short     us;
	signed    int       si;
	unsigned  int       ui;
	float               f;
};

// 各種パラメータ構造体
struct T_SDO_PARAM {
	bool            SetCmdFlg;
	unsigned int	Md4kwType;
	unsigned int    SlaveNo;
	unsigned short  ObIndex;
	unsigned char   ObSubIndex;
	unsigned int    DataSize;
	U_SET_VALUE     Data;
};

// Slaveから取得した全共有データ
struct T_SHM_INPUT_MD4KW
{
	// SDO Result
	EC_T_INT					SdoResNo;
	T_SDO_PARAM					SdoRes[SDO_PARAM_MAX];
	EC_T_DWORD					SdoResRet[SDO_PARAM_MAX];

    EC_T_DWORD					Idx;			// 最新PDOのアクセスINDEX

    // 常時PDO通信により更新されるデータは、以下で3面管理
    T_SHM_INPUT_ACC_MD4KW		Acc[SHM_ACCNUM_MAX];
};

union U_SHM_OUTPUT_ACC_MD4KW_3M_SLV {
    struct {
        T_MD4KW_3M_RXPDO_BASIC		tOutOrg;
    } MD4KW_3M;
};
union U_SHM_OUTPUT_ACC_MD4KW_2MFS_SLV {
    struct {
        T_MD4KW_2MFS_RXPDO_BASIC        tOutOrg;
    } MD4KW_2MFS;
};
union U_SHM_OUTPUT_ACC_MD4KW_HAND_SLV {
    struct {
        T_MD4KW_HAND_RXPDO_BASIC	tOutOrg;
    } MD4KW_HAND;
};
union U_SHM_OUTPUT_ACC_MD4KW_IMU_SLV {
    struct {
        T_MD4KW_IMU_RXPDO_BASIC		tOutOrg;
    } MD4KW_IMU;
};

typedef struct _S_SHM_OUTPUT_ACC_EHA_STATE
{
    EC_T_DWORD EHA_pos;
    EC_T_WORD  EHA_vel;
    EC_T_WORD  EHA_tau;
    EC_T_WORD  EHA_tau_in;
    EC_T_WORD  EHA_tau_motor;
    EC_T_WORD  EHA_ctrlword;
} S_SHM_OUTPUT_ACC_EHA_STATE;

typedef struct _S_SHM_OUTPUT_ACC_JOINT_STATE
{
	int     ctrl_mode;
    double  joint_pos;
    double  joint_vel;
    double  joint_tau;
} S_SHM_OUTPUT_ACC_JOINT_STATE;

typedef struct _S_SHM_OUTPUT_ACC_FS_STATE
{
  EC_T_WORD  ctrlword;
} S_SHM_OUTPUT_ACC_FS_STATE;

typedef struct _S_SHM_OUTPUT_ACC_IMU_STATE
{
  EC_T_WORD  ctrlword;
} S_SHM_OUTPUT_ACC_IMU_STATE;

struct T_SHM_OUTPUT_ACC_MD4KW {
  U_SHM_OUTPUT_ACC_MD4KW_3M_SLV    SlvMD4KW_3M[SLAVE_NUM_MAX];
  U_SHM_OUTPUT_ACC_MD4KW_2MFS_SLV  SlvMD4KW_2MFS[SLAVE_NUM_MAX];
  U_SHM_OUTPUT_ACC_MD4KW_HAND_SLV  SlvMD4KW_Hand[SLAVE_NUM_MAX];
  U_SHM_OUTPUT_ACC_MD4KW_IMU_SLV   SlvMD4KW_IMU[SLAVE_NUM_MAX];
  S_SHM_OUTPUT_ACC_EHA_STATE       Actuators[47];
  S_SHM_OUTPUT_ACC_JOINT_STATE     Joints[41];
  S_SHM_OUTPUT_ACC_FS_STATE        FS[2];
  S_SHM_OUTPUT_ACC_IMU_STATE       IMU[1];
};

// Slaveに送信する指令値の全共有データ
struct T_SHM_OUTPUT_MD4KW
{
	// SDO Request
	EC_T_INT						SdoReqNo;
	T_SDO_PARAM						SdoReq[SDO_PARAM_MAX];

    EC_T_DWORD 						Idx;			// 最新PDOのアクセスINDEX

    // 常時PDO通信により更新されるデータは、以下で3面管理
    T_SHM_OUTPUT_ACC_MD4KW			Acc[SHM_ACCNUM_MAX];
};
#if 0
enum {
    joint_rhip_yaw        = 0,
    joint_rhip_pitch      = 1,
    joint_rhip_roll       = 2,
    joint_rknee_pitch     = 3,
    joint_rankle_roll     = 4,
    joint_rankle_pitch    = 5,
    joint_lhip_yaw        = 6,
    joint_lhip_pitch      = 7,
    joint_lhip_roll       = 8,
    joint_lknee_pitch     = 9,
    joint_lankle_roll     = 10,
    joint_lankle_pitch    = 11,
    joint_waist_pitch     = 12,
    joint_waist_roll      = 13,
    joint_neck_pitch      = 14,
    joint_rscaplae_yaw    = 15,
    joint_rshoulder_pitch = 16,
    joint_rshoulder_roll  = 17,
    joint_rshoulder_yaw   = 18,
    joint_relbow_pitch    = 19,
    joint_rwrist_yaw      = 20,
    joint_rwrist_roll     = 21,
    joint_rwrist_pitch    = 22,
    joint_lscaplae_yaw    = 23,
    joint_lshoulder_pitch = 24,
    joint_lshoulder_roll  = 25,
    joint_lshoulder_yaw   = 26,
    joint_lelbow_pitch    = 27,
    joint_lwrist_yaw      = 28,
    joint_lwrist_roll     = 29,
    joint_lwrist_pitch    = 30,
    joint_rhand_thb_AA    = 31,
    joint_rhand_thb_FE    = 32,
    joint_rhand_idx       = 33,
    joint_rhand_mid       = 34,
    joint_rhand_rng       = 35,
    joint_lhand_thb_AA    = 36,
    joint_lhand_thb_FE    = 37,
    joint_lhand_idx       = 38,
    joint_lhand_mid       = 39,
    joint_lhand_rng       = 40
};
#endif

enum {
	EHA_rhip_vane,
	EHA_rhip_cyl1,
	EHA_rhip_cyl2,
	EHA_rknee_single,
	EHA_rknee_tandem1,
	EHA_rknee_tandem2,
	EHA_rankle_cyl1,
	EHA_rankle_cyl2,
	EHA_lhip_vane,
	EHA_lhip_cyl1,
	EHA_lhip_cyl2,
	EHA_lknee_single,
	EHA_lknee_tandem1,
	EHA_lknee_tandem2,
	EHA_lankle_cyl1,
	EHA_lankle_cyl2,
	EHA_waist_cyl1,
	EHA_waist_cyl2,
	EHA_neck,
	EHA_rshoulder_vane,
	EHA_rshoulder_cyl1,
	EHA_rshoulder_cyl2,
	EHA_relbow_vane,
	EHA_relbow_tandem1,
	EHA_relbow_tandem2,
	EHA_rwrist_vane,
	EHA_rwrist_cyl1,
	EHA_rwrist_cyl2,
	EHA_lshoulder_vane,
	EHA_lshoulder_cyl1,
	EHA_lshoulder_cyl2,
	EHA_lelbow_vane,
	EHA_lelbow_tandem1,
	EHA_lelbow_tandem2,
	EHA_lwrist_vane,
	EHA_lwrist_cyl1,
	EHA_lwrist_cyl2,
	EHA_rhand_thb_AA,
	EHA_rhand_thb_FE,
	EHA_rhand_idx,
	EHA_rhand_mid,
	EHA_rhand_rng,
    EHA_lhand_thb_AA,
	EHA_lhand_thb_FE,
	EHA_lhand_idx,
	EHA_lhand_mid,
	EHA_lhand_rng
};

#endif
