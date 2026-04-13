#include "iic_manager.h"
#include "IIC.h"

struct IIC_Dev *g_IICDevs 		= NULL;
struct IIC_Dev *g_IICDevsTail = NULL; 

void RegisterIIC(struct IIC_Dev * PIICOpr)
{
    if (g_IICDevs == NULL) 
		{
        g_IICDevs = PIICOpr;
        g_IICDevsTail = PIICOpr;
    } 
		else 
		{
        g_IICDevsTail->ptNext = PIICOpr;
        g_IICDevsTail = PIICOpr;
    }
    PIICOpr->ptNext = NULL; 
}

struct IIC_Dev * SelectDefaultIIC(char *name)
{
	struct IIC_Dev * pTmp = g_IICDevs;
	while (pTmp)
	{
		if (strcmp(name, pTmp->name) == 0)
		{
			return pTmp;
		}
		pTmp = pTmp->ptNext;
	}
 
	return NULL;
}


struct IIC_Dev * IIC_Get_Devs(void)
{
	return g_IICDevs;
}

uint8_t IIC_Dev_ReadReg(struct t_IIC_GPIO *iic, uint8_t Reg)
{
	uint8_t data;
	if (iic->mode == IIC_SOFTWARE)
		data = I2C_ReadReg(iic, Reg);
	else if (iic->mode == IIC_HARDWARE)
	{
		#ifdef HAL_I2C_MODULE_ENABLED
			HAL_I2C_Mem_Read(&iic->hi2cx, iic->Addr, Reg, 1, &data, 1, 1000);
		#endif
	}
	return data;
}

void IIC_Dev_WriteReg(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t Data)
{
	if (iic->mode == IIC_SOFTWARE)
		I2C_WriteReg(iic, Reg, Data);
	else if (iic->mode == IIC_HARDWARE)
	{
		#ifdef HAL_I2C_MODULE_ENABLED
			HAL_I2C_Mem_Write(&iic->hi2cx, iic->Addr, Reg, 1, &Data, 1 , 1000);
		#endif
	}
	 
}

void IIC_Dev_Write_Regs(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t *DataBuff, uint8_t Lenght)
{
	if (iic->mode == IIC_SOFTWARE)
		I2C_Write_Regs(iic, Reg, DataBuff, Lenght);
	else if (iic->mode == IIC_HARDWARE)
	{
		#ifdef HAL_I2C_MODULE_ENABLED
			HAL_I2C_Mem_Write(&iic->hi2cx, iic->Addr, Reg, 1, DataBuff, Lenght , 1000);
		#endif
	}

}
void IIC_Dev_Read_Regs(struct t_IIC_GPIO *iic, uint8_t Reg, uint8_t *DataBuff, uint8_t Lenght)
{
	if (iic->mode == IIC_SOFTWARE)
		I2C_Read_Regs(iic, Reg, DataBuff, Lenght);
	else if (iic->mode == IIC_HARDWARE)
	{
		#ifdef HAL_I2C_MODULE_ENABLED
			HAL_I2C_Mem_Read(&iic->hi2cx, iic->Addr, Reg, 1, DataBuff, Lenght, 1000);
		#endif
	}
}


