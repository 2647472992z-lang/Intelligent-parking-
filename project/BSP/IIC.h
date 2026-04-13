#ifndef	__I2C_H
#define __I2C_H
#include "stm32f1xx_hal.h"
#include "iic_manager.h"
uint8_t I2C_ReadReg(struct t_IIC_GPIO *iic, uint8_t Reg);
void I2C_WriteReg(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t Data);
void I2C_Write_Regs(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t *DataBuff, uint8_t Lenght);
void I2C_Read_Regs(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t *DataBuff, uint8_t Lenght);
#endif


