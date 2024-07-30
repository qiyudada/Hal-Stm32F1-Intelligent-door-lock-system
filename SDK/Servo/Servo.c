#include "Servo.h"
#include "tim.h"
void Servo_Init(void)
{
	HAL_TIM_PWM_Start(&SERVO_TIMER, SERVO_CHANNEL);
}

void Servo_SetAngle(float Angle)
{
	__HAL_TIM_SET_COMPARE(&SERVO_TIMER, TIM_CHANNEL_2, Angle / 180 * 2000 + 500);
}

void Servo_Control(uint16_t angle)
{
	float temp;
	temp = (1.0 / 9.0) * angle + 5.0; // 占空比值 = 1/9 * 角度 + 5
	__HAL_TIM_SET_COMPARE(&SERVO_TIMER, SERVO_CHANNEL, (uint16_t)temp);
}

void Servo_TEST(void)
{
	int i = 1000;											// 循环时间间隔1s
	__HAL_TIM_SET_COMPARE(&SERVO_TIMER, SERVO_CHANNEL, 50); // 相当于一个周期内�?20ms）有0.5ms高脉
	Delay_ms(i);
	__HAL_TIM_SET_COMPARE(&SERVO_TIMER, SERVO_CHANNEL, 250); // 相当于一个周期内�?20ms）有2.5ms高脉
	Delay_ms(i);
}