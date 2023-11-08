#ifndef PROXLOG_PROC_H
#define PROXLOG_PROC_H

#include "pack.h"
#include "packin.h"
#include "packout.h"

namespace proxlog
{
    PackIn procIn(PackIn in);
    PackOut procOut(PackOut in);
}


#endif