#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f1xx_hal.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

#include "Delay.h"
#include "LED.h"
#include "Servo.h"
#include "main.h"
#include "lcd.h"

#define BlueTooth_UART huart3
#define USART3_BUFFER_MAX (100)
#define BlueTooth_RX_BUF USART3_RX_BUF
#define BlueTooth_RX_BUF_MAX USART3_BUFFER_MAX
#define BlueTooth_RX_STA USART3_RX_STA

typedef enum
{
    BlueTooth_Door_Open = 1,
    BlueTooth_Door_Close,
    BlueTooth_LED_Red_On,
    BlueTooth_LED_Red_Off,
    BlueTooth_LED_White_On,
    BlueTooth_LED_White_Off,
    BlueTooth_LED_Green_On,
    BlueTooth_LED_Green_Off,
    BlueTooth_RX_BUFF_STATE_ERR
} BlueTooth_RX_BUFF_STATE;

extern uint8_t BlueTooth_RX_BUF[USART3_BUFFER_MAX];
extern uint8_t BlueTooth_RX_STA;

void BlueTooth_Init(void);
void Blue_Buffer_Clear(void);
int BlueTooth_RX_BUF_Check(uint8_t *Blue_Rx_Buffer);
void HAL_UART3_IdleCpltCallback(UART_HandleTypeDef *huart);
void Bluetooth_Feedback(void);

#endif