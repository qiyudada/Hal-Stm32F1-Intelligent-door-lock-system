#include "RC522.h"

RC522InfoTypeDef RC522handle = {
    .readUid = {0},
    .UID = {0x09, 0xF3, 0x4F, 0xA3}, // Compiler will add zeros for remaining elements
    .Card_KEYA = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    .Card_KEYB = {0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
    .DATA1 = {0x12, 0x34, 0x56, 0x78, 0x9A, 0x00, 0xff, 0x07, 0x80, 0x29, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

/**************************************************************************************
 * 函数名称：MFRC_Init
 * 功能描述：MFRC初始化
 * 入口参数：无
 * 出口参数：无
 * 返 回 值：无
 * 说    明：MFRC的SPI接口速率为0~10Mbps
 ***************************************************************************************/
void MFRC_Init(void)
{
    RC522_NSS(1);
    RC522_RST(1);
}

/**************************************************************************************
 * 函数名称: SPI_RW_Byte
 * 功能描述: 模拟SPI读写一个字节
 * 入口参数: -byte:要发送的数据
 * 出口参数: -byte:接收到的数据
 ***************************************************************************************/

uint8_t SPI2_RW_Byte(uint8_t byte)
{
    HAL_SPI_TransmitReceive(&hspi2, &byte, &RC522handle.ret, 1, 10); // 把byte 写入，并读出一个值，把它存入ret
    return RC522handle.ret;                                          // 入口是byte 的地址，读取时用的也是ret地址，一次只写入一个值10
}

/**************************************************************************************
 * 函数名称：MFRC_WriteReg
 * 功能描述：写一个寄存器
 * 入口参数：-addr:待写的寄存器地址
 *           -data:待写的寄存器数据
 * 出口参数：无
 * 返 回 值：无
 * 说    明：无
 ***************************************************************************************/
void MFRC_WriteReg(uint8_t addr, uint8_t data)
{
    uint8_t AddrByte;
    AddrByte = (addr << 1) & 0x7E; // 求出地址字节
    RC522_NSS(0);                  // NSS拉低
    SPI2_RW_Byte(AddrByte);        // 写地址字节
    SPI2_RW_Byte(data);            // 写数据
    RC522_NSS(1);                  // NSS拉高
}

/**************************************************************************************
 * 函数名称：MFRC_ReadReg
 * 功能描述：读一个寄存器
 * 入口参数：-addr:待读的寄存器地址
 * 出口参数：无
 * 返 回 值：-data:读到寄存器的数据
 * 说    明：无
 ***************************************************************************************/
uint8_t MFRC_ReadReg(uint8_t addr)
{
    uint8_t AddrByte, data;
    AddrByte = ((addr << 1) & 0x7E) | 0x80; // 求出地址字节
    RC522_NSS(0);                           // NSS拉低
    SPI2_RW_Byte(AddrByte);                 // 写地址字节
    data = SPI2_RW_Byte(0x00);              // 读数据
    RC522_NSS(1);                           // NSS拉高
    return data;
}

/**************************************************************************************
 * 函数名称：MFRC_SetBitMask
 * 功能描述：设置寄存器的位
 * 入口参数：-addr:待设置的寄存器地址
 *           -mask:待设置寄存器的位(可同时设置多个bit)
 * 出口参数：无
 * 返 回 值：无
 * 说    明：无
 ***************************************************************************************/
void MFRC_SetBitMask(uint8_t addr, uint8_t mask)
{
    uint8_t temp;
    temp = MFRC_ReadReg(addr);        // 先读回寄存器的值
    MFRC_WriteReg(addr, temp | mask); // 处理过的数据再写入寄存器
}

/**************************************************************************************
 * 函数名称：MFRC_ClrBitMask
 * 功能描述：清除寄存器的位
 * 入口参数：-addr:待清除的寄存器地址
 *           -mask:待清除寄存器的位(可同时清除多个bit)
 * 出口参数：无
 * 返 回 值：无
 * 说    明：无
 ***************************************************************************************/
void MFRC_ClrBitMask(uint8_t addr, uint8_t mask)
{
    uint8_t temp;
    temp = MFRC_ReadReg(addr);         // 先读回寄存器的值
    MFRC_WriteReg(addr, temp & ~mask); // 处理过的数据再写入寄存器
}

/**************************************************************************************
 * 函数名称：MFRC_CalulateCRC
 * 功能描述：用MFRC计算CRC结果
 * 入口参数：-pInData：带进行CRC计算的数据
 *           -len：带进行CRC计算的数据长度
 *           -pOutData：CRC计算结果
 * 出口参数：-pOutData：CRC计算结果
 * 返 回 值：无
 * 说    明：无
 ***************************************************************************************/
void MFRC_CalulateCRC(uint8_t *pInData, uint8_t len, uint8_t *pOutData)
{
    // 0xc1 1        2           pInData[2]
    uint8_t temp;
    uint32_t i;
    MFRC_ClrBitMask(MFRC_DivIrqReg, 0x04);     // 使能CRC中断
    MFRC_WriteReg(MFRC_CommandReg, MFRC_IDLE); // 取消当前命令的执行
    MFRC_SetBitMask(MFRC_FIFOLevelReg, 0x80);  // 清除FIFO及其标志位
    for (i = 0; i < len; i++)                  // 将待CRC计算的数据写入FIFO
    {
        MFRC_WriteReg(MFRC_FIFODataReg, *(pInData + i));
    }
    MFRC_WriteReg(MFRC_CommandReg, MFRC_CALCCRC); // 执行CRC计算
    i = 100000;
    do
    {
        temp = MFRC_ReadReg(MFRC_DivIrqReg); // 读取DivIrqReg寄存器的值
        i--;
    } while ((i != 0) && !(temp & 0x04)); // 等待CRC计算完成
    pOutData[0] = MFRC_ReadReg(MFRC_CRCResultRegL); // 读取CRC计算结果
    pOutData[1] = MFRC_ReadReg(MFRC_CRCResultRegM);
}

/**************************************************************************************
 * 函数名称：MFRC_CmdFrame
 * 功能描述：MFRC522和ISO14443A卡通讯的命令帧函数
 * 入口参数：-cmd：MFRC522命令字
 *           -pIndata：MFRC522发送给MF1卡的数据的缓冲区首地址
 *           -InLenByte：发送数据的字节长度
 *           -pOutdata：用于接收MF1卡片返回数据的缓冲区首地址
 *           -pOutLenBit：MF1卡返回数据的位长度
 * 出口参数：-pOutdata：用于接收MF1卡片返回数据的缓冲区首地址
 *           -pOutLenBit：用于MF1卡返回数据位长度的首地址
 * 返 回 值：-status：错误代码(MFRC_OK、MFRC_NOTAGERR、MFRC_ERR)
 * 说    明：无
 ***************************************************************************************/
char MFRC_CmdFrame(uint8_t cmd, uint8_t *pInData, uint8_t InLenByte, uint8_t *pOutData, uint16_t *pOutLenBit)
{
    uint8_t lastBits;
    uint8_t n;
    uint32_t i;
    char status = MFRC_ERR;
    uint8_t irqEn = 0x00;
    uint8_t waitFor = 0x00;

    /*根据命令设置标志位*/
    switch (cmd)
    {
    case MFRC_AUTHENT: // Mifare认证
        irqEn = 0x12;
        waitFor = 0x10; // idleIRq中断标志
        break;
    case MFRC_TRANSCEIVE: // 发送并接收数据
        irqEn = 0x77;
        waitFor = 0x30; // RxIRq和idleIRq中断标志
        break;
    }

    /*发送命令帧前准备*/
    MFRC_WriteReg(MFRC_ComIEnReg, irqEn | 0x80); // 开中断
    MFRC_ClrBitMask(MFRC_ComIrqReg, 0x80);       // 清除中断标志位SET1
    MFRC_WriteReg(MFRC_CommandReg, MFRC_IDLE);   // 取消当前命令的执行
    MFRC_SetBitMask(MFRC_FIFOLevelReg, 0x80);    // 清除FIFO缓冲区及其标志位

    /*发送命令帧*/
    for (i = 0; i < InLenByte; i++) // 写入命令参数
    {
        MFRC_WriteReg(MFRC_FIFODataReg, pInData[i]);
    }
    MFRC_WriteReg(MFRC_CommandReg, cmd); // 执行命令
    if (cmd == MFRC_TRANSCEIVE)
    {
        MFRC_SetBitMask(MFRC_BitFramingReg, 0x80); // 启动发送
    }
    i = 300000; // 根据时钟频率调整,操作M1卡最大等待时间25ms
    do
    {
        n = MFRC_ReadReg(MFRC_ComIrqReg);
        i--;
    } while ((i != 0) && !(n & 0x01) && !(n & waitFor)); // 等待命令完成
    MFRC_ClrBitMask(MFRC_BitFramingReg, 0x80); // 停止发送

    /*处理接收的数据*/
    if (i != 0)
    {
        if (!(MFRC_ReadReg(MFRC_ErrorReg) & 0x1B))
        {
            status = MFRC_OK;
            if (n & irqEn & 0x01)
            {
                status = MFRC_NOTAGERR;
            }
            if (cmd == MFRC_TRANSCEIVE)
            {
                n = MFRC_ReadReg(MFRC_FIFOLevelReg);
                lastBits = MFRC_ReadReg(MFRC_ControlReg) & 0x07;
                if (lastBits)
                {
                    *pOutLenBit = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *pOutLenBit = n * 8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MFRC_MAXRLEN)
                {
                    n = MFRC_MAXRLEN;
                }
                for (i = 0; i < n; i++)
                {
                    pOutData[i] = MFRC_ReadReg(MFRC_FIFODataReg);
                }
            }
        }
        else
        {
            status = MFRC_ERR;
        }
    }

    MFRC_SetBitMask(MFRC_ControlReg, 0x80);    // 停止定时器运行
    MFRC_WriteReg(MFRC_CommandReg, MFRC_IDLE); // 取消当前命令的执行

    return status;
}

/**************************************************************************************
 * 函数名称：PCD_Reset
 * 功能描述：PCD复位
 * 入口参数：无
 * 出口参数：无
 * 返 回 值：无
 * 说    明：无
 ***************************************************************************************/
void PCD_Reset(void)
{
    /*硬复位*/
    RC522_RST(1); // 用到复位引脚
    Delay_ms(2);
    RC522_RST(0);
    Delay_ms(2);
    RC522_RST(1);
    Delay_ms(2);

    /*软复位*/
    MFRC_WriteReg(MFRC_CommandReg, MFRC_RESETPHASE);
    Delay_ms(2);

    /*复位后的初始化配置*/
    MFRC_WriteReg(MFRC_ModeReg, 0x3D);   // CRC初始值0x6363
    MFRC_WriteReg(MFRC_TReloadRegL, 30); // 定时器重装值
    MFRC_WriteReg(MFRC_TReloadRegH, 0);
    MFRC_WriteReg(MFRC_TModeReg, 0x8D);      // 定时器设置
    MFRC_WriteReg(MFRC_TPrescalerReg, 0x3E); // 定时器预分频值
    MFRC_WriteReg(MFRC_TxAutoReg, 0x40);     // 100%ASK

    PCD_AntennaOff(); // 关天线
    Delay_ms(2);
    PCD_AntennaOn(); // 开天线
}

/**************************************************************************************
 * 函数名称：PCD_AntennaOn
 * 功能描述：开启天线,使能PCD发送能量载波信号
 * 入口参数：无
 * 出口参数：无
 * 返 回 值：无
 * 说    明：每次开启或关闭天线之间应至少有1ms的间隔
 ***************************************************************************************/
void PCD_AntennaOn(void)
{
    uint8_t temp;
    temp = MFRC_ReadReg(MFRC_TxControlReg);
    if (!(temp & 0x03))
    {
        MFRC_SetBitMask(MFRC_TxControlReg, 0x03);
    }
}

/**************************************************************************************
 * 函数名称：PCD_AntennaOff
 * 功能描述：关闭天线,失能PCD发送能量载波信号
 * 入口参数：无
 * 出口参数：无
 * 返 回 值：无
 * 说    明：每次开启或关闭天线之间应至少有1ms的间隔
 ***************************************************************************************/
void PCD_AntennaOff(void)
{
    MFRC_ClrBitMask(MFRC_TxControlReg, 0x03);
}

/***************************************************************************************
 * 函数名称：PCD_Init
 * 功能描述：读写器初始化
 * 入口参数：无
 * 出口参数：无
 * 返 回 值：无
 * 说    明：无
 ***************************************************************************************/
void PCD_Init(void)
{
    MFRC_Init();      // MFRC管脚配置
    PCD_Reset();      // PCD复位  并初始化配置
    PCD_AntennaOff(); // 关闭天线
    PCD_AntennaOn();  // 开启天线
}

/***************************************************************************************
 * 函数名称：PCD_Request
 * 功能描述：寻卡
 * 入口参数： -RequestMode：讯卡方式
 *            PICC_REQIDL：寻天线区内未进入休眠状态
 *            PICC_REQALL(0x52)：寻天线区内全部卡
 *           -pCardType：用于保存卡片类型
 * 出口参数：-pCardType：卡片类型
 *                               0x4400：Mifare_UltraLight
 *                       0x0400：Mifare_One(S50)
 *                       0x0200：Mifare_One(S70)
 *                       0x0800：Mifare_Pro(X)
 *                       0x4403：Mifare_DESFire
 * 返 回 值：-status：错误代码(PCD_OK、PCD_NOTAGERR、PCD_ERR)
 * 说    明：无
 ***************************************************************************************/
char PCD_Request(uint8_t RequestMode, uint8_t *pCardType)
{
    int status;
    uint16_t unLen;
    uint8_t CmdFrameBuf[MFRC_MAXRLEN];

    MFRC_ClrBitMask(MFRC_Status2Reg, 0x08);   // 关内部温度传感器
    MFRC_WriteReg(MFRC_BitFramingReg, 0x07);  // 存储模式，发送模式，是否启动发送等
    MFRC_SetBitMask(MFRC_TxControlReg, 0x03); // 配置调制信号13.56MHZ

    CmdFrameBuf[0] = RequestMode;

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 1, CmdFrameBuf, &unLen);

    if ((status == PCD_OK) && (unLen == 0x10))
    {
        *pCardType = CmdFrameBuf[0];
        *(pCardType + 1) = CmdFrameBuf[1];
    }

    return status;
}

/***************************************************************************************
 * 函数名称：PCD_Anticoll
 * 功能描述：防冲突,获取卡号
 * 入口参数：-pSnr：用于保存卡片序列号,4字节
 * 出口参数：-pSnr：卡片序列号,4字节
 * 返 回 值：-status：错误代码(PCD_OK、PCD_NOTAGERR、PCD_ERR)
 * 说    明：无
 ***************************************************************************************/
char PCD_Anticoll(uint8_t *pSnr)
{
    char status;
    uint8_t i, snr_check = 0;
    uint16_t unLen;
    uint8_t CmdFrameBuf[MFRC_MAXRLEN];

    MFRC_ClrBitMask(MFRC_Status2Reg, 0x08);
    MFRC_WriteReg(MFRC_BitFramingReg, 0x00);
    MFRC_ClrBitMask(MFRC_CollReg, 0x80);

    CmdFrameBuf[0] = PICC_ANTICOLL1;
    CmdFrameBuf[1] = 0x20;

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 2, CmdFrameBuf, &unLen);

    if (status == PCD_OK)
    {
        for (i = 0; i < 4; i++)
        {
            *(pSnr + i) = CmdFrameBuf[i];
            snr_check ^= CmdFrameBuf[i];
        }
        if (snr_check != CmdFrameBuf[i])
        {
            status = PCD_ERR;
        }
    }

    MFRC_SetBitMask(MFRC_CollReg, 0x80);
    return status;
}

/***************************************************************************************
 * 函数名称：PCD_Select
 * 功能描述：选卡
 * 入口参数：-pSnr：卡片序列号,4字节
 * 出口参数：无
 * 返 回 值：-status：错误代码(PCD_OK、PCD_NOTAGERR、PCD_ERR)
 * 说    明：无
 ***************************************************************************************/
char PCD_Select(uint8_t *pSnr)
{
    char status;
    uint8_t i;
    uint16_t unLen;
    uint8_t CmdFrameBuf[MFRC_MAXRLEN];

    CmdFrameBuf[0] = PICC_ANTICOLL1;
    CmdFrameBuf[1] = 0x70;
    CmdFrameBuf[6] = 0;
    for (i = 0; i < 4; i++)
    {
        CmdFrameBuf[i + 2] = *(pSnr + i);
        CmdFrameBuf[6] ^= *(pSnr + i);
    }
    MFRC_CalulateCRC(CmdFrameBuf, 7, &CmdFrameBuf[7]);

    MFRC_ClrBitMask(MFRC_Status2Reg, 0x08);

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 9, CmdFrameBuf, &unLen);

    if ((status == PCD_OK) && (unLen == 0x18))
    {
        status = PCD_OK;
    }
    else
    {
        status = PCD_ERR;
    }
    return status;
}

/***************************************************************************************
 * 函数名称：PCD_AuthState
 * 功能描述：验证卡片密码
 * 入口参数：-AuthMode：验证模式
 *                   PICC_AUTHENT1A：验证A密码
 *                   PICC_AUTHENT1B：验证B密码
 *           -BlockAddr：块地址(0~63)
 *           -pKey：密码
 *           -pSnr：卡片序列号,4字节
 * 出口参数：无
 * 返 回 值：-status：错误代码(PCD_OK、PCD_NOTAGERR、PCD_ERR)
 * 说    明：验证密码时,以扇区为单位,BlockAddr参数可以是同一个扇区的任意块
 ***************************************************************************************/
char PCD_AuthState(uint8_t AuthMode, uint8_t BlockAddr, uint8_t *pKey, uint8_t *pSnr)
{
    char status;
    uint16_t unLen;
    uint8_t i, CmdFrameBuf[MFRC_MAXRLEN];
    CmdFrameBuf[0] = AuthMode;
    CmdFrameBuf[1] = BlockAddr;
    for (i = 0; i < 6; i++)
    {
        CmdFrameBuf[i + 2] = *(pKey + i);
    }
    for (i = 0; i < 4; i++)
    {
        CmdFrameBuf[i + 8] = *(pSnr + i);
    }

    status = MFRC_CmdFrame(MFRC_AUTHENT, CmdFrameBuf, 12, CmdFrameBuf, &unLen);
    if ((status != PCD_OK) || (!(MFRC_ReadReg(MFRC_Status2Reg) & 0x08)))
    {
        status = PCD_ERR;
    }

    return status;
}

/***************************************************************************************
 * 函数名称：PCD_WriteBlock
 * 功能描述：读MF1卡数据块
 * 入口参数：-BlockAddr：块地址
 *           -pData: 用于保存待写入的数据,16字节
 * 出口参数：无
 * 返 回 值：-status：错误代码(PCD_OK、PCD_NOTAGERR、PCD_ERR)
 * 说    明：无
 ***************************************************************************************/
char PCD_WriteBlock(uint8_t BlockAddr, uint8_t *pData)
{
    char status;
    uint16_t unLen;
    uint8_t i, CmdFrameBuf[MFRC_MAXRLEN];

    CmdFrameBuf[0] = PICC_WRITE;
    CmdFrameBuf[1] = BlockAddr;
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);

    if ((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
    {
        status = PCD_ERR;
    }

    if (status == PCD_OK)
    {
        for (i = 0; i < 16; i++)
        {
            CmdFrameBuf[i] = *(pData + i);
        }
        MFRC_CalulateCRC(CmdFrameBuf, 16, &CmdFrameBuf[16]);

        status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 18, CmdFrameBuf, &unLen);

        if ((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
        {
            status = PCD_ERR;
        }
    }

    return status;
}

/***************************************************************************************
 * 函数名称：PCD_ReadBlock
 * 功能描述：读MF1卡数据块
 * 入口参数：-BlockAddr：块地址
 *           -pData: 用于保存读出的数据,16字节
 * 出口参数：-pData: 用于保存读出的数据,16字节
 * 返 回 值：-status：错误代码(PCD_OK、PCD_NOTAGERR、PCD_ERR)
 * 说    明：无
 ***************************************************************************************/
char PCD_ReadBlock(uint8_t BlockAddr, uint8_t *pData)
{
    char status;
    uint16_t unLen;
    uint8_t i, CmdFrameBuf[MFRC_MAXRLEN];

    CmdFrameBuf[0] = PICC_READ;
    CmdFrameBuf[1] = BlockAddr;
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);
    if ((status == PCD_OK) && (unLen == 0x90))
    {
        for (i = 0; i < 16; i++)
        {
            *(pData + i) = CmdFrameBuf[i];
        }
    }
    else
    {
        status = PCD_ERR;
    }

    return status;
}

/***************************************************************************************
 * 函数名称：PCD_Value
 * 功能描述：对MF1卡数据块增减值操作
 * 入口参数：
 *           -BlockAddr：块地址
 *           -pValue：四字节增值的值,低位在前
 *           -mode：数值块操作模式
 *                  PICC_INCREMENT：增值
 *                PICC_DECREMENT：减值
 * 出口参数：无
 * 返 回 值：-status：错误代码(PCD_OK、PCD_NOTAGERR、PCD_ERR)
 * 说    明：无
 ***************************************************************************************/
char PCD_Value(uint8_t mode, uint8_t BlockAddr, uint8_t *pValue)
{
    // 0XC1        1           Increment[4]={0x03, 0x01, 0x01, 0x01};
    char status;
    uint16_t unLen;
    uint8_t i, CmdFrameBuf[MFRC_MAXRLEN];

    CmdFrameBuf[0] = mode;
    CmdFrameBuf[1] = BlockAddr;
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);

    if ((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
    {
        status = PCD_ERR;
    }

    if (status == PCD_OK)
    {
        for (i = 0; i < 16; i++)
        {
            CmdFrameBuf[i] = *(pValue + i);
        }
        MFRC_CalulateCRC(CmdFrameBuf, 4, &CmdFrameBuf[4]);
        unLen = 0;
        status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 6, CmdFrameBuf, &unLen);
        if (status != PCD_ERR)
        {
            status = PCD_OK;
        }
    }

    if (status == PCD_OK)
    {
        CmdFrameBuf[0] = PICC_TRANSFER;
        CmdFrameBuf[1] = BlockAddr;
        MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);

        status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);

        if ((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
        {
            status = PCD_ERR;
        }
    }
    return status;
}

/***************************************************************************************
 * 函数名称：PCD_BakValue
 * 功能描述：备份钱包(块转存)
 * 入口参数：-sourceBlockAddr：源块地址
 *                -goalBlockAddr   ：目标块地址
 * 出口参数：无
 * 返 回 值：-status：错误代码(PCD_OK、PCD_NOTAGERR、PCD_ERR)
 * 说    明：只能在同一个扇区内转存
 ***************************************************************************************/
char PCD_BakValue(uint8_t sourceBlockAddr, uint8_t goalBlockAddr)
{
    char status;
    uint16_t unLen;
    uint8_t CmdFrameBuf[MFRC_MAXRLEN];

    CmdFrameBuf[0] = PICC_RESTORE;
    CmdFrameBuf[1] = sourceBlockAddr;
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);
    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);
    if ((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
    {
        status = PCD_ERR;
    }

    if (status == PCD_OK)
    {
        CmdFrameBuf[0] = 0;
        CmdFrameBuf[1] = 0;
        CmdFrameBuf[2] = 0;
        CmdFrameBuf[3] = 0;
        MFRC_CalulateCRC(CmdFrameBuf, 4, &CmdFrameBuf[4]);
        status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 6, CmdFrameBuf, &unLen);
        if (status != PCD_ERR)
        {
            status = PCD_OK;
        }
    }

    if (status != PCD_OK)
    {
        return PCD_ERR;
    }

    CmdFrameBuf[0] = PICC_TRANSFER;
    CmdFrameBuf[1] = goalBlockAddr;
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);
    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);
    if ((status != PCD_OK) || (unLen != 4) || ((CmdFrameBuf[0] & 0x0F) != 0x0A))
    {
        status = PCD_ERR;
    }

    return status;
}

/***************************************************************************************
 * 函数名称：PCD_Halt
 * 功能描述：命令卡片进入休眠状态
 * 入口参数：无
 * 出口参数：无
 * 返 回 值：-status：错误代码(PCD_OK、PCD_NOTAGERR、PCD_ERR)
 * 说    明：无
 ***************************************************************************************/
char PCD_Halt(void)
{
    char status;
    uint16_t unLen;
    uint8_t CmdFrameBuf[MFRC_MAXRLEN];

    CmdFrameBuf[0] = PICC_HALT;
    CmdFrameBuf[1] = 0;
    MFRC_CalulateCRC(CmdFrameBuf, 2, &CmdFrameBuf[2]);

    status = MFRC_CmdFrame(MFRC_TRANSCEIVE, CmdFrameBuf, 4, CmdFrameBuf, &unLen);

    return status;
}

/***************************************************************************************
 * @name     readCard
 * @note     read card and feedback a function
 * @param    readUid:(uint8_t) A buffer store the card uid
 * @param    funCallBack:(void) A function pointer
 * @retval   uint8_t (char 0 correct -2 error)
 * 说    明：无
 ***************************************************************************************/
uint8_t readCard(uint8_t *readUid, void (*funCallBack)(void))
{
    uint8_t Temp[5];
    if (PCD_Request(0x52, Temp) == 0)
    {
        if (PCD_Anticoll(readUid) == 0)
        {
            if (funCallBack != NULL)
                funCallBack();
            return PCD_OK;
        }
    }
    return PCD_ERR;
}
/***************************************************************************************
 * @name     PCD_Init_RST
 * @note     Enable the RC522 chip directly
 * @param    None
 * @retval   uint8_t (char 0 correct)
 * 说    明：无
 ***************************************************************************************/
uint8_t PCD_Init_RST(void)
{
    MFRC_Init();
    PCD_Reset();
    return PCD_OK;
}

/***************************************************************************************
 * @name     Show_Card_ID
 * @note     printf the card ID
 * @param    uint8_t *readUid
 * @retval   None
 ***************************************************************************************/
void Show_Card_ID(uint8_t *readUid)
{
    printf("card id:%x-%x-%x-%x\n", readUid[0], readUid[1], readUid[2], readUid[3]);
}

/***************************************************************************************
 * @name     Card_Write_Read
 * @note     A gross test of the card read and write function
 * @param    uint8_t UID  : your card ID
 * @param    uint8_t Key_Type : Key type for choosing KeyA or KeyB(0:KeyA, 1:KeyB)
 * @param    uint8_t *Key   : Key for authentication (your set key password 0->Card_KEYA,1->Card_KEYB)
 * @param    uint8_t RW     : 1:read, 0:write
 * @param    uint8_t Address: Block address for read or write
 * @param    uint8_t *Data : Data for read or write
 * @retval   state
 ***************************************************************************************/
char Card_Write_Read(uint8_t *UID, uint8_t Key_Type, uint8_t *Key, uint8_t RW, uint8_t Address, uint8_t *Data)
{
    char state = PCD_NOTAGERR;
    uint8_t i = 0;
    uint8_t Temp_ID[4] = {0};

    if (PCD_Request(PICC_REQALL, Temp_ID) == PCD_OK)
    {
        if (PCD_Anticoll(Temp_ID) == PCD_OK)
        {
            if (PCD_Select(UID) == PCD_OK)
            {
                if (Key_Type == 0)
                {
                    if (PCD_AuthState(PICC_AUTHENT1A, Address, Key, UID) == PCD_OK)
                    {
                        printf("KEY MATCH\r\n");
                        state = PCD_OK;
                    }
                    else
                    {
                        state = PCD_ERR;
                        printf("KEY NO MATCH\r\n");
                        return state;
                    }
                }
                else
                {
                    if (PCD_AuthState(PICC_AUTHENT1B, Address, Key, UID) == PCD_OK)
                    {
                        printf("KEY MATCH\r\n");
                        state = PCD_OK;
                    }
                    else
                    {
                        printf("KEY NO MATCH\r\n");
                        state = PCD_ERR;
                        return state;
                    }
                }
            }
            else
            {
                state = PCD_ERR;
                printf("UID doesn't match\r\n");
                return state;
            }
        }
        else
        {
            state = PCD_ERR;
            printf("PCD_Anticoll error\n");
            return state;
        }
    }
    if (RW && (state == PCD_OK))
    {
        if (PCD_ReadBlock(Address, Data) == PCD_OK)
        {
            printf("data:");
            for (i = 0; i < 16; i++)
            {
                printf("%02x", Data[i]);
            }
            printf("\r\n");
        }
        else
        {
            state = PCD_ERR;
            printf("PcdRead failed\r\n");
            return state;
        }
    }
    else
    {
        if (PCD_WriteBlock(Address, Data) == PCD_OK)
        {
            printf("PcdWrite finished\r\n");
        }
        else
        {
            state = PCD_ERR;
            printf("PcdWrite failed\r\n");
            return state;
        }
    }
    if (PCD_Halt() == PCD_OK)
    {
        state = PCD_OK;
        printf("PcdHalt finished\r\n");
        return state;
    }
    else
    {
        state = PCD_ERR;
        printf("PcdHalt failed\r\n");
        return state;
    }
}

void Card_Feedback(void)
{
    LCD_Fill(-10, -10, 240, 240, WHITE);
    LCD_ShowString(24, 64, "Verify ID...", BLACK, WHITE, LCD_8x16, 0);
    Delay_ms(200);
    if (Card_Check() == PCD_OK)
    {
        LCD_Fill(-10, -10, 240, 240, WHITE);
        Door_Open();
    }
    else
    {
        LCD_Fill(-10, -10, 240, 240, WHITE);
        Door_Error();
    }
}

char Card_Check(void)
{
    if (PCD_Anticoll(RC522handle.readUid) == PCD_OK)
    {
        if (PCD_Select(RC522handle.UID) == PCD_OK)
        {
            if (PCD_AuthState(PICC_AUTHENT1A, 0x0B, RC522handle.Card_KEYA, RC522handle.UID) == PCD_OK)
            {
                printf("KEY MATCH\r\n");
                if (PCD_ReadBlock(0x08, RC522handle.DATA1) == PCD_OK)
                {
                    LCD_Fill(-10, -10, 240, 240, WHITE);
                    LCD_ShowString(30, 64, "ID PASS", BLACK, WHITE, LCD_8x16, 0);
                    Delay_ms(500);
                    return PCD_OK;
                }
                else
                {
                    LCD_Fill(-10, -10, 240, 240, WHITE);
                    LCD_ShowString(30, 64, "ID ERROR", BLACK, WHITE, LCD_8x16, 0);
                    Delay_ms(500);
                    return PCD_ERR;
                }
            }
            else
            {
                LCD_Fill(-10, -10, 240, 240, WHITE);
                LCD_ShowString(30, 64, "ID ERROR", BLACK, WHITE, LCD_8x16, 0);
                Delay_ms(500);
                return PCD_ERR;
            }
        }
        else
        {
            LCD_Fill(-10, -10, 240, 240, WHITE);
            LCD_ShowString(30, 64, "Unknown ID", BLACK, WHITE, LCD_8x16, 0);
            Delay_ms(500);
            return PCD_ERR;
        }
    }
    else
    {
        LCD_Fill(-10, -10, 240, 240, WHITE);
        LCD_ShowString(30, 64, "ID ANTI", BLACK, WHITE, LCD_8x16, 0);
        Delay_ms(500);
        return PCD_ERR;
    }
}

/***************************************************************************************
 * @name     PCD_TEST
 * @note     Test
 * @param    uint8_t *readUid
 * @retval   None
 ***************************************************************************************/
void PCD_TEST(uint8_t *readUid)
{
    if (!readCard(readUid, NULL))
    {
        Show_Card_ID(readUid);
        if (!strncmp((char *)readUid, (char *)RC522handle.UID, 4))
        {
            printf("success\r\n");
        }
        else
        {
            printf("error\r\n");
        }
    }
}
