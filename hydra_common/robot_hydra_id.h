/*
  $id$
 */
#ifndef __ROBOT_HYDRA_ID_H__
#define __ROBOT_HYDRA_ID_H__

/****************************************/
/* モデルパラメータID */
/****************************************/
/* ジョイントID */
enum joint_hydra_id   {
    JOINT_HYDRA_RHIP_YAW,
    JOINT_HYDRA_RHIP_PITCH,
    JOINT_HYDRA_RHIP_ROLL,
    JOINT_HYDRA_RKNEE_PITCH,
    JOINT_HYDRA_RANKLE_ROLL,
    JOINT_HYDRA_RANKLE_PITCH,
    JOINT_HYDRA_LHIP_YAW,
    JOINT_HYDRA_LHIP_PITCH,
    JOINT_HYDRA_LHIP_ROLL,
    JOINT_HYDRA_LKNEE_PITCH,
    JOINT_HYDRA_LANKLE_ROLL,
    JOINT_HYDRA_LANKLE_PITCH,
    JOINT_HYDRA_WAIST_PITCH,
    JOINT_HYDRA_WAIST_ROLL,
    JOINT_HYDRA_NECK_PITCH,

    JOINT_HYDRA_RSCAPLAE_YAW,
    JOINT_HYDRA_RSHOULDER_PITCH,
    JOINT_HYDRA_RSHOULDER_ROLL,
    JOINT_HYDRA_RSHOULDER_YAW,
    JOINT_HYDRA_RELBOW_PITCH,
    JOINT_HYDRA_RWRIST_YAW,
    JOINT_HYDRA_RWRIST_ROLL,
    JOINT_HYDRA_RWRIST_PITCH,

    JOINT_HYDRA_LSCAPLAE_YAW,
    JOINT_HYDRA_LSHOULDER_PITCH,
    JOINT_HYDRA_LSHOULDER_ROLL,
    JOINT_HYDRA_LSHOULDER_YAW,
    JOINT_HYDRA_LELBOW_PITCH,
    JOINT_HYDRA_LWRIST_YAW,
    JOINT_HYDRA_LWRIST_ROLL,
    JOINT_HYDRA_LWRIST_PITCH,

    JOINT_HYDRA_RHAND_THB_AA,
    JOINT_HYDRA_RHAND_THB_FE,
    JOINT_HYDRA_RHAND_IDX,
    JOINT_HYDRA_RHAND_MID,
    JOINT_HYDRA_RHAND_RNG,
    JOINT_HYDRA_LHAND_THB_AA,
    JOINT_HYDRA_LHAND_THB_FE,
    JOINT_HYDRA_LHAND_IDX,
    JOINT_HYDRA_LHAND_MID,
    JOINT_HYDRA_LHAND_RNG,

    JOINT_HYDRA_MAX
}; 

const char joint_hydra_names[][25] = {
  "rhip_y",
  "rhip_p",
  "rhip_r",
  "rknee_p",
  "rankle_r",
  "rankle_p",
  "lhip_y",
  "lhip_p",
  "lhip_r",
  "lknee_p",
  "lankle_r",
  "lankle_p",
  "waist_p",
  "waist_r",
  "neck_p",
  "rscaplae_y",
  "rshoulder_p",
  "rshoulder_r" ,
  "rshoulder_y",
  "relbow_p" ,
  "rwrist_y",
  "rwrist_r",
  "rwrist_p",
  "lscaplae_y",
  "lshoulder_p",
  "lshoulder_r",
  "lshoulder_y",
  "lelbow_p",
  "lwrist_y",
  "lwrist_r",
  "lwrist_p",
  "rhand_thb_AA",
  "rhand_thb_FE",
  "rhand_idx",
  "rhand_mid",
  "rhand_rng",
  "lhand_thb_AA",
  "lhand_thb_FE",
  "lhand_idx",
  "lhand_mid",
  "lhand_rng"};

const char eha_names[][25] = {
    "EHA_rhip_vane",
    "EHA_rhip_cyl1",
    "EHA_rhip_cyl2",
    "EHA_rknee_single",
    "EHA_rknee_tandem1",
    "EHA_rknee_tandem2",
    "EHA_rankle_cyl1",
    "EHA_rankle_cyl2",
    "EHA_lhip_vane",
    "EHA_lhip_cyl1",
    "EHA_lhip_cyl2",
    "EHA_lknee_single",
    "EHA_lknee_tandem1",
    "EHA_lknee_tandem2",
    "EHA_lankle_cyl1",
    "EHA_lankle_cyl2",
    "EHA_waist1_cyl1",
    "EHA_waist1_cyl2",
    "EHA_neck",
    "EHA_rshoulder_vane",
    "EHA_rshoulder_cyl1",
    "EHA_rshoulder_cyl2",
    "EHA_relbow_vane",
    "EHA_relbow_tandem1",
    "EHA_relbow_tandem2",
    "EHA_rwrist_vane",
    "EHA_rwrist_cyl1",
    "EHA_rwrist_cyl2",
    "EHA_lshoulder_vane",
    "EHA_lshoulder_cyl1",
    "EHA_lshoulder_cyl2",
    "EHA_lelbow_vane",
    "EHA_lelbow_tandem1",
    "EHA_lelbow_tandem2",
    "EHA_lwrist_vane",
    "EHA_lwrist_cyl1",
    "EHA_lwrist_cyl2",
    "EHA_rhand_thb_AA",
    "EHA_rhand_thb_FE",
    "EHA_rhand_idx",
    "EHA_rhand_mid",
    "EHA_rhand_rng",
    "EHA_lhand_thb_AA",
    "EHA_lhand_thb_FE",
    "EHA_lhand_idx",
    "EHA_lhand_mid",
    "EHA_lhand_rng"
};

const int joint_to_joint_power[][6] = {
        {1, 0, -1,-1,-1,-1},
        {2, 1,  2,-1,-1,-1},
        {2, 1,  2,-1,-1,-1},
        {1, 3, -1,-1,-1,-1},
        {2, 4,  5,-1,-1,-1},
        {2, 4,  5,-1,-1,-1},
        {1, 6, -1,-1,-1,-1},
        {2, 7,  8,-1,-1,-1},
        {2, 7,  8,-1,-1,-1},
        {1, 9, -1,-1,-1,-1},
        {2, 10,11,-1,-1,-1},
        {2, 10,11,-1,-1,-1},
        {2, 12,13,-1,-1,-1},
        {2, 12,13,-1,-1,-1},
        {1, 14,-1,-1,-1,-1},
        {1, 15,-1,-1,-1,-1},
        {2, 16,17,-1,-1,-1},
        {2, 16,17,-1,-1,-1},
        {1, 18,-1,-1,-1,-1},
        {1, 19,-1,-1,-1,-1},
        {1, 20,-1,-1,-1,-1},
        {2, 21,22,-1,-1,-1},
        {2, 21,22,-1,-1,-1},
        {1, 23,-1,-1,-1,-1},
        {2, 24,25,-1,-1,-1},
        {2, 24,25,-1,-1,-1},
        {1, 26,-1,-1,-1,-1},
        {1, 27,-1,-1,-1,-1},
        {1, 28,-1,-1,-1,-1},
        {2, 29,30,-1,-1,-1},
        {2, 29,30,-1,-1,-1},
        {1, 31,-1,-1,-1,-1},
        {1, 32,-1,-1,-1,-1},
        {1, 33,-1,-1,-1,-1},
        {1, 34,-1,-1,-1,-1},
        {1, 35,-1,-1,-1,-1},
        {1, 36,-1,-1,-1,-1},
        {1, 37,-1,-1,-1,-1},
        {1, 38,-1,-1,-1,-1},
        {1, 39,-1,-1,-1,-1},
        {1, 40,-1,-1,-1,-1}
};

const int joint_to_EHA_power[][6] = {
        {1, 0, -1,-1,-1,-1},//rhip_yaw
        {2, 1,  2,-1,-1,-1},//rhip_pitch
        {2, 1,  2,-1,-1,-1},//rhip_roll
        {3, 3,  4, 5,-1,-1},//rknee
        {2, 6,  7,-1,-1,-1},//rankle_roll
        {2, 6,  7,-1,-1,-1},//rankle_pitch
        {1, 8, -1,-1,-1,-1},//lhip_yaw
        {2, 9, 10,-1,-1,-1},//lhip_pitch
        {2, 9, 10,-1,-1,-1},//lhip_roll
        {3, 11,12,13,-1,-1},//lknee
        {2, 14,15,-1,-1,-1},//lankle_roll
        {2, 14,15,-1,-1,-1},//lankle_pitch
        {2, 16,17,-1,-1,-1},//waist_pitch
        {2, 16,17,-1,-1,-1},//waist_roll
        {1, 18,-1,-1,-1,-1},//neck_pitch
        {1, 19,-1,-1,-1,-1},//rscaplae_yaw
        {2, 20,21,-1,-1,-1},//rshoulder_pitch
        {2, 20,21,-1,-1,-1},//rshoulder_roll
        {1, 22,-1,-1,-1,-1},//rshoulder_yaw
        {2, 23,24,-1,-1,-1},//relbow
        {1, 25,-1,-1,-1,-1},//rwrist_yaw
        {2, 26,27,-1,-1,-1},//rwrist_roll
        {2, 26,27,-1,-1,-1},//rwrist_pitch
        {1, 28,-1,-1,-1,-1},//lscaplae_yaw
        {2, 29,30,-1,-1,-1},//lshoulder_pitch
        {2, 29,30,-1,-1,-1},//lshoulder_roll
        {1, 31,-1,-1,-1,-1},//lshoulder_yaw
        {2, 32,33,-1,-1,-1},
        {1, 34,-1,-1,-1,-1},
        {2, 35,36,-1,-1,-1},
        {2, 35,36,-1,-1,-1},
        {1, 37,-1,-1,-1,-1},
        {1, 38,-1,-1,-1,-1},
        {1, 39,-1,-1,-1,-1},
        {1, 40,-1,-1,-1,-1},
        {1, 41,-1,-1,-1,-1},
        {1, 42,-1,-1,-1,-1},
        {1, 43,-1,-1,-1,-1},
        {1, 44,-1,-1,-1,-1},
        {1, 45,-1,-1,-1,-1},
        {1, 46,-1,-1,-1,-1}
};

#endif   /* #ifndef __ROBOT_HYDRA_ID_H__ */
