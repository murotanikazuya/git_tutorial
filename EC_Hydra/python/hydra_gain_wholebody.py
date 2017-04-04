''' gain definition for the whole body hydra '''

from hydra_gain_common import *

# EHA linear encoder reading with the piston is in the middle
EHA_LENC_OFFSET_WHOLEBODY = (
    0x02fc5cb9,      #rhip
    6880213,      #rhip
    6549639,      #rhip
    7438839,      #rknee
    7301354,      #rknee
    0,      #rknee
    6649957,      #rankle
    7148834,      #rankle
    0x01547c4d,      #lhip
    7145056,      #lhip
    6774503,      #lhip
    7408253,      #lknee
    6302172,      #lknee
    0,      #lknee
    6485571,      #lankle
    6968358,      #lankle
    7081466,      #body
    6761922,      #body
    0,      #body
    0x02587A0D,      #rshoulder
    7094460,      #rshoulder
    7012733,      #rshoulder
    0x0287C905,      #relbow
    7105844,      #relbow
    0,      #relbow
    0x01160239,      #rwrist
    6954306,      #rwrist
    6596830,      #rwrist
    0x00ad9e99,      #lshoulder
    6669299,      #lshoulder
    7112456,      #lshoulder
    0x017c0d8b,      #lelbow
    7041344,      #lelbow
    0,      #lelbow
    0x02B15C6E,      #lwrist
    7107122,      #lwrist
    6938594,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

# motor encoder reading with magnetic field origin
EHA_MENC_OFFSET_WHOLEBODY = (
    264,      #rhip
    608,      #rhip
    601,      #rhip
    974,      #rknee
    773,      #rknee
    411,      #rknee
    1018,      #rankle
    710,      #rankle
    540,      #lhip
    313,      #lhip
    93,      #lhip
    344,      #lknee
    712,      #lknee
    844,      #lknee
    721,      #lankle
    924,      #lankle
    13,      #body
    241,      #body
    0,      #body
    7,      #rshoulder
    544,      #rshoulder
    476,      #rshoulder
    849,      #relbow
    585,      #relbow
    562,      #relbow
    867,      #rwrist
    308,      #rwrist
    810,      #rwrist
    873,      #lshoulder
    6,      #lshoulder
    721,      #lshoulder
    998,      #lelbow
    719,      #lelbow
    921,      #lelbow
    465,      #lwrist
    409,      #lwrist
    71,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

# first send reset signal to "ctrl_wd"
EHA_RESET = (
    CTRLWD_RESET,      #rhip
    CTRLWD_RESET,      #rhip
    CTRLWD_RESET,      #rhip
    CTRLWD_RESET,      #rknee
    CTRLWD_RESET,      #rknee
    CTRLWD_RESET,      #rknee
    CTRLWD_RESET,      #rankle
    CTRLWD_RESET,      #rankle
    CTRLWD_RESET,      #lhip
    CTRLWD_RESET,      #lhip
    CTRLWD_RESET,      #lhip
    CTRLWD_RESET,      #lknee
    CTRLWD_RESET,      #lknee
    CTRLWD_RESET,      #lknee
    CTRLWD_RESET,      #lankle
    CTRLWD_RESET,      #lankle
    CTRLWD_RESET,      #body
    CTRLWD_RESET,      #body
    CTRLWD_RESET,      #body
    CTRLWD_RESET,      #rshoulder
    CTRLWD_RESET,      #rshoulder
    CTRLWD_RESET,      #rshoulder
    CTRLWD_RESET,      #relbow
    CTRLWD_RESET,      #relbow
    CTRLWD_RESET,      #relbow
    CTRLWD_RESET,      #rwrist
    CTRLWD_RESET,      #rwrist
    CTRLWD_RESET,      #rwrist
    CTRLWD_RESET,      #lshoulder
    CTRLWD_RESET,      #lshoulder
    CTRLWD_RESET,      #lshoulder
    CTRLWD_RESET,      #lelbow
    CTRLWD_RESET,      #lelbow
    CTRLWD_RESET,      #lelbow
    CTRLWD_RESET,      #lwrist
    CTRLWD_RESET,      #lwrist
    CTRLWD_RESET,      #lwrist
    CTRLWD_RESET,      #rhand
    CTRLWD_RESET,      #rhand
    CTRLWD_RESET,      #rhand
    CTRLWD_RESET,      #rhand
    CTRLWD_RESET,      #rhand
    CTRLWD_RESET,      #lhand
    CTRLWD_RESET,      #lhand
    CTRLWD_RESET,      #lhand
    CTRLWD_RESET,      #lhand
    CTRLWD_RESET,      #lhand
)

# unreset the driver in the end
EHA_UNRESET = ( #0x0200
    CTRLWD_UNRESET,      #rhip
    CTRLWD_UNRESET,      #rhip
    CTRLWD_UNRESET,      #rhip
    CTRLWD_UNRESET,      #rknee
    CTRLWD_UNRESET,      #rknee
    CTRLWD_UNRESET,      #rknee
    CTRLWD_UNRESET,      #rankle
    CTRLWD_UNRESET,      #rankle
    CTRLWD_UNRESET,      #lhip
    CTRLWD_UNRESET,      #lhip
    CTRLWD_UNRESET,      #lhip
    CTRLWD_UNRESET,      #lknee
    CTRLWD_UNRESET,      #lknee
    CTRLWD_UNRESET,      #lknee
    CTRLWD_UNRESET,      #lankle
    CTRLWD_UNRESET,      #lankle
    CTRLWD_UNRESET,      #body
    CTRLWD_UNRESET,      #body
    CTRLWD_UNRESET,      #body
    CTRLWD_UNRESET,      #rshoulder
    CTRLWD_UNRESET,      #rshoulder
    CTRLWD_UNRESET,      #rshoulder
    CTRLWD_UNRESET,      #relbow
    CTRLWD_UNRESET,      #relbow
    CTRLWD_UNRESET,      #relbow
    CTRLWD_UNRESET,      #rwrist
    CTRLWD_UNRESET,      #rwrist
    CTRLWD_UNRESET,      #rwrist
    CTRLWD_UNRESET,      #lshoulder
    CTRLWD_UNRESET,      #lshoulder
    CTRLWD_UNRESET,      #lshoulder
    CTRLWD_UNRESET,      #lelbow
    CTRLWD_UNRESET,      #lelbow
    CTRLWD_UNRESET,      #lelbow
    CTRLWD_UNRESET,      #lwrist
    CTRLWD_UNRESET,      #lwrist
    CTRLWD_UNRESET,      #lwrist
    CTRLWD_UNRESET,      #rhand
    CTRLWD_UNRESET,      #rhand
    CTRLWD_UNRESET,      #rhand
    CTRLWD_UNRESET,      #rhand
    CTRLWD_UNRESET,      #rhand
    CTRLWD_UNRESET,      #lhand
    CTRLWD_UNRESET,      #lhand
    CTRLWD_UNRESET,      #lhand
    CTRLWD_UNRESET,      #lhand
    CTRLWD_UNRESET,      #lhand
)

''' whether linear encoder direction and
    physical EHA direction in the robot flipped '''
EHA_LENC_PHY_INV_WHOLEBODY = (
    False,      #rhip
    False,      #rhip
    True,      #rhip
    True,      #rknee
    True,      #rknee
    False,      #rknee
    False,      #rankle
    True,      #rankle
    False,      #lhip
    False,      #lhip
    True,      #lhip
    True,      #lknee
    False,      #lknee
    False,      #lknee
    False,      #lankle
    True,      #lankle
    False,      #body
    True,      #body
    False,      #body
    True,      #rshoulder
    True,      #rshoulder
    False,      #rshoulder
    False,      #relbow
    False,      #relbow
    False,      #relbow
    False,      #rwrist
    True,      #rwrist
    False,      #rwrist
    True,      #lshoulder
    False,      #lshoulder
    True,      #lshoulder
    False,      #lelbow
    True,      #lelbow
    False,      #lelbow
    False,      #lwrist
    True,      #lwrist
    False,      #lwrist
    False,      #rhand
    False,      #rhand
    False,      #rhand
    False,      #rhand
    False,      #rhand
    False,      #lhand
    False,      #lhand
    False,      #lhand
    False,      #lhand
    False,      #lhand
)

''' whether linear encoder direction and
    motor current direction is flipped '''
EHA_MOTOR_LENC_INV_WHOLEBODY = (
    True,      #rhip
    False,      #rhip
    True,      #rhip
    True,      #rknee
    True,      #rknee
    False,      #rknee
    False,      #rankle
    False,      #rankle
    True,      #lhip
    False,      #lhip
    True,      #lhip
    True,      #lknee
    True,      #lknee
    False,      #lknee
    True,      #lankle
    False,      #lankle
    True,      #body
    False,      #body
    False,      #body
    True,      #rshoulder
    False,      #rshoulder
    True,      #rshoulder
    False,      #relbow
    True,      #relbow
    False,      #relbow
    False,      #rwrist
    False,      #rwrist
    True,      #rwrist
    True,      #lshoulder
    True,      #lshoulder
    False,      #lshoulder
    False,      #lelbow
    True,      #lelbow
    True,      #lelbow
    False,      #lwrist
    False,      #lwrist
    True,      #lwrist
    False,      #rhand
    False,      #rhand
    False,      #rhand
    False,      #rhand
    False,      #rhand
    False,      #lhand
    False,      #lhand
    False,      #lhand
    False,      #lhand
    False,      #lhand
)

''' whether strain gauge direction and
    motor current direction is flipped
    when the rod is compressed, the strain value decreases '''
EHA_STRAIN_MOTOR_INV_WHOLEBODY = (
    False,      #rhip
    True,      #rhip
    False,      #rhip
    True,      #rknee
    True,      #rknee
    False,      #rknee
    False,      #rankle
    True,      #rankle
    False,      #lhip
    True,      #lhip
    False,      #lhip
    True,      #lknee
    False,      #lknee
    False,      #lknee
    False,      #lankle
    True,      #lankle
    False,      #body
    True,      #body
    False,      #body
    True,      #rshoulder
    True,      #rshoulder
    False,      #rshoulder
    False,      #relbow
    False,      #relbow
    False,      #relbow
    False,      #rwrist
    True,      #rwrist
    False,      #rwrist
    False,      #lshoulder
    False,      #lshoulder
    True,      #lshoulder
    False,      #lelbow
    True,      #lelbow
    False,      #lelbow
    False,      #lwrist
    True,      #lwrist
    False,      #lwrist
    False,      #rhand
    False,      #rhand
    False,      #rhand
    False,      #rhand
    False,      #rhand
    False,      #lhand
    False,      #lhand
    False,      #lhand
    False,      #lhand
    False,      #lhand
)

EHA_POS_GAIN_P_WHOLEBODY = (
    80,      #rhip
    150,      #rhip
    150,      #rhip
    80,      #rknee
    60,      #rknee
    60,      #rknee
    60,      #rankle
    50,      #rankle
    80,      #lhip
    150,      #lhip
    150,      #lhip
    120,      #lknee
    70,      #lknee
    70,      #lknee
    60,      #lankle
    50,      #lankle
    50,      #body
    40,      #body
    0,      #body
    00,      #rshoulder
    30,      #rshoulder
    30,      #rshoulder
    50,      #relbow
    50,      #relbow
    50,      #relbow
    50,      #rwrist
    50,      #rwrist
    50,      #rwrist
    30,      #lshoulder
    30,      #lshoulder
    30,      #lshoulder
    50,      #lelbow
    50,      #lelbow
    50,      #lelbow
    50,      #lwrist
    50,      #lwrist
    50,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

EHA_POS_GAIN_I_WHOLEBODY = (
    0,      #rhip
    0,      #rhip
    0,      #rhip
    10,      #rknee
    10,      #rknee
    10,      #rknee
    10,      #rankle
    10,      #rankle
    0,      #lhip
    0,      #lhip
    0,      #lhip
    10,      #lknee
    10,      #lknee
    10,      #lknee
    20,      #lankle
    20,      #lankle
    10,      #body
    10,      #body
    0,      #body
    10,      #rshoulder
    10,      #rshoulder
    10,      #rshoulder
    10,      #relbow
    40,      #relbow
    30,      #relbow
    10,      #rwrist
    10,      #rwrist
    10,      #rwrist
    10,      #lshoulder
    10,      #lshoulder
    10,      #lshoulder
    10,      #lelbow
    40,      #lelbow
    30,      #lelbow
    10,      #lwrist
    10,      #lwrist
    10,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

EHA_POS_GAIN_D_WHOLEBODY = (
    30,      #rhip
    40,      #rhip
    40,      #rhip
    40,      #rknee
    50,      #rknee
    50,      #rknee
    10,      #rankle
    30,      #rankle
    30,      #lhip
    40,      #lhip
    40,      #lhip
    40,      #lknee
    50,      #lknee
    50,      #lknee
    10,      #lankle
    30,      #lankle
    0,      #body
    0,      #body
    0,      #body
    0,      #rshoulder
    0,      #rshoulder
    0,      #rshoulder
    0,      #relbow
    0,      #relbow
    0,      #relbow
    0,      #rwrist
    0,      #rwrist
    0,      #rwrist
    0,      #lshoulder
    0,      #lshoulder
    0,      #lshoulder
    0,      #lelbow
    0,      #lelbow
    0,      #lelbow
    0,      #lwrist
    0,      #lwrist
    0,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

EHA_VEL_GAIN_P_WHOLEBODY = (
    0,      #rhip
    0,      #rhip
    0,      #rhip
    0,      #rknee
    0,      #rknee
    0,      #rknee
    0,      #rankle
    0,      #rankle
    0,      #lhip
    0,      #lhip
    0,      #lhip
    0,      #lknee
    0,      #lknee
    0,      #lknee
    0,      #lankle
    0,      #lankle
    0,      #body
    0,      #body
    0,      #body
    0,      #rshoulder
    0,      #rshoulder
    0,      #rshoulder
    0,      #relbow
    0,      #relbow
    0,      #relbow
    0,      #rwrist
    0,      #rwrist
    0,      #rwrist
    0,      #lshoulder
    0,      #lshoulder
    0,      #lshoulder
    0,      #lelbow
    0,      #lelbow
    0,      #lelbow
    0,      #lwrist
    0,      #lwrist
    0,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

EHA_VEL_GAIN_I_WHOLEBODY = (
    0,      #rhip
    0,      #rhip
    0,      #rhip
    0,      #rknee
    0,      #rknee
    0,      #rknee
    0,      #rankle
    0,      #rankle
    0,      #lhip
    0,      #lhip
    0,      #lhip
    0,      #lknee
    0,      #lknee
    0,      #lknee
    0,      #lankle
    0,      #lankle
    0,      #body
    0,      #body
    0,      #body
    0,      #rshoulder
    0,      #rshoulder
    0,      #rshoulder
    0,      #relbow
    0,      #relbow
    0,      #relbow
    0,      #rwrist
    0,      #rwrist
    0,      #rwrist
    0,      #lshoulder
    0,      #lshoulder
    0,      #lshoulder
    0,      #lelbow
    0,      #lelbow
    0,      #lelbow
    0,      #lwrist
    0,      #lwrist
    0,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

EHA_VEL_GAIN_D_WHOLEBODY = (
    0,      #rhip
    0,      #rhip
    0,      #rhip
    0,      #rknee
    0,      #rknee
    0,      #rknee
    0,      #rankle
    0,      #rankle
    0,      #lhip
    0,      #lhip
    0,      #lhip
    0,      #lknee
    0,      #lknee
    0,      #lknee
    0,      #lankle
    0,      #lankle
    0,      #body
    0,      #body
    0,      #body
    0,      #rshoulder
    0,      #rshoulder
    0,      #rshoulder
    0,      #relbow
    0,      #relbow
    0,      #relbow
    0,      #rwrist
    0,      #rwrist
    0,      #rwrist
    0,      #lshoulder
    0,      #lshoulder
    0,      #lshoulder
    0,      #lelbow
    0,      #lelbow
    0,      #lelbow
    0,      #lwrist
    0,      #lwrist
    0,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

EHA_TAU_GAIN_P_WHOLEBODY = (
    0,      #rhip
    0,      #rhip
    0,      #rhip
    0,      #rknee
    0,      #rknee
    0,      #rknee
    0,      #rankle
    0,      #rankle
    0,      #lhip
    0,      #lhip
    0,      #lhip
    0,      #lknee
    0,      #lknee
    0,      #lknee
    0,      #lankle
    0,      #lankle
    0,      #body
    0,      #body
    0,      #body
    0,      #rshoulder
    0,      #rshoulder
    0,      #rshoulder
    0,      #relbow
    0,      #relbow
    0,      #relbow
    0,      #rwrist
    0,      #rwrist
    0,      #rwrist
    0,      #lshoulder
    0,      #lshoulder
    0,      #lshoulder
    0,      #lelbow
    0,      #lelbow
    0,      #lelbow
    0,      #lwrist
    0,      #lwrist
    0,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

EHA_TAU_GAIN_I_WHOLEBODY = (
    0,      #rhip
    0,      #rhip
    0,      #rhip
    0,      #rknee
    0,      #rknee
    0,      #rknee
    0,      #rankle
    0,      #rankle
    0,      #lhip
    0,      #lhip
    0,      #lhip
    0,      #lknee
    0,      #lknee
    0,      #lknee
    0,      #lankle
    0,      #lankle
    0,      #body
    0,      #body
    0,      #body
    0,      #rshoulder
    0,      #rshoulder
    0,      #rshoulder
    0,      #relbow
    0,      #relbow
    0,      #relbow
    0,      #rwrist
    0,      #rwrist
    0,      #rwrist
    0,      #lshoulder
    0,      #lshoulder
    0,      #lshoulder
    0,      #lelbow
    0,      #lelbow
    0,      #lelbow
    0,      #lwrist
    0,      #lwrist
    0,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

EHA_TAU_GAIN_D_WHOLEBODY = (
    0,      #rhip
    0,      #rhip
    0,      #rhip
    0,      #rknee
    0,      #rknee
    0,      #rknee
    0,      #rankle
    0,      #rankle
    0,      #lhip
    0,      #lhip
    0,      #lhip
    0,      #lknee
    0,      #lknee
    0,      #lknee
    0,      #lankle
    0,      #lankle
    0,      #body
    0,      #body
    0,      #body
    0,      #rshoulder
    0,      #rshoulder
    0,      #rshoulder
    0,      #relbow
    0,      #relbow
    0,      #relbow
    0,      #rwrist
    0,      #rwrist
    0,      #rwrist
    0,      #lshoulder
    0,      #lshoulder
    0,      #lshoulder
    0,      #lelbow
    0,      #lelbow
    0,      #lelbow
    0,      #lwrist
    0,      #lwrist
    0,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

EHA_CUR_GAIN_P_WHOLEBODY = (
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rhip
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rhip
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rhip
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rknee
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rknee
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rknee
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rankle
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rankle
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lhip
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lhip
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lhip
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lknee
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lknee
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lknee
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lankle
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lankle
    CUR_PGAIN_MAXON_4P_200W  << 8,      #body
    CUR_PGAIN_MAXON_4P_200W  << 8,      #body
    CUR_PGAIN_MAXON_4P_200W  << 8,      #body
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rshoulder
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rshoulder
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rshoulder
    CUR_PGAIN_MAXON_4P_200W  << 8,      #relbow
    CUR_PGAIN_MAXON_4P_200W  << 8,      #relbow
    CUR_PGAIN_MAXON_4P_200W  << 8,      #relbow
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rwrist
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rwrist
    CUR_PGAIN_MAXON_4P_200W  << 8,      #rwrist
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lshoulder
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lshoulder
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lshoulder
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lelbow
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lelbow
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lelbow
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lwrist
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lwrist
    CUR_PGAIN_MAXON_4P_200W  << 8,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

EHA_CUR_GAIN_I_WHOLEBODY = (
    CUR_IGAIN_MAXON_4P_200W << 8,      #rhip
    CUR_IGAIN_MAXON_4P_200W << 8,      #rhip
    CUR_IGAIN_MAXON_4P_200W << 8,      #rhip
    CUR_IGAIN_MAXON_4P_200W << 8,      #rknee
    CUR_IGAIN_MAXON_4P_200W << 8,      #rknee
    CUR_IGAIN_MAXON_4P_200W << 8,      #rknee
    CUR_IGAIN_MAXON_4P_200W << 8,      #rankle
    CUR_IGAIN_MAXON_4P_200W << 8,      #rankle
    CUR_IGAIN_MAXON_4P_200W << 8,      #lhip
    CUR_IGAIN_MAXON_4P_200W << 8,      #lhip
    CUR_IGAIN_MAXON_4P_200W << 8,      #lhip
    CUR_IGAIN_MAXON_4P_200W << 8,      #lknee
    CUR_IGAIN_MAXON_4P_200W << 8,      #lknee
    CUR_IGAIN_MAXON_4P_200W << 8,      #lknee
    CUR_IGAIN_MAXON_4P_200W << 8,      #lankle
    CUR_IGAIN_MAXON_4P_200W << 8,      #lankle
    CUR_IGAIN_MAXON_4P_200W << 8,      #body
    CUR_IGAIN_MAXON_4P_200W << 8,      #body
    CUR_IGAIN_MAXON_4P_200W << 8,      #body
    CUR_IGAIN_MAXON_4P_200W << 8,      #rshoulder
    CUR_IGAIN_MAXON_4P_200W << 8,      #rshoulder
    CUR_IGAIN_MAXON_4P_200W << 8,      #rshoulder
    CUR_IGAIN_MAXON_4P_200W << 8,      #relbow
    CUR_IGAIN_MAXON_4P_200W << 8,      #relbow
    CUR_IGAIN_MAXON_4P_200W << 8,      #relbow
    CUR_IGAIN_MAXON_4P_200W << 8,      #rwrist
    CUR_IGAIN_MAXON_4P_200W << 8,      #rwrist
    CUR_IGAIN_MAXON_4P_200W << 8,      #rwrist
    CUR_IGAIN_MAXON_4P_200W << 8,      #lshoulder
    CUR_IGAIN_MAXON_4P_200W << 8,      #lshoulder
    CUR_IGAIN_MAXON_4P_200W << 8,      #lshoulder
    CUR_IGAIN_MAXON_4P_200W << 8,      #lelbow
    CUR_IGAIN_MAXON_4P_200W << 8,      #lelbow
    CUR_IGAIN_MAXON_4P_200W << 8,      #lelbow
    CUR_IGAIN_MAXON_4P_200W << 8,      #lwrist
    CUR_IGAIN_MAXON_4P_200W << 8,      #lwrist
    CUR_IGAIN_MAXON_4P_200W << 8,      #lwrist
    CUR_IGAIN_MAXON_4P_200W << 8,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

EHA_CUR_GAIN_D_WHOLEBODY = (
    0,      #rhip
    0,      #rhip
    0,      #rhip
    0,      #rknee
    0,      #rknee
    0,      #rknee
    0,      #rankle
    0,      #rankle
    0,      #lhip
    0,      #lhip
    0,      #lhip
    0,      #lknee
    0,      #lknee
    0,      #lknee
    0,      #lankle
    0,      #lankle
    0,      #body
    0,      #body
    0,      #body
    0,      #rshoulder
    0,      #rshoulder
    0,      #rshoulder
    0,      #relbow
    0,      #relbow
    0,      #relbow
    0,      #rwrist
    0,      #rwrist
    0,      #rwrist
    0,      #lshoulder
    0,      #lshoulder
    0,      #lshoulder
    0,      #lelbow
    0,      #lelbow
    0,      #lelbow
    0,      #lwrist
    0,      #lwrist
    0,      #lwrist
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #rhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
    0,      #lhand
)

