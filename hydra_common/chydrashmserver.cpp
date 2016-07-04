#include "chydrashmserver.h"

#include <string.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


CHydraShmServer::~CHydraShmServer()
{
    if(
            (pShmIn_MD4KW  != NULL) ||
            (pShmOut_MD4KW != NULL) ||
            (semSync       != NULL) )
    {
        Close();
    }
}

int CHydraShmServer::Init(void)
{
    long psize;
    char tmp;

    // open sahred memory on RAM disk
    sfd_in  = open(p_name_shmin, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);
    sfd_out = open(p_name_shmout, O_CREAT | O_TRUNC | O_RDWR, S_IRWXU);

    if((sfd_in == -1) || (sfd_out == -1)) {
        fprintf(stderr, "ERROR: sfd open NG!\n");
        return -1;
    }

    // calculate size aligned to the page size
    psize 		= sysconf(_SC_PAGE_SIZE);
    mmapInSize	= (sizeof(T_SHM_INPUT_MD4KW) / psize + 1) * psize;

    // seek and write 0 to shm to make the file at necessary size
    if(lseek(sfd_in, mmapInSize, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: sfd-in lseek NG!\n");
        return -1;
    }
    tmp ='\0';
    if(write(sfd_in,&tmp,sizeof(char)) == -1) {
        fprintf(stderr, "ERROR: sfd-in write NG!\n");
        return -1;
    }

    // calculate size aligned to the page size
    psize 		= sysconf(_SC_PAGE_SIZE);
    mmapOutSize	= (sizeof(T_SHM_OUTPUT_MD4KW) / psize + 1) * psize;

    // seek and write 0 to shm to make the file at necessary size
    if(lseek(sfd_out, mmapOutSize, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: sfd-out lseek NG!\n");
        return -1;
    }
    tmp ='\0';
    if(write(sfd_out,&tmp,sizeof(char)) == -1) {
        fprintf(stderr, "ERROR: sfd-out write NG!\n");
        return -1;
    }

    // change permission so other uses can read/write shared memory
    chmod(p_name_shmin,  S_IRUSR|S_IWUSR| S_IRGRP|S_IWGRP| S_IROTH|S_IWOTH);
    chmod(p_name_shmout, S_IRUSR|S_IWUSR| S_IRGRP|S_IWGRP| S_IROTH|S_IWOTH);

    pShmIn_MD4KW  = (T_SHM_INPUT_MD4KW *) mmap(0, mmapInSize,  PROT_READ | PROT_WRITE, MAP_SHARED, sfd_in, 0);
    pShmOut_MD4KW = (T_SHM_OUTPUT_MD4KW *)mmap(0, mmapOutSize, PROT_READ,  MAP_SHARED, sfd_out, 0);

    if((pShmIn_MD4KW == MAP_FAILED) || (pShmOut_MD4KW == MAP_FAILED)) {
        fprintf(stderr, "ERROR: mmap NG!\n");
        return -1;
    }

    // open named semaphore for IPC synchronization
    semSync = sem_open( p_name_sem, O_CREAT, 0777, 0 );
    if(semSync == SEM_FAILED)
    {
        semSync = NULL;
        perror("sem open failed");
        return -1;
    }

    return 0;
}

void CHydraShmServer::Close()
{
    int ret;

    if(semSync!= NULL)
    {
        ret = sem_close( semSync );
        if (ret == -1)
        {
            perror("sem_close NG");
        }

#if 0 // 他でOpenされていてもなぜか削除されてしまうため、
	  // Server側だけunlinkをコメントアウト 20160401 okamoto
 		sem_unlink( p_name_sem );
        if (ret == -1)
        {
            perror("sem_unlink NG");
        }
#endif
    }

    // release shared memory
    if(pShmIn_MD4KW != NULL) {
        munmap(pShmIn_MD4KW, mmapInSize);
        pShmIn_MD4KW = NULL;
    }
    if(pShmOut_MD4KW != NULL) {
        munmap(pShmOut_MD4KW, mmapOutSize);
        pShmOut_MD4KW = NULL;
    }

    // close shared memory file descriptors
    if(sfd_in >= 0) {
        close(sfd_in);
        sfd_in = -1;
    }
    if(sfd_out >= 0) {
        close(sfd_out);
        sfd_out = -1;
    }
}

int CHydraShmServer::ReadCommand(joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t senosr_cmd[])
{
    for(int i=0; i<HYDRA_JNT_MAX; i++){
        jnt_cmd[i].DATA.pos_ref = SHM_HYDRA_JOINT_REFPOS_OUT(0, i);
    }

    for(int i=0; i<EHA_MAX; i++) {
        eha_cmd[i].DATA.ctlword = SHM_HYDRA_EHA_CTRLWORD_OUT(0, i);
//        eha_state[i].DATA.stsword = SHM_HYDRA_EHA_CTRLWORD_OUT(0, i);
    }
    return 0;
}

int CHydraShmServer::ReadCommand(int idx, joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t sensor_cmd[])
{
    for(int i=0; i<HYDRA_JNT_MAX; i++){
        jnt_cmd[i].DATA.pos_ref = SHM_HYDRA_JOINT_REFPOS_OUT(idx, i);
        jnt_cmd[i].DATA.tau_ref = SHM_HYDRA_JOINT_REFTAU_OUT(idx, i);
        jnt_cmd[i].DATA.vel_ref = SHM_HYDRA_JOINT_REFVEL_OUT(idx, i);
        jnt_cmd[i].DATA.enable = SHM_HYDRA_JOINT_CTRLMODE_OUT(idx, i);
    }

    for(int i=0; i<EHA_MAX; i++) {
        eha_cmd[i].DATA.ctlword = SHM_HYDRA_EHA_CTRLWORD_OUT(idx, i);
        eha_cmd[i].DATA.pos_ref = SHM_HYDRA_EHA_REFPOS_OUT(idx, i);
        eha_cmd[i].DATA.vel_ref = SHM_HYDRA_EHA_REFVEL_OUT(idx, i);
        eha_cmd[i].DATA.tau_ref = SHM_HYDRA_EHA_REFTAU_OUT(idx, i);
//        eha_state[i].DATA.stsword = SHM_HYDRA_EHA_CTRLWORD_OUT(0, i);
    }

    sensor_cmd[0].DATA.ft_sensor[0].ctlword = SHM_HYDRA_FS_CTRLWORD_OUT(idx, 0);
    sensor_cmd[0].DATA.ft_sensor[1].ctlword = SHM_HYDRA_FS_CTRLWORD_OUT(idx, 1);
    sensor_cmd[0].DATA.imu[0].ctlword       = SHM_HYDRA_IMU_CTRLWORD_OUT(idx, 0);
    return 0;
}
int CHydraShmServer::WriteStatus(const joint_state_t jnt_state[], const eha_state_t eha_state[], const sensor_state_t sensor_state[])
{
    for(int i=0; i<HYDRA_JNT_MAX; i++) {
        SHM_HYDRA_JOINT_POS_IN(1, i)    = jnt_state[i].DATA.pos_act;
        SHM_HYDRA_JOINT_VEL_IN(1, i)    = jnt_state[i].DATA.vel_act;
        SHM_HYDRA_JOINT_TAU_IN(1, i)    = jnt_state[i].DATA.tau_act;
        SHM_HYDRA_JOINT_STATUS_IN(1, i) = jnt_state[i].DATA.enabled;
    }
    for(int i=0; i<HYDRA_HAND_JNT_MAX; i++) {
        SHM_HYDRA_HAND_POS_IN(1, i)    = jnt_state[i+HYDRA_JNT_MAX].DATA.pos_act;
        SHM_HYDRA_HAND_VEL_IN(1, i)    = jnt_state[i+HYDRA_JNT_MAX].DATA.vel_act;
    }
    for(int i=0; i<EHA_MAX; i++) {
        SHM_HYDRA_EHA_POS_IN(1, i)    = eha_state[i].DATA.pos_act;
        SHM_HYDRA_EHA_STATUS_IN(1, i) = eha_state[i].DATA.stsword;
    }
    return 0;
}

int CHydraShmServer::ReadStatus(int idx, joint_state_t jnt_state[], eha_state_t eha_state[], sensor_state_t sensor_state[])
{
    for(int i=0; i<HYDRA_JNT_MAX; i++) {
        jnt_state[i].DATA.pos_act = SHM_HYDRA_JOINT_POS_IN(idx, i);
        jnt_state[i].DATA.vel_act = SHM_HYDRA_JOINT_VEL_IN(idx, i);
        jnt_state[i].DATA.tau_act = SHM_HYDRA_JOINT_TAU_IN(idx, i);
        jnt_state[i].DATA.enabled = SHM_HYDRA_JOINT_STATUS_IN(idx, i);
    }
    for(int i=0; i<HYDRA_HAND_JNT_MAX; i++) {
        jnt_state[i+HYDRA_JNT_MAX].DATA.pos_act = SHM_HYDRA_HAND_POS_IN(idx, i);
        jnt_state[i+HYDRA_JNT_MAX].DATA.vel_act = SHM_HYDRA_HAND_VEL_IN(idx, i);
    }
    for(int i=0; i<EHA_MAX; i++) {
        eha_state[i].DATA.pos_act = SHM_HYDRA_EHA_POS_IN(idx, i);
        eha_state[i].DATA.rawpos_act = SHM_HYDRA_EHA_RAWPOS_IN(idx, i);
        eha_state[i].DATA.tau_act = SHM_HYDRA_EHA_TAU_IN(idx, i);
        eha_state[i].DATA.tau2_act = SHM_HYDRA_EHA_TAU2_IN(idx, i);
        eha_state[i].DATA.tau3_act = SHM_HYDRA_EHA_TAU3_IN(idx, i);
        eha_state[i].DATA.vel_act = SHM_HYDRA_EHA_VEL_IN(idx, i);
        eha_state[i].DATA.stsword = SHM_HYDRA_EHA_STATUS_IN(idx, i);
    }
    return 0;
}

int CHydraShmServer::Sync(void)
{
    int ret;
    int semCount = 0;

    if(pShmIn_MD4KW == NULL) {
        return E_SHM_NOT_EXIST;
    }

    // 更新を反映させる。反映が完了するまで待つ
    msync(pShmIn_MD4KW, mmapInSize, MS_SYNC);

    // 全データの反映が完了してから、改めて管理面番号を更新する
    pShmIn_MD4KW->Idx  = (pShmIn_MD4KW->Idx + 1) % SHM_ACCNUM_MAX;

    // 更新を反映させる。反映が完了するまで待つ
    msync(pShmIn_MD4KW, mmapInSize, MS_SYNC);

#if 1 // okamoto
    // 処理プロセス同期用セマフォのロックを解除する
	ret = sem_post(semSync);
	if (ret == -1)
	{
		perror("sem_post NG!!");
	}

    ret = sem_getvalue(semSync, &semCount);
    if (ret == -1)
    {
        perror("sem_getvalue NG!!");
    }
    return semCount;
#endif // okamoto

#if 0 // okamoto 
    // 処理プロセス同期用セマフォのロックを解除する
    ret = sem_getvalue(semSync, &semCount);
    if (ret == -1)
    {
        perror("sem_getvalue NG!!");
        return E_SEM_INVLD_NUM;
    }

    // 解除前にセマフォのカウンタを確認
    if(semCount <= 0)
    {
        // ロックされていることが確認できれば、解除を行う
        ret = sem_post(semSync);
        if (ret == -1)
        {
            perror("sem_post NG!!");
        }
    }
    // 処理プロセスでセマフォがロックされていない?
    else
    {
        return E_SEM_UNLK;
    }

    return 0;
#endif // okamoto
}


int CHydraShmServer::PostSemaphore(void)
{
    int ret;
    int semCount = 0;

    // 処理プロセス同期用セマフォのロックを解除する
    ret = sem_getvalue(semSync, &semCount);
    if (ret == -1)
    {
        perror("sem_getvalue NG!!");
        return E_SEM_INVLD_NUM;
    }

    // 解除前にセマフォのカウンタを確認
    if(semCount <= 0)
    {
        // ロックされていることが確認できれば、解除を行う
        ret = sem_post(semSync);
        if (ret == -1)
        {
            perror("sem_post NG!!");
        }
    }
    // 処理プロセスでセマフォがロックされていない?
    else
    {
        return E_SEM_UNLK;
    }

    return 0;
}
