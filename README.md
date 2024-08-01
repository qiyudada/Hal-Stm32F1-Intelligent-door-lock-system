# Hal-Stm32F1-Intelligent-door-lock-system
 ## A Intelligent door lock system which using Stm32F103VET6 based on Hal bank 

### 1.Work show

![46a0bad7f95915147ae3c839b3f1e874](C:\Users\Lenovo\AppData\Roaming\Tencent\TIM\Temp\46a0bad7f95915147ae3c839b3f1e874.jpg)

### 2.Some bugs I meet

#### 1. AS608 Data packet don’t update and be covered by new data

```
HAL_UART_Receive_IT(&AS608_UART, AS608_USART_RX_BUF, sizeof(AS608_USART_RX_BUF));
__HAL_UART_ENABLE_IT(&AS608_UART, UART_IT_IDLE); //Only enable it can enter the interrupt

//Reason(No update the pointer)
void HAL_UART2_IdleCpltCallback(UART_HandleTypeDef *huart)
{
	if (__HAL_UART_GET_FLAG(&AS608_UART, UART_FLAG_IDLE) != RESET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&AS608_UART);
		AS608_UART.pRxBuffPtr = AS608_USART_RX_BUF;//importance
		AS608_UART.RxXferCount = sizeof(AS608_USART_RX_BUF);//importance
		USART2_RX_STA = 1;
		HAL_UART_Receive_IT(&AS608_UART, AS608_USART_RX_BUF, sizeof(AS608_USART_RX_BUF));
	}
}

//idle interrupt will collect all data when its uart doesn't work and this condition also occur in no model  insert the uart,detect the voltage always low and enter irqH continously.

```

#### 2.Servo doesn’t work

+ forget its work voltage is 4.2~6V

+ its clock period limited at 20ms

#### 3.RC522 doesn’t work

use stm32cubemx spi  created by default configuration,exceed the frequency that rc522 can accept



