#ifndef __KEY_H 
#define __KEY_H
#include "stm32f1xx_hal.h"

struct keys{
	uint8_t key_state;
	uint8_t key_flag;
	uint8_t key_value;
	uint8_t key_long;
	uint32_t key_time;
	uint8_t key_double;
	uint32_t key_double_time;
	uint8_t key_num;
};
struct Tim_Dev* Key_Dev_Reg(char *dev, TIM_TypeDef *timx, TIM_HandleTypeDef htimx);
#endif  /* __KEY_H */


