#include "servo.h"
#include "string.h"
#include "stdlib.h"
static void Servo_Tim_Dev_Start(struct t_Tim *Tim, uint16_t value);
struct Tim_Dev* Servo_Dev_Reg(char *dev, TIM_TypeDef *timx, TIM_HandleTypeDef htimx, uint32_t TIM_CHx)
{
	Tim_Dev* new_servo = (Tim_Dev*)malloc(sizeof(Tim_Dev));
	if (new_servo == NULL)
		return NULL;
	memset(new_servo->name, 0, sizeof(new_servo->name));
	strcpy(new_servo->name, dev);

	new_servo->Tim.TIMx								= timx;
	new_servo->Tim.htimx							= htimx;
	new_servo->Tim.TIM_CHANNELx				= TIM_CHx;
	new_servo->Tim_Dev_Start					= Servo_Tim_Dev_Start;
	RegisterTim(new_servo);
	return new_servo;
}

static void Servo_SetAnagl(struct t_Tim *Tim, float Angal)
{
	__HAL_TIM_SET_COMPARE(&Tim->htimx, Tim->TIM_CHANNELx, Angal / 180.0 * 2000 + 500);
}


static void Servo_Tim_Dev_Start(struct t_Tim *Tim, uint16_t value)
{
  HAL_TIM_PWM_Start(&Tim->htimx, Tim->TIM_CHANNELx);
  Servo_SetAnagl(Tim, value);
}

//void Servo_Switch(void)
//{
//	static uint8_t servo_time;
//  if(switch_flag == 1)
//  {
//    Servo_Start(90);
//    if (servo_time < 20)
//    {
//      servo_time++;
//      return ;
//    }
//    servo_time = 0;
//    switch_flag  = 0;
//    Servo_Start(0);
//  }
//}

