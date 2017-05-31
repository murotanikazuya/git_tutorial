#ifndef __SERVO_UI_H_
#define __SERVO_UI_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <pthread.h>
#include <ncurses.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/eventfd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>
#include <sys/utsname.h>

double** AllocateMotionData(int num_joints, int length);
void     FreeMotionData(double **data, int length);
int      CheckMotionFileLength(FILE *fp);
int      LoadMotionFile(FILE *fp, double** data, int num_joints, int length);

#endif
