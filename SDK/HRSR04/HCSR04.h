#ifndef __HCSR04_H
#define __HCSR04_H

#include "stm32f1xx_hal.h"
#include "tim.h"
#include "Delay.h"

#define HCSR04_TIM htim5
#define HCSR04_CHANNEL TIM_CHANNEL_2

#define Trig_Port GPIOB
#define Trig_Pin GPIO_PIN_2
#define Echo_Port GPIOA
#define Echo_Pin GPIO_Pin_1

#define HCSR_Write(N) HAL_GPIO_WritePin(Trig_Port, TRIG_Pin, N == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);

typedef struct
{
    uint8_t edge_state;
    uint16_t tim_overflow_counter;
    uint32_t prescaler;
    uint32_t period;
    uint32_t t1;            //	上升沿时间
    uint32_t t2;            //	下降沿时间
    uint32_t high_level_us; //	高电平持续时间
    TIM_TypeDef *instance;
    float distance;
    uint32_t ic_tim_ch;
    HAL_TIM_ActiveChannel active_channel;
} Hcsr04InfoTypeDef;

typedef struct
{
    float LastP;       // last covariance
    float CurrentP;    // current covariance
    float Optimized_X; // optimized value
    float Kg;          // kalman gain coefficient
    float Q;           // process noise
    float R;           // measurement noise
} Kalman;

extern Hcsr04InfoTypeDef Hcsr04Info;

void Hcsr04Init(TIM_HandleTypeDef *htim, uint32_t Channel);
void Hcsr04Start(void);
void Hcsr04TimOverflowIsr(TIM_HandleTypeDef *htim);
void Hcsr04TimIcIsr(TIM_HandleTypeDef *htim);
float Hcsr04Read(void);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HCSR04_TEST(void);
float KalmanFilter(Kalman *P, float Input);
void Kalman_Init(float LastP, float Optimized_X, float Kg, float Q, float R);
void HCSR04_Kalman_TEST(void);
void HCSR04_Detect(void);

#endif
