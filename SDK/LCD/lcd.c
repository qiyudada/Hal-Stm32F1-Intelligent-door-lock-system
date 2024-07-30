#include "lcd.h"
#include "lcdfont.h"
#include "pic.h"

/******************************************************************************
	Function：	Fill color with demanded area
	Parameter： xsta,ysta    Start coordinate
				xend,yend    End coordinate
				color        Filled color
	Return：  	None
******************************************************************************/
void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{
	u16 num;
	num = (xend - xsta) * (yend - ysta);			 // the Maxium number of data that can be sent
	LCD_Address_Set(xsta, ysta, xend - 1, yend - 1); // Set up the cursor position scope
	LCD_CS_Clr();
	LCD_RES_Set();
	for (u16 i = 0; i < num; i++)
	{
		SPIv_WriteData(color >> 8);
		SPIv_WriteData(color);
	}
	LCD_CS_Set();
}

/******************************************************************************
	Function：	Draw a point
	Parameter： x,y      Coordinate
				color    Point color
	Return：  	None
******************************************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
	LCD_Address_Set(x, y, x, y); // set up the cursor position
	LCD_WR_DATA(color);
}
/******************************************************************************
	Function：	Draw a line
	Parameter： x1,y1     start Coordinate
				x2,y2     end Coordinate
				color    Point color
	Return：  	None
******************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
	u16 t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; // calculate the x distance
	delta_y = y2 - y1; // y distance
	uRow = x1;		   // start point
	uCol = y1;

	if (delta_x > 0)
	{
		incx = 1; // set up the step direction
	}
	else if (delta_x == 0)
	{
		incx = 0; // set normal line
	}
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}
	if (delta_y > 0)
	{
		incy = 1;
	}
	else if (delta_y == 0)
	{
		incy = 0; // horizontal line
	}
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}
	if (delta_x > delta_y)
	{
		distance = delta_x;
	} // choose the basic increment coordinate axis
	else
	{
		distance = delta_y;
	}
	for (t = 0; t < distance + 1; t++)
	{
		LCD_DrawPoint(uRow, uCol, color); // draw point
		xerr += delta_x;
		yerr += delta_y;
		if (xerr > distance)
		{
			xerr -= distance;
			uRow += incx;
		}
		if (yerr > distance)
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}

/******************************************************************************
	Function：	Display a character
	Parameter： x,y      Coordinate
				num      Display character
				fontcolor  Font color
				backgroundcolor  Background color
				sizey    Font size
				mode     Display mode(0:non-overlay mode;1:overlay mode)
	Return：  	None
******************************************************************************/
void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fontcolor, u16 backgroundcolor, u8 sizey, u8 mode)
{
	u8 temp, sizex, t, m = 0;
	u16 i, TypefaceNum; // a character size in bytes
	u16 x0 = x;
	sizex = sizey / 2;
	TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
	num = num - ' ';									 //  get the offset value
	LCD_Address_Set(x, y, x + sizex - 1, y + sizey - 1); //  set up the cursor position
	for (i = 0; i < TypefaceNum; i++)
	{
		if (sizey == 12)
			temp = ascii_1206[num][i]; // call 6x12 font
		else if (sizey == 16)
			temp = ascii_1608[num][i]; // call 8x16 font
		else if (sizey == 24)
			temp = ascii_2412[num][i]; // call 12x24 font
		else if (sizey == 32)
			temp = ascii_3216[num][i]; // call 16x32 font
		else
			return;
		for (t = 0; t < 8; t++)
		{
			if (!mode) // non-overlay mode
			{
				if (temp & (0x01 << t))
					LCD_WR_DATA(fontcolor);
				else
					LCD_WR_DATA(backgroundcolor);
				m++;
				if (m % sizex == 0)
				{
					m = 0;
					break;
				}
			}
			else // overlay mode
			{
				if (temp & (0x01 << t))
					LCD_DrawPoint(x, y, fontcolor); // draw a point
				x++;
				if ((x - x0) == sizex)
				{
					x = x0;
					y++;
					break;
				}
			}
		}
	}
}
/******************************************************************************
	Function：	mypow(m,n)
	Parameter： m value
				n power
	Return：  	result
******************************************************************************/
u32 mypow(u8 m, u8 n)
{
	u32 result = 1;
	while (n--)
		result *= m;
	return result;
}
/******************************************************************************
	Function：	Display a decimal number
	Parameter： x,y      Coordinate
				num      Display decimal number
				len      Number of digits
				fontcolor  Font color
				backgroundcolor  Background color
				sizey    Font size
	Return：  	None
******************************************************************************/
void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fontcolor, u16 backgroundcolor, u8 sizey)
{
	u8 t, temp;
	u8 enshow = 0;
	u8 sizex = sizey / 2;
	for (t = 0; t < len; t++)
	{
		temp = (num / mypow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				LCD_ShowChar(x + t * sizex, y, ' ', fontcolor, backgroundcolor, sizey, 0);
				continue;
			}
			else
				enshow = 1;
		}
		LCD_ShowChar(x + t * sizex, y, temp + 48, fontcolor, backgroundcolor, sizey, 0);
	}
}
/******************************************************************************
	Function：	show float number(two bits after the decimal point )
	Parameter： x,y      Coordinate
				num      Display float number
				len      Number of digits
				fontcolor  Font color
				backgroundcolor  Background color
				sizey    Font size
	Return：  	None
******************************************************************************/
void LCD_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fontcolor, u16 backgroundcolor, u8 sizey)
{
	u8 t, temp, sizex;
	u16 num1;
	sizex = sizey / 2;
	num1 = num * 100;
	for (t = 0; t < len; t++)
	{
		temp = (num1 / mypow(10, len - t - 1)) % 10;
		if (t == (len - 2))
		{
			LCD_ShowChar(x + (len - 2) * sizex, y, '.', fontcolor, backgroundcolor, sizey, 0);
			t++;
			len += 1;
		}
		LCD_ShowChar(x + t * sizex, y, temp + 48, fontcolor, backgroundcolor, sizey, 0);
	}
}
/******************************************************************************
	Function：	show  string
	Parameter： x,y      Coordinate
				num      Display float number
				len      Number of digits
				fontcolor  Font color
				backgroundcolor  Background color
				sizey    Font size
	Return：  	None
******************************************************************************/
void LCD_ShowString(u16 x, u16 y, const char *p, u16 fontcolor, u16 backgroundcolor, u8 sizey, u8 mode)
{
	while (*p != '\0')
	{
		LCD_ShowChar(x, y, *p, fontcolor, backgroundcolor, sizey, mode);
		x += sizey / 2;
		p++;
	}
}
/******************************************************************************
	Function：	show  Chinese character
	Parameter： x,y      	Coordinate
				s   	 	Chinese character buffer
				fontcolor  	Font color
				backgroundcolor  Background color
				sizey    		Font size
	Return：  	None
******************************************************************************/
void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fontcolor, u16 backgroundcolor, u8 sizey, u8 mode)
{
	while (*s != 0)
	{
		if (sizey == 12)
			LCD_ShowChinese12x12(x, y, s, fontcolor, backgroundcolor, sizey, mode);
		else if (sizey == 16)
			LCD_ShowChinese16x16(x, y, s, fontcolor, backgroundcolor, sizey, mode);
		else if (sizey == 24)
			LCD_ShowChinese24x24(x, y, s, fontcolor, backgroundcolor, sizey, mode);
		else if (sizey == 32)
			LCD_ShowChinese32x32(x, y, s, fontcolor, backgroundcolor, sizey, mode);
		else
			return;
		s += 2;
		x += sizey;
	}
}

/******************************************************************************
	  函数说明：显示单个12x12汉字
	  入口数据：x,y显示坐标
				*s 要显示的汉字
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowChinese12x12(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 k;
	u16 HZnum;		 // 汉字数目
	u16 TypefaceNum; // 一个字符所占字节大小
	u16 x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;

	HZnum = sizeof(tfont12) / sizeof(typFNT_GB12); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont12[k].Index[0] == *(s)) && (tfont12[k].Index[1] == *(s + 1)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont12[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont12[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/******************************************************************************
	  函数说明：显示单个16x16汉字
	  入口数据：x,y显示坐标
				*s 要显示的汉字
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 k;
	u16 HZnum;		 // 汉字数目
	u16 TypefaceNum; // 一个字符所占字节大小
	u16 x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont16) / sizeof(typFNT_GB16); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont16[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont16[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/******************************************************************************
	  函数说明：显示单个24x24汉字
	  入口数据：x,y显示坐标
				*s 要显示的汉字
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 k;
	u16 HZnum;		 // 汉字数目
	u16 TypefaceNum; // 一个字符所占字节大小
	u16 x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont24) / sizeof(typFNT_GB24); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont24[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont24[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}

/******************************************************************************
	  函数说明：显示单个32x32汉字
	  入口数据：x,y显示坐标
				*s 要显示的汉字
				fc 字的颜色
				bc 字的背景色
				sizey 字号
				mode:  0非叠加模式  1叠加模式
	  返回值：  无
******************************************************************************/
void LCD_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
	u8 i, j, m = 0;
	u16 k;
	u16 HZnum;		 // 汉字数目
	u16 TypefaceNum; // 一个字符所占字节大小
	u16 x0 = x;
	TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
	HZnum = sizeof(tfont32) / sizeof(typFNT_GB32); // 统计汉字数目
	for (k = 0; k < HZnum; k++)
	{
		if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1)))
		{
			LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
			for (i = 0; i < TypefaceNum; i++)
			{
				for (j = 0; j < 8; j++)
				{
					if (!mode) // 非叠加方式
					{
						if (tfont32[k].Msk[i] & (0x01 << j))
							LCD_WR_DATA(fc);
						else
							LCD_WR_DATA(bc);
						m++;
						if (m % sizey == 0)
						{
							m = 0;
							break;
						}
					}
					else // 叠加方式
					{
						if (tfont32[k].Msk[i] & (0x01 << j))
							LCD_DrawPoint(x, y, fc); // 画一个点
						x++;
						if ((x - x0) == sizey)
						{
							x = x0;
							y++;
							break;
						}
					}
				}
			}
		}
		continue; // 查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
}
/******************************************************************************
	Function：	show  picture
	Parameter： x,y      	Coordinate
				length   	picture length
				width    	picture width
				pic   	 	picture buffer
	Return：  	None
******************************************************************************/
void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[])
{
	u16 num, picH, picL;
	num = length * width;
	LCD_Address_Set(x, y, x + length - 1, y + width - 1);
	LCD_CS_Clr();
	for (int i = 0; i < num; i++)
	{
		picL = pic[2 * i];
		picH = pic[2 * i + 1];
		LCD_WR_DATA(picH << 8 | picL);
	}
	LCD_CS_Set();
}

void LCD_Test(void)
{
	float t;
	t += 0.11; // change it to global value
	LCD_ShowString(24, 30, "LCD_W:", GREEN, WHITE, 16, 0);
	LCD_ShowIntNum(72, 30, LCD_W, 3, RED, WHITE, 16);
	LCD_ShowString(24, 50, "LCD_H:", BLUE, WHITE, 16, 0);
	LCD_ShowIntNum(72, 50, LCD_H, 3, RED, WHITE, 16);
	LCD_ShowFloatNum1(20, 80, t, 4, RED, WHITE, 16);
	LCD_ShowPicture(65, 80, 40, 40, gImage_1);
}