#ifndef ENCODER_H
#define ENCODER_H

#include "ti_msp_dl_config.h"

void encoder_init();
long encoderL_get_pos();
long encoderR_get_pos();

#endif
