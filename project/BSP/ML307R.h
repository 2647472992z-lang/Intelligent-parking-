#ifndef __ML307R_H
#define __ML307R_H
#include "stm32f1xx_hal.h"
#include "uart_manager.h"

#define ML307R_UARTx				USE_USART1			

#define ML307R_ERROR				0
#define ML307R_OK						1			
#define ML307R_WAIT					-1		

#define ML304_GPS_OPEN			1
#define	ML304_GPS_CLOSE			0

#define ML307R_STATE_IDLE     0
#define ML307R_STATE_BUSY     1
#define ML307R_STATE_ERROR    2

typedef struct 
{
	double longitude;			/* 经度 */
	double latitude;			/* 纬度 */
}GPS_Info_t;
struct tML307R
{
	struct Uart_Dev uart;
	GPS_Info_t	GPS;			/* 存放GPS数据 */
	void 			(*Init)(struct tML307R	**dev, _Bool is_gps);															/* 初始化 */
	uint8_t 			(*Send_Data)(struct tML307R **dev, char *data);				/* 发送数据 */
	uint32_t 	(*RxData_Dispose)(struct tML307R **dev);																	/* 接收数据并提取 */
	void   		(*Get_GPS)(struct tML307R **dev);																					/* 获取定位 */
	
};

void ML307R_Reset_Module_State(void);
void ML307R_Reconnect(struct Uart_Dev **uart);//重连

void ML307R_Recover_From_Error(void);

struct tML307R* ML307R_Register(char *dev,  USART_TypeDef *Uartx);
#endif
