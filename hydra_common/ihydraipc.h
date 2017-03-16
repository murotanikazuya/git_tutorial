#ifndef IHYDRAIPC_H
#define IHYDRAIPC_H

#include "hydra_types.h"

class IHydraIPC
{
public:
    IHydraIPC()
    {
    }

    virtual ~IHydraIPC()
    {
    }

    virtual int Init(void)=0;
    virtual void Close(void)=0;

    virtual int ReadStatus(joint_state_t jnt_state[], eha_state_t eha_state[], sensor_state_t sensor_state[])=0;
    virtual int ReadCommand(joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t senosr_cmd[])=0;
    virtual int ReadCommandAsLog(joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t senosr_cmd[])=0;

    virtual int WriteStatus(const joint_state_t jnt_state[], const eha_state_t eha_state[], const sensor_state_t sensor_state[])=0;
    virtual int WriteCommand(const joint_cmd_t jnt_cmd[], const eha_cmd_t eha_cmd[], const sensor_cmd_t sensor_cmd[])=0;
    virtual int WriteCommandAsLog(const joint_cmd_t jnt_cmd[], const eha_cmd_t eha_cmd[], const sensor_cmd_t sensor_cmd[])=0;

    virtual int Sync(void)=0;
    virtual int GetSyncStatus(void)=0;
};

#endif // CHYDRAIPC_H
