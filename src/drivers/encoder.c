#include "encoder.h"

static volatile long _encoder_l_count = 0, _encoder_r_count = 0;
static uint32_t _encoder_val = 0;

void encoder_init()
{
    NVIC_EnableIRQ(ENCODER_INT_IRQN);
}

long encoderL_get_pos()
{
    return _encoder_l_count;
}

long encoderR_get_pos()
{
    return _encoder_r_count;
}

void GROUP1_IRQHandler(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
    case ENCODER_INT_IIDX: // encoder
        _encoder_val = DL_GPIO_readPins(GPIOA,
        ENCODER_L_A_PIN | ENCODER_L_B_PIN | ENCODER_R_A_PIN | ENCODER_R_B_PIN);
        if (DL_GPIO_getEnabledInterruptStatus(GPIOA, ENCODER_L_A_PIN))
        {
            if (_encoder_val & ENCODER_L_B_PIN)
            {
                _encoder_l_count++;
            }
            else
            {
                _encoder_l_count--;
            }
            DL_GPIO_clearInterruptStatus(GPIOA, ENCODER_L_A_PIN);
        }
        if (DL_GPIO_getEnabledInterruptStatus(GPIOA, ENCODER_L_B_PIN))
        {
            if (_encoder_val & ENCODER_L_A_PIN)
            {
                _encoder_l_count++;
            }
            else
            {
                _encoder_l_count--;
            }
            DL_GPIO_clearInterruptStatus(GPIOA, ENCODER_L_B_PIN);
        }

        if (DL_GPIO_getEnabledInterruptStatus(GPIOA, ENCODER_R_A_PIN))
        {
            if (_encoder_val & ENCODER_R_B_PIN)
            {
                _encoder_r_count++;
            }
            else
            {
                _encoder_r_count--;
            }
            DL_GPIO_clearInterruptStatus(GPIOA, ENCODER_R_A_PIN);
        }
        if (DL_GPIO_getEnabledInterruptStatus(GPIOA, ENCODER_R_B_PIN))
        {
            if (_encoder_val & ENCODER_R_A_PIN)
            {
                _encoder_r_count++;
            }
            else
            {
                _encoder_r_count--;
            }
            DL_GPIO_clearInterruptStatus(GPIOA, ENCODER_R_B_PIN);
        }
        break;

    default:
        break;
    }
}
