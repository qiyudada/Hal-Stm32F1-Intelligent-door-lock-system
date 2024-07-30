#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "spi.h"
#include "stm32f1xx_hal.h"
#include "Delay.h"

#define USE_HORIZONTAL 1  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏


#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160

#else
#define LCD_W 160
#define LCD_H 128
#endif

#define LCD_RST_PORT    GPIOC
#define LCD_DC_PORT     GPIOC
#define LCD_BLK_PORT    GPIOB
#define LCD_CS_PORT     GPIOA

#define LCD_RST_PIN     GPIO_PIN_4
#define LCD_DC_PIN      GPIO_PIN_5
#define LCD_BLK_PIN     GPIO_PIN_0
#define LCD_CS_PIN      GPIO_PIN_4

/*-----------------LCD端口定义----------------*/

#define LCD_RES_Clr()  HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_RESET)//RES
#define LCD_RES_Set()  HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_SET)

#define LCD_DC_Clr()   HAL_GPIO_WritePin(LCD_DC_PORT,LCD_DC_PIN,GPIO_PIN_RESET)//DC
#define LCD_DC_Set()   HAL_GPIO_WritePin(LCD_DC_PORT,LCD_DC_PIN,GPIO_PIN_SET)
 		     
#define LCD_CS_Clr()   HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_RESET)//CS
#define LCD_CS_Set()   HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_SET)

#define LCD_BLK_Clr()  HAL_GPIO_WritePin(LCD_BLK_PORT,LCD_BLK_PIN,GPIO_PIN_RESET)//BLK
#define LCD_BLK_Set()  HAL_GPIO_WritePin(LCD_BLK_PORT,LCD_BLK_PIN,GPIO_PIN_SET)

/*********************function declaration***************************/

void LCD_GPIO_Init(void);//初始化GPIO
void SPIv_WriteData(uint8_t Data);
void LCD_Writ_Bus(uint8_t data);//模拟SPI时序
void LCD_WR_DATA8(uint8_t data);//写入一个字节
void LCD_WR_DATA(uint16_t data);//写入两个字节
void LCD_WR_REG(uint8_t data);//写入一个指令
void LCD_Address_Set(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);//设置坐标函数
void LCD_Init(void);//LCD初始化


#endif




