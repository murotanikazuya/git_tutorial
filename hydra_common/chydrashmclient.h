#ifndef CHYDRASHMCLIENT_H
#define CHYDRASHMCLIENT_H

#include "hydra_types.h"
#include <semaphore.h>
#include "chydrashm.h"
#include "chydradatamanager.h"
#include "chydradata.h"

class CHydraShmClient : public CHydraShm
{
public:
    ~CHydraShmClient();
    int ReadStatus(joint_state_t jnt_state[], eha_state_t eha_state[], sensor_state_t sensor_state[]);
    int ReadStatus(CHydraDataManager* hydraData);
    int ReadStatus(CHydraData* hydraData);
    int WriteCommand(const joint_cmd_t jnt_cmd[], const eha_cmd_t eha_cmd[], const sensor_cmd_t sensor_cmd[]);
    int WriteCommand(CHydraDataManager* hydraData);
    int WriteCommand(CHydraData* hydraData);
    int WaitSemaphore(void);
    int Sync(void);

    int  Init(void);
    void Close(void);

};

#endif // CHYDRASHMCLIENT_H
