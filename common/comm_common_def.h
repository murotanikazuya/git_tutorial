
#ifndef __COMM_COMMON_DEF_H__
#define __COMM_COMMON_DEF_H__ 
#include "EcType.h"

#define SLAVE_NUM_MAX			100
#define RTP_BUFF_SIZE			8192
#define RTP_PORT_SLAVE			12300
#define RTP_PORT_MASTER			12400


// モデルタイプの定義
typedef enum _T_ePfModel
{
	PF_MODEL_NONE = 0,		// 

	// 単軸バイラテラル環境
	PF_MODEL_WAXIS,

	// B-Type用モジュール評価ベンチ
	PF_MODEL_BTYPE_BENCH,

	// 7軸アーム環境
	PF_MODEL_7ARM_RIGHT,

	// HRモデル：LIM
	PF_MODEL_HR_LIM,

	// HRモデル：Hydra
	PF_MODEL_HYDRA = 100,

	// MAX
	PF_MODEL_MAX
} T_ePfModel;

// 動作モード定義
typedef enum _T_eOpMode
{
	OP_MODE_NONE = 0,

	// ユニラテラル
	OP_MODE_UNI_NORMAL = 1,
	OP_MODE_UNI_REVERSE = 2,

	// 一般的なバイラテラル
	OP_MODE_BI_SYMMETRIC = 10,		// 対称型
	OP_MODE_BI_FORCE_REF = 11,		// 力逆送型
	OP_MODE_BI_FORCE_FB  = 12,		// 力帰還型

	// 検証用のバイラテラル
	OP_MODE_EXP_4CH_POS_TRQ = 20,		// 4ch型（位置とトルク）

	// トヨタ独自モード
	OP_MODE_TMC_SPEC_	 = 100,
	
	// 東大独自モード
	OP_MODE_YNL_SPEC_	 = 200,

	// MAX
	OP_MODE_MAX
} T_eOpMode;


typedef enum _T_eOpCommand
{
	OP_CMD_NONE = 0,

	// MAX
	OP_CMD_MAX
} T_eOpCommand;


typedef enum _T_eOpStatus
{
	OP_STS_NONE = 0,

	// MAX
	OP_STS_MAX
} T_eOpStatus;


//
// 遠隔制御用の通信構造体
// 
struct T_JOINT_PARAM {
	double pos;
	double vel;
	double tau;
};

// 各軸毎の状態
union T_JOINT_DATA
{
	struct {
		int    			id;
		int    			enable;			// Servo On:1, off:0
		T_JOINT_PARAM	act;			// 送信元の制御対象の最新データ
		T_JOINT_PARAM	cmd;			// 送信先の制御対象への指令値
										// （使用するかは動作モードに依存)
	} DATA;
    unsigned char BYTE[56];
};

struct T_REMOTE_PAYLOAD_DATA {
	int				OpMode;			// 動作モード
	int				OpStatus;		// 動作状態
	int				OpCommand;		// 動作指示

	int				ModelType;		// 送信元の制御対象
	int				JointNum;		// Jointデータ数
	T_JOINT_DATA 	Joint[SLAVE_NUM_MAX];
};

#endif /* #ifndef __COMM_COMMON_DEF_H__ */
