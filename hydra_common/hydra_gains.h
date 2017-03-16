#ifndef HYDRA_GAINS_H
#define HYDRA_GAINS_H

const double all_joint_angle_limit[][2] = {//radians
        { DEG2RAD(-60),    DEG2RAD(60)   },
        { DEG2RAD(-105),   DEG2RAD(13)   },
        { DEG2RAD(-22.5),  DEG2RAD(22.5) },
        { DEG2RAD(-15),    DEG2RAD(120)  },//rknee
        { DEG2RAD(-24.2),  DEG2RAD(24.2) },
        { DEG2RAD(-44),    DEG2RAD(34)   },
        { DEG2RAD(-60),    DEG2RAD(60)   },
        { DEG2RAD(-105),   DEG2RAD(13)   },
        { DEG2RAD(-22.5),  DEG2RAD(22.5) },
        { DEG2RAD(-15),    DEG2RAD(120)  },//lknee
        { DEG2RAD(-24.2),  DEG2RAD(24.2) },
        { DEG2RAD(-44),    DEG2RAD(34)   },
        { DEG2RAD(-20),    DEG2RAD(60)   },
        { DEG2RAD(-22.5),  DEG2RAD(22.5) },
        { DEG2RAD(0),      DEG2RAD(120)  },
    { DEG2RAD(-30),    DEG2RAD(90)   },//rscaplae
    { DEG2RAD(-90),    DEG2RAD(90)   },
        { DEG2RAD(-117.5), DEG2RAD(17.5) },
    { DEG2RAD(-65),    DEG2RAD(90)   }, //rshoulder
        { DEG2RAD(-117),   DEG2RAD(9)    },
        { DEG2RAD(-10),    DEG2RAD(110)  },
    { DEG2RAD(-90),  DEG2RAD(90) },
        { DEG2RAD(-66),    DEG2RAD(85)   },
    { DEG2RAD(-90),    DEG2RAD(30)   },//lscaplae
    { DEG2RAD(-90),    DEG2RAD(90)   },
        { DEG2RAD(-17.5),  DEG2RAD(117.5)},
    { DEG2RAD(-90),    DEG2RAD(65)   }, //lshoulder
        { DEG2RAD(-117),   DEG2RAD(9)    },
        { DEG2RAD(-110),   DEG2RAD(10)   },
    { DEG2RAD(-90),  DEG2RAD(90) },
        { DEG2RAD(-66),    DEG2RAD(85)   },
        { DEG2RAD(-200),      DEG2RAD(100000)    },
        { DEG2RAD(-200),      DEG2RAD(100000)    },
        { DEG2RAD(-200),      DEG2RAD(100000)    },
        { DEG2RAD(-200),      DEG2RAD(100000)    },
        { DEG2RAD(-200),      DEG2RAD(100000)    },
        { DEG2RAD(0),      DEG2RAD(0)    },
        { DEG2RAD(0),      DEG2RAD(0)    },
        { DEG2RAD(0),      DEG2RAD(0)    },
        { DEG2RAD(0),      DEG2RAD(0)    },
        { DEG2RAD(0),      DEG2RAD(0)    }
};



#endif // HYDRA_GAINS_H
