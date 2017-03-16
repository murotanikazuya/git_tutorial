#include "chydrashmclient.h"
#include "chydradatamanager.h"

#include <string.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


CHydraShmClient::~CHydraShmClient()
{
    if(
            (pShmIn_MD4KW  != NULL) ||
            (pShmOut_MD4KW != NULL) ||
            (semSync       != NULL) )
    {
        Close();
    }
}


int CHydraShmClient::ReadStatus(joint_state_t jnt_state[], eha_state_t eha_state[], sensor_state_t sensor_state[])
{
    int loop;

    // read shm and store data
    for(loop = 0; loop < HYDRA_JNT_MAX; loop++) {
        jnt_state[loop].DATA.pos_act  = SHM_HYDRA_JOINT_POS_IN(0, loop);
        jnt_state[loop].DATA.vel_act  = SHM_HYDRA_JOINT_VEL_IN(0, loop);
        jnt_state[loop].DATA.tau_act  = SHM_HYDRA_JOINT_TAU_IN(0, loop);
        //jnt_state[loop].DATA.tau2_act = SHM_HYDRA_JOINT_TAU2_IN(0, loop);
        //jnt_state[loop].DATA.tau3_act = SHM_HYDRA_JOINT_TAU3_IN(0, loop);
        jnt_state[loop].DATA.enabled  = SHM_HYDRA_JOINT_STATUS_IN(0, loop);
    }
    for(loop = 0; loop < HYDRA_HAND_JNT_MAX; loop++) {
        jnt_state[loop+HYDRA_JNT_MAX].DATA.pos_act = SHM_HYDRA_HAND_POS_IN(0, loop);
        jnt_state[loop+HYDRA_JNT_MAX].DATA.vel_act = SHM_HYDRA_HAND_VEL_IN(0, loop);
    }

    for(loop = 0; loop < EHA_MAX; loop++) {
        eha_state[loop].DATA.pos_act    = SHM_HYDRA_EHA_POS_IN(0, loop);
        eha_state[loop].DATA.rawpos_act = SHM_HYDRA_EHA_RAWPOS_IN(0, loop);
        eha_state[loop].DATA.vel_act    = SHM_HYDRA_EHA_VEL_IN(0, loop);
        eha_state[loop].DATA.tau_act    = SHM_HYDRA_EHA_TAU_IN(0, loop);// - all_eha_tau_offset[loop];
        eha_state[loop].DATA.tau2_act   = SHM_HYDRA_EHA_TAU2_IN(0,loop);
        eha_state[loop].DATA.tau3_act   = SHM_HYDRA_EHA_TAU3_IN(0,loop);//temperature c
        eha_state[loop].DATA.stsword    = SHM_HYDRA_EHA_STATUS_IN(0, loop);
    }

    for(loop = 0; loop < 2; loop++){
        sensor_state[0].DATA.ft_sensor[loop].wrench[0] = SHM_HYDRA_FS_FX_IN(0, loop);
        sensor_state[0].DATA.ft_sensor[loop].wrench[1] = SHM_HYDRA_FS_FY_IN(0, loop);
        sensor_state[0].DATA.ft_sensor[loop].wrench[2] = SHM_HYDRA_FS_FZ_IN(0, loop);
        sensor_state[0].DATA.ft_sensor[loop].wrench[3] = SHM_HYDRA_FS_MX_IN(0, loop);
        sensor_state[0].DATA.ft_sensor[loop].wrench[4] = SHM_HYDRA_FS_MY_IN(0, loop);
        sensor_state[0].DATA.ft_sensor[loop].wrench[5] = SHM_HYDRA_FS_MZ_IN(0, loop);
    }

    sensor_state[0].DATA.imu[0].gyro[0] = SHM_HYDRA_IMU_GYROX_IN(0);
    sensor_state[0].DATA.imu[0].gyro[1] = SHM_HYDRA_IMU_GYROY_IN(0);
    sensor_state[0].DATA.imu[0].gyro[2] = SHM_HYDRA_IMU_GYROZ_IN(0);
    sensor_state[0].DATA.imu[0].acc[0]  = SHM_HYDRA_IMU_ACCX_IN(0);
    sensor_state[0].DATA.imu[0].acc[1]  = SHM_HYDRA_IMU_ACCY_IN(0);
    sensor_state[0].DATA.imu[0].acc[2]  = SHM_HYDRA_IMU_ACCZ_IN(0);

    return 0;
}

int CHydraShmClient::ReadStatus(CHydraDataManager* hydraData)
{
    int loop;

    // read shm and store data
    for(loop = 0; loop < HYDRA_JNT_MAX; loop++) {
        hydraData->GetJointStatePtr()[loop].DATA.pos_act = SHM_HYDRA_JOINT_POS_IN(0, loop);
        hydraData->GetJointStatePtr()[loop].DATA.vel_act  = SHM_HYDRA_JOINT_VEL_IN(0, loop);
        hydraData->GetJointStatePtr()[loop].DATA.tau_act  = SHM_HYDRA_JOINT_TAU_IN(0, loop);
        //hydraData->GetJointStatePtr()[loop].DATA.tau2_act = SHM_HYDRA_JOINT_TAU2_IN(0, loop);
        //hydraData->GetJointStatePtr()[loop].DATA.tau3_act = SHM_HYDRA_JOINT_TAU3_IN(0, loop);
        hydraData->GetJointStatePtr()[loop].DATA.enabled  = SHM_HYDRA_JOINT_STATUS_IN(0, loop);
    }
    for(loop = 0; loop < HYDRA_HAND_JNT_MAX; loop++) {
        hydraData->GetJointStatePtr()[loop+HYDRA_JNT_MAX].DATA.pos_act = SHM_HYDRA_HAND_POS_IN(0, loop);
        hydraData->GetJointStatePtr()[loop+HYDRA_JNT_MAX].DATA.vel_act = SHM_HYDRA_HAND_VEL_IN(0, loop);
    }

    for(loop = 0; loop < EHA_MAX; loop++) {
        hydraData->GetEHAStatePtr()[loop].DATA.pos_act    = SHM_HYDRA_EHA_POS_IN(0, loop);
        hydraData->GetEHAStatePtr()[loop].DATA.rawpos_act = SHM_HYDRA_EHA_RAWPOS_IN(0, loop);
        hydraData->GetEHAStatePtr()[loop].DATA.vel_act    = SHM_HYDRA_EHA_VEL_IN(0, loop);
        hydraData->GetEHAStatePtr()[loop].DATA.tau_act    = SHM_HYDRA_EHA_TAU_IN(0, loop);// - all_eha_tau_offset[loop];
        hydraData->GetEHAStatePtr()[loop].DATA.tau2_act   = SHM_HYDRA_EHA_TAU2_IN(0,loop);
        hydraData->GetEHAStatePtr()[loop].DATA.tau3_act   = SHM_HYDRA_EHA_TAU3_IN(0,loop);//temperature c
        hydraData->GetEHAStatePtr()[loop].DATA.stsword    = SHM_HYDRA_EHA_STATUS_IN(0, loop);
    }

    for(loop = 0; loop < 2; loop++){
        hydraData->GetSensorStatePtr()[0].DATA.ft_sensor[loop].wrench[0] = SHM_HYDRA_FS_FX_IN(0, loop);
        hydraData->GetSensorStatePtr()[0].DATA.ft_sensor[loop].wrench[1] = SHM_HYDRA_FS_FY_IN(0, loop);
        hydraData->GetSensorStatePtr()[0].DATA.ft_sensor[loop].wrench[2] = SHM_HYDRA_FS_FZ_IN(0, loop);
        hydraData->GetSensorStatePtr()[0].DATA.ft_sensor[loop].wrench[3] = SHM_HYDRA_FS_MX_IN(0, loop);
        hydraData->GetSensorStatePtr()[0].DATA.ft_sensor[loop].wrench[4] = SHM_HYDRA_FS_MY_IN(0, loop);
        hydraData->GetSensorStatePtr()[0].DATA.ft_sensor[loop].wrench[5] = SHM_HYDRA_FS_MZ_IN(0, loop);
    }

    hydraData->GetSensorStatePtr()[0].DATA.imu[0].gyro[0] = SHM_HYDRA_IMU_GYROX_IN(0);
    hydraData->GetSensorStatePtr()[0].DATA.imu[0].gyro[1] = SHM_HYDRA_IMU_GYROY_IN(0);
    hydraData->GetSensorStatePtr()[0].DATA.imu[0].gyro[2] = SHM_HYDRA_IMU_GYROZ_IN(0);
    hydraData->GetSensorStatePtr()[0].DATA.imu[0].acc[0]  = SHM_HYDRA_IMU_ACCX_IN(0);
    hydraData->GetSensorStatePtr()[0].DATA.imu[0].acc[1]  = SHM_HYDRA_IMU_ACCY_IN(0);
    hydraData->GetSensorStatePtr()[0].DATA.imu[0].acc[2]  = SHM_HYDRA_IMU_ACCZ_IN(0);

    return 0;
}

int CHydraShmClient::WriteCommand(const joint_cmd_t jnt_cmd[], const eha_cmd_t eha_cmd[], const sensor_cmd_t sensor_cmd[])
{
    int loop;
    for(loop = 0; loop < HYDRA_JNT_MAX; loop++) {
        SHM_HYDRA_JOINT_REFPOS_OUT(1, loop)   = jnt_cmd[loop].DATA.pos_ref; // 目標位置指令[rad]
        SHM_HYDRA_JOINT_REFVEL_OUT(1, loop)   = jnt_cmd[loop].DATA.vel_ref;  // 目標速度指令[rad/sec]
        SHM_HYDRA_JOINT_REFTAU_OUT(1, loop)   = jnt_cmd[loop].DATA.tau_ref;  // 目標電流指令値[0.01A]
//        SHM_HYDRA_JOINT_CTRLMODE_OUT(1, loop) = all_joint_servo_switch[loop];
    }

    for(loop=0; loop<EHA_MAX; loop++) {
        SHM_HYDRA_EHA_CTRLWORD_OUT(1, loop) = (unsigned short)((eha_cmd[loop].DATA.ctlword)&0xffff);
        //SHM_HYDRA_EHA_REFPOS_OUT(1,loop)    = eha_cmd[loop].DATA.rawpos_ref;
        SHM_HYDRA_EHA_REFPOS_OUT(1,loop)    = eha_cmd[loop].DATA.pos_ref;
    }

    SHM_HYDRA_FS_CTRLWORD_OUT(1,0)  = sensor_cmd[0].DATA.ft_sensor[0].ctlword;
    SHM_HYDRA_FS_CTRLWORD_OUT(1,1)  = sensor_cmd[0].DATA.ft_sensor[1].ctlword;
    SHM_HYDRA_IMU_CTRLWORD_OUT(1,0) = sensor_cmd[0].DATA.imu[0].ctlword;
    return 0;
}

int CHydraShmClient::WriteCommand(CHydraDataManager* hydraData)
{
    int loop;
    for(loop = 0; loop < HYDRA_JNT_MAX; loop++) {
        SHM_HYDRA_JOINT_REFPOS_OUT(1, loop)   = hydraData->GetJointCmdPtr()[loop].DATA.pos_ref; // 目標位置指令[rad]
        SHM_HYDRA_JOINT_REFVEL_OUT(1, loop)   = hydraData->GetJointCmdPtr()[loop].DATA.vel_ref;  // 目標速度指令[rad/sec]
        SHM_HYDRA_JOINT_REFTAU_OUT(1, loop)   = hydraData->GetJointCmdPtr()[loop].DATA.tau_ref;  // 目標電流指令値[0.01A]
//        SHM_HYDRA_JOINT_CTRLMODE_OUT(1, loop) = all_joint_servo_switch[loop];
    }

    for(loop=0; loop<EHA_MAX; loop++) {
        SHM_HYDRA_EHA_CTRLWORD_OUT(1, loop) = (unsigned short)((hydraData->GetEHACmdPtr()[loop].DATA.ctlword)&0xffff);
        //SHM_HYDRA_EHA_REFPOS_OUT(1,loop)    = hydraData->GetEHACmdPtr()[loop].DATA.rawpos_ref;
        SHM_HYDRA_EHA_REFPOS_OUT(1,loop)    = hydraData->GetEHACmdPtr()[loop].DATA.pos_ref;
    }

    SHM_HYDRA_FS_CTRLWORD_OUT(1,0)  = hydraData->GetSensorCmdPtr()[0].DATA.ft_sensor[0].ctlword;
    SHM_HYDRA_FS_CTRLWORD_OUT(1,1)  = hydraData->GetSensorCmdPtr()[0].DATA.ft_sensor[1].ctlword;
    SHM_HYDRA_IMU_CTRLWORD_OUT(1,0) = hydraData->GetSensorCmdPtr()[0].DATA.imu[0].ctlword;
    return 0;
}
/*
int CHydraShmClient::WaitSemaphore(void)
{
    return sem_wait(semSync);
}
*/

int CHydraShmClient::WaitSemaphore(void)
{
    int ret,semcnt;
    ret = sem_wait(semSync);
    if (ret == -1){
        return ret;
    }
    ret = sem_getvalue(semSync,&semcnt);
    if (ret == -1){
        return ret;
    }
    return semcnt;
}



int CHydraShmClient::Sync(void)
{
    if (pShmOut_MD4KW == EC_NULL)
        return -1;

    // 更新を反映させる。反映が完了するまで待つ
    msync(pShmOut_MD4KW, mmapOutSize, MS_SYNC);

    // 全データの反映が完了してから、改めて管理面番号を更新する
    pShmOut_MD4KW->Idx  = (pShmOut_MD4KW->Idx + 1) % SHM_ACCNUM_MAX;

    // 更新を反映させる。反映が完了するまで待つ
    msync(pShmOut_MD4KW, mmapOutSize, MS_SYNC);

    return 0;
}

int CHydraShmClient::Init(void)
{
    long psize;
    char tmp;

    // open shared memory on RAM disk
    sfd_in  = open(p_name_shmin, O_RDWR, S_IRWXU);
    sfd_out = open(p_name_shmout, O_RDWR, S_IRWXU);

    if((sfd_in == -1) || (sfd_out == -1))
    {
        printf("sfd open NG\n");
        return -1;
    }

    /* ページサイズで境界合わせを行なったサイズを計算 */
    psize   = sysconf(_SC_PAGE_SIZE);
    mmapInSize = (sizeof(T_SHM_INPUT_MD4KW) / psize + 1) * psize;

    /* ファイルの必要サイズ分先にシークし、0を書き込み */
    /* ファイルのサイズをマップしたいサイズにする為 */
    if(lseek(sfd_in, mmapInSize, SEEK_SET) < 0) {
        printf("sfd-in lseek NG\n");
        return -1;
    }

    tmp ='\0';
    if(write(sfd_in,&tmp,sizeof(char)) == -1) {
        printf("sfd-in write NG\n");
        return -1;
    }

    /* ページサイズで境界合わせを行なったサイズを計算 */
    psize   = sysconf(_SC_PAGE_SIZE);
    mmapOutSize = (sizeof(T_SHM_OUTPUT_MD4KW) / psize + 1) * psize;

    /* ファイルの必要サイズ分先にシークし、0を書き込み */
    /* ファイルのサイズをマップしたいサイズにする為 */
    if(lseek(sfd_out, mmapOutSize, SEEK_SET) < 0) {
        printf("sfd-out lseek NG\n");
        return -1;
    }

    tmp ='\0';
    if(write(sfd_out,&tmp,sizeof(char)) == -1) {
        printf("sfd-out write NG\n");
        return -1;
    }

    // WORKでは、INは読み込み限定、OUTは書き込み限定とする
    pShmIn_MD4KW  = (T_SHM_INPUT_MD4KW *)mmap(0, mmapInSize,
                                              PROT_READ, MAP_SHARED,
                                              sfd_in, 0);
    pShmOut_MD4KW = (T_SHM_OUTPUT_MD4KW *)mmap(0, mmapOutSize,
                                               PROT_READ | PROT_WRITE,
                                               MAP_SHARED, sfd_out, 0);
    if((pShmIn_MD4KW == MAP_FAILED) || (pShmOut_MD4KW == MAP_FAILED)) {
        fprintf(stderr, "mmap NG\n");
        return -1;
    }

    // named semaphore for inter process communication synchroization
    semSync = sem_open( p_name_sem, O_CREAT, 0777, 0 );
    if(semSync == SEM_FAILED) {
        semSync = NULL;
        perror("sem open failed");
        return -1;
    }
    printf("In: %s Out: %s Sem: %s\n", p_name_shmin, p_name_shmout, p_name_sem);
    return 0;
}

void CHydraShmClient::Close(void)
{
    int ret;

    if(semSync!= NULL)
    {
        ret = sem_close( semSync );
        if (ret == -1)
            perror("sem_close NG");

        sem_unlink( p_name_sem );
        if (ret == -1)
            perror("sem_unlink NG");

        semSync = NULL;
    }

    // unmap shared memories
    if(pShmIn_MD4KW != EC_NULL)
    {
        munmap(pShmIn_MD4KW, mmapInSize);
        pShmIn_MD4KW = EC_NULL;
    }
    if(pShmOut_MD4KW != EC_NULL)
    {
        munmap(pShmOut_MD4KW, mmapOutSize);
        pShmOut_MD4KW = EC_NULL;
    }

    // close file pointers to shared memories
    if(sfd_in >= 0)
    {
        close(sfd_in);
        sfd_in = -1;
    }
    if(sfd_out >= 0)
    {
        close(sfd_out);
        sfd_out = -1;
    }
}
