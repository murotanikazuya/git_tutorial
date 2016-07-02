#ifndef __CHYDRADATAREC_
#define __CHYDRADATAREC_

class CHydraDataRec
{
public:
    CHydraDataRec();
    ~CHydraDataRec();
    int InitDataRec(int ch, int len);
    void Record(double data_in[]);
    void Record(int ch, double data_in);
    void Increment(void);
    int  WriteData(const char filename[]);
private:
    double **data;
    int channels;
    int length;
    int counter;
};

#endif


