#ifndef CIPCOMM_H
#define CIPCOMM_H

#include "ihydraipc.h"
#include "hydra_types.h"

class CIPComm : public IHydraIPC
{
public:
    CIPComm()
    {
    };

    virtual ~CIPComm()
    {

    };

    void SetTargetAddress(const char tgt_addr[])
    {
        strcpy(p_tgt_addr, tgt_addr);
    }
    void GetTargetAddress(char tgt_addr[])
    {
        strcpy(tgt_addr, p_tgt_addr);
    }
    void SetPortBase(int port)
    {
        port_base = port;
    }
    void GetPortBase(int *port)
    {
        *port = port_base;
    }
    void SetTargetPort(int port)
    {
        target_port = port;
    }
    void GetTargetPort(int *port)
    {
        *port = target_port;
    }

    virtual int Init(void)=0;
    virtual void Close(void)=0;

    virtual int ReadStatus(joint_state_t jnt_state[], eha_state_t eha_state[], sensor_state_t sensor_state[])
    {
        return 0;
    }
    virtual int ReadCommand(joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t sensor_cmd[])
    {
        return 0;
    }

    virtual int WriteStatus(const joint_state_t jnt_state[], const eha_state_t eha_state[], const sensor_state_t sensor_state[])
    {
        return 0;
    }

    virtual int WriteCommand(const joint_cmd_t jnt_cmd[], const eha_cmd_t eha_cmd[], const sensor_cmd_t sensor_cmd[])
    {
        return 0;
    }

    virtual int Sync(void)
    {
        return 0;
    }

protected:
    char p_tgt_addr[16];
    int  port_base;
    int  target_port;
};

#endif // CIPCOMM_H
