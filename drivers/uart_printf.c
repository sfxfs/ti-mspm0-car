#include "ti_msp_dl_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static char UART_TX_BUF[256] = {0};

void printf_to_uart(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(UART_TX_BUF, fmt, ap);
    va_end(ap);
    DL_UART_Main_fillTXFIFO(CH340_INST, (uint8_t *)UART_TX_BUF, strlen(UART_TX_BUF));
}
