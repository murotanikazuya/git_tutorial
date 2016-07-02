#include "chydrajrtpcomm.h"
#include "hydra_types.h"
#include "comm_common_def.h"

#define LIM_FULL_VERSION
#include "robot_id.h"
#include "robot_hydra_id.h"
#include "7arm_id.h"

#include <stdio.h>
CHydraJrtpComm::CHydraJrtpComm()
{
}

CHydraJrtpComm::~CHydraJrtpComm()
{
    Close();
}

int CHydraJrtpComm::Init(void)
{
    int status;
    RTPSessionParams sessionparams;
    RTPUDPv4TransmissionParams transparams;
    uint32_t destip;

    sessionparams.SetOwnTimestampUnit(1.0/1000.0);
    sessionparams.SetAcceptOwnPackets(true);
    sessionparams.SetMaximumPacketSize(4096);
    transparams.SetPortbase(port_base);

    status = session.Create(sessionparams,&transparams);
    if (status < 0)
    {
        fprintf(stderr, "%s\n", RTPGetErrorString(status).c_str());
        return -1;
    }

    destip = inet_addr(p_tgt_addr);
    destip = ntohl(destip);
    RTPIPv4Address addr(destip,target_port);
    status = session.AddDestination(addr);
    if (status < 0)
    {
        fprintf(stderr, "%s\n", RTPGetErrorString(status).c_str());
        return -1;
    }

    return 0;
}


void CHydraJrtpComm::Close(void)
{
    session.Destroy( );
}

int CHydraJrtpComm::WriteStatus(const joint_state_t jnt_state[], const eha_state_t eha_state[], const sensor_state_t sensor_state[])
{
    int n = 0;
    int status;

    T_REMOTE_PAYLOAD_DATA tmpPkt = {0, };

    // 現状はHydra決め打ち
    tmpPkt.ModelType = PF_MODEL_HYDRA;
    tmpPkt.JointNum  = JOINT_HYDRA_MAX;

    // 動作モードの設定（本来なら設定変更関数などが必要）
    // 今後の仕様検討で詳細を検討予定
    tmpPkt.OpMode		= 0;
    tmpPkt.OpStatus  	= 0;
    tmpPkt.OpCommand  	= 0;

    for(int i = 0; i < tmpPkt.JointNum; i++)
    {
        tmpPkt.Joint[i].DATA.id			= i;
        tmpPkt.Joint[i].DATA.enable		= jnt_state[i].DATA.enabled;

        tmpPkt.Joint[i].DATA.act.pos	= jnt_state[i].DATA.pos_act;	// 関節 角度[rad]
        tmpPkt.Joint[i].DATA.act.vel	= jnt_state[i].DATA.vel_act; 	// 関節 速度[rad/sec]
        tmpPkt.Joint[i].DATA.act.tau	= jnt_state[i].DATA.tau_act; 	// 関節 トルク[Nm]
        tmpPkt.Joint[i].DATA.cmd.pos	= jnt_state[i].DATA.pos_cmd;	// 目標関節 角度指令値[rad]
        tmpPkt.Joint[i].DATA.cmd.vel	= jnt_state[i].DATA.vel_cmd;	// 目標関節 速度指令値[rad/sec]
        tmpPkt.Joint[i].DATA.cmd.tau	= 0;							// 目標関節 トルク指令値[Nm]
    }
    int	size = sizeof( T_REMOTE_PAYLOAD_DATA )
            - (sizeof(T_JOINT_DATA) * (SLAVE_NUM_MAX - tmpPkt.JointNum ));
    //memcpy(tx_buffer, &tmpPkt, sizeof(struct T_REMOTE_PAYLOAD_DATA));
    memcpy(tx_buffer, &tmpPkt, size);


    status = session.SendPacket((void *)tx_buffer, size, 0, false, 1);

    if (status < 0)
    {
        fprintf(stderr, "%s\n", RTPGetErrorString(status).c_str());
        return -1;
    }

    return size;
}

int CHydraJrtpComm::ReadCommand(joint_cmd_t jnt_cmd[], eha_cmd_t eha_cmd[], sensor_cmd_t sensor_cmd[])
{
    int n=0;
    int max_extseq = 0;

    session.BeginDataAccess();
    if (session.GotoFirstSource())
    {
        do
        {
            RTPPacket *packet;
            while ((packet = session.GetNextPacket()) != 0)
            {
                n = packet->GetPayloadLength();
                // 受信データサイズがバッファサイズ以上の場合は、データを破棄する
                if(sizeof(T_REMOTE_PAYLOAD_DATA) < n)
                {
//                   	os() << "Invalid PayloadLength " << n << std::endl;
                    session.DeletePacket(packet);
                    continue;
                }

                memcpy(rx_buffer, packet->GetPayloadData(), n);

                T_REMOTE_PAYLOAD_DATA *pTmpPkt = (T_REMOTE_PAYLOAD_DATA*)rx_buffer;

                switch(pTmpPkt->ModelType)
                {
                case PF_MODEL_7ARM_RIGHT :
                    {
                        if( pTmpPkt->JointNum  != JOINT_7ARM_MAX)
                        {
//							os() << "Invalid JointNum " << pTmpPkt->JointNum << std::endl;
                            session.DeletePacket(packet);
                            continue;
                        }

//						os() << "Got packet SeqNo=" << packet->GetExtendedSequenceNumber() << std::endl;
						n = packet->GetExtendedSequenceNumber(); // debug

                        //
                        // 送信元が認識している動作状態
                        //??? = pTmpPkt->OpMode;
                        //??? = pTmpPkt->OpStatus;
                        //??? = pTmpPkt->OpCommand;

                        // 7軸アームからHYDRAへの軸変換テーブル
                        const int conv_idx[JOINT_7ARM_MAX] = {
                            JOINT_HYDRA_RSHOULDER_PITCH,
                            JOINT_HYDRA_RSHOULDER_ROLL,
                            JOINT_HYDRA_RSHOULDER_YAW,
                            JOINT_HYDRA_RELBOW_PITCH,
                            JOINT_HYDRA_RWRIST_YAW,
                            JOINT_HYDRA_RWRIST_PITCH,
                            JOINT_HYDRA_RWRIST_ROLL };

                        for(int i=0; i < pTmpPkt->JointNum; ++i)
                        {
                            int id = pTmpPkt->Joint[i].DATA.id;

                            jnt_cmd[conv_idx[id]].DATA.id    		= conv_idx[id];
                            jnt_cmd[conv_idx[id]].DATA.enable		= pTmpPkt->Joint[i].DATA.enable;
                            jnt_cmd[conv_idx[id]].DATA.pos_ref 	= pTmpPkt->Joint[i].DATA.act.pos;
                            jnt_cmd[conv_idx[id]].DATA.vel_ref 	= pTmpPkt->Joint[i].DATA.act.vel;
                            jnt_cmd[conv_idx[id]].DATA.tau_ref 	= pTmpPkt->Joint[i].DATA.act.tau;

                            // 送信元からの指令値（とりあえずコメントアウト)
                            //joint_cmd[conv_idx[id]].DATA.pos_ref = pTmpPkt->Joint[i].ref.pos;
                            //joint_cmd[conv_idx[id]].DATA.vel_ref = pTmpPkt->Joint[i].ref.vel;
                            //joint_cmd[conv_idx[id]].DATA.tau_ref = pTmpPkt->Joint[i].ref.tau;
                        }
                    }
                    break;

                case PF_MODEL_HR_LIM:
                    {
                        if( pTmpPkt->JointNum  != MAX_JOINT_NUM)
                        {
//                            os() << "Invalid JointNum " << pTmpPkt->JointNum << std::endl;
                            session.DeletePacket(packet);
                            continue;
                        }

//						os() << "Got packet SeqNo=" << packet->GetExtendedSequenceNumber() << std::endl;
						n = packet->GetExtendedSequenceNumber(); // debug

                        // LIMモデルのアームを7軸アームとしてHYDRAへの軸変換
                        const int conv_idx[JOINT_7ARM_MAX] = {
                            JOINT_HYDRA_RSHOULDER_PITCH,
                            JOINT_HYDRA_RSHOULDER_ROLL,
                            JOINT_HYDRA_RSHOULDER_YAW,
                            JOINT_HYDRA_RELBOW_PITCH,
                            JOINT_HYDRA_RWRIST_YAW,
                            JOINT_HYDRA_RWRIST_PITCH,
                            JOINT_HYDRA_RWRIST_ROLL };

                        //for(int i=0; i < pTmpPkt->JointNum; ++i)
                        for(int i=0; i < JOINT_7ARM_MAX; ++i)
                        {
                            //int id = pTmpPkt->Joint[i].DATA.id;
                            int armjnt = JOINT_SHOULDER_PITCH_R;

                            jnt_cmd[conv_idx[i]].DATA.id    		= conv_idx[i];
                            jnt_cmd[conv_idx[i]].DATA.enable		= pTmpPkt->Joint[armjnt+i].DATA.enable;
                            jnt_cmd[conv_idx[i]].DATA.pos_ref 	= pTmpPkt->Joint[armjnt+i].DATA.act.pos;
                            jnt_cmd[conv_idx[i]].DATA.vel_ref 	= pTmpPkt->Joint[armjnt+i].DATA.act.vel;
                            jnt_cmd[conv_idx[i]].DATA.tau_ref 	= pTmpPkt->Joint[armjnt+i].DATA.act.tau;

                            // 送信元からの指令値（とりあえずコメントアウト)
                            //joint_cmd[conv_idx[id]].DATA.pos_ref = pTmpPkt->Joint[i].ref.pos;
                            //joint_cmd[conv_idx[id]].DATA.vel_ref = pTmpPkt->Joint[i].ref.vel;
                            //joint_cmd[conv_idx[id]].DATA.tau_ref = pTmpPkt->Joint[i].ref.tau;
                        }
                    }
                    break;

                default:
//                   	os() << "Invalid ModelType " << pTmpPkt->ModelType << std::endl;
                    break;
                }
                session.DeletePacket(packet);
				break;
            }
        } while (session.GotoNextSource());
    }
    session.EndDataAccess();
    return n;
}

void CHydraJrtpComm::Poll(void)
{
    session.Poll();
}
