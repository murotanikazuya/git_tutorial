#ifndef CHYDRASHM_H
#define CHYDRASHM_H

#include "hydra_types.h"
#include "ihydraipc.h"
#include <semaphore.h>

#define DEF_SEM_ECATSYNC_NAME 		("/EcatSync")
#define DEF_SHM_IN_NAME				("/tmp/shm/EcatIn")
#define DEF_SHM_OUT_NAME			("/tmp/shm/EcatOut")

class CHydraShm : public IHydraIPC
{
public:
    CHydraShm()
    {
        sfd_in        = -1;
        sfd_out       = -1;

        pShmIn_MD4KW  = NULL;
        pShmOut_MD4KW = NULL;
        mmapInSize    = 0;
        mmapOutSize   = 0;
        semSync       = NULL;

        strcpy(p_name_shmin, DEF_SHM_IN_NAME);
        strcpy(p_name_shmout, DEF_SHM_OUT_NAME);
        strcpy(p_name_sem, DEF_SEM_ECATSYNC_NAME);
    }

    CHydraShm(const char name_shmin[], const char name_shmout[], const char name_sem[])
    {
        sfd_in        = -1;
        sfd_out       = -1;
        pShmIn_MD4KW  = NULL;
        pShmOut_MD4KW = NULL;
        mmapInSize    = 0;
        mmapOutSize   = 0;
        semSync       = NULL;

        strcpy(p_name_shmin, name_shmin);
        strcpy(p_name_shmout, name_shmout);
        strcpy(p_name_sem, name_sem);
    }

    virtual ~CHydraShm()
    {
    }

    virtual int Init(void)=0;
    virtual void Close(void)=0;

    virtual int ReadStatus(joint_state_t jnt_state[], eha_state_t eha_state[], sensor_state_t sensor_state[])
    {
        return 0;
    }
    virtual int ReadStatus(int idx, joint_state_t jnt_state[], eha_state_t eha_state[], sensor_state_t sensor_state[])
    {
        return 0;
    }
    virtual int ReadCommand(joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t sensor_cmd[])
    {
        return 0;
    }
    virtual int ReadCommand(int idx, joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t sensor_cmd[])
    {
        return 0;
    }

    virtual int ReadCommandAsLog(joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t senosr_cmd[])
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


    virtual int WriteCommandAsLog(const joint_cmd_t jnt_cmd[], const eha_cmd_t eha_cmd[], const sensor_cmd_t sensor_cmd[])
    {
        return 0;
    }

    virtual int Sync(void)
    {
        return 0;
    }

    virtual int GetSyncStatus(void)
    {
        return 0;
    }

    virtual void SetTimeInfo(int time_msec)
    {
        SHM_MSEC(1) = time_msec;
        return;
    }
    //virtual int GetTimeInfo(int idx)
    virtual int GetTimeInfo(void)
    {
        //return SHM_MSEC(idx);
        return SHM_MSEC(0);
    }

    virtual int GetTimeInfo(int idx)
    {
        return pShmIn_MD4KW->Acc[idx % SHM_ACCNUM_MAX].iMsec;
    }

    void SetShmInName(const char name_shmin[])
    {
        strcpy(p_name_shmin, name_shmin);
    }

    void SetShmOutName(const char name_shmout[])
    {
        strcpy(p_name_shmout, name_shmout);
    }

    // set path of named semaphore
    void SetSemName(const char name_sem[])
    {
        strcpy(p_name_sem, name_sem);
    }

    void GetShmInName(char name_shmin[])
    {
        strcpy(name_shmin, p_name_shmin);
    }

    void GetShmOutName(char name_shmout[])
    {
        strcpy(name_shmout, p_name_shmout);
    }

    // set path of named semaphore
    void GetSemName(char name_sem[])
    {
        strcpy(name_sem, p_name_sem);
    }


    T_SHM_INPUT_MD4KW *GetShmInPtr(void)
    {
        return pShmIn_MD4KW;
    }

    T_SHM_OUTPUT_MD4KW *GetShmOutPtr(void)
    {
        return pShmOut_MD4KW;
    }

    int GetShmOutIdx(void)
    {
        return pShmOut_MD4KW->Idx;
    }

    int GetShmInIdx(void)
    {
        return pShmIn_MD4KW->Idx;
    }

    int GetSDOResNo(void)
    {
        return SDORES_NO;
    }

    int GetSDOReqNo(void)
    {
        return SDOREQ_NO;
    }


    T_SDO_PARAM *GetSDOParamPtr(void)
    {
        return &(SDOREQ(SDORES_NO));
    }

    void SetSDORetVal(unsigned int val)
    {
        SDORES_RET(SDORES_NO) = val;
    }

    void UpdateSDONo(void)
    {
        SDONO_UPDATE(SDORES_NO);
    }

protected:
    // shared memory
    char p_name_shmin[256];
    char p_name_shmout[256];
    char p_name_sem[256];

    int  sfd_in;
    int  sfd_out;
    T_SHM_INPUT_MD4KW *pShmIn_MD4KW;
    T_SHM_OUTPUT_MD4KW *pShmOut_MD4KW;
    long   mmapInSize;
    long   mmapOutSize;

    sem_t *semSync;
};

#endif // CHYDRASHM_H
