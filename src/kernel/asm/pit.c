#include "asm/pit.h"
#include "sys/const.h"
#include "sys/proto.h"
#include "type.h"

PUBLIC void init_pit(void)
{
    /* Initialize 8253 pit (10ms between clock interrupt) */
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (u8) (TIMER_FREQ/HZ));
    out_byte(TIMER0, (u8) ((TIMER_FREQ/HZ) >> 8)); 
}