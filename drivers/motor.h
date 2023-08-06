#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "ti_msp_dl_config.h"

typedef struct motor_config
{
    uint32_t in1;
    uint32_t in2;
    DL_TIMER_CC_INDEX pwmIndex;
    bool reverse;
} motor_config_t;

void motor_set_output(motor_config_t config, int32_t duty);

#endif
