#ifndef __AS608_H
#define __AS608_H

#include "stm32f1xx_hal.h"
#include "Delay.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

#include "lcd.h"
#include "Key.h"

#define CharBuffer1 0x01
#define CharBuffer2 0x02
#define AS608_UART huart2
#define USART2_MAX_RECV_LEN (200)
#define AS608_Correctness 0
#define AS608_Error 1
#define AS608_UART_MAX_RECV_LEN USART2_MAX_RECV_LEN
#define AS608_USART_RX_BUF USART2_RX_BUF
#define AS608_USART_RX_STA USART2_RX_STA
#define FP_Touch_Read HAL_GPIO_ReadPin(FP_Touch_GPIO_Port, FP_Touch_Pin)

extern uint8_t AS608_USART_RX_BUF[USART2_MAX_RECV_LEN];
extern volatile uint8_t USART2_RX_STA;

typedef struct
{
	uint16_t PS_max;  // 指纹最大容量
	uint8_t PS_level; // 安全等级
	uint32_t PS_addr;
	uint8_t PS_size; // 通讯数据包大小
	uint8_t PS_N;	 // 波特率基数N
} SysPara;

typedef struct
{
	uint16_t pageID;	// 指纹ID
	uint16_t mathscore; // 匹配得分
} SearchResult;

extern SysPara AS608Para; // 指纹模块AS608参数
extern uint16_t ValidN;	  // 模块内有效指纹个数

uint8_t AS608_Init(void);

uint8_t PS_GetImage(void); // 录入图像

uint8_t PS_GenChar(uint8_t BufferID); // 生成特征

uint8_t PS_Match(void); // 精确比对两枚指纹特征

uint8_t PS_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p); // 搜索指纹

uint8_t PS_RegModel(void); // 合并特征（生成模板）

uint8_t PS_StoreChar(uint8_t BufferID, uint16_t PageID); // 储存模板

uint8_t PS_DeletChar(uint16_t PageID, uint16_t N); // 删除模板

uint8_t PS_Empty(void); // 清空指纹库

uint8_t PS_WriteReg(uint8_t RegNum, uint8_t DATA); // 写系统寄存器

uint8_t PS_ReadSysPara(SysPara *p); // 读系统基本参数

uint8_t PS_SetAddr(uint32_t addr); // 设置模块地址

uint8_t PS_WriteNotepad(uint8_t NotePageNum, uint8_t *content); // 写记事本

uint8_t PS_ReadNotepad(uint8_t NotePageNum, uint8_t *note); // 读记事

uint8_t PS_HighSpeedSearch(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p); // 高速搜索

uint8_t PS_ValidTempleteNum(uint16_t *ValidN); // 读有效模板个数

uint8_t PS_HandShake(uint32_t *PS_Addr); // 与AS608模块握手

const char *EnsureMessage(uint8_t ensure); // 确认码错误信息解析

void ShowErrMessage(uint8_t ensure);

uint16_t Press_FR(void);

void Add_FR(void);

void Del_FR(void);

void Del_FR_Lib(void);

void FP_Feedback(void);

void HAL_UART2_IdleCpltCallback(UART_HandleTypeDef *huart);

#endif
