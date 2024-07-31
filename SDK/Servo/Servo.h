#ifndef __SERVO_H
#define __SERVO_H

#include "stm32f1xx_hal.h"
#include "Delay.h"

#define SERVO_TIMER htim2
#define SERVO_CHANNEL TIM_CHANNEL_1


void Servo_Init(void);
void Servo_SetAngle(float Angle);
void Servo_Control(uint16_t angle);
void Servo_TEST(void);
void Servo_CloseDoor(void);
void Servo_OpenDoor(void);

#endif
