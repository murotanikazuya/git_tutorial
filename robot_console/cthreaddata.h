#ifndef CTHREADDATA_H
#define CTHREADDATA_H

#include <pthread.h>


class CthreadData
{
public:
    CthreadData();

    pthread_mutex_t mutex;
    int interp_cnt;
    //const int interp_length = 10000; //10sec
    const int interp_length = 1000; //10sec

    bool interp_run;
    bool grasp_run;
    bool interp_ready;
    bool filemotion_en = false;
    bool filemotion_run;
    bool sot_en = false;
    bool sot_run = false;

    int argc;
    char** argv;


};

#endif // CTHREADDATA_H
