#ifndef CHYDRASHMSERVER_H
#define CHYDRASHMSERVER_H

#include "hydra_types.h"
#include <semaphore.h>
#include "chydrashm.h"


// 共有メモリは/tmp/shm配下にRAMDISKを作成して用いる
// RAMDISKは、「/etc/fstab」に以下を記載して作成。
// tmpfs /tmp/shm tmpfs defaults,size=64m 0 0

class CHydraShmServer : public CHydraShm
{
public:
    ~CHydraShmServer();
    int ReadCommand(joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t senosr_cmd[]);
    int ReadCommand(int idx, joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t senosr_cmd[]);
    int ReadCommandAsLog(int idx, joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t senosr_cmd[]);
    int WriteStatus(const joint_state_t jnt_state[], const eha_state_t eha_state[], const sensor_state_t sensor_state[]);
    int ReadStatus(int idx, joint_state_t jnt_state[], eha_state_t eha_state[], sensor_state_t sensor_state[]);
    int ReadStatus(joint_state_t jnt_state[], eha_state_t eha_state[], sensor_state_t sensor_state[]);
    int WriteCommandAsLog(const joint_cmd_t jnt_cmd[], const eha_cmd_t eha_cmd[], const sensor_cmd_t sensor_cmd[]);
    int PostSemaphore(void); // 20160401 okamoto
    int Sync(void);
    int GetSyncStatus(void);

    int  Init(void);
    void Close(void);
    enum {
        E_SEM_UNLK = -1,
        E_SEM_INVLD_NUM = -2,
        E_SHM_NOT_EXIST = -3
    } e_sync_err;

private:
};

#endif // CHYDRASHMSERVER_H
