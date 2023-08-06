/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"

#include "encoder.h"
#include "pid.h"
#include "jy901.h"
#include "time.h"
#include "motor.h"
#include "OLED.h"
#include "uart_printf.h"

uint8_t oled_delay = 0;
volatile float speed_l_ex = 0, speed_r_ex = 0;
volatile float speed_l_cur = 0, speed_r_cur = 0;
volatile long count_l = 0, count_r = 0;

pid_config_t pid_l = { .input = &speed_l_cur, .set = &speed_l_ex, .min = -300,
                       .max = 300, .kp = 0.5, .ki = 0, .kd = 0, };

pid_config_t pid_r = { .input = &speed_r_cur, .set = &speed_r_ex, .min = -300,
                       .max = 300, .kp = 0.5, .ki = 0, .kd = 0, };

motor_config_t motorL = { .in1 = MOTOR_DIR_L_IN1_PIN,
                          .in2 = MOTOR_DIR_L_IN2_PIN, .pwmIndex =
                                  DL_TIMER_CC_0_INDEX,
                          .reverse = false, };

motor_config_t motorR = { .in1 = MOTOR_DIR_R_IN1_PIN,
                          .in2 = MOTOR_DIR_R_IN2_PIN, .pwmIndex =
                                  DL_TIMER_CC_1_INDEX,
                          .reverse = true, };

jy901_t jy901 = { 0 };

int main(void)
{
    SYSCFG_DL_init();
    jy901_init(&jy901);
    encoder_init();
    NVIC_EnableIRQ(TIMER_50MS_INST_INT_IRQN);
    pid_config_init(&pid_r);
    pid_config_init(&pid_l);
    OLED_Init();

    for (;;)
    {
        if (get_run_time() - oled_delay >= 100)
        {
            OLED_Printf(1, 1, 8, 1, "sl:%.1f sr:%.1f", speed_l_cur, speed_r_cur);
            OLED_Printf(1, 10, 8, 1, "yaw:%.1f time:%ld", jy901.yaw, get_run_time());
            OLED_Printf(1, 19, 8, 1, "pl:%ld pr:%ld", encoderL_get_pos(), encoderR_get_pos());
            OLED_Refresh_Gram();
            oled_delay = get_run_time();
        }
    }
}

void TIMER_50MS_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_50MS_INST))
    {
    case DL_TIMER_IIDX_ZERO:
        speed_l_cur = encoderL_get_pos() - count_l;
        count_l = speed_l_cur + count_l;

        speed_r_cur = encoderR_get_pos() - count_r;
        count_r = speed_r_cur + count_r;

        break;
    default:
        break;
    }

}
