#ifndef __CBUTTER20_H_
#define __CBUTTER20_H_

#define FIL_BUF_RING_LEN 4
class CButter20
{
public:
    CButter20();
    double Step(double xin);
private:
    void filterbutter20();
    double x[FIL_BUF_RING_LEN];
    double y[FIL_BUF_RING_LEN];
    int idx;
    const double b[FIL_BUF_RING_LEN] = {4.1810871564134852e-04,   1.2543261469240455e-03,   1.2543261469240455e-03,   4.1810871564134852e-04};
    const double a[FIL_BUF_RING_LEN] = {1.0000000000000000e+00,  -2.6857463076854580e+00,   2.4189545822918888e+00,  -7.2986340488129964e-01};
};

#endif /*__CBUTTER20_H_*/
