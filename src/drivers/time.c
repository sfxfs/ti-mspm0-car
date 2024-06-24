#include "time.h"

static volatile unsigned long _run_time = 0;

unsigned long get_run_time()
{
    return _run_time;
}

void SysTick_Handler(void)
{
    _run_time++;
}
