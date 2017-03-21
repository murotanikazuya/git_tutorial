#include "cthreaddata.h"

CthreadData:: CthreadData()
{
    flags["interp_run"] = false;
    flags["grasp_run"] = false;
    flags["interp_ready"] = false;
    flags["filemotion_en"] = false;
    flags["filemotion_run"] = false;
    flags["interp_run"] = false;
    flags["logging_en"] = false;
    flags["logging_newfile"] = false;

    fp_msg = fopen("log_msg","w");
    //fprintf(fp_msg,"CthreadData constrution done\n");
    //fflush(fp_msg);
    this->dbgMsg("CthradData construction done \n");
}

void CthreadData::dbgMsg(const char* format, ...)
{
    va_list arg;
    va_start(arg,format);

    vfprintf(fp_msg,format,arg);
    fflush(fp_msg);

    va_end(arg);
}
