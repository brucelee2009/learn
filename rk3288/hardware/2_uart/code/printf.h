
#ifndef _PRINTF_H
#define _PRINTF_H

#include "uart.h"
#define  __out_putchar  rk_uart_sendbyte


#define  MAX_NUMBER_BYTES  64
extern int printf_test(void);
int printf(const char *fmt, ...);

#endif 

