#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H

#include "stm32f1xx_hal.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include "Delay.h"

#define BlueTooth_UART huart3
#define USART3_BUFFER_MAX (100)
#define BlueTooth_RX_BUF USART3_RX_BUF
#define BlueTooth_RX_BUF_MAX USART3_BUFFER_MAX
#define BlueTooth_RX_STA USART3_RX_STA

extern uint8_t BlueTooth_RX_BUF[USART3_BUFFER_MAX];
extern uint8_t UART3_Index;
extern uint8_t BlueTooth_RX_STA;

void HAL_UART_IdleCpltCallback(UART_HandleTypeDef *huart);
void BlueTooth_Init(void);
void Blue_Buffer_Clear(void);

#endif