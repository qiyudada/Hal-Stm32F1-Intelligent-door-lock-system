#ifndef __KEY_H
#define __KEY_H

#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "main.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "Servo.h"
#include "AS608.h"
#include "Delay.h"
#include "LED.h"
#include "lcd.h"
#include "StmFlash.h"

extern int Initation_PS[4];

#define KEY1_PORT GPIOA
#define KEY24_PORT GPIOC
#define KEY_FUNCTION_PORT GPIOA

#define KEY1_READ HAL_GPIO_ReadPin(KEY1_PORT, GPIO_PIN_8)
#define KEY2_READ HAL_GPIO_ReadPin(KEY24_PORT, GPIO_PIN_7)
#define KEY3_READ HAL_GPIO_ReadPin(KEY24_PORT, GPIO_PIN_8)
#define KEY4_READ HAL_GPIO_ReadPin(KEY24_PORT, GPIO_PIN_9)
#define KEY_MOVE_READ HAL_GPIO_ReadPin(KEY_FUNCTION_PORT, GPIO_PIN_11)
#define KEY_Confirm_READ HAL_GPIO_ReadPin(KEY_FUNCTION_PORT, GPIO_PIN_12)

#define Key_Timeout 5000000
#define Key_Delay 2000000

#define KEY_NoPRES 0
#define KEY1_PRES 1     // KEY1
#define KEY2_PRES 2     // KEY2
#define KEY3_PRES 3     // KEY3
#define KEY4_PRES 4     // KEY4
#define KEY_MOVE_PRES 5 // KEY_MOVE
#define KEY_Confirm 6   // KEY_Confirm
#define PassWord_True 7
#define PassWord_False 8
#define PassWord_FingerAdd 9

extern uint8_t Keynum;

void Key_Init(void);
uint8_t Key_GetNum(void);
void KEY_TEST(void);
void Write_PassWord(void);
uint8_t Password_Match(void);
uint8_t KEY_Judge_Unlock(void);
void KEY_Feedback(void);
void KEY_Change(void);

#endif
