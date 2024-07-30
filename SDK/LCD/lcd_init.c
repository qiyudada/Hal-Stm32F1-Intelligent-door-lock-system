#include "lcd_init.h"

void LCD_GPIO_Init(void)
{
}
void SPIv_WriteData(uint8_t Data)
{
    SPI1_WriteByte(&Data, 1);
}

/******************************************************************************
 *@name           LCD_Writ_Bus
 *@details        serial port write
 *@param          data
 *@return         None
 ******************************************************************************/
void LCD_Writ_Bus(uint8_t data)
{
    LCD_CS_Clr();
    SPIv_WriteData(data);
    LCD_CS_Set();
}

/******************************************************************************
 *@name     LCD_WR_DATA8
 *@details  LCD write data
 *@param    data
 *@return   none
 ******************************************************************************/
void LCD_WR_DATA8(uint8_t data)
{
    LCD_Writ_Bus(data);
}

/******************************************************************************
 *@name     LCD_WR_DATA
 *@details  LCD write double data (for picture)
 *@param    data
 *@return   none
 ******************************************************************************/
void LCD_WR_DATA(uint16_t data)
{
    LCD_Writ_Bus(data >> 8);
    LCD_Writ_Bus(data);
}

/******************************************************************************
 *@name     LCD_WR_REG
 *@details  LCD write register
 *@param    data
 *@return   none
 ******************************************************************************/
void LCD_WR_REG(uint8_t data)
{
    LCD_DC_Clr(); // 写命令
    LCD_Writ_Bus(data);
    LCD_DC_Set(); // 写数据
}

/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if (USE_HORIZONTAL == 0)
    {
        LCD_WR_REG(0x2a); // 列地址设置
        LCD_WR_DATA(x1 + 2);
        LCD_WR_DATA(x2 + 2);
        LCD_WR_REG(0x2b); // 行地址设置
        LCD_WR_DATA(y1 + 1);
        LCD_WR_DATA(y2 + 1);
        LCD_WR_REG(0x2c); // 储存器写
    }
    else if (USE_HORIZONTAL == 1)
    {
        LCD_WR_REG(0x2a); // 列地址设置
        LCD_WR_DATA(x1 + 2);
        LCD_WR_DATA(x2 + 2);
        LCD_WR_REG(0x2b); // 行地址设置
        LCD_WR_DATA(y1 + 1);
        LCD_WR_DATA(y2 + 1);
        LCD_WR_REG(0x2c); // 储存器写
    }
    else if (USE_HORIZONTAL == 2)
    {
        LCD_WR_REG(0x2a); // 列地址设置
        LCD_WR_DATA(x1 + 1);
        LCD_WR_DATA(x2 + 1);
        LCD_WR_REG(0x2b); // 行地址设置
        LCD_WR_DATA(y1 + 2);
        LCD_WR_DATA(y2 + 2);
        LCD_WR_REG(0x2c); // 储存器写
    }
    else
    {
        LCD_WR_REG(0x2a); // 列地址设置
        LCD_WR_DATA(x1 + 1);
        LCD_WR_DATA(x2 + 1);
        LCD_WR_REG(0x2b); // 行地址设置
        LCD_WR_DATA(y1 + 2);
        LCD_WR_DATA(y2 + 2);
        LCD_WR_REG(0x2c); // 储存器写
    }
}

void LCD_Init(void)
{
    LCD_RES_Clr(); // 复位
    Delay_ms(100);
    LCD_RES_Set();
    Delay_ms(100);

    LCD_BLK_Set(); // 打开背光
    Delay_ms(50);

    //************* Start Initial Sequence **********//
    LCD_WR_REG(0x11); // Sleep out
    Delay_ms(120);    // Delay 120ms
    //------------------------------------ST7735S Frame Rate-----------------------------------------//
    LCD_WR_REG(0xB1);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3A);
    LCD_WR_DATA8(0x3A);
    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3A);
    LCD_WR_DATA8(0x3A);
    LCD_WR_REG(0xB3);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3A);
    LCD_WR_DATA8(0x3A);
    LCD_WR_DATA8(0x05);
    LCD_WR_DATA8(0x3A);
    LCD_WR_DATA8(0x3A);
    //------------------------------------End ST7735S Frame Rate---------------------------------//
    LCD_WR_REG(0xB4); // Dot inversion
    LCD_WR_DATA8(0x03);
    //------------------------------------ST7735S Power Sequence---------------------------------//
    LCD_WR_REG(0xC0);
    LCD_WR_DATA8(0x62);
    LCD_WR_DATA8(0x02);
    LCD_WR_DATA8(0x04);
    LCD_WR_REG(0xC1);
    LCD_WR_DATA8(0xC0);
    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x0D);
    LCD_WR_DATA8(0x00);
    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x8D);
    LCD_WR_DATA8(0x6A);
    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x8D);
    LCD_WR_DATA8(0xEE);
    //---------------------------------End ST7735S Power Sequence-------------------------------------//
    LCD_WR_REG(0xC5); // VCOM
    LCD_WR_DATA8(0x12);
    LCD_WR_REG(0x36); // MX, MY, RGB mode
    if (USE_HORIZONTAL == 0)
        LCD_WR_DATA8(0x00);
    else if (USE_HORIZONTAL == 1)
        LCD_WR_DATA8(0xC0);
    else if (USE_HORIZONTAL == 2)
        LCD_WR_DATA8(0x70);
    else
        LCD_WR_DATA8(0xA0);
    //------------------------------------ST7735S Gamma Sequence---------------------------------//
    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0x03);
    LCD_WR_DATA8(0x1B);
    LCD_WR_DATA8(0x12);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x3A);
    LCD_WR_DATA8(0x32);
    LCD_WR_DATA8(0x34);
    LCD_WR_DATA8(0x2F);
    LCD_WR_DATA8(0x2B);
    LCD_WR_DATA8(0x30);
    LCD_WR_DATA8(0x3A);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x01);
    LCD_WR_DATA8(0x02);
    LCD_WR_DATA8(0x05);
    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0x03);
    LCD_WR_DATA8(0x1B);
    LCD_WR_DATA8(0x12);
    LCD_WR_DATA8(0x11);
    LCD_WR_DATA8(0x32);
    LCD_WR_DATA8(0x2F);
    LCD_WR_DATA8(0x2A);
    LCD_WR_DATA8(0x2F);
    LCD_WR_DATA8(0x2E);
    LCD_WR_DATA8(0x2C);
    LCD_WR_DATA8(0x35);
    LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x01);
    LCD_WR_DATA8(0x05);
    //------------------------------------End ST7735S Gamma Sequence-----------------------------//
    LCD_WR_REG(0xFC);
    LCD_WR_DATA8(0x8C);
    LCD_WR_REG(0x3A); // 65k mode
    LCD_WR_DATA8(0x05);
    LCD_WR_REG(0x36); // 65k mode
    LCD_WR_DATA8(0xC8);
    LCD_WR_REG(0x29); // Display on
}
