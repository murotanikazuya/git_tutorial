''' gain definition for the single axis testbed '''

from hydra_gain_common import *

EHA_LENC_OFFSET_SINGLEAXIS = (
    7103446,
)


EHA_MENC_OFFSET_SINGLEAXIS = (
    719,
)

''' whether linear encoder direction and
    physical EHA direction in the robot flipped '''
EHA_LENC_PHY_INV_SINGLEAXIS = (
    False,      #singleaxis
)

''' whether linear encoder direction and
    motor current direction is flipped '''
EHA_MOTOR_LENC_INV_SINGLEAXIS = (
    False,      #singleaxis
)


''' whether strain gauge direction and
    motor current direction is flipped
    when the rod is compressed, the strain value decreases '''
EHA_STRAIN_MOTOR_INV_SINGLEAXIS = (
    False,      #singleaxis
)

EHA_POS_GAIN_P_SINGLEAXIS = ( #[100N/m]
    #100, # original
    1000, # only for debug
)



EHA_POS_GAIN_I_SINGLEAXIS = (
    0,
)



EHA_POS_GAIN_D_SINGLEAXIS = (
    0,
)



EHA_VEL_GAIN_P_SINGLEAXIS = (
    1000,
)


EHA_VEL_GAIN_I_SINGLEAXIS = (
    0,
)


EHA_VEL_GAIN_D_SINGLEAXIS = (
    0,
)

EHA_TAU_GAIN_P_SINGLEAXIS = (
    1000,
)


EHA_TAU_GAIN_I_SINGLEAXIS = (
    0,
)


EHA_TAU_GAIN_D_SINGLEAXIS = (
    0,
)

EHA_PRES_GAIN_P_SINGLEAXIS = (
    1000,
)


EHA_PRES_GAIN_I_SINGLEAXIS = (
    0,
)


EHA_PRES_GAIN_D_SINGLEAXIS = (
    0,
)

EHA_GAIN_CUR_FF = (
    GAIN_CUR_FF,
)

EHA_GAIN_CUR_DOB = (
    GAIN_CUR_DOB,
)






EHA_CUR_GAIN_P_SINGLEAXIS = (
    CUR_PGAIN_MAXON_4P_200W  << 8,
)



EHA_CUR_GAIN_I_SINGLEAXIS = (
    CUR_IGAIN_MAXON_4P_200W << 8,
)


EHA_CUR_GAIN_D_SINGLEAXIS = (
    0,
)

