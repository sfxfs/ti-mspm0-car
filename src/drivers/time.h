#ifndef TIME_H
#define TIME_H

#include "ti_msp_dl_config.h"

#define delay_us(us) delay_cycles(CPUCLK_FREQ / 1000000 * (us))
#define delay_ms(ms) delay_cycles(CPUCLK_FREQ / 1000 * (ms))

unsigned long get_run_time(void);

#endif
