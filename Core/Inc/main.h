/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void Door_Open(void);
void Door_Close(void);
void Door_Error(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOC
#define SERVO_Pin GPIO_PIN_0
#define SERVO_GPIO_Port GPIOA
#define ECHO_Pin GPIO_PIN_1
#define ECHO_GPIO_Port GPIOA
#define SPI_NSS_Pin GPIO_PIN_4
#define SPI_NSS_GPIO_Port GPIOA
#define LCD_RES_Pin GPIO_PIN_4
#define LCD_RES_GPIO_Port GPIOC
#define LCD_DC_Pin GPIO_PIN_5
#define LCD_DC_GPIO_Port GPIOC
#define LCD_BLK_Pin GPIO_PIN_0
#define LCD_BLK_GPIO_Port GPIOB
#define TRIG_Pin GPIO_PIN_2
#define TRIG_GPIO_Port GPIOB
#define FP_Touch_Pin GPIO_PIN_15
#define FP_Touch_GPIO_Port GPIOE
#define SPI2_NSS_Pin GPIO_PIN_12
#define SPI2_NSS_GPIO_Port GPIOB
#define RCC522_IRQ_Pin GPIO_PIN_10
#define RCC522_IRQ_GPIO_Port GPIOD
#define RCC522_RST_Pin GPIO_PIN_11
#define RCC522_RST_GPIO_Port GPIOD
#define KEY2_Pin GPIO_PIN_7
#define KEY2_GPIO_Port GPIOC
#define KEY3_Pin GPIO_PIN_8
#define KEY3_GPIO_Port GPIOC
#define KEY4_Pin GPIO_PIN_9
#define KEY4_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_8
#define KEY1_GPIO_Port GPIOA
#define KEY_Move_Pin GPIO_PIN_11
#define KEY_Move_GPIO_Port GPIOA
#define KEY_Del_Pin GPIO_PIN_12
#define KEY_Del_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
extern uint32_t AS608Addr;
extern uint8_t Key_Change_num;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
