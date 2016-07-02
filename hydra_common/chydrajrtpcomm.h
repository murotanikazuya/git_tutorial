#ifndef CJRTPCOMM_H
#define CJRTPCOMM_H

#include "cipcomm.h"

#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtpsourcedata.h"

using namespace jrtplib;

#define DEFAULT_RTP_BUF_SIZE 8192

class CHydraJrtpComm : public CIPComm
{
public:
    CHydraJrtpComm();
    ~CHydraJrtpComm();
    int Init(void);

    void Close(void);

//    int ReadStatus(joint_state_t jnt_state[], eha_state_t eha_state[]);

    int ReadCommand(joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t sensor_cmd[]);

    int WriteStatus(const joint_state_t jnt_state[], const eha_state_t eha_state[], const sensor_state_t sensor_state[]);

//    int WriteCommand(const joint_cmd_t jnt_cmd[], const eha_cmd_t eha_cmd[]);

//    int Sync(void);

    void Poll(void);

protected:
    RTPSession session;

    unsigned char tx_buffer[DEFAULT_RTP_BUF_SIZE];
    unsigned char rx_buffer[DEFAULT_RTP_BUF_SIZE];
};

#endif // CJRTPCOMM_H

