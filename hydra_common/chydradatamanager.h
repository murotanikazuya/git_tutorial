#ifndef CHYDRADATAMANAGER_H
#define CHYDRADATAMANAGER_H

#include "hydra_types.h"
#include <assert.h>
#define BUF_IDX_MAX 1

#define DEBUG

class CHydraDataManager
{
public:
    CHydraDataManager();

    joint_state_t *GetJointStatePtr(int offset)
    {
        return &(jnt_state[(buffer_index+offset)%BUF_IDX_MAX][0]);
    }

    joint_cmd_t *GetJointCmdPtr(int offset)
    {
        return &(jnt_cmd[(buffer_index+offset)%BUF_IDX_MAX][0]);
    }

    eha_state_t *GetEHAStatePtr(int offset)
    {
        return &(eha_state[(buffer_index+offset)%BUF_IDX_MAX][0]);
    }

    eha_cmd_t *GetEHACmdPtr(int offset)
    {
        return &(eha_cmd[(buffer_index+offset)%BUF_IDX_MAX][0]);
    }

    sensor_state_t *GetSensorStatePtr(int offset)
    {
        return &(sensor_state[(buffer_index+offset)%BUF_IDX_MAX][0]);
    }

    sensor_cmd_t *GetSensorCmdPtr(int offset)
    {
        return &(sensor_cmd[(buffer_index+offset)%BUF_IDX_MAX][0]);
    }

    void IncrementIndex(void)
    {
        buffer_index= (buffer_index + 1)%BUF_IDX_MAX;
    }

    int GetCurrentIndex(void)
    {
        return buffer_index;
    }

private:
    joint_state_t   jnt_state[BUF_IDX_MAX][HYDRA_JNT_MAX+HYDRA_HAND_JNT_MAX];
    joint_cmd_t     jnt_cmd[BUF_IDX_MAX][HYDRA_JNT_MAX];
    eha_state_t     eha_state[BUF_IDX_MAX][EHA_MAX];
    eha_cmd_t       eha_cmd[BUF_IDX_MAX][EHA_MAX];
    sensor_state_t  sensor_state[BUF_IDX_MAX][1];
    sensor_cmd_t    sensor_cmd[BUF_IDX_MAX][1];

    int            buffer_index;
};

#endif // CHYDRADATAMANAGER_H
