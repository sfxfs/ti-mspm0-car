#include "pid.h"

void pid_config_init(pid_config_t *handle)
{
    handle->_P = 0;
    handle->_I = 0;
    handle->_D = 0;
    handle->_error = 0;
    handle->_last_error = 0;
}

float pid_cal_output(pid_config_t *handle)
{
    handle->_error = *handle->set - *handle->input;

    handle->_P = handle->_error;
    handle->_I = handle->_I + handle->_error;
    handle->_D = handle->_error - handle->_last_error;

    handle->_last_error = handle->_error;
    return constrain(((handle->kp * handle->_P) + (handle->ki * handle->_I) + (handle->kd * handle->_D)), handle->min, handle->max);
}
