/*
  $id$
 */
#ifndef __ROBOT_ID_H__
#define __ROBOT_ID_H__

#ifdef LIM_FULL_VERSION
/****************************************/
/* モデルパラメータID */
/****************************************/
/* ボディID */
enum body_id   {
	BODY_HIP1_L,              /* 左股関節1 */
	BODY_HIP2_L,              /* 左股関節2 */
	BODY_THIGH_L,             /* 左大腿 */
	BODY_CRURAL_L,            /* 左下腿 */
	BODY_ANKLE_L,             /* 左足首 */
	BODY_FOOT_L,              /* 左足 */
	BODY_TOE_L,               /* 左つま先 */

	BODY_HIP1_R,              /* 右股関節1 */
	BODY_HIP2_R,              /* 右股関節2 */
	BODY_THIGH_R,             /* 右大腿 */
	BODY_CRURAL_R,            /* 右下腿 */
	BODY_ANKLE_R,             /* 右足首 */
	BODY_FOOT_R,              /* 右足 */
	BODY_TOE_R,               /* 右つま先 */
	
	BODY_BASE,                /* 腰 */
	BODY_WAIST1,              /* 腰1 */
	BODY_WAIST2,              /* 腰2 */
	BODY_CHEST,               /* 胴体(胸) */
	
	BODY_BLADE_L,             /* 左肩甲骨 */
	BODY_SHOULDER1_L,         /* 左肩関節1 */
	BODY_SHOULDER2_L,         /* 左肩関節2 */
	BODY_UPPER_ARM_L,         /* 左上腕 */
	BODY_FORE_ARM_L,          /* 左前腕 */
	BODY_FORE_WRIST1_L,       /* 左手首1 */
	BODY_FORE_WRIST2_L,       /* 左手首2 */
	BODY_FORE_WRIST3_L,       /* 左手首3 */

	BODY_BLADE_R,             /* 右肩甲骨 */
	BODY_SHOULDER1_R,         /* 右肩関節1 */
	BODY_SHOULDER2_R,         /* 右肩関節2 */
	BODY_UPPER_ARM_R,         /* 右上腕 */
	BODY_FORE_ARM_R,          /* 右前腕 */
	BODY_FORE_WRIST1_R,       /* 右手首1 */
	BODY_FORE_WRIST2_R,       /* 右手首2 */
	BODY_FORE_WRIST3_R,       /* 右手首3 */
	
	BODY_NECK1,               /* 首1 */
	BODY_NECK2,               /* 首2 */
	BODY_HEAD1,               /* 頭1 */
	BODY_HEAD2,               /* 頭2 */

	BODY_MAX
}; 

/* フレームID */
enum frame_id   {
	FRAME_FOOT_POS_L,         /* 左足指令値保持 */
	FRAME_IK_ANKLE_L,         /* 左足逆キネマ計算用  */
	FRAME_IK_HIP_L,           /* 左足逆キネマ計算用 股側 */
	FRAME_FOOT_POS_R,         /* 右足指令値保持 */
	FRAME_IK_ANKLE_R,         /* 右足逆キネマ計算用 足首側 */
	FRAME_IK_HIP_R,           /* 右足逆キネマ計算用 股側 */
	
	FRAME_WAIST_POS,          /* 腰指令値保持 */
	FRAME_VIRTUAL_BODY,       /* ボディに固定された仮想座標系(i-foot想定) */
	
	FRAME_BASE,               /* 基準 */
	
	FRAME_SOLE_TOE_L,         /* 左足つま先 */
	FRAME_SOLE_HEEL_L,        /* 左足かかと */
	FRAME_SOLE_IN_L,          /* 左足内側 */
	FRAME_SOLE_OUT_L,         /* 左足外側 */
	FRAME_TOE_FR_L,           /* 足裏 */
	FRAME_TOE_RR_L,           /* 足裏 */
	FRAME_TOE_HIGH_L,         /* 足裏 */

	FRAME_SOLE_TOE_R,         /* 右足つま先 */
	FRAME_SOLE_HEEL_R,        /* 右足かかと */
	FRAME_SOLE_IN_R,          /* 右足内側 */
	FRAME_SOLE_OUT_R,         /* 右足外側 */
	FRAME_TOE_FR_R,           /* 足裏 */
	FRAME_TOE_RR_R,           /* 足裏 */
	FRAME_TOE_HIGH_R,         /* 足裏 */

	FRAME_IK_SHOULDER_L,      /* 左腕逆キネマ計算用肩側 */
	FRAME_FINGER_L,           /* 左手先位置 */
	FRAME_IK_SHOULDER_R,      /* 右腕逆キネマ計算用肩側 */
	FRAME_FINGER_R,           /* 右手先位置 */
	
	FRAME_IMU,                 /* IMU座標系 */
	
	FRAME_F6SNSR_FOOT_L,       /* 左足６軸力センサ */
	FRAME_F6SNSR_FOOT_R,       /* 右足６軸力センサ */
	FRAME_F6SNSR_HAND_L,       /* 左手先６軸力センサ */
	FRAME_F6SNSR_HAND_R,       /* 右手先６軸力センサ */

	FRAME_MAX
}; 

/* ジョイントID */
enum joint_id   {
	JOINT_HIP_PITCH_L,            /* 左股 Pitch */
	JOINT_HIP_ROLL_L,             /* 左股 Roll */
	JOINT_HIP_YAW_L,              /* 左股 Yaw */
	JOINT_KNEE_L,                 /* 左膝 */
	JOINT_ANKLE_PITCH_L,          /* 左足首 Pitch */
	JOINT_ANKLE_ROLL_L,           /* 左首 Roll */
	JOINT_TOE_PITCH_L,            /* 左つま先 */

	JOINT_HIP_PITCH_R,            /* 右股 Pitch */
	JOINT_HIP_ROLL_R,             /* 右股 Roll */
	JOINT_HIP_YAW_R,              /* 右股 Yaw */
	JOINT_KNEE_R,                 /* 右膝 */
	JOINT_ANKLE_PITCH_R,          /* 右足首 Pitch */
	JOINT_ANKLE_ROLL_R,           /* 右足首 Roll */
	JOINT_TOE_PITCH_R,            /* 右つま先 */
	
	JOINT_WAIST_ROLL,             /* 腰 Roll */
	JOINT_WAIST_PITCH,            /* 腰 Pitch */
	JOINT_WAIST_YAW,              /* 腰 Yaw */
	
	JOINT_SHOULDER_BLADE_L,       /* 左肩甲骨 Yaw */
	JOINT_SHOULDER_PITCH_L,       /* 左肩 Pitch */
	JOINT_SHOULDER_ROLL_L,        /* 左肩 Roll */
	JOINT_SHOULDER_YAW_L,         /* 右肩 Yaw */
	JOINT_ELBOW_L,                /* 左肘 */
	JOINT_WRIST_YAW_L,            /* 左手首Yaw */
	JOINT_WRIST_PITCH_L,          /* 左手首Pitch */
	JOINT_WRIST_ROLL_L,           /* 左手首Roll */

	JOINT_SHOULDER_BLADE_R,       /* 右肩甲骨 Yaw */
	JOINT_SHOULDER_PITCH_R,       /* 右肩 Pitch */
	JOINT_SHOULDER_ROLL_R,        /* 右肩 Roll */
	JOINT_SHOULDER_YAW_R,         /* 右肩 Yaw */
	JOINT_ELBOW_R,                /* 右肘 */
	JOINT_WRIST_YAW_R,            /* 右手首Yaw */
	JOINT_WRIST_PITCH_R,          /* 右手首Pitch */
	JOINT_WRIST_ROLL_R,           /* 右手首Roll */
	
	JOINT_NECK_YAW,               /* 首 Yaw */
	JOINT_NECK_PITCH,             /* 首 Pitch */
	JOINT_HEAD_PITCH,             /* 頭 Pitch */
	JOINT_HEAD_ROLL,              /* 頭 Roll */

	MAX_JOINT_NUM
};



/****************************************/
/* 部位関節ID */
/****************************************/
enum headtype{ 
	NECK_YAW,
	NECK_PITCH,
	HEAD_PITCH,
	HEAD_ROLL,

	MAX_HEAD_JOINT_NUM
} /*head_joint*/;

enum arm_ltype{ 
	SHOULDER_BLADE_L,
	SHOULDER_PITCH_L,
	SHOULDER_ROLL_L,
	SHOULDER_YAW_L,
	ELBOW_PITCH_L,
	WRIST_YAW_L,
	WRIST_PITCH_L,
	WRIST_ROLL_L,

	MAX_ARM_L_JOINT_NUM
} /*arm_l_joint*/;

enum arm_rtype{
	SHOULDER_BLADE_R,
	SHOULDER_PITCH_R,
	SHOULDER_ROLL_R,
	SHOULDER_YAW_R,
	ELBOW_PITCH_R,
	WRIST_YAW_R,
	WRIST_PITCH_R,
	WRIST_ROLL_R,
	
	MAX_ARM_R_JOINT_NUM
} /*arm_r_joint*/;

enum legtype{
	ANKLE_ROLL_L,
	ANKLE_PITCH_L,
	KNEE_L,
	HIP_PITCH_L,
	HIP_ROLL_L,
	HIP_YAW_L,
	TOE_PITCH_L,
	
	ANKLE_ROLL_R,
	ANKLE_PITCH_R,
	KNEE_R,
	HIP_PITCH_R,
	HIP_ROLL_R,
	HIP_YAW_R,
	TOE_PITCH_R,
	
	MAX_LEG_JOINT_NUM
} /*leg_joint*/;

enum waisttype{ 
	WAIST_ROLL,
	WAIST_PITCH,
	WAIST_YAW,
	
	MAX_WAIST_JOINT_NUM
} /*waist_joint*/;

const char joint_lim_names[][25] = {
	"JOINT_HIP_PITCH_L",
	"JOINT_HIP_ROLL_L",
	"JOINT_HIP_YAW_L",
	"JOINT_KNEE_L",
	"JOINT_ANKLE_PITCH_L",
	"JOINT_ANKLE_ROLL_L",
	"JOINT_TOE_L",

	"JOINT_HIP_PITCH_R",
	"JOINT_HIP_ROLL_R",
	"JOINT_HIP_YAW_R",
	"JOINT_KNEE_R",
	"JOINT_ANKLE_PITCH_R",
	"JOINT_ANKLE_ROLL_R",
	"JOINT_TOE_R",

	"JOINT_WAIST_ROLL",
	"JOINT_WAIST_PITCH",
	"JOINT_WAIST_YAW",

	"JOINT_SHOULDER_BLADE_L",
	"JOINT_SHOULDER_PITCH_L",
	"JOINT_SHOULDER_ROLL_L",
	"JOINT_SHOULDER_YAW_L",
	"JOINT_ELBOW_L",
	"JOINT_WRIST_YAW_L",
	"JOINT_WRIST_PITCH_L",
	"JOINT_WRIST_ROLL_L",

	"JOINT_SHOULDER_BLADE_R",
	"JOINT_SHOULDER_PITCH_R",
	"JOINT_SHOULDER_ROLL_R",
	"JOINT_SHOULDER_YAW_R",
	"JOINT_ELBOW_R",
	"JOINT_WRIST_YAW_R",
	"JOINT_WRIST_PITCH_R",
	"JOINT_WRIST_ROLL_R",
	
	"JOINT_NECK_YAW",
	"JOINT_NECK_PITCH",
	"JOINT_HEAD_PITCH",
	"JOINT_HEAD_ROLL"
};

#else
/****************************************/
/* モデルパラメータID */
/****************************************/
/* ボディID */
enum body_id   {
	BODY_FOOT_R,              /* 右足 */
	BODY_ANKLE_R,             /* 右足首 */
	BODY_CRURAL_R,            /* 右下腿 */
	BODY_THIGH_R,             /* 右大腿 */
	BODY_HIP1_R,              /* 右股関節1 */
	BODY_HIP2_R,              /* 右股関節2 */
	BODY_TOE_R,               /* 右つま先 */
	
	BODY_FOOT_L,              /* 左足 */
	BODY_ANKLE_L,             /* 左足首 */
	BODY_CRURAL_L,            /* 左下腿 */
	BODY_THIGH_L,             /* 左大腿 */
	BODY_HIP1_L,              /* 左股関節1 */
	BODY_HIP2_L,              /* 左股関節2 */
	BODY_TOE_L,               /* 左つま先 */
	
	BODY_WAIST,               /* 腰 */
	BODY_BODY,                /* 胴体 */
	
	BODY_SHOULDER1_R,         /* 右肩関節1 */
	BODY_SHOULDER2_R,         /* 右肩関節2 */
	BODY_UPPER_ARM_R,         /* 右上腕 */
	BODY_FORE_ARM_R,          /* 右前腕 */
	BODY_FORE_WRIST1_R,       /* 右手首1 */
	BODY_FORE_WRIST2_R,       /* 右手首2 */
	BODY_FORE_WRIST3_R,       /* 右手首3 */

	BODY_SHOULDER1_L,         /* 左肩関節1 */
	BODY_SHOULDER2_L,         /* 左肩関節2 */
	BODY_UPPER_ARM_L,         /* 左上腕 */
	BODY_FORE_ARM_L,          /* 左前腕 */
	BODY_FORE_WRIST1_L,       /* 左手首1 */
	BODY_FORE_WRIST2_L,       /* 左手首2 */
	BODY_FORE_WRIST3_L,       /* 左手首3 */
	
	BODY_HEAD,                /* 頭 */

	BODY_MAX
}; 

/* フレームID */
enum frame_id   {
	FRAME_GYRO,               /* ジャイロ計測座標系 */
	
	FRAME_FOOT_POS_R,         /* 右足指令値保持 */
	FRAME_IK_ANKLE_R,         /* 右足逆キネマ計算用 足首側 */
	FRAME_IK_HIP_R,           /* 右足逆キネマ計算用 股側 */
	FRAME_FOOT_POS_L,         /* 左足指令値保持 */
	FRAME_IK_ANKLE_L,         /* 左足逆キネマ計算用  */
	FRAME_IK_HIP_L,           /* 左足逆キネマ計算用 股側 */
	
	FRAME_WAIST_POS,          /* 腰指令値保持 */
	FRAME_VIRTUAL_BODY,       /* ボディに固定された仮想座標系(i-foot想定) */
	
	FRAME_BASE,               /* 基準 */
	
	FRAME_SOLE_TOE_R,         /* 右足つま先 */
	FRAME_SOLE_HEEL_R,        /* 右足かかと */
	FRAME_SOLE_IN_R,          /* 右足内側 */
	FRAME_SOLE_OUT_R,         /* 右足外側 */
	FRAME_TOE_FR_R,           /* 足裏 */
	FRAME_TOE_RR_R,           /* 足裏 */
	FRAME_TOE_HIGH_R,         /* 足裏 */
	
	FRAME_SOLE_TOE_L,         /* 左足つま先 */
	FRAME_SOLE_HEEL_L,        /* 左足かかと */
	FRAME_SOLE_IN_L,          /* 左足内側 */
	FRAME_SOLE_OUT_L,         /* 左足外側 */
	FRAME_TOE_FR_L,           /* 足裏 */
	FRAME_TOE_RR_L,           /* 足裏 */
	FRAME_TOE_HIGH_L,         /* 足裏 */
	
	FRAME_IK_SHOULDER_R,      /* 右腕逆キネマ計算用肩側 将来拡張用 */
	FRAME_FINGER_R,           /* 右手先位置 */
	FRAME_IK_SHOULDER_L,      /* 左腕逆キネマ計算用肩側 将来拡張用 */
	FRAME_FINGER_L,           /* 左手先位置 */
	
	FRAME_IMU,                /* IMU座標系 */
	
	FRAME_F6SNSR_FOOT_R,       /* 右足６軸力センサ */
	FRAME_F6SNSR_FOOT_L,       /* 左足６軸力センサ */
	FRAME_F6SNSR_HAND_R,       /* 右手先６軸力センサ */
	FRAME_F6SNSR_HAND_L,       /* 左手先６軸力センサ */

	FRAME_MAX
}; 

/* ジョイントID */
enum joint_id   {
	JOINT_ANKLE_ROLL_R,           /* 右足首 Roll */
	JOINT_ANKLE_PITCH_R,          /* 右足首 Pitch */
	JOINT_KNEE_R,                 /* 右膝 */
	JOINT_HIP_PITCH_R,            /* 右股 Pitch */
	JOINT_HIP_ROLL_R,             /* 右股 Roll */
	JOINT_HIP_YAW_R,              /* 右股 Yaw */
	JOINT_TOE_PITCH_R,            /* 右つま先 */
	
	JOINT_ANKLE_ROLL_L,           /* 左首 Roll */
	JOINT_ANKLE_PITCH_L,          /* 左足首 Pitch */
	JOINT_KNEE_L,                 /* 左膝 */
	JOINT_HIP_PITCH_L,            /* 左股 Pitch */
	JOINT_HIP_ROLL_L,             /* 左股 Roll */
	JOINT_HIP_YAW_L,              /* 左股 Yaw */
	JOINT_TOE_PITCH_L,            /* 左つま先 */
	
	JOINT_WAIST,                  /* 腰 */
	
	JOINT_SHOULDER_PITCH_R,       /* 右肩 Pitch */
	JOINT_SHOULDER_ROLL_R,        /* 右肩 Roll */
	JOINT_SHOULDER_YAW_R,         /* 右肩 Yaw */
	JOINT_ELBOW_R,                /* 右肘 */
	JOINT_WRIST_YAW_R,            /* 右手首Yaw */
	JOINT_WRIST_PITCH_R,          /* 右手首Pitch */
	JOINT_WRIST_ROLL_R,           /* 右手首Roll */
	
	JOINT_SHOULDER_PITCH_L,       /* 左肩 Pitch */
	JOINT_SHOULDER_ROLL_L,        /* 左肩 Roll */
	JOINT_SHOULDER_YAW_L,         /* 右肩 Yaw */
	JOINT_ELBOW_L,                /* 左肘 */
	JOINT_WRIST_YAW_L,            /* 左手首Yaw */
	JOINT_WRIST_PITCH_L,          /* 左手首Pitch */
	JOINT_WRIST_ROLL_L,           /* 左手首Roll */
	
	JOINT_NECK_YAW,               /* 首 Yaw */

	MAX_JOINT_NUM
};



/****************************************/
/* 部位関節ID */
/****************************************/
enum headtype{ 
	NECK_YAW,
	
	MAX_HEAD_JOINT_NUM
} /*head_joint*/;

enum arm_rtype{
	SHOULDER_PITCH_R,
	SHOULDER_ROLL_R,
	SHOULDER_YAW_R,
	ELBOW_PITCH_R,
	WRIST_YAW_R,
	WRIST_PITCH_R,
	WRIST_ROLL_R,
	
	MAX_ARM_R_JOINT_NUM
} /*arm_r_joint*/;

enum arm_ltype{ 
	SHOULDER_PITCH_L,
	SHOULDER_ROLL_L,
	SHOULDER_YAW_L,
	ELBOW_PITCH_L,
	WRIST_YAW_L,
	WRIST_PITCH_L,
	WRIST_ROLL_L,
	
	MAX_ARM_L_JOINT_NUM
} /*arm_l_joint*/;

enum legtype{ // ダブルモータ部は排除した
	ANKLE_ROLL_R,
	ANKLE_PITCH_R,
	KNEE_R,
	HIP_PITCH_R,
	HIP_ROLL_R,
	HIP_YAW_R,
	TOE_PITCH_R,
	
	ANKLE_ROLL_L,
	ANKLE_PITCH_L,
	KNEE_L,
	HIP_PITCH_L,
	HIP_ROLL_L,
	HIP_YAW_L,
	TOE_PITCH_L,
	
	MAX_LEG_JOINT_NUM
} /*leg_joint*/;

enum waisttype{ 
	WAIST_YAW,
	
	MAX_WAIST_JOINT_NUM
} /*waist_joint*/;
#endif

#endif   /* #ifndef __ROBOT_ID_H__ */
