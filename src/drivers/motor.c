#include <stdlib.h>
#include "motor.h"

void motor_set_output(motor_config_t config, int32_t duty)
{
    if (config.reverse == true)
    {
        duty = -duty;
    }
    if (duty >= 0)
    {
        DL_GPIO_setPins(GPIOA, config.in2);
        DL_GPIO_clearPins(GPIOA, config.in1);
    }
    else
    {
        DL_GPIO_setPins(GPIOA, config.in1);
        DL_GPIO_clearPins(GPIOA, config.in2);
    }
    DL_TimerG_setCaptureCompareValue(MOTOR_PWM_INST, 1000 - abs(duty), config.pwmIndex);
}
