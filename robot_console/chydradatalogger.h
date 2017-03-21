#ifndef CHYDRADATALOGGER_H
#define CHYDRADATALOGGER_H

#include "cthreaddata.h"

#include <list>

class CHydraDataLogger
{
public:
    CHydraDataLogger(CthreadData* hydraData_in);

    void openNewFile();
    void closeFile(void);

    void pushToBuffer(CthreadData* bufdata);

    void takeLog(void);
    void loggingTask(void);

    std::list<CthreadData> buffer;

private:
    FILE* fp=nullptr;
    CthreadData* hydraData;

};

void* loggingTask_servoui(void* param);

#endif // CHYDRADATALOGGER_H
