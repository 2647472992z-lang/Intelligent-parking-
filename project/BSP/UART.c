#include "UART.h"
#include "uart_manager.h"
extern UART_HandleTypeDef huart2;
uint8_t uart2_Buff[RX_MAX_SIZEOF];
uint16_t uart2_i;
uint8_t rxdata2;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	struct Uart_Dev* uart = UART_Get_Devs();
	while(uart)
	{
		if (huart->Instance == uart->t_uart.USARTx)
		{
			uart->UART_Dev_Rx_Data(&uart->t_uart);
		}
		uart = uart->ptNext;
	}
	if (huart->Instance == USART2)
  {
    uart2_Buff[uart2_i++] = rxdata2;
    HAL_UART_Receive_IT(&huart2, &rxdata2, 1);
    if (uart2_i >= RX_MAX_SIZEOF)
    {
      uart2_i = 0;
    }
    }
	
}




