#ifndef	__IIC_MANAGER_H
#define __IIC_MANAGER_H
#include "stm32f1xx_hal.h"
#include "string.h"
#define IIC_SOFTWARE		0
#define IIC_HARDWARE		1

#define IIC_HARDWARE_1	1
#define IIC_HARDWARE_2	2
typedef struct t_IIC_GPIO
{
	GPIO_TypeDef 			*SDA_GPIOx;
	uint32_t 					SDA_GPIO_PINx;
	GPIO_TypeDef 			*SCL_GPIOx;
	uint32_t 					SCL_GPIO_PINx;
	GPIO_TypeDef 			*INT_GPIOx;
	uint32_t 					INT_GPIO_PINx;
	uint8_t						Addr;
#ifdef HAL_I2C_MODULE_ENABLED
	I2C_HandleTypeDef hi2cx;
#endif
	uint8_t 					mode;
}t_IIC_GPIO, *Pt_IIC_GPIO;

typedef struct IIC_Dev
{
	char name[10];
	void (*IIC_Init)(struct t_IIC_GPIO *iic);
	uint8_t (*IIC_Dev_ID)(struct t_IIC_GPIO *iic);
	int16_t * (*IIC_Read_Data)(struct t_IIC_GPIO *iic);
	float * (*IIC_Read_FloatData)(struct t_IIC_GPIO *iic);
	struct t_IIC_GPIO IIC_GPIO;
	struct IIC_Dev *ptNext;
}IIC_Dev, *PIIC_Dev;

void RegisterIIC(struct IIC_Dev * PIICOpr);
struct IIC_Dev * IIC_Get_Devs(void);
struct IIC_Dev * SelectDefaultIIC(char *name);
uint8_t IIC_Dev_ReadReg(struct t_IIC_GPIO *iic, uint8_t Reg);
void IIC_Dev_WriteReg(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t Data);
void IIC_Dev_Write_Regs(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t *DataBuff, uint8_t Lenght);
void IIC_Dev_Read_Regs(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t *DataBuff, uint8_t Lenght);
#endif	/* __IIC_MANAGER_H */




