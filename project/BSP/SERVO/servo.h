#ifndef __SERIVO_H
#define __SERIVO_H
#include "stm32f1xx_hal.h"
#include "tim_manager.h"
struct Tim_Dev* Servo_Dev_Reg(char *dev, TIM_TypeDef *timx, TIM_HandleTypeDef htimx, uint32_t TIM_CHx);
#endif


