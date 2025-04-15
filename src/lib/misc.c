#include "sys/const.h"
#include "sys/proto.h"

/**
 * @brief Invoked by assert()
 * @param exp       The failure expression itself
 * @param file      __FILE__
 * @param base_file __BASE_FILE__
 * @param line      __LINE__
 * @return 
 */
PUBLIC void assertion_failure(char *exp, char *file, char *base_file, int line)
{
    printl("%c assert(%s) failed: file: %s, base_file: %s, ln%d",
            MAG_CH_ASSERT, exp, file, base_file, line);

    /**
     * If assertion fails in a TASK, the system will halt before
     * printl() returns. If it happens in a USER PROC, printl() will
     * return like a common routine and arrive here
     * @see sys_printx()
     * 
     * We use a forever loop to prevent the proc from going on
     */
    spin("assertion_failure()");

    /* Should never arrive here */
    __asm__ __volatile__("ud2");
}

/**
 * @brief Print some message and trap in forever loop
 * @param func_name The caller function name
 * @return 
 */
PUBLIC void spin(char *func_name)
{
    printl("\nspinning in %s ...\n", func_name);
    while (1) {}
}