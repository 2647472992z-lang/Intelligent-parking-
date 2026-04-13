#ifndef __JQ8900_H
#define __JQ8900_H
#include "stm32f1xx_hal.h"
void JQ8900_Init(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, GPIO_TypeDef *Bus_GPIOx, uint32_t Bus_GPIO_Pin);
void JQ8900_Play(uint8_t index, uint8_t volume);
uint8_t JQ8900_Read_Bus(void);
#endif

