#ifndef __LCD_H
#define __LCD_H

#include "stm32f1xx_hal.h"
#include "lcd_init.h"

#define LCD_6x12 12
#define LCD_8x16 16
#define LCD_12x24 24
#define LCD_16x32 32

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color);
void LCD_DrawPoint(u16 x, u16 y, u16 color);
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fontcolor, u16 backgroundcolor, u8 sizey, u8 mode);
void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fontcolor, u16 backgroundcolor, u8 sizey);
void LCD_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fontcolor, u16 backgroundcolor, u8 sizey);
void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[]);
void LCD_ShowString(u16 x, u16 y, const char *p, u16 fontcolor, u16 backgroundcolor, u8 sizey, u8 mode);

void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fontcolor, u16 backgroundcolor, u8 sizey, u8 mode); // 显示汉字串
void LCD_ShowChinese12x12(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);                // 显示单个12x12汉字
void LCD_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);                // 显示单个16x16汉字
void LCD_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);                // 显示单个24x24汉字
void LCD_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);                // 显示单个32x32汉字

void LCD_Test(void);

// Paintbrush colors
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40
#define BRRED 0XFC07
#define GRAY 0X8430
#define DARKBLUE 0X01CF
#define LIGHTBLUE 0X7D7C
#define GRAYBLUE 0X5458
#define LIGHTGREEN 0X841F
#define LGRAY 0XC618
#define LGRAYBLUE 0XA651
#define LBBLUE 0X2B12

#endif