#include "IIC.h"
#include "delay.h"
GPIO_TypeDef *I2C_SCL_GPIOx;
uint32_t 			I2C_SCL_Pin;
GPIO_TypeDef *I2C_SDA_GPIOx;
uint32_t 			I2C_SDA_Pin;
/* 写SCL操作函数*/
static void I2C_W_SCL(uint8_t value)
{
	HAL_GPIO_WritePin(I2C_SCL_GPIOx, I2C_SCL_Pin, (GPIO_PinState)value);
	Delay_us(1);
}
/* 写SDA操作函数 */
static void I2C_W_SDA(uint8_t value)
{
	HAL_GPIO_WritePin(I2C_SDA_GPIOx, I2C_SDA_Pin, (GPIO_PinState)value);
	Delay_us(1);
}
/* 读SDA操作函数 */
static uint8_t I2C_R_SDA(void)
{
	Delay_us(1);
	return HAL_GPIO_ReadPin(I2C_SDA_GPIOx, I2C_SDA_Pin);
}
/* I2C起始函数 */
static void I2C_Start(void)
{
	I2C_W_SDA(1);
	I2C_W_SCL(1);
	I2C_W_SDA(0);
	I2C_W_SCL(0);
}
/* I2C停止函数 */
static void I2C_Stop(void)
{
	I2C_W_SDA(0);
	I2C_W_SCL(1);
	I2C_W_SDA(1);
}
/* 发送一个字节 */
static void I2C_SendByte(uint8_t SendByte)
{
	for (int i = 0; i < 8; i++)
	{
		I2C_W_SDA(SendByte & (0x80 >> i));
		I2C_W_SCL(1);
		I2C_W_SCL(0);
	}
}

/* 接收一个字节 */
static uint8_t I2C_RecviceByte(void)
{
	uint8_t RecviceByte = 0x00;
	I2C_W_SDA(1);
	for (int i = 0; i < 8; i++)
	{
		I2C_W_SCL(1);
		if (I2C_R_SDA() == 1)
		{
			RecviceByte |= (0x80 >> i);
		}
		I2C_W_SCL(0);
	}
	return RecviceByte;
}

/* 发送ACK */
static void I2C_SendACK(uint8_t Ack)
{
	I2C_W_SDA(Ack);
	I2C_W_SCL(1);
	I2C_W_SCL(0);
}
/* 等待ACK */
static uint8_t I2C_WaitAck(void)
{
	uint8_t RecviceAck = 0;
	I2C_W_SDA(1);
	I2C_W_SCL(1);
	RecviceAck = I2C_R_SDA();
	I2C_W_SCL(0);
	return RecviceAck;
}


/* 读取一个寄存器的值 */
uint8_t I2C_ReadReg(struct t_IIC_GPIO *iic, uint8_t Reg)
{
	
	I2C_SCL_GPIOx = iic->SCL_GPIOx;
	I2C_SCL_Pin		= iic->SCL_GPIO_PINx;
	I2C_SDA_GPIOx = iic->SDA_GPIOx;
	I2C_SDA_Pin		= iic->SDA_GPIO_PINx;
	uint8_t RecviceData;
	I2C_Start();
	I2C_SendByte(iic->Addr);
	I2C_WaitAck();
	I2C_SendByte(Reg);
	I2C_WaitAck();
	I2C_Start();
	I2C_SendByte(iic->Addr | 0x01);
	I2C_WaitAck();
	RecviceData = I2C_RecviceByte();
	I2C_SendACK(1);
	I2C_Stop();
	return RecviceData;
}
/* 写入一个寄存器 */
void I2C_WriteReg(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t Data)
{
	I2C_SCL_GPIOx = iic->SCL_GPIOx;
	I2C_SCL_Pin		= iic->SCL_GPIO_PINx;
	I2C_SDA_GPIOx = iic->SDA_GPIOx;
	I2C_SDA_Pin		= iic->SDA_GPIO_PINx;
	I2C_Start();
	I2C_SendByte(iic->Addr);
	I2C_WaitAck();
	I2C_SendByte(Reg);
	I2C_WaitAck();
	I2C_SendByte(Data);
	I2C_WaitAck();
	I2C_Stop();
}

/* 连续写多个寄存器 */
void I2C_Write_Regs(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t *DataBuff, uint8_t Lenght)
{
	uint8_t i = 0;
	I2C_Start();
	I2C_SendByte(iic->Addr);
	I2C_WaitAck();
	I2C_SendByte(Reg);
	I2C_WaitAck();
	for (i = 0; i < Lenght; i++)
	{
		I2C_SendByte(DataBuff[i]);
		I2C_WaitAck();
	}
	I2C_Stop();
}
/* 连续读多个寄存器 */
void I2C_Read_Regs(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t *DataBuff, uint8_t Lenght)
{
	uint8_t i = 0;
	I2C_Start();
	I2C_SendByte(iic->Addr);
	I2C_WaitAck();
	I2C_SendByte(Reg);
	I2C_WaitAck();
	I2C_Start();
	I2C_SendByte(iic->Addr | 0x01);
	I2C_WaitAck();
	for (i = 0; i < Lenght; i++)
	{
		DataBuff[i] = I2C_RecviceByte();
		if (i == Lenght - 1)
			I2C_SendACK(1);
		else
			I2C_SendACK(0);
	}
	
	I2C_Stop();
}










