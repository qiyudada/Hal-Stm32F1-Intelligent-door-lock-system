/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Delay.h"
#include "LED.h"
#include "KEY.h"
#include "LCD.h"
#include "AS608.h"
#include "Servo.h"
#include "RC522.h"
#include "HCSR04.h"
#include "HC05.h"
#include "StmFlash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t Task;
uint8_t Key_Change_num;
uint8_t Keynum;
float Servo_Angle;
uint8_t Card_Type1[2];
uint8_t Card_ID[4];
uint8_t Card_KEY[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; //{0x11,0x11,0x11,0x11,0x11,0x11};
uint8_t Card_Data[16];
uint8_t status;
uint16_t cardType;
uint8_t readUid[5];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Door_Open(void)
{
  LED1_OFF();
  LED2_OFF();
  LED3_ON();
  Delay_ms(500);
  LCD_Fill(-10, -10, 240, 240, WHITE);
  LCD_ShowString(20, 64, "Door Open", GREEN, WHITE, LCD_8x16, 0);
  Servo_OpenDoor();
  Delay_ms(5000);
}
void Door_Error(void)
{
  LED1_ON();
  LED2_OFF();
  LED3_OFF();
  Delay_ms(500);
  LCD_Fill(-10, -10, 240, 240, WHITE);
  LCD_ShowString(14, 64, "Confirm Error", RED, WHITE, LCD_8x16, 0);
  Delay_ms(1000);
}

void Door_Close(void)
{
  LED1_OFF();
  LED2_ON();
  LED3_OFF();
  Delay_ms(500);
  // LCD_Fill(-10, -10, 240, 240, WHITE);
  LCD_ShowString(20, 64, "Door Close", BLACK, WHITE, LCD_8x16, 0);
  Servo_CloseDoor();
  Delay_ms(20);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  /*********LCD Initation***************************/
  LCD_Init();
  LCD_Fill(-10, -10, 240, 240, WHITE);
  /***********Servo Initation***********************/
  Servo_Init();
  /***********RFID Initation***********************/
  PCD_Init_RST();
  printf("RC522 Initation Completed\n");
  /***********Fingerprint Initation***********************/
  AS608_Init();
  while (PS_HandShake(&AS608Addr)) // shake hand with AS608 module
  {
    printf("Cannot connect with AS608!\r\n");
    Delay_ms(500);
    printf("Try to connect again....\r\n");
    Delay_ms(500);
  }
  printf("AS608 Connection successfully\r\n");
  /***********Ultrasonic Initation***********************/
  Hcsr04Init(&htim5, HCSR04_CHANNEL);
  Hcsr04Start();
  printf("RC522 Initation Completed\r\n");
  /***********BlueTooth Initation***********************/
  BlueTooth_Init();
  printf("BlueTooth Initation Completed\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if ((Key_Change_num = Key_GetNum()) != 0)
    {
      Task = 1;
    }
    else if (FP_Touch_Read != RESET)
    {
      Task = 2;
    }
    switch (Task)
    {
    case 1:
      KEY_Feedback();
      Task = 0;
      // HCSR04_Detect();
      break;
    case 2:
      FP_Feedback();
      Task = 0;
      //HCSR04_Detect();
      break;
    default:
      Door_Close();
      break;
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
