#ifndef __ESP8266_H
#define __ESP8266_H
#include "stm32f1xx_hal.h"
#include "uart_manager.h"

#define ESP8266_UARTx				USE_USART1			

#define ESP8266_MODE_STA		0			/* 客户端模式 */
#define ESP8266_MODE_AP			1			/* 服务器模式 */

#define RXERROR							-1
#define REV_OK							0			//接收完成
#define REV_WAIT						1			//等待接收完成
struct Uart_Dev* ESP8266_Dev_Reg(char *dev, uint8_t mode, USART_TypeDef *Uartx);
#endif
