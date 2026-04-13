#include "Run.h"
#include "delay.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "ESP8266.h"
#include "OLED.h"
#include "platenumber.h"
#include "KEY.h"
#include "TIMER.h"
#include "SERVO.h"
#include "JQ8900.h"
#include "ML307R.h"
#include "EM2000X.h"

struct plate_time stall_stop_time[2];

char str[200];
struct Uart_Dev		*ESP8266;
struct IIC_Dev		*OLED;
struct Uart_Dev		*OPENMV;
struct Tim_Dev		*KEY;
struct Tim_Dev		*SERVO_[2];
struct tML307R    	*ML307R;

struct plate_info *plate_number;
char* traking_number;

uint8_t mode, stall_[2];
uint16_t money = 1;
uint8_t press_[2];
uint32_t stop_money;
uint8_t old_state;
uint8_t i, s;
uint8_t res;
// 在全局变量部分，添加以下变量
uint8_t last_stall0_flag = 1;
uint8_t last_stall1_flag = 1;
uint8_t stall_flags_changed = 0;

static uint32_t run_loop_counter = 0;


static void ESP8266_Send(uint8_t flag, char *font, char *number, uint8_t stall, uint8_t hour, uint8_t minute, uint8_t sec, uint8_t money)
{
	static uint8_t esp8266_time = 0;
	if (flag == 1)
	{ 
		esp8266_time = 0;
		sprintf(str,"{\"font\":\"%s\",\"number\":\"%s\",\"stall\":%d,\"hour\":%d,\"minute\":%d,\"sec\":%d,\"money\":%d}", font, number,stall + 1, hour,minute,sec, money);
		ESP8266->UART_Dev_Send_Data(str);
	}
	else 
	{
		if (esp8266_time < 100)
		{
			esp8266_time++;
			return ;
		}
		esp8266_time = 0;
		sprintf(str,"{\"stall_1\":%d,\"stall_2\":%d}"\
		,(stall_stop_time[0].yuyue_flag != 0), (stall_stop_time[1].yuyue_flag != 0));
		ESP8266->UART_Dev_Send_Data(str);
	}
}


void Pressures_GetValue(uint8_t *press_1,uint8_t *press_2)
{
		*press_1 = !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
		*press_2 = !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
	
}
extern char temp_buff[RX_MAX_SIZEOF];
/* 主要逻辑 */
static __attribute__((unused)) void Ctrl(void)
{
	res = OPENMV->UART_Dev_Data_Dispose(0);
	if (res == PLATE_RX_OK){s = 1;}
	
		for (i = 0; i < 2; i++)
	{
		if (stall_[i] == 3)
		{
			mode = 10;
			stall_stop_time[i].time_flag = 0;
			uint8_t temp_hour = 0, temp_minute = 0, temp_sec = 0;
			temp_hour = stall_stop_time[i].hour;
			temp_minute = stall_stop_time[i].minute;
			temp_sec = stall_stop_time[i].sec;
			if (temp_sec > 0)
						temp_minute += 1;
			stop_money = (temp_hour * 60 + temp_minute) * money;
			OLED_Clear();
			OLED_ShowChinese(1, 16, stall_stop_time[i].font);
			OLED_ShowString(20, 16, stall_stop_time[i].number, 8);
			OLED_ShowChinese(1, 32, "时长：");
			sprintf(str, "%02d:%02d:%02d", stall_stop_time[i].hour, stall_stop_time[i].minute, stall_stop_time[i].sec);
			OLED_ShowString(40, 32, str, 8);
			OLED_ShowChinese(1, 48, "应缴费：");
			sprintf(str, "%d ", stop_money);
			OLED_ShowString(60, 48, str, 8);
			OLED_ShowChinese(60 + strlen(str) + 10, 48, "元");
			traking_number = EM2000X_ReceiveData(100);// 接收跟踪号数据
			if (traking_number != NULL)
            {
							stall_[i] = 4;
			}
			
		}
		else if (stall_[i] == 4)
		{
//				ESP8266_Send(1, stall_stop_time[i].yuyue_font_unicode, stall_stop_time[i].yuyue_number, i, stall_stop_time[i].hour, stall_stop_time[i].minute, stall_stop_time[i].sec, stop_money);
				OLED_Clear();
				OLED_ShowChinese(1, 16, "等待车辆驶出");
				OLED_Update();
				JQ8900_Play(5, 25);																				/* 等待车辆驶出 */
				SERVO_[i]->Tim_Dev_Start(&SERVO_[i]->Tim, 90);
				while(1)
				{
					if (press_[i]  == 1)
						break;
					Pressures_GetValue(&press_[0], &press_[1]);
					Delay_ms(10);
				}
				while(1)
				{
					if (press_[i] == 0)
						break;
					Pressures_GetValue(&press_[0], &press_[1]);
					Delay_ms(10);
				}
				Delay_ms(1000);
				SERVO_[i]->Tim_Dev_Start(&SERVO_[i]->Tim, 0);
				stall_stop_time[i].time_flag = 0;
				OLED_Clear();
				stall_[i] = 0;
				mode = 0;
				stop_money = 0;
				stall_stop_time[i].hour 	= 0;
				stall_stop_time[i].minute = 0;
				stall_stop_time[i].sec 		= 0;
				strcpy(stall_stop_time[i].font, "");
				strcpy(stall_stop_time[i].number, "");
				stall_stop_time[i].yuyue_flag = PLATE_Y;
		}
		if ((press_[i] == 1) && (res == PLATE_RX_OK))
		{
				plate_number = (struct plate_info *)OPENMV->UART_Get_StrData(NULL);
				switch(stall_stop_time[i].yuyue_flag)
				{
					case PLATE_K:																									/* 请先预约该停车位 */
						JQ8900_Play(1, 25);
						return ;
					case PLATE_Y:
//						if ((strcmp(plate_number->font_unicode, stall_stop_time[i].yuyue_font_unicode) == 0) && 
//								(strcmp(plate_number->number, stall_stop_time[i].yuyue_number) == 0))
//						{
					if ((plate_number->font_unicode != 0) && (plate_number->number != 0))		
						{
							strcpy(stall_stop_time[i].font_unicode, plate_number->font_unicode);  // 添加这行
							strcpy(stall_stop_time[i].font, plate_number->font);
							strcpy(stall_stop_time[i].number, plate_number->number);
							SERVO_[i]->Tim_Dev_Start(&SERVO_[i]->Tim, 90);
							OLED_Clear();
							OLED_ShowChinese(1, 16, "正在进入停车位");
							
							while(JQ8900_Read_Bus() == 1);
							JQ8900_Play(2, 25);																				/* 正在进入停车位 */
							OLED_Update();
							while(1)
							{
								if (press_[i] == 0)
									break;
								Pressures_GetValue(&press_[0], &press_[1]);
								Delay_ms(10);
							}
							Delay_ms(1000);
							SERVO_[i]->Tim_Dev_Start(&SERVO_[i]->Tim, 0);
							OLED_Clear();
							stall_stop_time[i].time_flag = 1;
							stall_stop_time[i].yuyue_flag = PLATE_Z;
							stall_[i] = 2;
						}
						break;
					case PLATE_Z:																									/* 车位已被占用 */
						if ((strstr(stall_stop_time[i].font_unicode, plate_number->font_unicode) != NULL) &&
														(strstr(stall_stop_time[i].number, plate_number->number) != NULL) ) {	
															stall_[i] = 3;
														}
						else{
									JQ8900_Play(4, 25);	
						}
						return ;
				}
		}
	}
}
uint32_t count;
extern uint8_t uart2_Buff[RX_MAX_SIZEOF];
/* 页面显示 */
static void Page(void)
{
	if (mode == 0)
	{
		OLED_ShowChinese(1, 1, "车位一：");
		OLED_ShowChinese(60, 1, (stall_stop_time[0].yuyue_flag == PLATE_Y) ? "未占用" : "已占用");
		OLED_ShowChinese(1, 16, "车位二：");
		OLED_ShowChinese(60, 16, (stall_stop_time[1].yuyue_flag == PLATE_Y) ? "未占用" : "已占用");
		OLED_ShowNum(1, 32, 1, 1, 8);
		OLED_ShowChinese(10, 32, "分钟");
		sprintf(str, "%d  ", money);
		OLED_ShowString(50, 32, str, 8);
		OLED_ShowChinese(50 + strlen(str) * 5, 32, "元");
		
//		OLED_ShowString(0, 48, traking_number, 8);
//		
//		Pressures_GetValue(&press_[0], &press_[1]);
//		sprintf(str, "%d %d  ", press_[0], press_[1]);
//		OLED_ShowString(88, 32, str, 8);
		
	}
	else if (mode == 1)
	{
		OLED_ShowChinese(1, 1, "车位一：");
		OLED_ShowChinese(60, 1, (stall_stop_time[0].yuyue_flag == PLATE_Y) ? "未占用" : "已占用");
		OLED_ShowChinese(1, 16, stall_stop_time[0].font);
		OLED_ShowString(20, 16, stall_stop_time[0].number, 8);
		sprintf(str, "%02d:%02d:%02d", stall_stop_time[0].hour, stall_stop_time[0].minute, stall_stop_time[0].sec);
		OLED_ShowString(1, 32, str, 8);
	}
	else if (mode == 2)
	{
		OLED_ShowChinese(1, 1, "车位二：");
		OLED_ShowChinese(60, 1, (stall_stop_time[1].yuyue_flag == PLATE_Y) ? "未占用" : "已占用");
		OLED_ShowChinese(1, 16, stall_stop_time[1].font);
		OLED_ShowString(20, 16, stall_stop_time[1].number, 8);
		sprintf(str, "%02d:%02d:%02d", stall_stop_time[1].hour, stall_stop_time[1].minute, stall_stop_time[1].sec);
		OLED_ShowString(1, 32, str, 8);
	}
	else if (mode == 3)
	{
		OLED_ShowChinese(1, 16, "等待车辆驶出");
	}
	else if (mode == 4)
	{
		OLED_ShowChinese(1, 16, "正在缴费");
	}
}

/* 设备初始化 */
void Init(void)
{
	OLED = OLED_IIC_SoftwareReg("OLED", GPIOB, GPIO_PIN_9, GPIOB, GPIO_PIN_8);
	OLED->IIC_Init(&OLED->IIC_GPIO);
//	
//	ESP8266 = ESP8266_Dev_Reg("ESP8266", ESP8266_MODE_STA, USART1);
//	ESP8266->UART_Dev_Init(&ESP8266->t_uart);
	
	OPENMV = Plate_Dev_Reg("OPENMV", USART3);
	OPENMV->UART_Dev_Init(&OPENMV->t_uart);
	
	KEY = Key_Dev_Reg("KEY", TIM1, htim1);
	KEY->Tim_Init(&KEY->Tim);
	
	SERVO_[0] = Servo_Dev_Reg("SERVO1", TIM4, htim4, TIM_CHANNEL_1);
	SERVO_[1] = Servo_Dev_Reg("SERVO2", TIM4, htim4, TIM_CHANNEL_2);
	
	SERVO_[0]->Tim_Dev_Start(&SERVO_[0]->Tim, 0);
	SERVO_[1]->Tim_Dev_Start(&SERVO_[1]->Tim, 0);
	
	JQ8900_Init(GPIOB, GPIO_PIN_1, GPIOB, GPIO_PIN_0);
	
	EM2000X_Init();
	
	OLED_Clear();
	
//	JQ8900_Play(3, 25);	
		stall_stop_time[0].yuyue_flag = PLATE_Y;
		stall_stop_time[1].yuyue_flag = PLATE_Y;
		stall_[0] = 1;
		stall_[1] = 1;
	HAL_TIM_Base_Start_IT(&htim2);
}


void Run(void)
{
	Pressures_GetValue(&press_[0], &press_[1]);

    // 3. 按键处理
    KEY->Tim_Opr(NULL);
    
    // 4. 主要控制逻辑
    Ctrl();
traking_number = EM2000X_ReceiveData(100);// 接收跟踪号数据
    // 5. 页面显示
    Page();
    OLED_Update();
    
}