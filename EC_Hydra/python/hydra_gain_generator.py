''' This is to generate .csv gain file to be read from ec_master
    author Tianyi Ko kang@ynl.t.u-tokyo.ac.jp '''

from hydra_gain_wholebody import *
from hydra_gain_singleaxis import *

EHA_SERVO_DIR_WHOLEBODY = ()
for it, val in enumerate(EHA_MOTOR_LENC_INV_WHOLEBODY):
    servo_dir = 0
    if EHA_LENC_PHY_INV_WHOLEBODY[it]:
        servo_dir += 1   # dir |= 0x01
    if EHA_MOTOR_LENC_INV_WHOLEBODY[it]:
        servo_dir += 16  # dir |= 0x10
    if EHA_STRAIN_MOTOR_INV_WHOLEBODY[it]:
        servo_dir += 32  # dir |= 0x20
    EHA_SERVO_DIR_WHOLEBODY += (servo_dir, )

EHA_SERVO_DIR_SINGLEAXIS = ()
for it, val in enumerate(EHA_MOTOR_LENC_INV_SINGLEAXIS):
    servo_dir = 0
    if EHA_LENC_PHY_INV_SINGLEAXIS[it]:
        servo_dir += 1   # dir |= 0x01
    if EHA_MOTOR_LENC_INV_SINGLEAXIS[it]:
        servo_dir += 16  # dir |= 0x10
    if EHA_STRAIN_MOTOR_INV_SINGLEAXIS[it]:
        servo_dir += 32  # dir |= 0x20
    EHA_SERVO_DIR_SINGLEAXIS += (servo_dir, )

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
            #(0, 0, 0, (0, 1, 2), "r_hip"),               #rhip
            (0, 0, 0, (0,), "singleaxis"),               #rhip
        )


GAINS = (
    GainWholebody("Reset", 0x7000, 2, EHA_RESET),
    GainWholebody("ServoDirection", 0x7046, 1, EHA_SERVO_DIR_WHOLEBODY),
    GainWholebody("PosPGain", 0x7011, 2, EHA_POS_GAIN_P_WHOLEBODY),
    GainWholebody("PosIGain", 0x7012, 2, EHA_POS_GAIN_I_WHOLEBODY),
    GainWholebody("PosDGain", 0x7013, 2, EHA_POS_GAIN_D_WHOLEBODY),
    GainWholebody("VelPGain", 0x7021, 2, EHA_VEL_GAIN_P_WHOLEBODY),
    GainWholebody("VelIGain", 0x7022, 2, EHA_VEL_GAIN_I_WHOLEBODY),
    GainWholebody("VelDGain", 0x7023, 2, EHA_VEL_GAIN_D_WHOLEBODY),
    GainWholebody("CurPGain", 0x7031, 2, EHA_CUR_GAIN_P_WHOLEBODY),
    GainWholebody("CurIGain", 0x7032, 2, EHA_CUR_GAIN_I_WHOLEBODY),
    GainWholebody("CurDGain", 0x7033, 2, EHA_CUR_GAIN_D_WHOLEBODY),
    GainWholebody("LencOffset", 0x7050, 4, EHA_LENC_OFFSET_WHOLEBODY),
    GainWholebody("MencOffset", 0x7051, 4, EHA_MENC_OFFSET_WHOLEBODY),
    GainWholebody("UnReset", 0x7000, 2, EHA_UNRESET),
)

F_WHOLEBODY = open('../hydra_gains_wholebody.csv', 'w')

for GAIN in GAINS:
    BUF = ''
    # boadtype, box_id, axis_id, slave_id, index(adress), size, value

    for box in GAIN.box:
        offset = 0
        for jnt in box[3]:
            BUF += '%d,%d,%d,%d,0x%04x,%d,%d, // %s \n' \
            % (box[0], box[1], offset, box[2],
               GAIN.addr + offset*0x0100, GAIN.size, GAIN.data[jnt], box[4]+'_'+GAIN.name)

            offset = offset + 1

    F_WHOLEBODY.write(BUF)

GAINS = (
    GainSingleAxis("Reset", 0x7000, 2, EHA_RESET),
    GainSingleAxis("ServoDirection", 0x7046, 1, EHA_SERVO_DIR_SINGLEAXIS),
    GainSingleAxis("PosPGain", 0x7011, 2, EHA_POS_GAIN_P_SINGLEAXIS),
    GainSingleAxis("PosIGain", 0x7012, 2, EHA_POS_GAIN_I_SINGLEAXIS),
    GainSingleAxis("PosDGain", 0x7013, 2, EHA_POS_GAIN_D_SINGLEAXIS),
    GainSingleAxis("VelPGain", 0x7021, 2, EHA_VEL_GAIN_P_SINGLEAXIS),
    GainSingleAxis("VelIGain", 0x7022, 2, EHA_VEL_GAIN_I_SINGLEAXIS),
    GainSingleAxis("VelDGain", 0x7023, 2, EHA_VEL_GAIN_D_SINGLEAXIS),
    GainSingleAxis("CurPGain", 0x7031, 2, EHA_CUR_GAIN_P_SINGLEAXIS),
    GainSingleAxis("CurIGain", 0x7032, 2, EHA_CUR_GAIN_I_SINGLEAXIS),
    GainSingleAxis("CurDGain", 0x7033, 2, EHA_CUR_GAIN_D_SINGLEAXIS),
    GainSingleAxis("TauPGain", 0x7034, 2, EHA_TAU_GAIN_P_SINGLEAXIS),
    GainSingleAxis("TauIGain", 0x7035, 2, EHA_TAU_GAIN_I_SINGLEAXIS),
    GainSingleAxis("TauDGain", 0x7036, 2, EHA_TAU_GAIN_D_SINGLEAXIS),
    GainSingleAxis("LencOffset", 0x7050, 4, EHA_LENC_OFFSET_SINGLEAXIS),
    GainSingleAxis("MencOffset", 0x7051, 4, EHA_MENC_OFFSET_SINGLEAXIS),
    GainSingleAxis("PresPGain", 0x7037, 2, EHA_PRES_GAIN_P_SINGLEAXIS),
    GainSingleAxis("PresIGain", 0x7038, 2, EHA_PRES_GAIN_I_SINGLEAXIS),
    GainSingleAxis("PresDGain", 0x7039, 2, EHA_PRES_GAIN_D_SINGLEAXIS),
    GainSingleAxis("UnReset", 0x7000, 2, EHA_UNRESET),
)

F_SINGLEAXIS = open('../hydra_gains_singleaxis.csv', 'w')

for GAIN in GAINS:
    BUF = ''
    # boadtype, box_id, axis_id, slave_id, index(adress), size, value

    for box in GAIN.box:
        offset = 0
        for jnt in box[3]:
            BUF += '%d,%d,%d,%d,0x%04x,%d,%d, // %s \n' \
            % (box[0], box[1], offset, box[2],
               GAIN.addr + offset*0x0100, GAIN.size, GAIN.data[jnt], box[4]+'_'+GAIN.name)
            offset = offset + 1

    F_SINGLEAXIS.write(BUF)
