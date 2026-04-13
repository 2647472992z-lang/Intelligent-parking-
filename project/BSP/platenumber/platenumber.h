#ifndef __PLATENUMBER_H
#define __PLATENUMBER_H
#include "stm32f1xx_hal.h"
#include "uart_manager.h"
#define Plate_UARTx		USE_USART3


#define PLATE_RX_WAIT      	0
#define PLATE_RX_OK					1

struct province_c
{
	char name[3];
	char unicode[5];
};

struct plate_info
{
	char font_unicode[5];
	char font[16];
	char number[7];
	char plate_number[13];
};
struct Uart_Dev* Plate_Dev_Reg(char *dev, USART_TypeDef *Uartx);
#endif /* __PLATENUMBER_H */



