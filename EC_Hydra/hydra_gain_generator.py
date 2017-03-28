''' This is to generate .csv gain file to be read from ec_master
    author Tianyi Ko kang@ynl.t.u-tokyo.ac.jp '''


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
EHA_RESET = ( # 32768 = 0x8000
    32768,      #rhip
    32768,      #rhip
    32768,      #rhip
    32768,      #rknee
    32768,      #rknee
    32768,      #rknee
    32768,      #rankle
    32768,      #rankle
    32768,      #lhip
    32768,      #lhip
    32768,      #lhip
    32768,      #lknee
    32768,      #lknee
    32768,      #lknee
    32768,      #lankle
    32768,      #lankle
    32768,      #body
    32768,      #body
    32768,      #body
    32768,      #rshoulder
    32768,      #rshoulder
    32768,      #rshoulder
    32768,      #relbow
    32768,      #relbow
    32768,      #relbow
    32768,      #rwrist
    32768,      #rwrist
    32768,      #rwrist
    32768,      #lshoulder
    32768,      #lshoulder
    32768,      #lshoulder
    32768,      #lelbow
    32768,      #lelbow
    32768,      #lelbow
    32768,      #lwrist
    32768,      #lwrist
    32768,      #lwrist
    32768,      #rhand
    32768,      #rhand
    32768,      #rhand
    32768,      #rhand
    32768,      #rhand
    32768,      #lhand
    32768,      #lhand
    32768,      #lhand
    32768,      #lhand
    32768,      #lhand
)

# unreset the driver in the end
EHA_UNRESET = (
    512,      #rhip
    512,      #rhip
    512,      #rhip
    512,      #rknee
    512,      #rknee
    512,      #rknee
    512,      #rankle
    512,      #rankle
    512,      #lhip
    512,      #lhip
    512,      #lhip
    512,      #lknee
    512,      #lknee
    512,      #lknee
    512,      #lankle
    512,      #lankle
    512,      #body
    512,      #body
    512,      #body
    512,      #rshoulder
    512,      #rshoulder
    512,      #rshoulder
    512,      #relbow
    512,      #relbow
    512,      #relbow
    512,      #rwrist
    512,      #rwrist
    512,      #rwrist
    512,      #lshoulder
    512,      #lshoulder
    512,      #lshoulder
    512,      #lelbow
    512,      #lelbow
    512,      #lelbow
    512,      #lwrist
    512,      #lwrist
    512,      #lwrist
    512,      #rhand
    512,      #rhand
    512,      #rhand
    512,      #rhand
    512,      #rhand
    512,      #lhand
    512,      #lhand
    512,      #lhand
    512,      #lhand
    512,      #lhand
)

''' whether linear encoder direction and
    physical EHA direction in the robot flipped '''
EHA_LENC_PHY_INV = (
    False,      #rhip
    False,      #rhip
    False,      #rhip
    False,      #rknee
    False,      #rknee
    False,      #rknee
    False,      #rankle
    False,      #rankle
    False,      #lhip
    False,      #lhip
    False,      #lhip
    False,      #lknee
    False,      #lknee
    False,      #lknee
    False,      #lankle
    False,      #lankle
    False,      #body
    False,      #body
    False,      #body
    False,      #rshoulder
    False,      #rshoulder
    False,      #rshoulder
    False,      #relbow
    False,      #relbow
    False,      #relbow
    False,      #rwrist
    False,      #rwrist
    False,      #rwrist
    False,      #lshoulder
    False,      #lshoulder
    False,      #lshoulder
    False,      #lelbow
    False,      #lelbow
    False,      #lelbow
    False,      #lwrist
    False,      #lwrist
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
EHA_MOTOR_LENC_INV = (
    False,      #rhip
    False,      #rhip
    False,      #rhip
    False,      #rknee
    False,      #rknee
    False,      #rknee
    False,      #rankle
    False,      #rankle
    False,      #lhip
    False,      #lhip
    False,      #lhip
    False,      #lknee
    False,      #lknee
    False,      #lknee
    False,      #lankle
    False,      #lankle
    False,      #body
    False,      #body
    False,      #body
    False,      #rshoulder
    False,      #rshoulder
    False,      #rshoulder
    False,      #relbow
    False,      #relbow
    False,      #relbow
    False,      #rwrist
    False,      #rwrist
    False,      #rwrist
    False,      #lshoulder
    False,      #lshoulder
    False,      #lshoulder
    False,      #lelbow
    False,      #lelbow
    False,      #lelbow
    False,      #lwrist
    False,      #lwrist
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

''' whether strain gauge direction and
    motor current direction is flipped '''
EHA_STRAIN_MOTOR_INV = (
    False,      #rhip
    False,      #rhip
    False,      #rhip
    False,      #rknee
    False,      #rknee
    False,      #rknee
    False,      #rankle
    False,      #rankle
    False,      #lhip
    False,      #lhip
    False,      #lhip
    False,      #lknee
    False,      #lknee
    False,      #lknee
    False,      #lankle
    False,      #lankle
    False,      #body
    False,      #body
    False,      #body
    False,      #rshoulder
    False,      #rshoulder
    False,      #rshoulder
    False,      #relbow
    False,      #relbow
    False,      #relbow
    False,      #rwrist
    False,      #rwrist
    False,      #rwrist
    False,      #lshoulder
    False,      #lshoulder
    False,      #lshoulder
    False,      #lelbow
    False,      #lelbow
    False,      #lelbow
    False,      #lwrist
    False,      #lwrist
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

EHA_SERVO_DIR = ()
for it in enumerate(EHA_MOTOR_LENC_INV):
    servo_dir = 0
    if EHA_LENC_PHY_INV[it]:
        servo_dir += 1   # dir |= 0x01
    if EHA_MOTOR_LENC_INV[it]:
        servo_dir += 16  # dir |= 0x10
    if EHA_STRAIN_MOTOR_INV[it]:
        servo_dir += 21  # dir |= 0x20
    EHA_SERVO_DIR = (EHA_SERVO_DIR, dir)



EHA_POS_GAIN_P_WHOLEBODY = (
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


EHA_POS_GAIN_I_WHOLEBODY = (
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

EHA_POS_GAIN_D_WHOLEBODY = (
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

EHA_CUR_GAIN_P_WHOLEBODY = (
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


EHA_CUR_GAIN_I_WHOLEBODY = (
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



class GainBase(object):
    ''' base class of gain '''
    def __init__(self, name_in, addr, size, gain):
        self.name = name_in
        self.addr = addr
        self.data = gain
        self.size = size

class GainWholebody(GainBase):
    ''' gain for whole body ethercat cofiguration'''
    def __init__(self, name_in, addr, size, gain):
        GainBase.__init__(self, name_in, addr, size, gain)
        self.box = ( # boad_type, box_id ,slave_id, axis0, 1, 2, box_name
            (0, 0, 0, (0, 1, 2), "r_hip"),               #rhip
            (0, 1, 1, (3, 4, 5), "r_knee"),               #rknee
            (1, 0, 2, (6, 7), "r_ankle"),                 #rankle
            (0, 2, 3, (8, 9, 10), "l_hip"),              #lhip
            (0, 3, 4, (11, 12, 13), "l_knee"),            #lknee
            (1, 1, 5, (14, 15), "l_ankle"),                #lankle
            (0, 4, 7, (16, 17, 18), "body"),            #body
            (0, 5, 8, (19, 20, 21), "r_shoulder"),            #rshoulder
            (0, 6, 9, (22, 23, 24), "r_elbow"),            #relbow
            (0, 7, 10, (25, 26, 27), "r_wrist"),           #rwrist
            (2, 0, 11, (37, 38, 39, 40, 41), "r_hand"),   #rhand
            (0, 8, 12, (28, 29, 30), "l_shoulder"),           #lshoulder
            (0, 9, 13, (31, 32, 33), "l_elbow"),           #lelbow
            (0, 10, 14, (34, 35, 36), "l_wrist"),          #lwrist
            (2, 1, 15, (42, 43, 44, 45, 46), "l_hand")    #lhand
        )

class GainSingleAxis(GainBase):
    ''' gain for the single axis acturator on the desk '''
    def __init__(self, name_in, addr, size, gain):
        GainBase.__init__(self, name_in, addr, size, gain)
        self.box = ( # boad_type, box_id ,slave_id, axis0, 1, 2, box_name
            (0, 0, 0, (0, 1, 2), "r_hip"),               #rhip
        )


GAINS = (
    GainWholebody("Reset", 7000, 2, EHA_RESET),
    GainWholebody("ServoDirection", 7046, 1, EHA_SERVO_DIR),
    GainWholebody("PosPGain", 7011, 2, EHA_POS_GAIN_P_WHOLEBODY),
    GainWholebody("PosIGain", 7012, 2, EHA_POS_GAIN_I_WHOLEBODY),
    GainWholebody("PosDGain", 7013, 2, EHA_POS_GAIN_D_WHOLEBODY),
    GainWholebody("VelPGain", 7021, 2, EHA_VEL_GAIN_P_WHOLEBODY),
    GainWholebody("VelIGain", 7022, 2, EHA_VEL_GAIN_I_WHOLEBODY),
    GainWholebody("VelDGain", 7023, 2, EHA_VEL_GAIN_D_WHOLEBODY),
    GainWholebody("CurPGain", 7031, 2, EHA_CUR_GAIN_P_WHOLEBODY),
    GainWholebody("CurIGain", 7032, 2, EHA_CUR_GAIN_I_WHOLEBODY),
    GainWholebody("CurDGain", 7033, 2, EHA_CUR_GAIN_D_WHOLEBODY),
    GainWholebody("LencOffset", 7050, 4, EHA_LENC_OFFSET_WHOLEBODY),
    GainWholebody("MencOffset", 7051, 4, EHA_MENC_OFFSET_WHOLEBODY),
    GainWholebody("UnReset", 7000, 2, EHA_UNRESET),
)





F = open('hydra_gains.csv', 'w')

for GAIN in GAINS:
    BUF = ''
    # boadtype, box_id, axis_id, slave_id, index(adress), size, value

    for box in GAIN.box:
        offset = 0
        for jnt in box[3]:
            BUF += '%d,%d,%d,%d,0x%d,%d,%d, // %s \n' \
            % (box[0], box[1], offset, box[2],
               GAIN.addr + offset*100, GAIN.size, GAIN.data[jnt], box[4]+'_'+GAIN.name)
            offset = offset + 1

    F.write(BUF)
