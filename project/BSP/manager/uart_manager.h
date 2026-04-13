#ifndef	__UART_MESSAGE_H
#define __UART_MESSAGE_H
#include "stm32f1xx_hal.h"
#include "UART.h"
#include <stdarg.h>

#define  USE_USART1					1
#define  USE_USART2					2
#define  USE_USART3					3



typedef struct Uart_Rx
{
	UART_HandleTypeDef  Huartx;
	USART_TypeDef  			*USARTx;
	uint8_t 						mode;
	uint8_t 						Uart_Buff[BUFF_MAX_SIZEOF];
	uint16_t 						Uart_i;
	uint8_t 						Rxdata;
	uint8_t   					rx_buff[RX_MAX_SIZEOF];
	uint8_t   					rx_state;
	uint8_t   					rx_flag;
	uint8_t   					rx_i;
	float								float_value1;
	float								float_value2;
	float								float_value3;
	float								float_value4;
	uint16_t 						int_value1;
	uint16_t 						int_value2;
	uint16_t 						int_value3;
	char 								buff[BUFF_MAX_SIZEOF];
}Uart_Rx, *PUart_Rx;

typedef struct Uart_Dev
{
	char name[10];
	/* 对接收到的一个字节一个字节的数据进行存放 */
	void (*UART_Dev_Rx_Data)(struct Uart_Rx *Uartx);
	/* 选择使用的串口 */
	void (*UART_Switch)(USART_TypeDef *Uartx, uint8_t mode);
	/* 初始化 */
	void (*UART_Dev_Init)(struct Uart_Rx *uartx);
	/* 向串口发送数据 */
	void (*UART_Dev_Send)(struct Uart_Rx *uartx, char *buf,unsigned char len);
	/* 向具体的设备发送数据 */
	void (*UART_Dev_Send_Data)(char *data);
	/* 将设备发来的数据进行解析 */
	uint32_t (*UART_Dev_Data_Dispose)(uint8_t timeout);
	/* 获取设备数据 */
	float* (*UART_Get_FloatData)(struct Uart_Rx *uartx);
	uint16_t* (*UART_Get_IntData)(struct Uart_Rx *uartx);
	void* (*UART_Get_StrData)(struct Uart_Rx *uartx);
	/* 清除缓冲区一 */
	void (*Clear_RxBuff1)(struct Uart_Rx *uartx);
	/* 清除缓冲区二 */
	void (*Clear_RxBuff2)(struct Uart_Rx *uartx);
	struct Uart_Rx t_uart;
	struct Uart_Dev *ptNext;
}Uart_Dev, *PUart_Dev;

struct Uart_Dev * SelectDefaultUart(char *name);
struct Uart_Dev * UART_Get_Devs(void);
void RegisterUart(struct Uart_Dev * PUartpOpr);
#endif /* __UART_MESSAGE_H */



