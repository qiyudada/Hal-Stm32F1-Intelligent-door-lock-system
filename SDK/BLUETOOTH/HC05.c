#include "HC05.h"

uint8_t USART3_RX_BUF[USART3_BUFFER_MAX] = {0};
uint8_t USART3_RX_STA = 0;
static int BlueTooth_State;

void BlueTooth_Init(void)
{
    HAL_UART_Receive_DMA(&BlueTooth_UART, BlueTooth_RX_BUF, sizeof(BlueTooth_RX_BUF)); // 开启中断接收数据
    __HAL_UART_ENABLE_IT(&BlueTooth_UART, UART_IT_IDLE);
}

int BlueTooth_RX_BUF_Check(uint8_t *Blue_Rx_Buffer)
{
    if (strcmp((char*)Blue_Rx_Buffer, "dooropen") == 0)
    {
        return BlueTooth_Door_Open;
    }
    else if (strcmp((char*)Blue_Rx_Buffer, "doorclose") == 0)
    {
        return BlueTooth_Door_Close;
    }
    else if (strcmp((char*)Blue_Rx_Buffer, "ledredon") == 0)
    {
        return BlueTooth_LED_Red_On;
    }
    else if (strcmp((char*)Blue_Rx_Buffer, "ledredoff") == 0)
    {
        return BlueTooth_LED_Red_Off;
    }
    else if (strcmp((char*)Blue_Rx_Buffer, "ledwhiteon") == 0)
    {
        return BlueTooth_LED_White_On;
    }
    else if (strcmp((char*)Blue_Rx_Buffer, "ledwhiteoff") == 0)
    {
        return BlueTooth_LED_White_Off;
    }
    else if (strcmp((char*)Blue_Rx_Buffer, "ledgreenon") == 0)
    {
        return BlueTooth_LED_Green_On;
    }
    else if (strcmp((char*)Blue_Rx_Buffer, "ledgreenoff") == 0)
    {
        return BlueTooth_LED_Green_Off;
    }
    else
    {
        return BlueTooth_RX_BUFF_STATE_ERR;
    }
}

void Blue_Buffer_Clear(void)
{
    memset(BlueTooth_RX_BUF, 0, sizeof(BlueTooth_RX_BUF));
}

void HAL_UART3_IdleCpltCallback(UART_HandleTypeDef *huart)
{
    if (__HAL_UART_GET_FLAG(&BlueTooth_UART, UART_FLAG_IDLE) != RESET)
    {
        printf("Uart3 idle\r\n");
        __HAL_UART_CLEAR_IDLEFLAG(&BlueTooth_UART);
        BlueTooth_UART.RxXferCount = sizeof(BlueTooth_RX_BUF);
        BlueTooth_UART.pRxBuffPtr = BlueTooth_RX_BUF;
        BlueTooth_RX_STA = 1;
        BlueTooth_State = BlueTooth_RX_BUF_Check(BlueTooth_RX_BUF);
        HAL_UART_Transmit(&BlueTooth_UART, BlueTooth_RX_BUF, sizeof(BlueTooth_RX_BUF), 1000); // Test for pc
        Blue_Buffer_Clear();
    }
}

void Bluetooth_Feedback(void)
{
    if (BlueTooth_State == BlueTooth_Door_Open)
    {
        BlueTooth_State = 0;
        BlueTooth_RX_STA = 0;
        Door_Open();
        LCD_Fill(-10, -10, 240, 240, WHITE);
    }
    else if (BlueTooth_State == BlueTooth_Door_Close)
    {
        BlueTooth_State = 0;
        BlueTooth_RX_STA = 0;
        Door_Close();
        LCD_Fill(-10, -10, 240, 240, WHITE);
    }
    else if (BlueTooth_State == BlueTooth_LED_Red_On)
    {
        BlueTooth_State = 0;
        BlueTooth_RX_STA = 0;
        LED1_ON();
        LCD_Fill(-10, -10, 240, 240, WHITE);
    }
    else if (BlueTooth_State == BlueTooth_LED_Red_Off)
    {
        BlueTooth_State = 0;
        BlueTooth_RX_STA = 0;
        LED1_OFF();
        LCD_Fill(-10, -10, 240, 240, WHITE);
    }
    else if (BlueTooth_State == BlueTooth_LED_White_On)
    {
        BlueTooth_State = 0;
        BlueTooth_RX_STA = 0;
        LED2_ON();
        LCD_Fill(-10, -10, 240, 240, WHITE);
    }
    else if (BlueTooth_State == BlueTooth_LED_White_Off)
    {
        BlueTooth_State = 0;
        BlueTooth_RX_STA = 0;
        LED2_OFF();
        LCD_Fill(-10, -10, 240, 240, WHITE);
    }
    else if (BlueTooth_State == BlueTooth_LED_Green_On)
    {
        BlueTooth_RX_STA = 0;
        LED3_ON();
        LCD_Fill(-10, -10, 240, 240, WHITE);
    }
    else if (BlueTooth_State == BlueTooth_LED_Green_Off)
    {
        BlueTooth_State = 0;
        BlueTooth_RX_STA = 0;
        LED3_OFF();
        LCD_Fill(-10, -10, 240, 240, WHITE);
    }
    else
    {
        BlueTooth_State = 0;
        BlueTooth_RX_STA = 0;
        Door_Error();
        LCD_Fill(-10, -10, 240, 240, WHITE);
    }
}