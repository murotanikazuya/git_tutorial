''' common hydra gain definition '''

CTRLWD_RESET = 0x8000
CTRLWD_UNRESET = 0x0200

''' for current gain, only the upper 8bits is used
    but they are sent as 16 bit, with the eight 0 in the LSB side '''
CUR_PGAIN_MAXON_4P_200W = 0x1c # 0x1c00
CUR_IGAIN_MAXON_4P_200W = 0x4
