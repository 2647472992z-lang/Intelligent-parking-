#ifndef __EM2000X_H
#define __EM2000X_H
#include "stm32f1xx_hal.h"

#define RXWAIT      0
#define RXOK        1
#define RXERROR     -1
struct PROVICE_info
{
	uint8_t i;
	char pro[10];
};


typedef struct Code
{
  char number[100];
	uint8_t source_addr;
	uint8_t dest_addr;
	char weight[100];
	char CSC[20];
  struct Code *pNext;
}TCode, *PCode;

void EM2000X_Init(void);
char* EM2000X_ReceiveData(uint16_t timeout);

#endif  /* __EM2000X_H */


