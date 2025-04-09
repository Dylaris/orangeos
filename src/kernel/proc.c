#include "type.h"
#include "const.h"
#include "proto.h"
#include "global.h"
#include "proc.h"

PUBLIC int sys_get_ticks(void)
{
    return ticks;
}