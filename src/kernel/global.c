#define GLOBAL_VARIABLE_HERE
#include "proto.h"
#include "global.h"

TASK task_table[NR_TASKS] = {
    {TestA, STACK_SIZE_TESTA, "TestA"},
    {TestB, STACK_SIZE_TESTB, "TestB"},
    {TestC, STACK_SIZE_TESTC, "TestC"}
};

system_call sys_call_table[NR_SYS_CALL] = {
    sys_get_ticks
};