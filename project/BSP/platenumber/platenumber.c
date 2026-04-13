#include "platenumber.h"
#include "UART.h"
#include "OLED.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
const struct province_c platebuff[] = {
"冀","5180",
"晋","664b",
"辽","8fbd",
"吉","5409",
"黑","9ed1",
"苏","82cf",
"浙","6d59",
"皖","7696",
"闽","95fd",
"赣","8d63",
"鲁","9c81",
"豫","8c6b",
"鄂","9102",
"湘","6e58",
"粤","7ca4",
"琼","743c",
"川","5ddd",
"贵","8d35",
"云","4e91",
"陕","9655",
"甘","7518",
"青","9752",
"蒙","8499",
"桂","6842",
"宁","5b81",
"京","4eac",
};


struct plate_info plate;

static void Plate_RX_DATA(struct Uart_Rx *Uartx);
static void Plate_UART_Switch(USART_TypeDef *Uartx, uint8_t mode);
static uint32_t Plate_UART_Dev_Data_Dispose(uint8_t timeout);
static void Plate_Clear_RxBuff2(struct Uart_Rx *uartx);
static void* Plate_Get_StrData(struct Uart_Rx *uartx);
static void Plate_Dev_Init(struct Uart_Rx *uartx);
struct Uart_Dev g_Uart_Plate = {
	.name 										= "Plate",
	.UART_Dev_Rx_Data 				= Plate_RX_DATA,
	.UART_Dev_Init						= Plate_Dev_Init,
	.UART_Switch 							= Plate_UART_Switch,
	.UART_Dev_Data_Dispose 		= Plate_UART_Dev_Data_Dispose,
	.Clear_RxBuff2						= Plate_Clear_RxBuff2,
	.UART_Get_StrData					= Plate_Get_StrData,
};
static void Plate_Dev_Init(struct Uart_Rx *uartx)
{
	#if (Plate_UARTx == USE_USART1)
		extern UART_HandleTypeDef huart1;
		HAL_UART_Receive_IT(&huart1, &uartx->Rxdata, 1);
	#elif (Plate_UARTx == USE_USART2)
		extern UART_HandleTypeDef huart2;
		HAL_UART_Receive_IT(&huart2, &uartx->Rxdata, 1);
	#elif (Plate_UARTx == USE_USART3)
		extern UART_HandleTypeDef huart3;
		HAL_UART_Receive_IT(&huart3, &uartx->Rxdata, 1);
	#endif
}
static void Plate_RX_DATA(struct Uart_Rx *Uartx)
{
	#if (Plate_UARTx == USE_USART1)
		extern UART_HandleTypeDef huart1;
		HAL_UART_Receive_IT(&huart1, &Uartx->Rxdata, 1);
	#elif (Plate_UARTx == USE_USART2)
		extern UART_HandleTypeDef huart2;
		HAL_UART_Receive_IT(&huart2, &Uartx->Rxdata, 1);
	#elif (Plate_UARTx == USE_USART3)
		extern UART_HandleTypeDef huart3;
		HAL_UART_Receive_IT(&huart3, &Uartx->Rxdata, 1);
	#endif
	switch(Uartx->rx_state)
	{
		case 0:
			if ((Uartx->Rxdata == '{') && (Uartx->rx_flag == 0))
			{
				Uartx->rx_state = 1;
				Uartx->rx_i = 0;
			}
			break;
		case 1:
			if (Uartx->Rxdata == '}')
			{
				Uartx->rx_state = 0;
				Uartx->rx_flag = 1;
			}
			else
				Uartx->rx_buff[Uartx->rx_i++] = Uartx->Rxdata;
			break;
	}
}


static void Plate_UART_Switch(USART_TypeDef *Uartx, uint8_t mode)
{
	uint8_t dat;
 UART_HandleTypeDef huartx;
	#if (Plate_UARTx == USE_USART1)
		extern UART_HandleTypeDef huart1;
		HAL_UART_Receive_IT(&huart1, &dat, 1);
		huartx = huart1;
	#elif (Plate_UARTx == USE_USART2)
		extern UART_HandleTypeDef huart2;
		HAL_UART_Receive_IT(&huart2, &dat, 1);
		huartx = huart2;
	#elif (Plate_UARTx == USE_USART3)
		extern UART_HandleTypeDef huart3;
		HAL_UART_Receive_IT(&huart3, &dat, 1);
		huartx = huart3;
	#endif
	g_Uart_Plate.t_uart.USARTx 		= Uartx;
	g_Uart_Plate.t_uart.Huartx  	= huartx;
}

struct Uart_Dev* Plate_Dev_Reg(char *dev, USART_TypeDef *Uartx)
{
	strcpy(g_Uart_Plate.name, dev);
	g_Uart_Plate.UART_Switch(Uartx, 0);
	RegisterUart(&g_Uart_Plate);
	return SelectDefaultUart(dev);
}
static void Plate_Clear_RxBuff2(struct Uart_Rx *uartx)
{
	memset(uartx->rx_buff, 0, sizeof(uartx->rx_buff));
}


uint8_t Plate_Font(char *name, char *unicode)
{
	uint8_t i = 0;
	for (i = 0; i < 26; i++)
	{
		if (strcmp(unicode, platebuff[i].unicode) == 0)
		{
			strcpy(name, platebuff[i].name);
			return 1;
		}
	}
	return 0;
}

//uint8_t Plate_Check(uint16_t timeout)
//{
//	if (g_Uart_Plate.t_uart.rx_flag == 1)
//	{
////		OLED_ShowString(1, 48, (char *)g_Uart_Plate.t_uart.rx_buff, 8);
//		g_Uart_Plate.t_uart.rx_buff[strlen((char *)g_Uart_Plate.t_uart.rx_buff)] = '\0';
//		sscanf((char *)g_Uart_Plate.t_uart.rx_buff, "%s,", plate.font_unicode);
//		plate.font_unicode[4] = '\0';
//		int i = 0;
//		char *p = (char *)g_Uart_Plate.t_uart.rx_buff;
//		p += 5;
//		while(*p != ',')
//			plate.number[i++] = *(p++);
//		plate.number[i] = '\0';
//		Plate_Font(plate.font, plate.font_unicode);
////		OLED_ShowChinese(1, 48, plate.font);
////		OLED_ShowString(40, 48, plate.number, 8);
//		g_Uart_Plate.Clear_RxBuff2(&g_Uart_Plate.t_uart);
//		g_Uart_Plate.t_uart.rx_flag = 0;
//		return PLATE_RX_OK;
//	}
//	return PLATE_RX_WAIT;
//}

uint8_t Plate_Check(uint16_t timeout)
{
    if (g_Uart_Plate.t_uart.rx_flag == 1)
    {
        char *rx_data = (char *)g_Uart_Plate.t_uart.rx_buff;
        int len = strlen(rx_data);
        
        // 调试：打印接收到的原始数据
        // printf("Received: %s, Len: %d\n", rx_data, len);
        
        if (len >= 10)  // 确保数据长度足够
        {
            // 方法1：固定长度解析（假设前4位是unicode，后6位是车牌号）
            // 提取前4个字符作为unicode
            strncpy(plate.font_unicode, rx_data, 4);
            plate.font_unicode[4] = '\0';
            
            // 提取后6个字符作为车牌号
            strncpy(plate.number, rx_data + 4, 6);
            plate.number[6] = '\0';
            
            // 解析省份名称
            Plate_Font(plate.font, plate.font_unicode);
						OLED_ShowChinese(1, 48, plate.font);
						OLED_ShowString(40, 48, plate.number, 8);
            // 调试输出
            // printf("Unicode: %s, Number: %s, Province: %s\n", 
            //        plate.font_unicode, plate.number, plate.font);
        }
        else
        {
            // 数据长度不足
            return PLATE_RX_WAIT;
        }
        
        g_Uart_Plate.Clear_RxBuff2(&g_Uart_Plate.t_uart);
        g_Uart_Plate.t_uart.rx_flag = 0;
        return PLATE_RX_OK;
    }
    return PLATE_RX_WAIT;
}

static uint32_t Plate_UART_Dev_Data_Dispose(uint8_t timeout)
{
	static char font_1[16] = {0}, font_2[16] = {0}, font_3[16] = {0};
	static uint8_t plate_count = 0;
//	return Plate_Check(timeout);
	if (Plate_Check(timeout) == PLATE_RX_OK)
	{
		if (plate_count == 0)
		{
			strcpy(font_1, plate.font);
			plate_count = 1;
		}
		else if (plate_count == 1)
		{
			if (strcmp(font_1, plate.font) == 0)
			{
				strcpy(font_2, plate.font);
				plate_count = 2;
			}
			else
				plate_count = 0;
		}
		else if (plate_count == 2)
		{
			if (strcmp(font_2, plate.font) == 0)
			{
				strcpy(font_3, plate.font);
				plate_count = 3;
			}
			else
				plate_count = 0;
		}
		else if (plate_count == 3)
		{
			if (strcmp(font_3, plate.font) == 0)
			{
				return PLATE_RX_OK;
			}
			plate_count = 0;
		}
	}
	return PLATE_RX_WAIT;
}



static void* Plate_Get_StrData(struct Uart_Rx *uartx)
{
	return (void *)&plate;
}






