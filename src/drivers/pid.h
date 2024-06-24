#ifndef PID_H
#define PID_H

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

typedef struct
{
    volatile float *input;
    volatile float *set;
    float min;
    float max;
    float kp;
    float ki;
    float kd;
    float _P;
    float _I;
    float _D;
    float _error;
    float _last_error;
} pid_config_t;

void pid_config_init(pid_config_t *handle);
float pid_cal_output(pid_config_t *handle);

#endif
