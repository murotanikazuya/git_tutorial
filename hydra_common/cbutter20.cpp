#include "cbutter20.h"

CButter20::CButter20()
{
    idx = 0;
    for(int i=0; i<FIL_BUF_RING_LEN; i++)
    {
        x[i] = 0.0;
        y[i] = 0.0;
    }
}

double CButter20::Step(double xin)
{
    int idx_start;
    double ret;
    idx_start = ((idx-3+FIL_BUF_RING_LEN)%FIL_BUF_RING_LEN);
    x[idx] = xin;
    filterbutter20();
    ret = y[idx];
    idx = (idx+1)%FIL_BUF_RING_LEN;
    return ret;
}

void CButter20::filterbutter20(void)
{
    y[idx] = b[0]*x[idx] 
        + b[1]*x[(idx-1+FIL_BUF_RING_LEN)%FIL_BUF_RING_LEN] 
        + b[2]*x[(idx-2+FIL_BUF_RING_LEN)%FIL_BUF_RING_LEN] 
        + b[3]*x[(idx-3+FIL_BUF_RING_LEN)%FIL_BUF_RING_LEN] 
        - a[1]*y[(idx-1+FIL_BUF_RING_LEN)%FIL_BUF_RING_LEN] 
        - a[2]*y[(idx-2+FIL_BUF_RING_LEN)%FIL_BUF_RING_LEN] 
        - a[3]*y[(idx-3+FIL_BUF_RING_LEN)%FIL_BUF_RING_LEN];
}

