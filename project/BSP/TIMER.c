#include "tim_manager.h"
#include "TIMER.h"
extern struct plate_time stall_stop_time[2];
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	struct Tim_Dev *temp = Tim_Get_Devs();
	while(temp)
	{
		if (htim->Instance == temp->Tim.TIMx)
		{
			temp->Tim_ISR_Opr(&temp->Tim);
		}
		temp = temp->ptNext;
	}
	
	if (htim->Instance == TIM2)
  {
		uint8_t i = 0;
		for (i = 0; i < 2; i++)
		{
			if (stall_stop_time[i].time_flag == 1)
			{
				if (stall_stop_time[i].sec == 59)
				{
					if (stall_stop_time[i].minute > 59)
					{
						if (stall_stop_time[i].hour > 23)
						{
							stall_stop_time[i].hour = 0;
							stall_stop_time[i].minute = 0;
							stall_stop_time[i].sec = 0;
							stall_stop_time[i].day++;
						}
						else
							stall_stop_time[i].hour++;
					}
					else
					{
						stall_stop_time[i].sec = 0;
						stall_stop_time[i].minute++;
					}
				}
				else
					stall_stop_time[i].sec++;
			}
		}
	}
}

