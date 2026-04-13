#include "EM2000X.h"
#include "UART.h"
#include "OLED.h"
#include "delay.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
extern UART_HandleTypeDef huart2;
extern uint8_t mode_flag;
extern uint8_t uart2_Buff[RX_MAX_SIZEOF];
extern uint8_t uart2_i;
extern uint8_t rxdata2;
extern uint8_t is_out_in;

#define EM2000X_RXBUFF      uart2_Buff
#define EM2000X_I           uart2_i

struct PROVICE_info provine[10] = {
0,"山西省",
1,"黑龙江省",
2,"吉林省",
3,"辽宁省",
4,"河北省",
5,"甘肃省",
6,"青海省",
7,"陕西省",
8,"河南省",
9,"山东省",

};

void EM2000X_Init(void)
{
  HAL_UART_Receive_IT(&huart2, &rxdata2, 1);
}


int8_t EM2000X_WaitReceive(void)
{
  static uint16_t rx_len = 0;
  if (EM2000X_I == 0)
    return RXERROR;
  Delay_ms(100);
  if (rx_len == EM2000X_I)
  {
     EM2000X_I = 0;
     return RXOK;
  }
  rx_len = EM2000X_I;  
  return RXWAIT;
}

void EMX2000X_ClearBuff(void)
{
  memset(EM2000X_RXBUFF, 0, sizeof(EM2000X_RXBUFF));
  EM2000X_I = 0;
}

char temp_buff[RX_MAX_SIZEOF];
char* EM2000X_ReceiveData(uint16_t timeout)
{
	while(timeout--)
	{
		if (EM2000X_WaitReceive() == RXOK)
		{
			strcpy(temp_buff, (char *)EM2000X_RXBUFF);
			temp_buff[RX_MAX_SIZEOF - 1] = '\0';
			EMX2000X_ClearBuff();
			return temp_buff;
		}
		Delay_ms(1);
	}
  return NULL;
}


