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
    flags["filetorque_en"] = false;
    flags["skip_jntcnv"] = false; // for single axis, skip joint <-> eha conversion
    flags["walking_run"] = false;//2017/05/12 murotani

    for(int i=0;i<HYDRA_JNT_MAX;i++)
        log_en_jnt[i] = false;

    for(int i=0;i<EHA_MAX;i++)
        log_en_eha[i] = false;

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
