#include "KEY.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "OLED.h"
#include "tim_manager.h"
static void Key_Tim_ISR_Opr(struct t_Tim *Tim);
static void Key_Tim_Init(struct t_Tim *Tim);
static void Key_Tim_Opr(struct t_Tim *Tim);

static struct Tim_Dev g_Key_Dev = {
	.name 				= "Key",
	.Tim_Init			= Key_Tim_Init,
	.Tim_Opr			= Key_Tim_Opr,
	.Tim_ISR_Opr	= Key_Tim_ISR_Opr,
};

struct Tim_Dev* Key_Dev_Reg(char *dev, TIM_TypeDef *timx, TIM_HandleTypeDef htimx)
{
	g_Key_Dev.Tim.TIMx								= timx;
	g_Key_Dev.Tim.htimx								= htimx;
	strcpy(g_Key_Dev.name, dev);
	RegisterTim(&g_Key_Dev);
	return SelectDefaultTim(dev);
}
static void Key_Tim_Init(struct t_Tim *Tim)
{
	HAL_TIM_Base_Start_IT(&Tim->htimx);
}

/* 需修改按键数量,超过一秒为长按 */
#define KEY_MAX_NUMBER    3
struct keys key[KEY_MAX_NUMBER];
static void Key_Tim_ISR_Opr(struct t_Tim *Tim)
{
		uint8_t i = 0;  
		key[0].key_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);	  
		key[1].key_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13);
		key[2].key_state = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);
		for ( i = 0; i < KEY_MAX_NUMBER; i++)
		{
			switch(key[i].key_value)
			{
				case 0:
					if (key[i].key_state == 0)
						key[i].key_value = 1;
					if (key[i].key_num == 1)
						key[i].key_double_time++;
					break;
				case 1:
					if (key[i].key_state == 0)
						key[i].key_value = 2;
					break;
				case 2:
					if (key[i].key_state == 1)
					{
						if (key[i].key_time >= (72000000u / ((TIM1->PSC + 1) * (TIM1->ARR + 1))))
								key[i].key_long = 1;
						else
								key[i].key_flag 	= 1;
						key[i].key_time 	= 0;
						key[i].key_value 	= 0;
					}
					else
						key[i].key_time++;
					break;
			}
    }
}
extern struct Tim_Dev		*SERVO_[2];

extern uint8_t mode;
extern uint8_t stall_[2];
static void Key_Tim_Opr(struct t_Tim *Tim)
{
  if (key[0].key_flag == 1)
  {
		if (mode == 2)
			mode = 0;
		else
			mode++;
		OLED_Clear();
    key[0].key_flag = 0;
  }
	else if (key[1].key_flag == 1)
  {
		if (mode == 1)
			SERVO_[0]->Tim_Dev_Start(&SERVO_[0]->Tim, 0);
		else if (mode == 2)
			SERVO_[1]->Tim_Dev_Start(&SERVO_[1]->Tim, 0);
    key[1].key_flag = 0;
  }
	else if (key[2].key_flag == 1)
  {
		if (mode == 1)
		{SERVO_[0]->Tim_Dev_Start(&SERVO_[0]->Tim, 90);
		}
		else if (mode == 2)
		{
			SERVO_[1]->Tim_Dev_Start(&SERVO_[1]->Tim, 90);
		}
    key[2].key_flag = 0;
  }
}





