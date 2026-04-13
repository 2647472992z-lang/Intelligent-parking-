#ifndef	__TIMER_H
#define __TIMER_H
#include "stm32f1xx_hal.h"
#include "tim_manager.h"

#define   PLATE_K		0
#define		PLATE_Y		1
#define 	PLATE_Z		2


struct plate_time
{
	uint8_t day;
	uint8_t time_flag;
	uint8_t hour;
	uint8_t minute;
	uint8_t sec;
	char font[16];
	char number[7];
	char font_unicode[5];   // ´æ´¢Unicode±àÂë
	char yuyue_font_unicode[5];
	char yuyue_number[7];
	uint8_t yuyue_flag;
};


#endif	/* __TIMER_H */





