#ifndef CTHREADDATA_H
#define CTHREADDATA_H

#include <pthread.h>

#include "chydradata.h"

#include <stdio.h>
#include <stdarg.h>


class CthreadData: public CHydraData
{
public:
    CthreadData();

    void dbgMsg(const char* format, ...);

    pthread_mutex_t mutex;

    int interp_cnt;
    //const int interp_length = 10000; //10sec
    const int interp_length = 1000; //10sec

    bool log_en_jnt[HYDRA_JNT_MAX];
    bool log_en_eha[EHA_MAX];


    int argc;
    char** argv;

    FILE* fp_msg;

};

#endif // CTHREADDATA_H
