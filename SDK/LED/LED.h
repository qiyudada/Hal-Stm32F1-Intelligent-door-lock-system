#ifndef __LED_H
#define __LED_H

#include "stm32f1xx_hal.h"
#include "gpio.h"

#define LED_PORT    GPIOC
#define LED_RED     GPIO_PIN_1
#define LED_WHTIE   GPIO_PIN_2
#define LED_GREEN   GPIO_PIN_3

#define LED_RED_OFF     HAL_GPIO_WritePin(LED_PORT, LED_RED, GPIO_PIN_SET);
#define LED_RED_ON      HAL_GPIO_WritePin(LED_PORT, LED_RED, GPIO_PIN_RESET);
#define LED_WHTIE_OFF   HAL_GPIO_WritePin(LED_PORT, LED_WHTIE, GPIO_PIN_SET);
#define LED_WHTIE_ON    HAL_GPIO_WritePin(LED_PORT, LED_WHTIE, GPIO_PIN_RESET);
#define LED_GREEN_OFF   HAL_GPIO_WritePin(LED_PORT, LED_GREEN, GPIO_PIN_SET);
#define LED_GREEN_ON    HAL_GPIO_WritePin(LED_PORT, LED_GREEN, GPIO_PIN_RESET);

void LED_Init(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED3_ON(void);
void LED3_OFF(void);
void LED_TEST(void);

#endif
