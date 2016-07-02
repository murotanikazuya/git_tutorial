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

#if 1 // user_interface.hから移動 okamoto
const char joint_hydra_names[][25] = {
  "joint_rhip_yaw",
  "joint_rhip_pitch",
  "joint_rhip_roll", 
  "joint_rknee_pitch",
  "joint_rankle_roll",
  "joint_rankle_pitch",
  "joint_lhip_yaw",
  "joint_lhip_pitch",
  "joint_lhip_roll",
  "joint_lknee_pitch",
  "joint_lankle_roll",
  "joint_lankle_pitch",
  "joint_waist_pitch",
  "joint_waist_roll",
  "joint_neck_pitch",
  "joint_rscaplae_yaw",
  "joint_rshoulder_pitch", 
  "joint_rshoulder_roll" , 
  "joint_rshoulder_yaw",   
  "joint_relbow_pitch" ,   
  "joint_rwrist_yaw",      
  "joint_rwrist_roll",    
  "joint_rwrist_pitch",    
  "joint_lscaplae_yaw",    
  "joint_lshoulder_pitch", 
  "joint_lshoulder_roll",  
  "joint_lshoulder_yaw",   
  "joint_lelbow_pitch",
  "joint_lwrist_yaw",      
  "joint_lwrist_roll",     
  "joint_lwrist_pitch",    
  "joint_rhand_thb_AA",    
  "joint_rhand_thb_FE",    
  "joint_rhand_idx",       
  "joint_rhand_mid",       
  "joint_rhand_rng",       
  "joint_lhand_thb_AA",
  "joint_lhand_thb_FE",    
  "joint_lhand_idx",       
  "joint_lhand_mid", 
  "joint_lhand_rng"};
#endif

#endif   /* #ifndef __ROBOT_HYDRA_ID_H__ */
