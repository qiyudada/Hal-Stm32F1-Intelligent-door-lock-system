#include "HC05.h"

uint8_t USART3_RX_BUF[USART3_BUFFER_MAX] = {0}; //":MSG\r\n"
uint8_t UART3_Index = 0;
uint8_t USART3_RX_STA = 0;

void BlueTooth_Init(void)
{
    HAL_UART_Receive_DMA(&BlueTooth_UART, BlueTooth_RX_BUF, sizeof(BlueTooth_RX_BUF)); // 开启中断接收数据
    __HAL_UART_ENABLE_IT(&BlueTooth_UART, UART_IT_IDLE);
}

void Blue_Buffer_Clear(void)
{
    memset(BlueTooth_RX_BUF, 0, sizeof(BlueTooth_RX_BUF));
}

void HAL_UART_IdleCpltCallback(UART_HandleTypeDef *huart)
{
    if (__HAL_UART_GET_FLAG(&BlueTooth_UART, UART_FLAG_IDLE) != RESET)
    {
        printf("uart3 idle\r\n");
        BlueTooth_UART.RxXferCount = sizeof(BlueTooth_RX_BUF);
        BlueTooth_UART.pRxBuffPtr = BlueTooth_RX_BUF;
        __HAL_UART_CLEAR_IDLEFLAG(&BlueTooth_UART);
        HAL_UART_Transmit(&BlueTooth_UART, BlueTooth_RX_BUF, sizeof(BlueTooth_RX_BUF), 1000); // Test
        Blue_Buffer_Clear();
    }
}
