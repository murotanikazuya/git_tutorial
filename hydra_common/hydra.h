#ifndef __HYDRA_H_
#define __HYDRA_H_

#include "EcType.h"

typedef enum _T_eSetupParam {
	SETPARAM_BDTYPE = 0,
	SETPARAM_SLVNO,
	SETPARAM_JOINT,
	SETPARAM_SLVID,
	SETPARAM_INDEX,
	SETPARAM_SIZE,
	SETPARAM_VALUE,

	SETPARAM_MAX
} T_eSetupParam;

#define BDTYPE_MD4KW_3M 	0
#define BDTYPE_MD4KW_2MFS 	1
#define BDTYPE_MD4KW_HAND 	2
#define BDTYPE_MD4KW_IMU 	3

#define EHA_MAX             47
#define HYDRA_JNT_MAX       41
#define EHA_POS_MAX         0.025
#define EHA_POS_MIN         -0.025

#define EHA_CTRLWD_RECT         0x0101
#define EHA_CTRLWD_POS          0x0201
#define EHA_CTRLWD_FORCE_PRES   0x0211
#define EHA_CTRLWD_FORCE_STRAIN 0x0221
#define EHA_CTRLWD_FORCE_COMPLE 0x0231
#define EHA_CTRLWD_CALIB        0x0301

typedef union _BytedFloat_t
{
	float      SHRT_REAL;
	EC_T_DWORD DWORD;
	EC_T_WORD  WORD[2];
	EC_T_BYTE  BYTE[4];
} BytedFloat_t;

enum {
	EHA_NO_ERR,
	ENC_READ_ERR=1,
	MOTOR_ERR=2,
	COMM_ERR=4
};

/****************** actuator numbering ********************/
const unsigned char joint_num_hydra_MD4KW_3M[][3] = {
	{0, 1, 2},    //rhip
	{3, 4, 5},    //rknee
	{8, 9, 10},   //lhip
	{11, 12, 13}, //lknee
	{16, 17, 18}, //body
	{19, 20, 21}, //rshoulder
	{22, 23, 24}, //relbow
	{25, 26, 27}, //rwrist
	{28, 29, 30}, //lshoulder
	{31, 32, 33}, //lelbow
	{34, 35, 36} //lwrist

};

const unsigned char joint_num_hydra_MD4KW_2MFS[][2] = {
	{6, 7}, //rankle
	{14, 15} //lankle
};

const unsigned char joint_num_hydra_MD4KW_HAND[][5] = {
	{37, 38, 39, 40, 41}, //rhand
	{42, 43, 44, 45, 46}  //lhand
};

const unsigned char joint_rev_num_hydra[][3] = {
	{BDTYPE_MD4KW_3M,0,0},   // rhip0
	{BDTYPE_MD4KW_3M,0,1},   // rhip1
	{BDTYPE_MD4KW_3M,0,2},   // rhip2
	{BDTYPE_MD4KW_3M,1,0},   // rknee0
	{BDTYPE_MD4KW_3M,1,1},   // rknee1
	{BDTYPE_MD4KW_3M,1,2},   // rknee2
	{BDTYPE_MD4KW_2MFS,0,0}, // rankle0
	{BDTYPE_MD4KW_2MFS,0,1}, // rankle1
	{BDTYPE_MD4KW_3M,2,0},   // lhip0
	{BDTYPE_MD4KW_3M,2,1},   // lhip1
	{BDTYPE_MD4KW_3M,2,2},   // lhip2
	{BDTYPE_MD4KW_3M,3,0},   // lknee0
	{BDTYPE_MD4KW_3M,3,1},   // lknee1
	{BDTYPE_MD4KW_3M,3,2},   // lknee2
	{BDTYPE_MD4KW_2MFS,1,0}, // lankle0
	{BDTYPE_MD4KW_2MFS,1,1}, // lankle1
	{BDTYPE_MD4KW_3M,4,0},   // body0
	{BDTYPE_MD4KW_3M,4,1},   // body1
	{BDTYPE_MD4KW_3M,4,2},   // neck
	{BDTYPE_MD4KW_3M,5,0},   // rshoulder0
	{BDTYPE_MD4KW_3M,5,1},   // rshoulder1
	{BDTYPE_MD4KW_3M,5,2},   // rshoulder2
	{BDTYPE_MD4KW_3M,6,0},   // relbow0
	{BDTYPE_MD4KW_3M,6,1},   // relbow0
	{BDTYPE_MD4KW_3M,6,2},   // relbow0
	{BDTYPE_MD4KW_3M,7,0},   // rwrist0
	{BDTYPE_MD4KW_3M,7,1},   // rwrist1
	{BDTYPE_MD4KW_3M,7,2},   // rwrist2
	{BDTYPE_MD4KW_3M,8,0},   // lshoulder0
	{BDTYPE_MD4KW_3M,8,1},   // lshoulder1
	{BDTYPE_MD4KW_3M,8,2},   // lshoulder2
	{BDTYPE_MD4KW_3M,9,0},   // lelbow0
	{BDTYPE_MD4KW_3M,9,1},   // lelbow0
	{BDTYPE_MD4KW_3M,9,2},   // lelbow0
	{BDTYPE_MD4KW_3M,10,0},   // lwrist0
	{BDTYPE_MD4KW_3M,10,1},   // lwrist1
	{BDTYPE_MD4KW_3M,10,2},   // lwrist2
	{BDTYPE_MD4KW_HAND,0,0},  // rhand0
	{BDTYPE_MD4KW_HAND,0,1},  // rhand1
	{BDTYPE_MD4KW_HAND,0,2},  // rhand2
	{BDTYPE_MD4KW_HAND,0,3},  // rhand3
	{BDTYPE_MD4KW_HAND,0,4},  // rhand4
	{BDTYPE_MD4KW_HAND,1,0},  // lhand0
	{BDTYPE_MD4KW_HAND,1,1},  // lhand1
	{BDTYPE_MD4KW_HAND,1,2},  // lhand2
	{BDTYPE_MD4KW_HAND,1,3},  // lhand3
	{BDTYPE_MD4KW_HAND,1,4}   // lhand4
};

/****************** actuator limits ********************/
#define LIM_U_RHIP0  0xffffffff//0x025E1AF2
#define LIM_U_RHIP1  0x00b3c075//0x00b3c4f2//0x00b2c12f//0x00AE84AF
#define LIM_U_RHIP2  0x00ac29d1//0x00ac4825//0x00ad5470//0x00AE5E0C
#define LIM_U_RKNEE0 0x00bbe8df//0x00b86d17//0x00b9538a//0x00B9F780
#define LIM_U_RKNEE1 0x00ba43fe//0x00b6d91a//0x00b6b405//0x00B60CA9
#define LIM_U_LHIP0  0xffffffff//0x035B0410//0x035b7930
#define LIM_U_LHIP1  0x00b7cb00//0x00b76df0//0x00B5CF31
#define LIM_U_LHIP2  0x00b40e10 //<--20161005 //0x00b038ff//0x00b0b601//0x00B43C9B
#define LIM_U_LKNEE0 0x00bbcf1d//0x00b97b3b//0x00baa389//0x00BB3CD2
#define LIM_U_LKNEE1 0x00aaee7c//0x00aa8517//0x00ab363b//0x00AA5333
#define LIM_U_BODY0  0x00b6d29a//0x00c52b40//0x00b874ac//0x00b452fa
#define LIM_U_BODY1  0x00b1f262//0x00b08e5c//0x00bd44f0//0x00A8EF05
#define LIM_U_BODY2  0x0FFFFFFF
#define LIM_U_RSHLD0 0xffffffff//0x029C24A0
#define LIM_U_RSHLD1 0x00b7055c//0x00B4EF26
#define LIM_U_RSHLD2 0x00b5c61d//0x00B7289A
#define LIM_U_RELBW0 0xffffffff//0x02C26BDC
#define LIM_U_RELBW1 0x00B65808
#define LIM_U_RWRST0 0xffffffff//0x02758D42
#define LIM_U_RWRST1 0x00B57335
#define LIM_U_RWRST2 0x00B04C01
#define LIM_U_LSHLD0 0xffffffff//0x01990F16
#define LIM_U_LSHLD1 0x00b08893//0x00B282C0
#define LIM_U_LSHLD2 0x00b74ba8//0x00A61BBE
#define LIM_U_LELBW0 0xffffffff//0x02B4C903
#define LIM_U_LELBW1 0x00B98EAC
#define LIM_U_LWRST0 0xffffffff//0x02DC93DD
#define LIM_U_LWRST1 0x00B83E59
#define LIM_U_LWRST2 0x00B0EE93
#define LIM_U_RANKL0 0x00ae4bd5//0x00ae32a8//0x00A95759
#define LIM_U_RANKL1 0x00b55f20//0x00b9ee4e//0x00BA7F35
#define LIM_U_LANKL0 0x00ab4299//0x00adadde//0x00adaa9e//0x00A9F53C
#define LIM_U_LANKL1 0x00b10dac//0x00b6c49d//0x00b6c31f//0x00B6861E

#define LIM_L_RHIP0  0x00000000//0x00fe05d9
#define LIM_L_RHIP1  0x001e3735//0x001e3bb2//0x001d37ef//0x001F1872
#define LIM_L_RHIP2  0x001bb73e//<--20161005 //0x0016a091//0x0016bee5//0x0017cb30//0x0019A76C
#define LIM_L_RKNEE0 0x00270dd8//0x0022e3d7//0x0023ca4a//0x00246E40
#define LIM_L_RKNEE1 0x002533b2//0x00214fda//0x00212ac5//0x00208369
#define LIM_L_LHIP0  0x00000000//0x01FB09DA//0x01FEAE7E
#define LIM_L_LHIP1  0x002241c0//0x0021e4b0//0x0020B94D
#define LIM_L_LHIP2  0x001aafbf//0x001b2cc1//0x00213E47
#define LIM_L_LKNEE0 0x002645dd//0x0023f1fb//0x00251a49//0x0025B392
#define LIM_L_LKNEE1 0x0015653c//0x0014fbd7//0x0015acfb//0x0014C9F3
#define LIM_L_BODY0  0x0021495a//0x002fa200//0x0022eb6c//0x00235C55
#define LIM_L_BODY1  0x001c6922//0x001b051c//0x0027bbb0//0x006F234D
#define LIM_L_BODY2  0x80000000
#define LIM_L_RSHLD0 0x00000000//0x015DF5C6
#define LIM_L_RSHLD1 0x00217c1c//0x002039C6
#define LIM_L_RSHLD2 0x00203cdd//0x002534DD
#define LIM_L_RELBW0 0x00000000//0x0162D718
#define LIM_L_RELBW1 0x00228260
#define LIM_L_RWRST0 0x00000000//0x01159B86
#define LIM_L_RWRST1 0x001EC750
#define LIM_L_RWRST2 0x001905BC
#define LIM_L_LSHLD0 0x00000000//0x0058E56D
#define LIM_L_LSHLD1 0x001aff53//0x0022CDA0
#define LIM_L_LSHLD2 0x0021c268//0x0022BF28
#define LIM_L_LELBW0 0x00000000//0x012CFA22
#define LIM_L_LELBW1 0x001D53D4
#define LIM_L_LWRST0 0x00000000//0x017C8966
#define LIM_L_LWRST1 0x0020A60C
#define LIM_L_LWRST2 0x0022D131
#define LIM_L_RANKL0 0x0018c295//0x0018a968//0x0018D2F6
#define LIM_L_RANKL1 0x002492d7//0x0024650e//0x00256C45
#define LIM_L_LANKL0 0x001aa9ed//0x0018215e//0x001940E4
#define LIM_L_LANKL1 0x0022abfd//0x002139df//0x001F944B

/****************** motor encoder offset ********************/

#define MENC_OFFSET_RHIP0  264 //769 //0x00000301//0x000002cf //719
#define MENC_OFFSET_RHIP1  608 //94 //0x0000005e
#define MENC_OFFSET_RHIP2  601 //91 //0x0000005b
#define MENC_OFFSET_RKNEE0 974 //461 //0x000001cd
#define MENC_OFFSET_RKNEE1 773 //258 //0x00000102
#define MENC_OFFSET_RKNEE2 411 //929 //0x000003a1
#define MENC_OFFSET_LHIP0  540 //32 //0x00000020
#define MENC_OFFSET_LHIP1  313 //827 //0x0000033b
#define MENC_OFFSET_LHIP2  93 //<-- correct value  //600 //0x00000256
#define MENC_OFFSET_LKNEE0 344 //853 //0x00000757
#define MENC_OFFSET_LKNEE1 712//203 //0x000000cb
#define MENC_OFFSET_LKNEE2 844//332 //0x0000014c
#define MENC_OFFSET_BODY0  13//537 //0x00000219
#define MENC_OFFSET_BODY1  241//817 //0x00000331
#define MENC_OFFSET_BODY2  0
#define MENC_OFFSET_RSHLD0 7
#define MENC_OFFSET_RSHLD1 544
#define MENC_OFFSET_RSHLD2 476
#define MENC_OFFSET_RELBW0 849
#define MENC_OFFSET_RELBW1 585
#define MENC_OFFSET_RELBW2 562
#define MENC_OFFSET_RWRST0 867
#define MENC_OFFSET_RWRST1 308
#define MENC_OFFSET_RWRST2 810
#define MENC_OFFSET_LSHLD0 873//875
#define MENC_OFFSET_LSHLD1 6//6
#define MENC_OFFSET_LSHLD2 721//725///////////////////
#define MENC_OFFSET_LELBW0 998//999
#define MENC_OFFSET_LELBW1 719//718
#define MENC_OFFSET_LELBW2 921//926
#define MENC_OFFSET_LWRST0 465
#define MENC_OFFSET_LWRST1 409
#define MENC_OFFSET_LWRST2 71
#define MENC_OFFSET_RANKL0 1018//603//89 //0x00000059
#define MENC_OFFSET_RANKL1 710//197 //0x000000c5
#define MENC_OFFSET_LANKL0 721//20160929motorchanged//541//539//919 //29//0x0000001d
#define MENC_OFFSET_LANKL1 924//on20160927//406 //0x00000196                 wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



const unsigned short EHA_CtrlWd_ON[] = {
  EHA_CTRLWD_POS, //rhip0 //0x0201    0
  EHA_CTRLWD_POS, //rhip1             1
  EHA_CTRLWD_POS, //rhip2             2
  EHA_CTRLWD_POS, //rknee0            3
  EHA_CTRLWD_POS, //rknee1            4
  EHA_CTRLWD_POS, //rknee2            5
  EHA_CTRLWD_POS, //rankle0           6
  EHA_CTRLWD_POS, //rankle1           7
  EHA_CTRLWD_POS, //lhip0             8
  EHA_CTRLWD_POS, //lhip1             9
  EHA_CTRLWD_POS, //lhip2             10
  EHA_CTRLWD_POS, //lknee0
  EHA_CTRLWD_POS, //lknee1
  EHA_CTRLWD_POS, //lknee2
  EHA_CTRLWD_POS, //lankle0
  EHA_CTRLWD_POS, //lankle1
  EHA_CTRLWD_POS, //body0
  EHA_CTRLWD_POS, //body1
  EHA_CTRLWD_POS, //body2
  EHA_CTRLWD_POS, //rshoulder0
  EHA_CTRLWD_POS, //rshoulder1
  EHA_CTRLWD_POS, //rshoulder2
  EHA_CTRLWD_POS, //relbow0
  EHA_CTRLWD_POS, //relbow1
  EHA_CTRLWD_POS, //relbow2
  EHA_CTRLWD_POS, //rwrist0
  EHA_CTRLWD_POS, //rwrist1
  EHA_CTRLWD_POS, //rwrist2
  EHA_CTRLWD_POS, //lshouldel0
  EHA_CTRLWD_POS, //lshouldel1
  EHA_CTRLWD_POS, //lshouldel2
  EHA_CTRLWD_POS, //lelbow0
  EHA_CTRLWD_POS, //lelbow1///////////////
  EHA_CTRLWD_POS, //lelbow2
  EHA_CTRLWD_POS, //lwlist0
  EHA_CTRLWD_POS, //lwlist1
  EHA_CTRLWD_POS, //lwlist2
  EHA_CTRLWD_RECT, //rhand0
  EHA_CTRLWD_RECT, //rhand1
  EHA_CTRLWD_RECT, //rhand2
  EHA_CTRLWD_RECT, //rhand3
  EHA_CTRLWD_RECT, //rhand4
  EHA_CTRLWD_RECT, //lhand0
  EHA_CTRLWD_RECT, //lhand1
  EHA_CTRLWD_RECT, //lhand2
  EHA_CTRLWD_RECT, //lhand3
  EHA_CTRLWD_RECT  //lhand4
};

const unsigned short EHA_CtrlWd_OFF[] = {
  0x0100, //rhip0
  0x0100, //rhip1
  0x0100, //rhip2
  0x0100, //rknee0
  0x0100, //rknee1
  0x0100, //rknee2
  0x0100, //rankle0
  0x0100, //rankle1
  0x0100, //lhip0
  0x0100, //lhip1
  0x0100, //lhip2
  0x0100, //lknee0
  0x0100, //lknee1
  0x0100, //lknee2
  0x0100, //lankle0
  0x0100, //lankle1
  0x0100, //body0
  0x0100, //body1
  0x0100, //body2
  0x0200, //rshoulder0
  0x0200, //rshoulder1
  0x0200, //rshoulder2
  0x0200, //relbow0
  0x0200, //relbow1
  0x0200, //relbow2
  0x0200, //rwrist0
  0x0200, //rwrist1
  0x0200, //rwrist2
  0x0200, //lshouldel0
  0x0200, //lshouldel1
  0x0200, //lshouldel2
  0x0200, //lelbow0
  0x0200, //lelbow1
  0x0200, //lelbow2
  0x0200, //lwlist0
  0x0200, //lwlist1
  0x0200, //lwlist2
  0x0100, //rhand0
  0x0100, //rhand1
  0x0100, //rhand2
  0x0100, //rhand3
  0x0100, //rhand4
  0x0100, //lhand0
  0x0100, //lhand1
  0x0100, //lhand2
  0x0100, //lhand3
  0x0100  //lhand4
};

const unsigned short EHA_CtrlWd_Calib[] = {
  EHA_CTRLWD_CALIB, //rhip0
  EHA_CTRLWD_CALIB, //rhip1
  EHA_CTRLWD_CALIB, //rhip2
  EHA_CTRLWD_CALIB, //rknee0
  EHA_CTRLWD_CALIB, //rknee1
  EHA_CTRLWD_CALIB, //rknee2
  EHA_CTRLWD_CALIB, //rankle0
  EHA_CTRLWD_CALIB, //rankle1
  EHA_CTRLWD_CALIB, //lhip0
  EHA_CTRLWD_CALIB, //lhip1
  EHA_CTRLWD_CALIB, //lhip2
  EHA_CTRLWD_CALIB, //lknee0
  EHA_CTRLWD_CALIB, //lknee1
  EHA_CTRLWD_CALIB, //lknee2
  EHA_CTRLWD_CALIB, //lankle0
  EHA_CTRLWD_CALIB, //lankle1
  EHA_CTRLWD_CALIB, //body0
  EHA_CTRLWD_CALIB, //body1
  EHA_CTRLWD_CALIB, //body2
  EHA_CTRLWD_CALIB, //rshoulder0
  EHA_CTRLWD_CALIB, //rshoulder1
  EHA_CTRLWD_CALIB, //rshoulder2
  EHA_CTRLWD_CALIB, //relbow0
  EHA_CTRLWD_CALIB, //relbow1
  EHA_CTRLWD_CALIB, //relbow2
  EHA_CTRLWD_CALIB, //rwrist0
  EHA_CTRLWD_CALIB, //rwrist1
  EHA_CTRLWD_CALIB, //rwrist2
  EHA_CTRLWD_CALIB, //lshouldel0
  EHA_CTRLWD_CALIB, //lshouldel1
  EHA_CTRLWD_CALIB, //lshouldel2
  EHA_CTRLWD_CALIB, //lelbow0
  EHA_CTRLWD_CALIB, //lelbow1
  EHA_CTRLWD_CALIB, //lelbow2
  EHA_CTRLWD_CALIB, //lwlist0
  EHA_CTRLWD_CALIB, //lwlist1
  EHA_CTRLWD_CALIB, //lwlist2
  EHA_CTRLWD_CALIB, //rhand0
  EHA_CTRLWD_CALIB, //rhand1
  EHA_CTRLWD_CALIB, //rhand2
  EHA_CTRLWD_CALIB, //rhand3
  EHA_CTRLWD_CALIB, //rhand4
  EHA_CTRLWD_CALIB, //lhand0
  EHA_CTRLWD_CALIB, //lhand1
  EHA_CTRLWD_CALIB, //lhand2
  EHA_CTRLWD_CALIB, //lhand3
  EHA_CTRLWD_CALIB  //lhand4
};

#define EHA_FORCE_BIT2PHY 0.1831055   //short <-> +-6000N
#define EHA_VANE_BIT2PHY  0.0036621   //short <-> +-120Nm
#define EHA_TENDON_BIT2PHY 0
#define CURRENT_BIT2PHY   0.0024414   //[A]

const double EHA_force_bit2phy[] = {
  EHA_VANE_BIT2PHY, //rhip0
  EHA_FORCE_BIT2PHY, //rhip1
  EHA_FORCE_BIT2PHY, //rhip2
  EHA_FORCE_BIT2PHY, //rknee0
  EHA_FORCE_BIT2PHY, //rknee1
  EHA_FORCE_BIT2PHY, //rknee2
  EHA_FORCE_BIT2PHY, //rankle0
  EHA_FORCE_BIT2PHY, //rankle1
  EHA_VANE_BIT2PHY, //lhip0
  EHA_FORCE_BIT2PHY, //lhip1
  EHA_FORCE_BIT2PHY, //lhip2
  EHA_FORCE_BIT2PHY, //lknee0
  EHA_FORCE_BIT2PHY, //lknee1
  EHA_FORCE_BIT2PHY, //lknee2
  EHA_FORCE_BIT2PHY, //lankle0
  EHA_FORCE_BIT2PHY, //lankle1
  EHA_FORCE_BIT2PHY, //body0
  EHA_FORCE_BIT2PHY, //body1
  EHA_FORCE_BIT2PHY, //body2
  EHA_VANE_BIT2PHY, //rshoulder0
  EHA_FORCE_BIT2PHY, //rshoulder1
  EHA_FORCE_BIT2PHY, //rshoulder2
  EHA_VANE_BIT2PHY, //relbow0
  EHA_FORCE_BIT2PHY, //relbow1
  EHA_FORCE_BIT2PHY, //relbow2
  EHA_VANE_BIT2PHY, //rwrist0
  EHA_FORCE_BIT2PHY, //rwrist1
  EHA_FORCE_BIT2PHY, //rwrist2
  EHA_VANE_BIT2PHY, //lshouldel0
  EHA_FORCE_BIT2PHY, //lshouldel1
  EHA_FORCE_BIT2PHY, //lshouldel2
  EHA_VANE_BIT2PHY, //lelbow0
  EHA_FORCE_BIT2PHY, //lelbow1
  EHA_FORCE_BIT2PHY, //lelbow2
  EHA_VANE_BIT2PHY, //lwlist0
  EHA_FORCE_BIT2PHY, //lwlist1
  EHA_FORCE_BIT2PHY, //lwlist2
  EHA_TENDON_BIT2PHY, //rhand0
  EHA_TENDON_BIT2PHY, //rhand1
  EHA_TENDON_BIT2PHY, //rhand2
  EHA_TENDON_BIT2PHY, //rhand3
  EHA_TENDON_BIT2PHY, //rhand4
  EHA_TENDON_BIT2PHY, //lhand0
  EHA_TENDON_BIT2PHY, //lhand1
  EHA_TENDON_BIT2PHY, //lhand2
  EHA_TENDON_BIT2PHY, //lhand3
  EHA_TENDON_BIT2PHY  //lhand4
};



const unsigned int EHA_ulim_hydra_MD4KW_3M[][3] = {
	{LIM_U_RHIP0,  LIM_U_RHIP1,  LIM_U_RHIP2},   //rhip
	{LIM_U_RKNEE0, LIM_U_RKNEE1, 0x0FFFFFFF},    //rknee
	{LIM_U_LHIP0,  LIM_U_LHIP1,  LIM_U_LHIP2},   //lhip
	{LIM_U_LKNEE0, LIM_U_LKNEE1, 0x0FFFFFFF},    //lknee
	{LIM_U_BODY0,  LIM_U_BODY1,  LIM_U_BODY2},   //body
	{LIM_U_RSHLD0, LIM_U_RSHLD1, LIM_U_RSHLD2},  //rshoulder
	{LIM_U_RELBW0, LIM_U_RELBW1, 0x0FFFFFFF},    //relbow
	{LIM_U_RWRST0, LIM_U_RWRST1, LIM_U_RWRST2},  //rwrist
	{LIM_U_LSHLD0, LIM_U_LSHLD1, LIM_U_LSHLD2},  //lshoulder
	{LIM_U_LELBW0, LIM_U_LELBW1, 0x0FFFFFFF},    //lelbow
	{LIM_U_LWRST0, LIM_U_LWRST1, LIM_U_LWRST2}   //lwrist
};

const unsigned int EHA_llim_hydra_MD4KW_3M[][3] = {
	{LIM_L_RHIP0,  LIM_L_RHIP1,  LIM_L_RHIP2},  //rhip
	{LIM_L_RKNEE0, LIM_L_RKNEE1, 0x00000000},   //rknee
	{LIM_L_LHIP0,  LIM_L_LHIP1,  LIM_L_LHIP2},  //lhip
	{LIM_L_LKNEE0, LIM_L_LKNEE1, 0x00000000},   //lknee
	{LIM_L_BODY0,  LIM_L_BODY1,  LIM_L_BODY2},  //body
	{LIM_L_RSHLD0, LIM_L_RSHLD1, LIM_L_RSHLD2}, //rshoulder
	{LIM_L_RELBW0, LIM_L_RELBW1, 0x00000000},   //relbow
	{LIM_L_RWRST0, LIM_L_RWRST1, LIM_L_RWRST2}, //rwrist
	{LIM_L_LSHLD0, LIM_L_LSHLD1, LIM_L_LSHLD2}, //lshoulder
	{LIM_L_LELBW0, LIM_L_LELBW1, 0x00000000},   //lelbow
	{LIM_L_LWRST0, LIM_L_LWRST1, LIM_L_LWRST2}  //lwrist
};

const unsigned int EHA_ulim_hydra_MD4KW_2MFS[][2] = {
	{LIM_U_RANKL0, LIM_U_RANKL1}, //rankle
	{LIM_U_LANKL0, LIM_U_LANKL1} //lankle
};

const unsigned int EHA_llim_hydra_MD4KW_2MFS[][2] = {
	{LIM_L_RANKL0, LIM_L_RANKL1}, //rankle
	{LIM_L_LANKL0, LIM_L_LANKL1} //lankle
};

const EC_T_DWORD enc_offset_MD4KW_3M [][3] = {
	{0x02fddf38/*0x02fe0600*//*0x01A7DA7E*/, (LIM_U_RHIP1+LIM_L_RHIP1)>>1, (LIM_U_RHIP2+LIM_L_RHIP2)>>1},     //rhip
	{(LIM_U_RKNEE0+LIM_L_RKNEE0)>>1, (LIM_U_RKNEE1+LIM_L_RKNEE1)>>1, 0x00000000}, //rknee
	{0x01555555/*0x02AB25BB*//*0x02AB8E51*/, (LIM_U_LHIP1+LIM_L_LHIP1)>>1, (LIM_U_LHIP2+LIM_L_LHIP2)>>1},     //lhip
	{(LIM_U_LKNEE0+LIM_L_LKNEE0)>>1, (LIM_U_LKNEE1+LIM_L_LKNEE1)>>1, 0x00000000}, //lknee
	{(LIM_U_BODY0+LIM_L_BODY0)>>1,   (LIM_U_BODY1+LIM_L_BODY1)>>1, 0xffff1e11},   //body
	{0x02587A0D, (LIM_U_RSHLD1+LIM_L_RSHLD1)>>1, (LIM_U_RSHLD2+LIM_L_RSHLD2)>>1}, //rshoulder
	{0x0287C905, (LIM_U_RELBW1+LIM_L_RELBW1)>>1, 0},                              //relbow
	{0x01160239, (LIM_U_RWRST1+LIM_L_RWRST1)>>1, (LIM_U_RWRST2+LIM_L_RWRST2)>>1}, //rwrist
	{0x00ad9e99/*0x017f467d*/, (LIM_U_LSHLD1+LIM_L_LSHLD1)>>1, (LIM_U_LSHLD2+LIM_L_LSHLD2)>>1}, //lshoulder
	{0x017c0d8b/*x01D32C9D*/, (LIM_U_LELBW1+LIM_L_LELBW1)>>1, 0},                              //lelbow
	{0x02B15C6E, (LIM_U_LWRST1+LIM_L_LWRST1)>>1, (LIM_U_LWRST2+LIM_L_LWRST2)>>1}  //lwrist
};

const EC_T_DWORD menc_offset_MD4KW_3M [][3] = {
  {MENC_OFFSET_RHIP0,  MENC_OFFSET_RHIP1,  MENC_OFFSET_RHIP2},  //rhip
  {MENC_OFFSET_RKNEE0, MENC_OFFSET_RKNEE1, MENC_OFFSET_RKNEE2}, //rknee
  {MENC_OFFSET_LHIP0,  MENC_OFFSET_LHIP1,  MENC_OFFSET_LHIP2},  //lhip
  {MENC_OFFSET_LKNEE0, MENC_OFFSET_LKNEE1, MENC_OFFSET_LKNEE2}, //lknee
  {MENC_OFFSET_BODY0,  MENC_OFFSET_BODY1,  MENC_OFFSET_BODY2},  //body
  {MENC_OFFSET_RSHLD0, MENC_OFFSET_RSHLD1, MENC_OFFSET_RSHLD2}, //rshoulder
  {MENC_OFFSET_RELBW0, MENC_OFFSET_RELBW1, MENC_OFFSET_RELBW2}, //relbow
  {MENC_OFFSET_RWRST0, MENC_OFFSET_RWRST1, MENC_OFFSET_RWRST2}, //rwrist
  {MENC_OFFSET_LSHLD0, MENC_OFFSET_LSHLD1, MENC_OFFSET_LSHLD2}, //lshoulder
  {MENC_OFFSET_LELBW0, MENC_OFFSET_LELBW1, MENC_OFFSET_LELBW2}, //lelbow
  {MENC_OFFSET_LWRST0, MENC_OFFSET_LWRST1, MENC_OFFSET_LWRST2}  //lwrist
};

#define KEMF_MAXON_4P_200W 0x12

const EC_T_WORD motor_KEMF_MD4KW_3M [][3] = {
  {KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W},  //rhip
  {KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W}, //rknee
  {KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W},  //lhip
  {KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W}, //lknee
  {KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W}, //lelbow
  {KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W}  //lwrist
};


const EC_T_DWORD enc_offset_MD4KW_2MFS [][2] = {
	{(LIM_U_RANKL0+LIM_L_RANKL0)>>1, (LIM_U_RANKL1+LIM_L_RANKL1)>>1},  //rankle
	{(LIM_U_LANKL0+LIM_L_LANKL0)>>1, (LIM_U_LANKL1+LIM_L_LANKL1)>>1}   //lankle
};

const EC_T_DWORD menc_offset_MD4KW_2MFS [][2] = {
	{MENC_OFFSET_RANKL0,MENC_OFFSET_RANKL1},  //rankle
	{MENC_OFFSET_LANKL0,MENC_OFFSET_LANKL1}   //lankle
};

const EC_T_WORD motor_KEMF_MD4KW_2MFS [][2] = {
  {KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W},  //rankle
  {KEMF_MAXON_4P_200W, KEMF_MAXON_4P_200W} //lankle
};


const EC_T_DWORD enc_offset_MD4KW_hand [][5] = {
	{0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0}
};

const EC_T_WORD enc_offset_MD4KW_fingers [][16] = {
	{0x0baf, 0x2ccd, 0x21c0, 0x16f0, 0x17a0, 0x2eed, 0x1b1c, 0x199e, 0x01db, 0x15ae, 0x108e, 0x0929, 0x1b3a, 0x2def, 0x19cd, 0x2247},
	//	{0x0ba0, 0x2cbd, 0x21ac, 0x177b, 0x1797, 0x2eed, 0x1ad4, 0x1945, 0x01cd, 0x15b4, 0x1079, 0x092d, 0x1b28, 0x2df9, 0x19c7, 0x2248},
	//{0x0a45, 0x34f7, 0x2bce, 0x329f, 0x2b49, 0x3dca, 0x2d0c, 0x0668, 0x380b, 0x02a6, 0x166f, 0x1c5a, 0x00ac, 0x1bc2, 0x24dd, 0x3785}
    {0x0a1a, 0x3530, 0x3040, 0x32a0, 0x2b4c, 0x3dbd, 0x2d1b, 0x0793, 0x380e, 0x028c, 0x1659, 0x1c82, 0x00b7, 0x1bac, 0x2785, 0x3798}
};



const double CONV_RTLA = 5e-9;
const double CONV_RESA = 2.0*M_PI/67108864.0;
const double CONV_neck = 2.0*M_PI/200000;
const double CONV_hand   = 2.0*M_PI/16384.0;
const double CONV_finger = 2.0*M_PI/16384.0;

const double eha_phys_pos_hydra_MD4KW_3M[][3] = {
	{   CONV_RESA,    CONV_RTLA, -1*CONV_RTLA}, //rhip
	{-1*CONV_RTLA, -1*CONV_RTLA,    1},         //rknee
	{   CONV_RESA,    CONV_RTLA, -1*CONV_RTLA}, //lhip
	//{-1*CONV_RTLA, -1*CONV_RTLA,    1},         //lknee
	{-1*CONV_RTLA,    CONV_RTLA,    1},         //lknee
	{   CONV_RTLA, -1*CONV_RTLA,    CONV_neck}, //body
	{-1*CONV_RESA, -1*CONV_RTLA,    CONV_RTLA}, //rshoulder
	{   CONV_RESA,    CONV_RTLA,    1},         //relbow
	{   CONV_RESA, -1*CONV_RTLA,    CONV_RTLA}, //rwrist
	{-1*CONV_RESA,    CONV_RTLA, -1*CONV_RTLA}, //lshoulder
	{   CONV_RESA, -1*CONV_RTLA,    1},         //lelbow
	{   CONV_RESA, -1*CONV_RTLA,    CONV_RTLA}  //lwrist
};

#define PRES7MPA_V_TO_Pa  3227000
#define PRES21MPA_V_TO_Pa (PRES7MPA_V_TO_Pa*3)
//#define ADC_BIT_TO_V      1221
#define ADC_BIT_TO_V      1.221e-3
#define CYL_Pa_TO_N       (91*M_PI*1e-6)
#define VANE_Pa_TO_Nm     0
#define V_TO_TEMP         (1/6.25e-3)
#define TEMP_V_OFFSET     (424e-3)

#define CYL7M_BIT_TO_N    (CYL_Pa_TO_N*PRES7MPA_V_TO_Pa*ADC_BIT_TO_V)
#define CYL21M_BIT_TO_N   (CYL_Pa_TO_N*PRES21MPA_V_TO_Pa*ADC_BIT_TO_V)
#define VANE_BIT_TO_Nm    (VANE_Pa_TO_Nm*PRES7MPA_V_TO_Pa*ADC_BIT_TO_V)

const double eha_phys_forcepres_hydra_MD4KW_3M[][3] = {
	{   VANE_BIT_TO_Nm,    CYL7M_BIT_TO_N,  -1*CYL7M_BIT_TO_N}, //rhip
	{-1*CYL7M_BIT_TO_N, -1*CYL21M_BIT_TO_N,    0},              //rknee
	{   VANE_BIT_TO_Nm,    CYL7M_BIT_TO_N,  -1*CYL7M_BIT_TO_N}, //lhip
	{-1*CYL7M_BIT_TO_N, -1*CYL21M_BIT_TO_N,    0},              //lknee
	{   CYL7M_BIT_TO_N, -1*CYL7M_BIT_TO_N,     0},              //body
	{-1*VANE_BIT_TO_Nm, -1*CYL7M_BIT_TO_N,     CYL7M_BIT_TO_N}, //rshoulder
	{   VANE_BIT_TO_Nm,    CYL21M_BIT_TO_N,    0},              //relbow
	{   VANE_BIT_TO_Nm, -1*CYL7M_BIT_TO_N,     CYL7M_BIT_TO_N}, //rwrist
	{-1*VANE_BIT_TO_Nm,    CYL7M_BIT_TO_N,  -1*CYL7M_BIT_TO_N}, //lshoulder
	{   VANE_BIT_TO_Nm,    CYL21M_BIT_TO_N,    0},              //lelbow
	{   VANE_BIT_TO_Nm, -1*CYL7M_BIT_TO_N,     CYL7M_BIT_TO_N}  //lwrist
};

const double eha_phys_forcestrain_hydra_MD4KW_3M[][3] = {
        {   VANE_BIT_TO_Nm,    CYL7M_BIT_TO_N,  -1*CYL7M_BIT_TO_N}, //rhip
        {-1*CYL7M_BIT_TO_N, -1*CYL21M_BIT_TO_N,    0},              //rknee
        {   VANE_BIT_TO_Nm,    CYL7M_BIT_TO_N,  -1*CYL7M_BIT_TO_N}, //lhip
        {-1*CYL7M_BIT_TO_N, -1*CYL21M_BIT_TO_N,    0},              //lknee
        {   CYL7M_BIT_TO_N, -1*CYL7M_BIT_TO_N,     0},              //body
        {-1*VANE_BIT_TO_Nm, -1*CYL7M_BIT_TO_N,     CYL7M_BIT_TO_N}, //rshoulder
        {   VANE_BIT_TO_Nm,    CYL21M_BIT_TO_N,    0},              //relbow
        {   VANE_BIT_TO_Nm, -1*CYL7M_BIT_TO_N,     CYL7M_BIT_TO_N}, //rwrist
        {-1*VANE_BIT_TO_Nm,    CYL7M_BIT_TO_N,  -1*CYL7M_BIT_TO_N}, //lshoulder
        {   VANE_BIT_TO_Nm,    CYL21M_BIT_TO_N,    0},              //lelbow
        {   VANE_BIT_TO_Nm, -1*CYL7M_BIT_TO_N,     CYL7M_BIT_TO_N}  //lwrist
};



const double eha_phys_forcecomple_hydra_MD4KW_3M[][3] = {
        {   VANE_BIT_TO_Nm,    CYL7M_BIT_TO_N,  -1*CYL7M_BIT_TO_N}, //rhip
        {-1*CYL7M_BIT_TO_N, -1*CYL21M_BIT_TO_N,    0},              //rknee
        {   VANE_BIT_TO_Nm,    CYL7M_BIT_TO_N,  -1*CYL7M_BIT_TO_N}, //lhip
        {-1*CYL7M_BIT_TO_N, -1*CYL21M_BIT_TO_N,    0},              //lknee
        {   CYL7M_BIT_TO_N, -1*CYL7M_BIT_TO_N,     0},              //body
        {-1*VANE_BIT_TO_Nm, -1*CYL7M_BIT_TO_N,     CYL7M_BIT_TO_N}, //rshoulder
        {   VANE_BIT_TO_Nm,    CYL21M_BIT_TO_N,    0},              //relbow
        {   VANE_BIT_TO_Nm, -1*CYL7M_BIT_TO_N,     CYL7M_BIT_TO_N}, //rwrist
        {-1*VANE_BIT_TO_Nm,    CYL7M_BIT_TO_N,  -1*CYL7M_BIT_TO_N}, //lshoulder
        {   VANE_BIT_TO_Nm,    CYL21M_BIT_TO_N,    0},              //lelbow
        {   VANE_BIT_TO_Nm, -1*CYL7M_BIT_TO_N,     CYL7M_BIT_TO_N}  //lwrist
};


//const int eha_pos_gain_hydra_MD4KW_3M[][3] = {
//    //{  -100,  150, -150}, //rhip
//    {  -100,  180, -180}, //rhip
//    //{ -100,  100, -100}, //rknee with single encoder
//    { 150,  150, -150}, //rknee with tandem encoder
//    { -100,  180, -180}, //lhip
//    { -150,  -150, 150}, //lknee with both single or tandem encoder
//    { -100,  100,  100}, //body
//    { -50,  50, -50}, //rshoulder
//    {  100, -100,  100}, //relbow
//    {  50,  100, -100}, //rwrist
//    { -50, -50,  50}, //lshoulder
//    {  50,  100, -100}, //lelbow
//    {  50,  100, -100}  //lwrist
//};
//const int eha_vel_gain_hydra_MD4KW_3M[][3] = {
//    //{ 100,  300, 300}, //rhip
//    { 100,  250, 250}, //rhip
//  //{ 200,  200, 200}, //rknee with single encoder
//  { -300,  300, 300}, //rknee with tandem encoder
//  { 100,  250, 250}, //lhip
//  { 300,  300, 300}, //lknee
//  { 200,  200, 0}, //body
//  { 0,  200, 200}, //rshoulder
//  { 0,  0, 0}, //relbow
//  { 0,  100, 100}, //rwrist
//  { 0,  200, 200}, //lshoulder
//  { 0,  0, 0}, //lelbow
//  { 0,  0, 0}  //lwrist
//};

//#define TESTNUM 3*2 //for 30V
//#define TESTNUM 1 //for 20V
//#define TESTNUM 5*2 //for 48V

const int eha_pos_p_gain_hydra_MD4KW_3M[][3] = {//16bitshifted
  //  { -50, 1, -1}, //rhip
  /*{-20,100,-100},//*/{ -80, 180, -180}, //rhip
  { -60,  -90, 90},//rknee with single encoder //{ 90,  90, -90}, //rknee with tandem encoder
  { -80,  150, -150},//{ -20,  150, -150}, //lhip
  { -70,  -60, 60},//  { -30,  -30, 30}, //lknee with both single or tandem encoder
  { -100,  100,  30}, //body
  { -30,  30, -30}, //rshoulder
  {  50, -50,  50}, //relbow
  {  50,  50, -50}, //rwrist
  { -30, -30,  30},//{ -100, -100,  100}, //lshoulder
  {  50,  50, -50}, //{  100,  500, -250}, //lelbow 250
  {  50,  50, -50}//{  100,  150, -150}  //lwrist
};
const int eha_pos_d_gain_hydra_MD4KW_3M[][3] = {//10bitshifted
        { 0,  70, 70}, //rhip
        { -20,  20, 20},//{ -20,  30, 30}, //rknee with tandem encoder
        { 0,  70, 70}, //lhip
        { 20,  20, 20},//{ 50,  50, 50}, //lkneeQ
	{ 20,  20, 1}, //body
        { 1,  1, 1}, //rshoulder
        { 1,  1, 1}, //relbow
        { 1,  1, 1}, //rwrist
        { 1,  1, 1 },//{ 50,  100, 100}, //lshoulder
        { 1,  1, 1},//{ 200,  500, 500}, //lelbow 300 500
        { 1,  1, 1} //lwrist//{ 200,  100, 100} //lwrist
};

const int eha_pos_i_gain_hydra_MD4KW_3M[][3] = {
  {10,30,30},//{10,300,300},  //rhip
  {10,10,10},//{20,20,20},  //rknee
  {50,25,25},//{50,250,250}, //lhip
  {10,10,10},  //lknee
  {1,1,1},  //body
  {10,10,10},  //rshoulder
  {10,10,10},  //relbow
  {10,10,10},  //rwrist
  {10,10,10},  //lshoulder//{100,100,100},  //lshoulder
  {10,40,30}, //{100,400,300},  //lelbow 300
  {10,10,10}   //lwrist//{100,100,100}   //lwrist
};

const int eha_vel_p_gain_hydra_MD4KW_3M[][3] = {
  {18,18,18}, //rhip
  {18,18,18}, //rknee
  {18,18,18}, //lhip
  {18,18,18}, //lknee
  {18,18,18}, //body
  {18,18,18}, //rshoulder
  {18,18,18}, //relbow
  {18,18,18}, //rwrist
  {18,18,18}, //lshoulder
  {18,18,18}, //lelbow
  {18,18,18}  //lwrist
};
const int eha_vel_d_gain_hydra_MD4KW_3M[][3] = {
  {0,0,0},//rhip
  {0,0,0},//rknee with tandem encoder
  {0,0,0},//lhip
  {0,0,0},//lkneeQ
  {0,0,0},//body
  {0,0,0},//rshoulder
  {0,0,0},//relbow
  {0,0,0},//rwrist
  {0,0,0},//lshoulder
  {0,0,0},//lelbow
  {0,0,0} //lwrist
};

const int eha_vel_i_gain_hydra_MD4KW_3M[][3] = {
  //  {20,0,0}, //rhip
  {20,20,20}, //rhip
  {20,20,20}, //rknee
  {20,20,20},//lhip
  {20,20,20}, //lknee
  {20,20,20}, //body
  {20,20,20}, //rshoulder
  {20,20,20}, //relbow
  {20,20,20}, //rwrist
  {20,20,20}, //lshoulder
  {20,20,20}, //lelbow
  {20,20,20}  //lwrist
};

const int eha_tau_p_gain_hydra_MD4KW_3M[][3] = { //rod compress -> strain decrease
  {1,-1,1}, //rhip *--
  {-1,-1,1}, //rknee ++*
  {1,-1,1}, //lhip *--
  {-1,1,1}, //lknee ++*
  {1,-1,1}, //body ++*
  {-1,-1,1}, //rshoulder *++
  {1,1,1}, //relbow *+*
  {1,-1,1}, //rwrist *++
  {1,1,-1}, //lshoulder *++
  {1,-30000,30000}, //lelbow *+*    30000 ?
  {1,-30000,30000}  //lwrist *++
};
const int eha_tau_d_gain_hydra_MD4KW_3M[][3] = {
  {0,0,0},//rhip
  {0,0,0},//rknee with tandem encoder
  {0,0,0},//lhip
  {0,0,0},//lkneeQ
  {0,0,0},//body
  {0,0,0},//rshoulder
  {0,0,0},//relbow
  {0,0,0},//rwrist
  {0,0,0},//lshoulder
  {15000,15000,15000},//lelbow 15000?
  {15000,15000,15000} //lwrist
};

const int eha_tau_i_gain_hydra_MD4KW_3M[][3] = {
  //  {20,0,0}, //rhip
  {20,0,0}, //rhip
  {0,0,0}, //rknee
  {0,0,0},//lhip
  {0,0,0}, //lknee
  {0,0,0}, //body
  {20,20,20}, //rshoulder
  {20,20,20}, //relbow
  {20,20,20}, //rwrist
  {20,20,20}, //lshoulder
  {300,300,300}, //lelbow 300?
  {300,300,300}  //lwrist
};

#define CUR_PGAIN_MAXON_4P_200W  0x1c00//0x300 //0x40//0x0600 20? 2a0
#define CUR_IGAIN_MAXON_4P_200W  0x1400//0x100  //0x800//0xf00
//#define CUR_PGAIN_MAXON_4P_200W  0x0600
//#define CUR_IGAIN_MAXON_4P_200W  0xf00

const int eha_cur_p_gain_hydra_MD4KW_3M[][3] = {
  {CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W},//rhip
  {CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W},//rknee with tandem encoder
  {CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W},//lhip
  {CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W},//lknee with both single or tandem encoder
  {CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W},//body
  {CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W},//rshoulder
  {CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W},//relbow
  {CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W},//rwrist
  {CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W},//lshoulder
  {CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W},//lelbow
  {CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W,CUR_PGAIN_MAXON_4P_200W} //lwrist
};
const int eha_cur_d_gain_hydra_MD4KW_3M[][3] = {
  {0,0,0},    //rhip
  {0,0,0},    //rknee with tandem encoder
  {0,0,0},    //lhip
  {0,0,0},    //lkneeQ
  {0,0,0},    //body
  {0,0,0},    //rshoulder
  {0,0,0},    //relbow
  {0,0,0},    //rwrist
  {0,0,0},    //lshoulder
  {0,0,0},    //lelbow
  {0,0,0}     //lwrist
};

const int eha_cur_i_gain_hydra_MD4KW_3M[][3] = {//26+4bitshifted
  {CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W},  //rhip
  {CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W},  //rknee
  {CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W},  //lhip
  {CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W},  //lknee
  {CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W},  //body
  {CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W},  //rshoulder
  {CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W},  //relbow
  {CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W},  //rwrist
  {CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W},  //lshoulder
  {CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W},  //lelbow
  {CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W,CUR_IGAIN_MAXON_4P_200W}   //lwrist
};


const double eha_phys_pos_hydra_MD4KW_2MFS[][2] = {
	{CONV_RTLA, -1*CONV_RTLA},
	{CONV_RTLA, -1*CONV_RTLA}
};

const double eha_phys_forcepres_hydra_MD4KW_2MFS[][2] = {
	{CYL7M_BIT_TO_N, CYL7M_BIT_TO_N},
	{CYL7M_BIT_TO_N, CYL7M_BIT_TO_N}
};
//const int eha_pos_gain_hydra_MD4KW_2MFS[][2] = {
//	{ -100,  100}, //right
//	{ -100,  100}, //left
//};
//const int eha_vel_gain_hydra_MD4KW_2MFS[][2] = {
//	{ 100,  100}, //right
//	{ 100,  100}, //left
//};
const int eha_pos_p_gain_hydra_MD4KW_2MFS[][2] = {
        { 70,  70}, //right

	{-60,  50},//{-40,  40},//{0,0}, //{ -80,  80},//{ -80,  80}, //left
};
const int eha_pos_d_gain_hydra_MD4KW_2MFS[][2] = {
        { 1,  1}, //right
        {0,0}, //{ 10,  10},//{ 10,  10}, //left
};
const int eha_pos_i_gain_hydra_MD4KW_2MFS[][2] = {
        { 10,  10}, //right
        {0,0}, //{ 1,  1},//{ 1,  1}, //left
};

const int eha_vel_p_gain_hydra_MD4KW_2MFS[][2] = {
        { -18,  18}, //right
        { -18,  18}, //left
};
const int eha_vel_d_gain_hydra_MD4KW_2MFS[][2] = {
        { 200,  200}, //right
        { 200,  200}, //left
};
const int eha_vel_i_gain_hydra_MD4KW_2MFS[][2] = {
        { 20,  20}, //right
        { 20,  20}, //left
};

const int eha_tau_p_gain_hydra_MD4KW_2MFS[][2] = {
        { 1,  -1}, //right ++
        { 1,  -1}, //left ++
};
const int eha_tau_d_gain_hydra_MD4KW_2MFS[][2] = {
        { 200,  200}, //right
        { 200,  200}, //left
};
const int eha_tau_i_gain_hydra_MD4KW_2MFS[][2] = {
        { 50,  50}, //right
        { 50,  50}, //left
};

const int eha_cur_p_gain_hydra_MD4KW_2MFS[][2] = {
        { CUR_PGAIN_MAXON_4P_200W,  CUR_PGAIN_MAXON_4P_200W}, //right
        { CUR_PGAIN_MAXON_4P_200W,  CUR_PGAIN_MAXON_4P_200W}, //left
};
const int eha_cur_d_gain_hydra_MD4KW_2MFS[][2] = {
        { 0,  0}, //right
        { 0,  0}, //left
};
const int eha_cur_i_gain_hydra_MD4KW_2MFS[][2] = {
        { CUR_IGAIN_MAXON_4P_200W,  CUR_IGAIN_MAXON_4P_200W}, //right
        { CUR_IGAIN_MAXON_4P_200W,  CUR_IGAIN_MAXON_4P_200W}, //left
};

const double eha_phys_pos_hydra_MD4KW_Hand[][21] = {
	{CONV_hand, CONV_hand, CONV_hand, CONV_hand, CONV_hand,
	-1*CONV_finger, -1*CONV_finger, CONV_finger, CONV_finger,
	-1*CONV_finger, -1*CONV_finger, CONV_finger, CONV_finger,
	-1*CONV_finger, -1*CONV_finger, CONV_finger, CONV_finger,
	-1*CONV_finger, -1*CONV_finger, CONV_finger, CONV_finger },
	{CONV_hand, CONV_hand, CONV_hand, CONV_hand, CONV_hand,
	   CONV_finger, -1*CONV_finger, CONV_finger, CONV_finger,
	-1*CONV_finger, -1*CONV_finger, CONV_finger, CONV_finger,
	-1*CONV_finger, -1*CONV_finger, CONV_finger, CONV_finger,
	-1*CONV_finger, -1*CONV_finger, CONV_finger, CONV_finger }
};

const int eha_pos_p_gain_hydra_MD4KW_Hand[][21] = {
	{160, 160, 160, 160, 160,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0 },
	{160, 160, 160, 160, 160,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0 }
};
const int eha_pos_d_gain_hydra_MD4KW_Hand[][21] = {
	{0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0 },
	{0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0 }
};

const double IMU_phys_MD4KW_IMU[6] = {
	2.18651e-5, -2.19621e-5, -2.19136e-5, 0.00994704, 0.01004151, 0.019990373
};

/*
const double FS_phys_MD4KW_2MFS[][6] = {
	{0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0}
};
*/

// Actual force sensor coodinate is back=X right=Y up=Z
const double FS_phys_MD4KW_2MFS[][6] = {
	{2000.0/8191.0, 2000.0/8191.0, -2000.0/8191.0, 120.0/8191.0, 120.0/8191.0, -40.0/8191.0},
	{2000.0/8191.0, 2000.0/8191.0, -2000.0/8191.0, 120.0/8191.0, 120.0/8191.0, -40.0/8191.0}
};
// 0x0000 to 0x3FFF. zero is 0x1FFF = 8191
const double FS_phys_offset_MD4KW_2MFS[][6] = {
		{8224.0, 8156.0, 11054.0, 8146.0, 8049.0, 8255.0},
		{7801.0, 8213.0, 11288.0, 7824.0, 7441.0, 7855.0}
};




/* {board type, id number, axis} */

const unsigned char joint_hydra_R_arm0[] = {BDTYPE_MD4KW_3M,0,0}; // shoulder yaw0
const unsigned char joint_hydra_R_arm1[] = {BDTYPE_MD4KW_3M,0,1}; // shoulder pr0
const unsigned char joint_hydra_R_arm2[] = {BDTYPE_MD4KW_3M,0,2}; // shoulder pr1
const unsigned char joint_hydra_R_arm3[] = {BDTYPE_MD4KW_3M,1,0}; // arm yaw
const unsigned char joint_hydra_R_arm4[] = {BDTYPE_MD4KW_3M,1,1}; // elbow tandem0
const unsigned char joint_hydra_R_arm5[] = {BDTYPE_MD4KW_3M,1,2}; // elbow tandem1
const unsigned char joint_hydra_R_arm6[] = {BDTYPE_MD4KW_3M,2,0}; // wrist pr0
const unsigned char joint_hydra_R_arm7[] = {BDTYPE_MD4KW_3M,2,1}; // wrist pr1
const unsigned char joint_hydra_R_arm8[] = {BDTYPE_MD4KW_3M,2,2}; // wrist yaw

const unsigned char joint_hydra_R_hand0[] = {BDTYPE_MD4KW_HAND,0,0}; //
const unsigned char joint_hydra_R_hand1[] = {BDTYPE_MD4KW_HAND,0,1}; //
const unsigned char joint_hydra_R_hand2[] = {BDTYPE_MD4KW_HAND,0,2}; //
const unsigned char joint_hydra_R_hand3[] = {BDTYPE_MD4KW_HAND,0,3}; //
const unsigned char joint_hydra_R_hand4[] = {BDTYPE_MD4KW_HAND,0,4}; //

const unsigned char joint_hydra_R_leg0[] = {BDTYPE_MD4KW_3M,3,0}; // hip yaw
const unsigned char joint_hydra_R_leg1[] = {BDTYPE_MD4KW_3M,3,1}; // hip pr0
const unsigned char joint_hydra_R_leg2[] = {BDTYPE_MD4KW_3M,3,2}; // hip pr1
const unsigned char joint_hydra_R_leg3[] = {BDTYPE_MD4KW_3M,4,0}; // knee tandem0
const unsigned char joint_hydra_R_leg4[] = {BDTYPE_MD4KW_3M,4,1}; // knee tandem1
const unsigned char joint_hydra_R_leg5[] = {BDTYPE_MD4KW_3M,4,2}; // knee back1
const unsigned char joint_hydra_R_leg6[] = {BDTYPE_MD4KW_2MFS,0,0}; // ankle pr0
const unsigned char joint_hydra_R_leg7[] = {BDTYPE_MD4KW_2MFS,0,1}; //ankle pr1

const unsigned char joint_hydra_L_leg0[] = {BDTYPE_MD4KW_3M,5,0}; // hip yaw
const unsigned char joint_hydra_L_leg1[] = {BDTYPE_MD4KW_3M,5,1}; // hip pr0
const unsigned char joint_hydra_L_leg2[] = {BDTYPE_MD4KW_3M,5,2}; // hip pr1
const unsigned char joint_hydra_L_leg3[] = {BDTYPE_MD4KW_3M,6,0}; // knee tandem0
const unsigned char joint_hydra_L_leg4[] = {BDTYPE_MD4KW_3M,6,1}; // knee tandem1
const unsigned char joint_hydra_L_leg5[] = {BDTYPE_MD4KW_3M,6,2}; // knee back1
const unsigned char joint_hydra_L_leg6[] = {BDTYPE_MD4KW_2MFS,1,0}; // ankle pr0
const unsigned char joint_hydra_L_leg7[] = {BDTYPE_MD4KW_2MFS,1,1}; //ankle pr1

const unsigned char joint_hydra_L_arm0[] = {BDTYPE_MD4KW_3M,7,0}; // shoulder yaw0
const unsigned char joint_hydra_L_arm1[] = {BDTYPE_MD4KW_3M,7,1}; // shoulder pr0
const unsigned char joint_hydra_L_arm2[] = {BDTYPE_MD4KW_3M,7,2}; // shoulder pr1
const unsigned char joint_hydra_L_arm3[] = {BDTYPE_MD4KW_3M,8,0}; // arm yaw
const unsigned char joint_hydra_L_arm4[] = {BDTYPE_MD4KW_3M,8,1}; // elbow tandem0
const unsigned char joint_hydra_L_arm5[] = {BDTYPE_MD4KW_3M,8,2}; // elbow tandem1
const unsigned char joint_hydra_L_arm6[] = {BDTYPE_MD4KW_3M,9,0}; // wrist pr0
const unsigned char joint_hydra_L_arm7[] = {BDTYPE_MD4KW_3M,9,1}; // wrist pr1
const unsigned char joint_hydra_L_arm8[] = {BDTYPE_MD4KW_3M,9,2}; // wrist yaw

const unsigned char joint_hydra_L_hand0[] = {BDTYPE_MD4KW_HAND,1,0}; //
const unsigned char joint_hydra_L_hand1[] = {BDTYPE_MD4KW_HAND,1,1}; //
const unsigned char joint_hydra_L_hand2[] = {BDTYPE_MD4KW_HAND,1,2}; //
const unsigned char joint_hydra_L_hand3[] = {BDTYPE_MD4KW_HAND,1,3}; //
const unsigned char joint_hydra_L_hand4[] = {BDTYPE_MD4KW_HAND,1,4}; //

const unsigned char joint_hydra_body0[] = {BDTYPE_MD4KW_3M,10,0}; // body pr0
const unsigned char joint_hydra_body1[] = {BDTYPE_MD4KW_3M,10,1}; // body pr1
const unsigned char joint_hydra_neck[]  = {BDTYPE_MD4KW_3M,10,2}; // neck

const unsigned char sensor_hydra_R_FS[] = {BDTYPE_MD4KW_2MFS,0,2};
const unsigned char sensor_hydra_L_FS[] = {BDTYPE_MD4KW_2MFS,1,2};
const unsigned char sensor_hydra_IMU[]  = {BDTYPE_MD4KW_IMU,0,0};

//const unsigned char joint_rhip_yaw        = 0;
//const unsigned char joint_rhip_pitch      = 1;
//const unsigned char joint_rhip_roll       = 2;
//const unsigned char joint_rknee_pitch     = 3;
//const unsigned char joint_rankle_roll     = 4;
//const unsigned char joint_rankle_pitch    = 5;
//const unsigned char joint_lhip_yaw        = 6;
//const unsigned char joint_lhip_pitch      = 7;
//const unsigned char joint_lhip_roll       = 8;
//const unsigned char joint_lknee_pitch     = 9;
//const unsigned char joint_lankle_roll     = 10;
//const unsigned char joint_lankle_pitch    = 11;
//const unsigned char joint_waist_pitch     = 12;
//const unsigned char joint_waist_roll      = 13;
//const unsigned char joint_neck_pitch      = 14;
//const unsigned char joint_rscaplae_yaw    = 15;
//const unsigned char joint_rshoulder_pitch = 16;
//const unsigned char joint_rshoulder_roll  = 17;
//const unsigned char joint_rshoulder_yaw   = 18;
//const unsigned char joint_relbow_pitch    = 19;
//const unsigned char joint_rwrist_yaw      = 20;
//const unsigned char joint_rwrist_roll     = 21;
//const unsigned char joint_rwrist_pitch    = 22;
//const unsigned char joint_lscaplae_yaw    = 23;
//const unsigned char joint_lshoulder_pitch = 24;
//const unsigned char joint_lshoulder_roll  = 25;
//const unsigned char joint_lshoulder_yaw   = 26;
//const unsigned char joint_lelbow_pitch    = 27;
//const unsigned char joint_lwrist_yaw      = 28;
//const unsigned char joint_lwrist_roll     = 29;
//const unsigned char joint_lwrist_pitch    = 30;
//const unsigned char joint_rhand_thb_AA    = 31;
//const unsigned char joint_rhand_thb_FE    = 32;
//const unsigned char joint_rhand_idx       = 33;
//const unsigned char joint_rhand_mid       = 34;
//const unsigned char joint_rhand_rng       = 35;
//const unsigned char joint_lrhand_thb_AA    = 36;
//const unsigned char joint_lhand_thb_FE    = 37;
//const unsigned char joint_lhand_idx       = 38;
//const unsigned char joint_lhand_mid       = 39;
//const unsigned char joint_lhand_rng       = 40;
#endif /* __HYDRA_H_ */
