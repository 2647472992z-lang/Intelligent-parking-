#include "uart_manager.h"
#include "UART.h"
#include "string.h"
struct Uart_Dev *g_UartDevs 		= NULL;
struct Uart_Dev *g_UartDevsTail = NULL; 

void RegisterUart(struct Uart_Dev * PUartOpr)
{
    if (g_UartDevs == NULL) 
		{
        g_UartDevs = PUartOpr;
        g_UartDevsTail = PUartOpr;
    } 
		else 
		{
        g_UartDevsTail->ptNext = PUartOpr;
        g_UartDevsTail = PUartOpr;
    }
    PUartOpr->ptNext = NULL; 
}

struct Uart_Dev * SelectDefaultUart(char *name)
{
	struct Uart_Dev * pTmp = g_UartDevs;
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


struct Uart_Dev * UART_Get_Devs(void)
{
	return g_UartDevs;
}

