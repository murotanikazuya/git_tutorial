#include "chydradatarec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CHydraDataRec::CHydraDataRec()
{
    data = NULL;
    channels = 0;
    length   = 0;
    counter  = 0;
}

CHydraDataRec::~CHydraDataRec()
{
    if(data!=NULL)
    {
        for(int i=0; i<channels; i++)
        {
            free(data[i]);
            data[i] = NULL;
        }
    }
    channels = 0;
}

int CHydraDataRec::InitDataRec(int ch, int len)
{
    data = (double **)malloc(sizeof(double*)*ch);
    if(data==NULL)
    {
        perror("Failed to malloc datarec");
        return -1;
    }

    for(int i=0; i<ch; i++)
    {
        data[i] = (double*)malloc(sizeof(double)*len);
        if(data[i]==NULL)
        {
            perror("Failed to malloc datarec");
            for(int j=i; j>=0; j--)
                free(data[j]);
            free(data);
            return -1;
        }
        memset(data[i], 0, sizeof(double)*len);
    }
    channels = ch;
    length   = len;
    return 0;
}

void CHydraDataRec::Record(double data_in[])
{
    for(int i=0; i<channels; i++)
        data[i][counter] = data_in[i];
}

void CHydraDataRec::Record(int ch, double data_in)
{
    data[ch][counter] = data_in;
}

void CHydraDataRec::Increment(void)
{
    counter = (counter + 1)%length;
}

int CHydraDataRec::WriteData(const char filename[])
{
    FILE *fp;
    fp = fopen(filename, "w");
    if(fp==NULL)
    {
        perror("Data output file open failed");
        return -1;
    }

    for(int i=0; i<length; i++)
    {
        for(int j=0; j<channels; j++)
            fprintf(fp, "%f, ", data[j][(counter+i)%length]);
        fprintf(fp, "\n");
    }
    fclose(fp);
    return 0;
}
