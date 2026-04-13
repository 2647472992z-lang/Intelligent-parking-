#include "ESP8266.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "OLED.h"
#define  	aliyun			0
#define 	huaweiyun		1
/********* 使用STA模式需要更改一下参数 *********/
#if huaweiyun
/* 华为云 */
static char *Username			= "69b28a61e094d6159225ab81_XAMD003_ESPDUAN";
static char *Password			= "665508a751b802b72331da034112e0f9d7c89bc304b45b124d2b0d938db2ddf5";
static char *ClientID			= "69b28a61e094d6159225ab81_XAMD003_ESPDUAN_0_0_2026031209";
static char *MQTTHostURL	= "d712a3b7a2.st1.iotda-device.cn-east-3.myhuaweicloud.com";
static char *PropPort			= "$oc/devices/XAMD003_ESPDUAN/sys/messages/up";	
static char *Wifi_Info		= "\"XAMD003\",\"12345678\"";
static char *SubTopic			= "XAMD003_ESPDUAN";
#endif
#if aliyun
/* 阿里云 */
static char *ProductKey  	= "k2806KbItki";
static char *DeviceName		= "DANPIANJIDUAN";
static char *Password			= "682d4bb06f805a09b38bef4a84f50c989503feb3ef4cc526ebce3fc13448d39d";
static char *ClientID			= "k2806KbItki.DANPIANJIDUAN|securemode=2\\,signmethod=hmacsha256\\,timestamp=1758965325945|";
static char *MQTTHostURL	= "iot-06z00dr932rjmi3.mqtt.iothub.aliyuncs.com";
static char *PropPort			= "/k2806KbItki/DANPIANJIDUAN/user/DANPIANJIDUAN";	
static char *Wifi_Info		= "\"DPJ039\",\"12345678\"";
static char *SubTopic			= "/sys/k2806KbItki/DANPIANJIDUAN/thing/service/property/set";
#endif
static char *Port					= "1883";
/**********************************************/

static void ESP8266_RX_DATA(struct Uart_Rx *Uartx);
static void ESP8266_UART_Switch(USART_TypeDef *Uartx, uint8_t mode);
static void ESP8266_UART_Dev_Init(struct Uart_Rx *uartx);
static void ESP8266_UART_Dev_Send(struct Uart_Rx *uartx, char *buf,unsigned char len);
static void ESP8266_UART_Dev_Send_Data(char *data);
static uint32_t ESP8266_UART_Dev_Data_Dispose(uint8_t timeout);
static void ESP8266_Clear_RxBuff1(struct Uart_Rx *uartx);
static void ESP8266_Clear_RxBuff2(struct Uart_Rx *uartx);

uint8_t ESP8266_SendCmd(char *cmd, char *res, uint16_t time);

struct Uart_Dev g_Uart_ESP8266 = {
	.name 										= "ESP8266",
	.UART_Dev_Rx_Data 				= ESP8266_RX_DATA,
	.UART_Switch 							= ESP8266_UART_Switch,
	.UART_Dev_Init						= ESP8266_UART_Dev_Init,
	.UART_Dev_Send						= ESP8266_UART_Dev_Send,
	.UART_Dev_Send_Data 			= ESP8266_UART_Dev_Send_Data,
	.UART_Dev_Data_Dispose 		= ESP8266_UART_Dev_Data_Dispose,
	.Clear_RxBuff1						= ESP8266_Clear_RxBuff1,
	.Clear_RxBuff2						= ESP8266_Clear_RxBuff2,
};

static void ESP8266_RX_DATA(struct Uart_Rx *Uartx)
{
	#if (ESP8266_UARTx == USE_USART1)
		extern UART_HandleTypeDef huart1;
		HAL_UART_Receive_IT(&huart1, &Uartx->Rxdata, 1);
	#elif (ESP8266_UARTx == USE_USART2)
		extern UART_HandleTypeDef huart2;
		HAL_UART_Receive_IT(&huart2, &Uartx->Rxdata, 1);
	#elif (ESP8266_UARTx == USE_USART3)
		extern UART_HandleTypeDef huart3;
		HAL_UART_Receive_IT(&huart3, &Uartx->Rxdata, 1);
	#endif
	Uartx->Uart_Buff[Uartx->Uart_i++] = Uartx->Rxdata;
	switch(Uartx->rx_state)
	{
		case 0:
			if (Uartx->Rxdata == 'M')
				Uartx->rx_state = 1;
			break;
		case 1:
			if (Uartx->Rxdata == 'Q')
				Uartx->rx_state = 2;
			else
				Uartx->rx_state = 0;
			break;
		case 2:
			if (Uartx->Rxdata == 'T')
				Uartx->rx_state = 3;
			else
				Uartx->rx_state = 0;
			break;
		case 3: 
			if (Uartx->Rxdata == 'T')
				Uartx->rx_state = 4;
			else
				Uartx->rx_state = 0;
			break;
		case 4:
			if (Uartx->Rxdata == 'S')
				Uartx->rx_state = 5;
			else
				Uartx->rx_state = 0;
			break;
		case 5:
			if (Uartx->Rxdata == 'U')
				Uartx->rx_state = 6;
			else
				Uartx->rx_state = 0;
		break;
		case 6:
			if (Uartx->Rxdata == 'B')
				Uartx->rx_state = 7;
			else
				Uartx->rx_state = 0;
			break;
		case 7:
			if (Uartx->Rxdata == 'R')
				Uartx->rx_state = 8;
			else
				Uartx->rx_state = 0;
		break;
		case 8:
			if (Uartx->Rxdata == 'E')
				Uartx->rx_state = 9;
			else
				Uartx->rx_state = 0;
		break;
		case 9:
			if (Uartx->Rxdata == 'C')
				Uartx->rx_state = 10;
			else
				Uartx->rx_state = 0;
		break;
		case 10:
			if (Uartx->Rxdata == 'V')
				Uartx->rx_state = 11;
			else
				Uartx->rx_state = 0;
		break;
		case 11:
			if(Uartx->Rxdata == '}')
			{
				Uartx->rx_state = 0;
				Uartx->rx_flag = 1;
				Uartx->rx_i = 0;
			}
			else
				Uartx->rx_buff[Uartx->rx_i++] = Uartx->Rxdata;
			break;
	}
}


static void ESP8266_UART_Switch(USART_TypeDef *Uartx, uint8_t mode)
{
	uint8_t dat;
	UART_HandleTypeDef huartx;
	#if (ESP8266_UARTx == USE_USART1)
		extern UART_HandleTypeDef huart1;
		HAL_UART_Receive_IT(&huart1, &dat, 1);
		huartx = huart1;
	#elif (ESP8266_UARTx == USE_USART2)
		extern UART_HandleTypeDef huart2;
		HAL_UART_Receive_IT(&huart2, &dat, 1);
		huartx = huart2;
	#elif (ESP8266_UARTx == USE_USART3)
		extern UART_HandleTypeDef huart3;
		huartx = huart3;
		HAL_UART_Receive_IT(&huart3, &dat, 1);
	#endif
	g_Uart_ESP8266.t_uart.USARTx 	= Uartx;
	g_Uart_ESP8266.t_uart.Huartx  	= huartx;
	g_Uart_ESP8266.t_uart.mode     = mode;
}

struct Uart_Dev* ESP8266_Dev_Reg(char *dev, uint8_t mode, USART_TypeDef *Uartx)
{
	strcpy(g_Uart_ESP8266.name, dev);
	g_Uart_ESP8266.UART_Switch(Uartx, mode);
	RegisterUart(&g_Uart_ESP8266);
	return SelectDefaultUart(dev);
}

static void ESP8266_Clear_RxBuff1(struct Uart_Rx *uartx)
{
  memset(uartx->Uart_Buff, 0, sizeof(uartx->Uart_Buff));
  uartx->Uart_i = 0;
}

static void ESP8266_Clear_RxBuff2(struct Uart_Rx *uartx)
{
  memset(uartx->rx_buff, 0, sizeof(uartx->rx_buff));
  uartx->rx_i = 0;
}

uint8_t wifi_count;
static void ESP8266_UART_Dev_Init(struct Uart_Rx *uartx)			
{
	char temp[200];
	while(ESP8266_SendCmd("AT\r\n","OK", 100) == 1);
	ESP8266_SendCmd("AT+RST\r\n", "OK", 100);
	Delay_ms(500);
	if (uartx->mode == ESP8266_MODE_AP)
	{
		while(ESP8266_SendCmd("AT+CWMODE=2\r\n", "OK", 100) == 1);
		sprintf(temp, "AT+CWSAP=%s,5,3\r\n", Wifi_Info);
		while(ESP8266_SendCmd(temp, "OK", 100) == 1);		
		Delay_ms(500);

		while(ESP8266_SendCmd("AT+CIPMUX=1\r\n", "OK", 100) == 1);															
		Delay_ms(500);

		while(ESP8266_SendCmd("AT+CIPSERVER=1,8080\r\n", "OK", 100) == 1)
		
		Delay_ms(500);
	}
	else if (uartx->mode == ESP8266_MODE_STA)
	{
		
		while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK", 100) == 1);
		while(ESP8266_SendCmd("AT+CIPMUX=0\r\n", "OK", 100) == 1);
		
		sprintf(temp, "AT+CWJAP=%s\r\n", Wifi_Info);
		while(ESP8266_SendCmd(temp, "WIFI GOT IP", 100) == 1)
		{
			if (wifi_count == 100)
			{
				OLED_ShowChinese(1, 16, "热点连接失败");
				OLED_Update();
			}
			else 
				wifi_count++;
		};
		wifi_count = 0;
		Delay_ms(100);
		#if aliyun
		sprintf(temp, "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s&%s\",\"%s\",0,0,\"\"\r\n", DeviceName, ProductKey, Password);
		#else
		sprintf(temp, "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n", Username, Password);
		#endif
		while(ESP8266_SendCmd(temp, "OK", 100) == 1);
		sprintf(temp, "AT+MQTTCLIENTID=0,\"%s\"\r\n", ClientID);
		while(ESP8266_SendCmd(temp, "OK", 100) == 1);
		sprintf(temp, "AT+MQTTCONN=0,\"%s\",%s,1\r\n", MQTTHostURL, Port);
		while(ESP8266_SendCmd(temp, "OK", 100) == 1);
		#if aliyun
		sprintf(temp, "AT+MQTTSUB=0,\"/%s/%s/user/get\",1\r\n", ProductKey, DeviceName);
		while(ESP8266_SendCmd(temp, "OK", 100) == 1);
		#endif
		sprintf(temp, "AT+MQTTSUB=0,\"%s\",1\r\n", SubTopic);
		while(ESP8266_SendCmd(temp, "OK", 100) == 1);
	}
}

uint16_t rx_len;
static _Bool ESP8266_WaitRecive(struct Uart_Rx *uartx)
{
	if(uartx->Uart_i == 0) 					
		return REV_WAIT;
		
	if(uartx->Uart_i == rx_len)			
	{
		uartx->Uart_i = 0;						
		return REV_OK;								
	}
	rx_len = uartx->Uart_i;					
	return REV_WAIT;								
}


static void ESP8266_UART_Dev_Send(struct Uart_Rx *uartx, char *bufs, unsigned char len)
{
	HAL_UART_Transmit(&uartx->Huartx, (uint8_t *)bufs, len, 0xffff);
}


static uint8_t ESP8266_SendCmd(char *cmd, char *res, uint16_t time)
{	
	struct Uart_Dev * temp = SelectDefaultUart("ESP8266");
  temp->UART_Dev_Send(&temp->t_uart, (char *)cmd,strlen((const char *)cmd));
	
	while(time--)
	{
		if(ESP8266_WaitRecive(&temp->t_uart) == REV_OK)							      
		{
			if(strstr((const char *)temp->t_uart.Uart_Buff, res) != NULL)		
			{
				temp->Clear_RxBuff1(&temp->t_uart);
				return 0;
			}
		}
		Delay_ms(1);
	}
	return 1;

}


static void ESP8266_UART_Dev_Send_Data(char *data)				/* ESP8266-01S设备数据发送 */
{
	char cmd[100];
	uint16_t len = strlen(data);
	if (g_Uart_ESP8266.t_uart.mode == ESP8266_MODE_AP)
	{
		sprintf(cmd, "AT+CIPSEND=0,%d\r\n", len);	
		if(!ESP8266_SendCmd(cmd, ">", 100))				
		{
				g_Uart_ESP8266.UART_Dev_Send(&g_Uart_ESP8266.t_uart, data , len);                   
		}
	}
	else if (g_Uart_ESP8266.t_uart.mode == ESP8266_MODE_STA)
	{
		sprintf(cmd,"AT+MQTTPUBRAW=0,\"%s\",%u,0,0\r\n",PropPort,len);
		while(ESP8266_SendCmd(cmd, ">", 100));
		Delay_ms(100);
		ESP8266_SendCmd(data, "MQTTPUB:OK", 1);
		Delay_ms(100);
	}
}

static __attribute__((unused)) int16_t Received_Data_Processing(const char *str)	/* 对ESP8266-01S设备返回的数据解析 */				
{
	char temp[10];
	uint8_t temp_i = 0;
	char *p1 = NULL;
	if (g_Uart_ESP8266.t_uart.mode == ESP8266_MODE_AP)
	{
		if ((p1 = strstr((char *)g_Uart_ESP8266.t_uart.Uart_Buff, str)) != NULL)													
		{
			p1 += strlen(str) + 1;
			while(*p1 != ',')
			{
				temp[temp_i++] = *p1;
				p1++;
			}
			return atoi(temp);
		}
	}
	else if (g_Uart_ESP8266.t_uart.mode == ESP8266_MODE_STA)
	{
		if ((p1 = strstr((char *)g_Uart_ESP8266.t_uart.rx_buff, str)) != NULL)												
		{
			p1 += strlen(str) + 3;
			while(*p1 != ',')
			{
				temp[temp_i++] = *p1;
				p1++;
			}
			return atoi(temp);
		}
	}
	return RXERROR;
}


static __attribute__((unused)) char* Received_Data_Processing_Str(const char *str)	/* 对ESP8266-01S设备返回的数据解析 */				
{
	static char temp[20] = {0};
	memset(temp, 0, sizeof(temp));
	uint8_t temp_i = 0;
	char *p1 = NULL;
		if ((p1 = strstr((char *)g_Uart_ESP8266.t_uart.rx_buff, str)) != NULL)												
		{
			p1 += strlen(str) + 3;
			while(*p1 != ',')
			{
				temp[temp_i++] = *p1;
				p1++;
			}
			return temp;
		}
	return " ";
}


#include "TIMER.h"
extern struct plate_time stall_stop_time[2];
extern uint8_t stall_[2];
extern uint16_t money;

uint32_t ESP8266_UART_Dev_Data_Dispose(uint8_t timeout)							/* 对解析后的数据进行处理 */
{
	int16_t value __attribute__((unused));
	char temp[50] = {0};
	if (g_Uart_ESP8266.t_uart.mode == ESP8266_MODE_AP)
	{
		if((ESP8266_WaitRecive(&g_Uart_ESP8266.t_uart) == REV_OK) && (strstr((char *)g_Uart_ESP8266.t_uart.Uart_Buff, "IPD") != NULL))										
		{
			OLED_Clear();
			
			g_Uart_ESP8266.Clear_RxBuff1(&g_Uart_ESP8266.t_uart);
			return REV_OK;
		}
	}
	else if (g_Uart_ESP8266.t_uart.mode == ESP8266_MODE_STA)
	{
		/* "stall_1":1,,"font":6d59,"number":A66666, */
		if (g_Uart_ESP8266.t_uart.rx_flag == 1)
		{
			OLED_Clear();
			value = Received_Data_Processing("money");
			if (value != RXERROR)
				money = value;
			
			value = Received_Data_Processing("stall_1");
			if (value != RXERROR)
			{
				if (value != (stall_[0] + 1))
				{
					g_Uart_ESP8266.Clear_RxBuff2(&g_Uart_ESP8266.t_uart);
					g_Uart_ESP8266.t_uart.rx_flag = 0;
					return REV_WAIT;
				}
				stall_[0] = value;
				if (stall_[0] != 1)
				{
					g_Uart_ESP8266.Clear_RxBuff2(&g_Uart_ESP8266.t_uart);
					g_Uart_ESP8266.t_uart.rx_flag = 0;
					return REV_WAIT;
				}
				

				if (stall_[0] == 1)
				{
					memset(stall_stop_time[0].yuyue_font_unicode, 0, sizeof(stall_stop_time[0].yuyue_font_unicode));
					memset(stall_stop_time[0].yuyue_number, 0, sizeof(stall_stop_time[0].yuyue_number));

					strcpy(temp, Received_Data_Processing_Str("font"));
					if (strcmp(temp , " ") != NULL)
					{
						strcpy(stall_stop_time[0].yuyue_font_unicode, temp);
						stall_stop_time[0].yuyue_font_unicode[strlen((char *)stall_stop_time[0].yuyue_font_unicode)] = '\0';
						stall_stop_time[0].yuyue_flag = PLATE_Y;
					}
					
					strcpy(temp, Received_Data_Processing_Str("number"));
					if (strcmp(temp , " ") != NULL)
					{
						strcpy(stall_stop_time[0].yuyue_number, temp);
						stall_stop_time[0].yuyue_number[strlen((char *)stall_stop_time[0].yuyue_number)] = '\0';
					}
				}
			}
			
			value = Received_Data_Processing("stall_2");
			if (value != RXERROR)
			{
				if (value != (stall_[1] + 1))
				{
					g_Uart_ESP8266.Clear_RxBuff2(&g_Uart_ESP8266.t_uart);
					g_Uart_ESP8266.t_uart.rx_flag = 0;
					return REV_WAIT;
				}
				stall_[1] = value;
				if (stall_[1] != 1)
				{
					g_Uart_ESP8266.Clear_RxBuff2(&g_Uart_ESP8266.t_uart);
					g_Uart_ESP8266.t_uart.rx_flag = 0;
					return REV_WAIT;
				}

				if (stall_[1] == 1)
				{
					memset(stall_stop_time[1].yuyue_font_unicode, 0, sizeof(stall_stop_time[1].yuyue_font_unicode));
					memset(stall_stop_time[1].yuyue_number, 0, sizeof(stall_stop_time[1].yuyue_number));

					strcpy(temp, Received_Data_Processing_Str("font"));
					if (strcmp(temp , " ") != NULL)
					{
						strcpy(stall_stop_time[1].yuyue_font_unicode, temp);
						stall_stop_time[1].yuyue_font_unicode[strlen((char *)stall_stop_time[1].yuyue_font_unicode)] = '\0';
						stall_stop_time[1].yuyue_flag = PLATE_Y;
					}
					
					strcpy(temp, Received_Data_Processing_Str("number"));
					if (strcmp(temp , " ") != NULL)
					{
						strcpy(stall_stop_time[1].yuyue_number, temp);
						stall_stop_time[1].yuyue_number[strlen((char *)stall_stop_time[1].yuyue_number)] = '\0';
					}
				}
			}
			g_Uart_ESP8266.Clear_RxBuff2(&g_Uart_ESP8266.t_uart);
			g_Uart_ESP8266.t_uart.rx_flag = 0;
			return REV_OK;
		}
	}
	return REV_WAIT;														
}




