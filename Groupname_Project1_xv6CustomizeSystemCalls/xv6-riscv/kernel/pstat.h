#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"


struct uproc {
    int pid;
    int state;
    int ppid;
    char name[20];
};


#endif