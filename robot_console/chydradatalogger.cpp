#include "chydradatalogger.h"
#include "cthreaddata.h"

#include <list>

CHydraDataLogger::CHydraDataLogger(CthreadData* hydraData_in)
{
    hydraData =hydraData_in;
}

void CHydraDataLogger::openNewFile(void)
{
    if(fp!=nullptr) fclose(fp);

    time_t     timer;
    time(&timer);
    struct tm *local = localtime(&timer);
    char       datename[256],filename[256];

    snprintf(datename,256,"_%04d%02d%02d_%02d%02d%02d",
             local->tm_year + 1900, local->tm_mon + 1, local->tm_mday, local->tm_hour, local->tm_min, local->tm_sec);
    snprintf(filename,256,"%s%s%s", "pdo",datename,".csv");

    fp = fopen(filename,"w+");

}

void CHydraDataLogger::closeFile(void)
{
    if(fp!=nullptr) fclose(fp);
}

void CHydraDataLogger::pushToBuffer(CthreadData* bufdata)
{
    buffer.push_back(*bufdata);

    if(buffer.size()>100)
        buffer.pop_front();
}

void CHydraDataLogger::takeLog(void)
{
    while(!buffer.empty()){

        //CthreadData* hydraData = buffer.begin();

        for(int i=0;i<HYDRA_JNT_MAX;i++){
            fprintf(fp,"%lf,", buffer.begin()->jnt.act.pos[i]);
        }

        fprintf(fp,"\n");
        buffer.pop_front();
    }
    fflush(fp);

}

void CHydraDataLogger::loggingTask()
{

    while(1){
        if(hydraData->flags["logging_newfile"]){
            this->openNewFile();
            hydraData->flags["logging_newfile"] = false;
        }

        if(hydraData->flags["logging_en"])
            takeLog();

        usleep(20);

    }
}

void* loggingTask_servoui(void* param)
{
    CHydraDataLogger* logger = (CHydraDataLogger*)param;
    logger->loggingTask();
}


