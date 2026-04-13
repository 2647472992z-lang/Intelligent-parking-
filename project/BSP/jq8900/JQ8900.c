#include "JQ8900.h"
#include "delay.h"
GPIO_TypeDef *JQ8900_GPIOx;
uint32_t JQ8900_Pin;

GPIO_TypeDef *JQ8900_Bus_GPIOx;
uint32_t JQ8900_Bus_Pin;
static void JQ8900_W_GPIO(uint8_t x)
{
	HAL_GPIO_WritePin(JQ8900_GPIOx, JQ8900_Pin, (GPIO_PinState)x);
}
void JQ8900_Send_Cmd(uint8_t cmd)
{
	uint8_t i = 0;
	JQ8900_W_GPIO(1);
	Delay_ms(1);
	JQ8900_W_GPIO(0);
	Delay_ms(4);
	for (i = 0; i < 8; i++)
	{
		JQ8900_W_GPIO(1);
		if(cmd & 0x01)
		{
			Delay_us(600);
			JQ8900_W_GPIO(0);
			Delay_us(200);
		}
		else
		{
			Delay_us(200);
			JQ8900_W_GPIO(0);
			Delay_us(600);
		}
		cmd >>= 1;
	}
	JQ8900_W_GPIO(1);
}


void JQ8900_Set_Volume(uint8_t volume)
{
	uint8_t v1 = volume / 10, v2 = volume % 10;
	JQ8900_Send_Cmd(0x0a);
	JQ8900_Send_Cmd(v1);
	JQ8900_Send_Cmd(v2);
	JQ8900_Send_Cmd(0x0C);
}
uint8_t JQ8900_Read_Bus(void)
{
	return HAL_GPIO_ReadPin(JQ8900_Bus_GPIOx, JQ8900_Bus_Pin);
}
void JQ8900_Play(uint8_t index, uint8_t volume)
{
	if (JQ8900_Read_Bus() == GPIO_PIN_RESET)
	{
		uint8_t v1 = index / 10, v2 = index % 10;
		JQ8900_Set_Volume(volume);
		JQ8900_Send_Cmd(0x0a);
		if (v1 > 0)
		{
			JQ8900_Send_Cmd(v1);
			JQ8900_Send_Cmd(v2);	
		}
		else
		{
			JQ8900_Send_Cmd(index);
		}
		JQ8900_Send_Cmd(0x0b);
//		Delay_ms(1000);
	}
}


void JQ8900_Init(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, GPIO_TypeDef *Bus_GPIOx, uint32_t Bus_GPIO_Pin)
{
	JQ8900_GPIOx 			= GPIOx;
	JQ8900_Pin				= GPIO_Pin;
	JQ8900_Bus_GPIOx 	= Bus_GPIOx;
	JQ8900_Bus_Pin   	= Bus_GPIO_Pin;
}

