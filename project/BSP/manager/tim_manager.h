#ifndef	__TIM_MANAGER_H
#define __TIM_MANAGER_H
#include "stm32f1xx_hal.h"
#ifdef HAL_TIM_MODULE_ENABLED
#include "tim.h"
#endif

typedef struct t_Tim
{
	TIM_TypeDef					*TIMx;
	TIM_HandleTypeDef  	htimx;
	uint32_t						TIM_CHANNELx;

	GPIO_TypeDef     	  *TRIG_GPIOx;
	uint32_t						TRIG_GPIO_PINx;
	GPIO_TypeDef     	  *ECHO_GPIOx;
	uint32_t						ECHO_GPIO_PINx;

	
	GPIO_TypeDef     	  *B1_GPIOx;
	uint32_t						B1_GPIO_PINx;
	GPIO_TypeDef     	  *B2_GPIOx;
	uint32_t						B2_GPIO_PINx;
	GPIO_TypeDef     	  *B3_GPIOx;
	uint32_t						B3_GPIO_PINx;
	GPIO_TypeDef     	  *B4_GPIOx;
	uint32_t						B4_GPIO_PINx;

}t_Tim, *pt_Tim;


typedef struct Tim_Dev
{
	char name[10];
	struct t_Tim Tim;
	void (*Tim_Init)(struct t_Tim *Tim);
	void (*Tim_ISR_Opr)(struct t_Tim *Tim);
	void (*Tim_Opr)(struct t_Tim *Tim);
	int (*Tim_Get_Value)(struct t_Tim *Tim);
	void (*Tim_Dev_Start)(struct t_Tim *Tim, uint16_t value);
	void (*Tim_Dev_Stop)(struct t_Tim *Tim);
	struct Tim_Dev *ptNext;
}Tim_Dev, *pTim_Dev;

void RegisterTim(struct Tim_Dev * PTimOpr);
struct Tim_Dev * SelectDefaultTim(char *name);
struct Tim_Dev * Tim_Get_Devs(void);
#endif	/* __TIM_MANAGER_H */




