#include "HCSR04.h"

Hcsr04InfoTypeDef Hcsr04Info;
Kalman Ultuarsonic_Kalman;
void Hcsr04Init(TIM_HandleTypeDef *htim, uint32_t Channel)
{
	/*--------[ Configure The HCSR04 IC Timer Channel ] */
	// MX_TIM2_Init();  // cubemx中配置
	Hcsr04Info.prescaler = htim->Init.Prescaler; //  72-1
	Hcsr04Info.period = htim->Init.Period;		 //  65535
	Hcsr04Info.instance = htim->Instance;		 //  TIM
	Hcsr04Info.ic_tim_ch = Channel;

	if (Hcsr04Info.ic_tim_ch == TIM_CHANNEL_1)
	{
		Hcsr04Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_1; //  TIM_CHANNEL_4
	}
	else if (Hcsr04Info.ic_tim_ch == TIM_CHANNEL_2)
	{
		Hcsr04Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_2; //  TIM_CHANNEL_4
	}
	else if (Hcsr04Info.ic_tim_ch == TIM_CHANNEL_3)
	{
		Hcsr04Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_3; //  TIM_CHANNEL_4
	}
	else if (Hcsr04Info.ic_tim_ch == TIM_CHANNEL_4)
	{
		Hcsr04Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_4; //  TIM_CHANNEL_4
	}
	else if (Hcsr04Info.ic_tim_ch == TIM_CHANNEL_4)
	{
		Hcsr04Info.active_channel = HAL_TIM_ACTIVE_CHANNEL_4; //  TIM_CHANNEL_4
	}
	/*--------[ Start The ICU Channel ]-------*/
	HAL_TIM_Base_Start_IT(htim);
	HAL_TIM_IC_Start_IT(htim, Channel);
}

/*******************************************************
 * @name: 	Kalman_Init
 * @param 	(float)LastP
 * @param 	(float)Optimized_X
 * @param 	(float)Kg
 * @param 	(float)Q
 * @param 	(float)R
 * @retval 	None
 *********************************************************/
void Kalman_Init(float LastP, float Optimized_X, float Kg, float Q, float R)
{
	Ultuarsonic_Kalman.LastP = LastP;
	Ultuarsonic_Kalman.Optimized_X = Optimized_X;
	Ultuarsonic_Kalman.Kg = Kg;
	Ultuarsonic_Kalman.Q = Q;
	Ultuarsonic_Kalman.R = R;
}

/*******************************************************
 * @name: 	KalmanFilter
 * @param 	(Kalman*)P	A pointer to Kalman struct
 * @param 	(float)Input	Input Value
 * @retval 	None
 *********************************************************/
float KalmanFilter(Kalman *P, float Input)
{
	// Predict covariance equation [Pk=A*Pk_1*At+Q]
	P->CurrentP = P->LastP + P->Q;
	// Kalman gain equation [Kg=Pk/(Pk+R)]
	P->Kg = P->CurrentP / (P->CurrentP + P->R);
	// Update Optimzed value equation [Xk=Xk_1+Kg*(Input-Xk_1)]
	P->Optimized_X = P->Optimized_X + P->Kg * (Input - P->Optimized_X);
	// Update covariance equation [Pk=(1-Kg)*Pk_1]
	P->LastP = (1 - P->Kg) * P->CurrentP;
	return P->Optimized_X;
}

/**
 * @description: HC-SR04 Trigger
 * @param 	void
 * @return 	void
 */
void Hcsr04Start(void)
{
	HCSR_Write(1);
	Delay_ms(10);
	HCSR_Write(0);
}

/**
 * @description: 定时器计数溢出中断处理函数
 * @param {*}    main.c中重定义void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
 * @return {*}
 */
void Hcsr04TimOverflowIsr(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == Hcsr04Info.instance) //  TIM2
	{
		Hcsr04Info.tim_overflow_counter++;
	}
}

/**
 * @description: 输入捕获计算高电平时间->距离
 * @param {*}    main.c中重定义void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
 * @return {*}
 */
void Hcsr04TimIcIsr(TIM_HandleTypeDef *htim)
{
	if ((htim->Instance == Hcsr04Info.instance) && (htim->Channel == Hcsr04Info.active_channel))
	{
		if (Hcsr04Info.edge_state == 0) //  捕获上升沿
		{
			// 得到上升沿开始时间T1，并更改输入捕获为下降沿
			Hcsr04Info.t1 = HAL_TIM_ReadCapturedValue(htim, Hcsr04Info.ic_tim_ch);
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, Hcsr04Info.ic_tim_ch, TIM_INPUTCHANNELPOLARITY_FALLING);
			Hcsr04Info.tim_overflow_counter = 0; //  定时器溢出计数器清零
			Hcsr04Info.edge_state = 1;			 //  上升沿、下降沿捕获标志位
		}
		else if (Hcsr04Info.edge_state == 1) //  捕获下降沿
		{
			// 捕获下降沿时间T2，并计算高电平时间
			Hcsr04Info.t2 = HAL_TIM_ReadCapturedValue(htim, Hcsr04Info.ic_tim_ch);
			Hcsr04Info.t2 += Hcsr04Info.tim_overflow_counter * Hcsr04Info.period; //  需要考虑定时器溢出中断
			Hcsr04Info.high_level_us = Hcsr04Info.t2 - Hcsr04Info.t1;			  //  高电平持续时间 = 下降沿时间点 - 上升沿时间点
			// 计算距离
			Hcsr04Info.distance = (Hcsr04Info.high_level_us / 1000000.0) * 340.0 / 2.0 * 100.0;
			// 重新开启上升沿捕获
			Hcsr04Info.edge_state = 0; //  一次采集完毕，清零
			__HAL_TIM_SET_CAPTUREPOLARITY(htim, Hcsr04Info.ic_tim_ch, TIM_INPUTCHANNELPOLARITY_RISING);
		}
	}
}

/**
 * @description: 读取距离
 * @param {*}
 * @return {*}
 */
float Hcsr04Read()
{
	// 测距结果限幅
	if (Hcsr04Info.distance >= 450)
	{
		Hcsr04Info.distance = 450;
	}
	return Hcsr04Info.distance;
}

/*
 * @description:  定时器中断回调函数
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	Hcsr04TimIcIsr(htim);
}
/*
 * @description:  定时器溢出中断回调函数
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	Hcsr04TimOverflowIsr(htim);
}

/************************************************
 * @description: HCSR04 Test
 * @param void
 * @retval None
 *************************************************/
void HCSR04_TEST(void)
{
	printf("distance:%.1f cm\r\n", Hcsr04Read());
	Hcsr04Start();
	Delay_ms(200); //  测距周期200ms
}

/************************************************
 * @description: HCSR04 kalman test
 * @param void
 * @retval None
 *************************************************/
void HCSR04_Kalman_TEST(void)
{
	//Kalman_Init(1,0,0,0,0.01); put it in Initation space
	float distance = Hcsr04Read();
	printf("distance:%.1f cm\r\n", distance);
	distance=KalmanFilter(&Ultuarsonic_Kalman, distance);
	printf("KalmanFilter:%.1f cm\r\n", distance);
	Hcsr04Start();
	Delay_ms(1000);
}