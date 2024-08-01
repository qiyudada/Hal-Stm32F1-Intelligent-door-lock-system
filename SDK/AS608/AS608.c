#include "AS608.h"

uint32_t AS608Addr = 0XFFFFFFFF;
uint8_t AS608_USART_RX_BUF[USART2_MAX_RECV_LEN] = {0};										// 接收缓冲,最大USART2_MAX_RECV_LEN个字节.
volatile uint8_t USART2_RX_STA = 0;															// 串口是否接收到数据
uint8_t Get_Device_Code[10] = {0x01, 0x00, 0x07, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b}; // 口令验证
SysPara AS608Para = {0};																	// 指纹模块AS608参数
uint16_t ValidN = 0;																		// 模块内有效指纹个数

/*send a byte*/
static uint8_t MYUSART_SendData(uint8_t data)
{
	if (HAL_UART_Transmit(&AS608_UART, &data, 1, HAL_MAX_DELAY) == HAL_OK)
	{
		return AS608_Correctness;
	}
	else
	{
		return AS608_Error;
	}
}

/*send a head*/
static void SendHead(void)
{
	memset(AS608_USART_RX_BUF, 0, sizeof(AS608_USART_RX_BUF)); // 发送前清空数据，因为所有包都需要发送包头，在发送包头前清空
	MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}

/*send a address*/
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr >> 24);
	MYUSART_SendData(AS608Addr >> 16);
	MYUSART_SendData(AS608Addr >> 8);
	MYUSART_SendData(AS608Addr);
}

/*send a head flag*/
static void SendFlag(uint8_t flag)
{
	MYUSART_SendData(flag);
}
/*send a length*/
static void SendLength(int length)
{
	MYUSART_SendData(length >> 8);
	MYUSART_SendData(length);
}
/*send a cmd */
static void Sendcmd(uint8_t cmd)
{
	MYUSART_SendData(cmd);
}
/*send a check*/
static void SendCheck(uint16_t check)
{
	MYUSART_SendData(check >> 8);
	MYUSART_SendData(check);
}

/*****************************************
 @name  AS608_Check(void)
 @param none
 @details Check module connection
 @return  0 correctness 1 error
*****************************************/
static uint8_t AS608_Check(void)
{
	AS608_USART_RX_BUF[9] = 1;

	SendHead();
	SendAddr();
	for (int i = 0; i < 10; i++)
	{
		MYUSART_SendData(Get_Device_Code[i]);
	}
	// HAL_UART_Receive(&AS608_UART, USART2_RX_BUF, 12, 100); // 串口三接收12个数据
	Delay_ms(100); // 等待200ms
	if (AS608_USART_RX_BUF[9] == 0)
	{
		return AS608_Correctness;
	}
	return AS608_Error;
}

/*****************************************
 @name  AS608_Init(void)
 @param none
 @details Initate module connection
 @return  AS608_Check()
*****************************************/
uint8_t AS608_Init(void)
{
	USART2_RX_STA = 0;
	// 设置uart2接收中断
	HAL_UART_Receive_IT(&AS608_UART, AS608_USART_RX_BUF, sizeof(AS608_USART_RX_BUF)); // 接收数据，且产生中断
	// 使能空闲中断
	__HAL_UART_ENABLE_IT(&AS608_UART, UART_IT_IDLE);
	return AS608_Check();
}

/*****************************************
 @name  	JudgeStr(uint16_t waittime)
 @param 	waittime
 @details 	judge array has answer package or not
 @return  	data package address or 0(failed)
*****************************************/
static uint8_t *JudgeStr(uint16_t waittime)
{
	char *data;
	uint8_t str[8];
	str[0] = 0xef;
	str[1] = 0x01;
	str[2] = AS608Addr >> 24;
	str[3] = AS608Addr >> 16;
	str[4] = AS608Addr >> 8;
	str[5] = AS608Addr;
	str[6] = 0x07;
	str[7] = '\0';
	AS608_USART_RX_STA = 0;
	while (--waittime)
	{
		Delay_ms(1);
		if (AS608_USART_RX_STA) // 接收到一次数据
		{
			AS608_USART_RX_STA = 0; // reset flag
			data = strstr((const char *)AS608_USART_RX_BUF, (const char *)str);
			if (data)
			{
				return (uint8_t *)data;
			}
		}
	}
	return AS608_Correctness;
}

/*********************************************************************************
 @name  	PS_GetImage(void)
 @param 	None
 @details 	Detect finger, if detected, store finger image in ImageBuffer.
 @return  	Confirmed word
**********************************************************************************/
uint8_t PS_GetImage(void)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x01);
	temp = 0x01 + 0x03 + 0x01;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}

/*********************************************************************************
 @name  	PS_GenChar
 @param 	BufferID
 @details 	Create a fingerprint feature file in CharBuffer1 or CharBuffer2
 @return  	Confirmed word
**********************************************************************************/
uint8_t PS_GenChar(uint8_t BufferID)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x04);
	Sendcmd(0x02);
	MYUSART_SendData(BufferID);
	temp = 0x01 + 0x04 + 0x02 + BufferID;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}

/*********************************************************************************
 @name  	PS_Match
 @param 	None
 @details 	Compare the fingerprint features precisely in CharBuffer1 and CharBuffer2
 @return  	Confirmed word
**********************************************************************************/
uint8_t PS_Match(void)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x03);
	temp = 0x01 + 0x03 + 0x03;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}

/*********************************************************************************
 @name  	PS_Search
 @param 	BufferID	charbuffer1 or 2 id
 @param 	StartPage	start page
 @param 	PageNum		page num
 @param 	p			SearchResult structure
 @details 	search fingerprint template
 @return  	Confirmed word(page num)
**********************************************************************************/
uint8_t PS_Search(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x08);
	Sendcmd(0x04);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage >> 8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum >> 8);
	MYUSART_SendData(PageNum);
	temp = 0x01 + 0x08 + 0x04 + BufferID + (StartPage >> 8) + (uint8_t)StartPage + (PageNum >> 8) + (uint8_t)PageNum;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
	{
		ensure = data[9];
		p->pageID = (data[10] << 8) + data[11];
		p->mathscore = (data[12] << 8) + data[13];
	}
	else
		ensure = 0xff;
	return ensure;
}

/*********************************************************************************
 @name  	PS_RegModel
 @param 	None
 @details 	merge fingerprint template and store to CharBuffer1 and CharBuffer2
 @return  	Confirmed word
**********************************************************************************/
uint8_t PS_RegModel(void)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x05);
	temp = 0x01 + 0x03 + 0x05;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}

/*********************************************************************************
 @name  	PS_StoreChar
 @param 	BufferID	@ref charBuffer1:0x01	charBuffer1:0x02
 @param 	PageID  	Finger print database location number
 @details 	Store the template file in CharBuffer1 or CharBuffer2 to the flash database location of PageID.
 @return  	Confirmed word
**********************************************************************************/
uint8_t PS_StoreChar(uint8_t BufferID, uint16_t PageID)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x06);
	Sendcmd(0x06);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(PageID >> 8);
	MYUSART_SendData(PageID);
	temp = 0x01 + 0x06 + 0x06 + BufferID + (PageID >> 8) + (uint8_t)PageID;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}

/*********************************************************************************
 @name  	PS_DeletChar
 @param 	PageID  	Finger print database location number
 @param 	N  			Delete the number of templates
 @details 	Delete templates in the flash database starting from the specified ID number.
 @return  	Confirmed word
**********************************************************************************/
uint8_t PS_DeletChar(uint16_t PageID, uint16_t N)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x07);
	Sendcmd(0x0C);
	MYUSART_SendData(PageID >> 8);
	MYUSART_SendData(PageID);
	MYUSART_SendData(N >> 8);
	MYUSART_SendData(N);
	temp = 0x01 + 0x07 + 0x0C + (PageID >> 8) + (uint8_t)PageID + (N >> 8) + (uint8_t)N;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}

/*********************************************************************************
 @name  	PS_Empty
 @param 	None
 @details 	Delete all of templates in the flash database
 @return  	Confirmed word
**********************************************************************************/
uint8_t PS_Empty(void)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x0D);
	temp = 0x01 + 0x03 + 0x0D;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}

/*********************************************************************************
 @name  	PS_WriteReg
 @param 	RegNum:4\5\6
 @param 	DATA
 @details 	write register
 @return  	Confirmed word
**********************************************************************************/
uint8_t PS_WriteReg(uint8_t RegNum, uint8_t DATA)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x05);
	Sendcmd(0x0E);
	MYUSART_SendData(RegNum);
	MYUSART_SendData(DATA);
	temp = RegNum + DATA + 0x01 + 0x05 + 0x0E;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	if (ensure == 0)
		printf("\r\nSet up Parameter successfully\r\n");
	else
		printf("\r\n%s", EnsureMessage(ensure));
	return ensure;
}

/*********************************************************************************
 @name  	PS_ReadSysPara
 @param 	p 	Finger based parameter data
 @details 	read register
 @return  	Confirmed word + basic parameter (16 bytes)
**********************************************************************************/
uint8_t PS_ReadSysPara(SysPara *p)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x0F);
	temp = 0x01 + 0x03 + 0x0F;
	SendCheck(temp);
	data = JudgeStr(1000);
	if (data)
	{
		ensure = data[9];
		p->PS_max = (data[14] << 8) + data[15];
		p->PS_level = data[17];
		p->PS_addr = (data[18] << 24) + (data[19] << 16) + (data[20] << 8) + data[21];
		p->PS_size = data[23];
		p->PS_N = data[25];
	}
	else
		ensure = 0xff;
	if (ensure == 0x00)
	{
		printf("\r\nThe maximum capacity of module=%d", p->PS_max);
		printf("\r\nContrasted rank=%d", p->PS_level);
		printf("\r\nAdress=%x", p->PS_addr);
		printf("\r\nBound=%d", p->PS_N * 9600);
	}
	else
		printf("\r\n%s", EnsureMessage(ensure));
	return ensure;
}

/*********************************************************************************
 @name  	PS_SetAddr
 @param 	PS_addr
 @details 	set up module address
 @return  	Confirmed word
**********************************************************************************/
uint8_t PS_SetAddr(uint32_t PS_addr)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x07);
	Sendcmd(0x15);
	MYUSART_SendData(PS_addr >> 24);
	MYUSART_SendData(PS_addr >> 16);
	MYUSART_SendData(PS_addr >> 8);
	MYUSART_SendData(PS_addr);
	temp = 0x01 + 0x07 + 0x15 + (uint8_t)(PS_addr >> 24) + (uint8_t)(PS_addr >> 16) + (uint8_t)(PS_addr >> 8) + (uint8_t)PS_addr;
	SendCheck(temp);
	AS608Addr = PS_addr; // 发送完指令，更换地址
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	AS608Addr = PS_addr;
	if (ensure == 0x00)
		printf("\r\nSet up Parameter successfully");
	else
		printf("\r\n%s", EnsureMessage(ensure));
	return ensure;
}

/*********************************************************************************
 @name  	PS_WriteNotepad
 @param 	NotePageNum		0~15
 @param 	Byte32			will write content,32 bytes
 @details 	create 256 bytes FLASH space for user to store user notes
 @return  	Confirmed word
**********************************************************************************/
uint8_t PS_WriteNotepad(uint8_t NotePageNum, uint8_t *Byte32)
{
	uint16_t temp;
	uint8_t ensure, i;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(36);
	Sendcmd(0x18);
	MYUSART_SendData(NotePageNum);
	for (i = 0; i < 32; i++)
	{
		MYUSART_SendData(Byte32[i]);
		temp += Byte32[i];
	}
	temp = 0x01 + 36 + 0x18 + NotePageNum + temp;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
		ensure = data[9];
	else
		ensure = 0xff;
	return ensure;
}

/*********************************************************************************
 @name  	PS_ReadNotepad
 @param 	NotePageNum		0~15
 @param 	Byte32			will read content,32 bytes
 @details 	read 128 bytes FLASH user space
 @return  	Confirmed word + user message
**********************************************************************************/
uint8_t PS_ReadNotepad(uint8_t NotePageNum, uint8_t *Byte32)
{
	uint16_t temp;
	uint8_t ensure, i;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x04);
	Sendcmd(0x19);
	MYUSART_SendData(NotePageNum);
	temp = 0x01 + 0x04 + 0x19 + NotePageNum;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
	{
		ensure = data[9];
		for (i = 0; i < 32; i++)
		{
			Byte32[i] = data[10 + i];
		}
	}
	else
		ensure = 0xff;
	return ensure;
}

/*********************************************************************************
 @name  	PS_HighSpeedSearch
 @param 	BufferID		charbuffer1 or charbuffer2
 @param 	StartPage		start page
 @param 	PageNum			page number
 @param 	p				SearchResult
 @details 	search fingerprint template fastly
 @return  	Confirmed word + marked page number
**********************************************************************************/
uint8_t PS_HighSpeedSearch(uint8_t BufferID, uint16_t StartPage, uint16_t PageNum, SearchResult *p)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x08);
	Sendcmd(0x1b);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage >> 8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum >> 8);
	MYUSART_SendData(PageNum);
	temp = 0x01 + 0x08 + 0x1b + BufferID + (StartPage >> 8) + (uint8_t)StartPage + (PageNum >> 8) + (uint8_t)PageNum;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
	{
		ensure = data[9];
		p->pageID = (data[10] << 8) + data[11];
		p->mathscore = (data[12] << 8) + data[13];
	}
	else
		ensure = 0xff;
	return ensure;
}

/*********************************************************************************
 @name  	PS_ValidTempleteNum
 @param 	ValidN 		Valid template number
 @details 	read valid template number
 @return  	Confirmed word + user message
**********************************************************************************/
uint8_t PS_ValidTempleteNum(uint16_t *ValidN)
{
	uint16_t temp;
	uint8_t ensure;
	uint8_t *data;
	SendHead();
	SendAddr();
	SendFlag(0x01); // 命令包标识
	SendLength(0x03);
	Sendcmd(0x1d);
	temp = 0x01 + 0x03 + 0x1d;
	SendCheck(temp);
	data = JudgeStr(2000);
	if (data)
	{
		ensure = data[9];
		*ValidN = (data[10] << 8) + data[11];
	}
	else
		ensure = 0xff;

	if (ensure == 0x00)
	{
		printf("\r\nVaild finger print number=%d", (data[10] << 8) + data[11]);
	}
	else
		printf("\r\n%s", EnsureMessage(ensure));
	return ensure;
}

/*********************************************************************************
 @name  	PS_HandShake
 @param 	PS_Addr
 @details 	return correct address pointer
 @return  	correct or error
**********************************************************************************/
uint8_t PS_HandShake(uint32_t *PS_Addr)
{
	SendHead();
	SendAddr();
	MYUSART_SendData(0X01);
	MYUSART_SendData(0X00);
	MYUSART_SendData(0X00);
	Delay_ms(200);
	if (USART2_RX_STA) // 接收到数据
	{
		if (AS608_USART_RX_BUF[0] == 0XEF && AS608_USART_RX_BUF[1] == 0X01 && AS608_USART_RX_BUF[6] == 0X07) // 判断是不是模块返回的应答包
		{
			*PS_Addr = (AS608_USART_RX_BUF[2] << 24) + (AS608_USART_RX_BUF[3] << 16) + (AS608_USART_RX_BUF[4] << 8) + (AS608_USART_RX_BUF[5]);
			USART2_RX_STA = 0;
			return 0;
		}
		USART2_RX_STA = 0;
	}
	return 1;
}

/*********************************************************************************
 @name  	EnsureMessage
 @param 	ensure
 @details 	Analysis the error message of the module
 @return  	pointer to the error message
**********************************************************************************/
const char *EnsureMessage(uint8_t ensure)
{
	const char *p;
	switch (ensure)
	{
	case 0x00:
		p = "OK \r\n";
		break;
	case 0x01:
		p = "Data Package receive error\r\n";
		break;
	case 0x02:
		p = "No finger detected on sensor\r\n";
		break;
	case 0x03:
		p = "Logging fingerprint image failed\r\n";
		break;
	case 0x04:
		p = "Fingerprint is too dry or too weak\r\n";
		break;
	case 0x05:
		p = "Fingerprint is too wet or too strong\r\n";
		break;
	case 0x06:
		p = "Fingerprint image is too messy\r\n";
		break;
	case 0x07:
		p = "Fingerprint feature point is too few\r\n";
		break;
	case 0x08:
		p = "Fingerprint is not matched\r\n";
		break;
	case 0x09:
		p = "No fingerprint is found\r\n";
		break;
	case 0x0a:
		p = "Merge feature is failed\r\n";
		break;
	case 0x0b:
		p = "Address number is out of range\r\n";
	case 0x10:
		p = "Delete template is failed\r\n";
		break;
	case 0x11:
		p = "Clear fingerprint bank is failed\r\n";
		break;
	case 0x15:
		p = "No vaild image in buffer\r\n";
		break;
	case 0x18:
		p = "Read or write FLASH is failed\r\n";
		break;
	case 0x19:
		p = "Unknown error\r\n";
		break;
	case 0x1a:
		p = "Invaild register number\r\n";
		break;
	case 0x1b:
		p = "Register content error\r\n";
		break;
	case 0x1c:
		p = "Notebook page number is incorrect\r\n";
		break;
	case 0x1f:
		p = "Fingerprint bank is fulled\r\n";
		break;
	case 0x20:
		p = "Address is incorrect\r\n";
		break;
	default:
		p = "Return code error\r\n";
		break;
	}
	return p;
}

/*********************************************************************************
 @name  	ShowErrMessage
 @param 	ensure
 @details 	Print error message
 @return  	None
**********************************************************************************/
void ShowErrMessage(uint8_t ensure)
{
	// OLED_ShowCH(5,0,(uint8_t*)EnsureMessage(ensure));
	printf("%s\r\n", EnsureMessage(ensure));
}
/*********************************************************************************
 @name  	press_FR
 @param 	void
 @note 		input fingerprint
 @return  	None
**********************************************************************************/
uint16_t Press_FR(void)
{
	SearchResult seach;
	uint8_t ensure;
	char str[20];
	uint32_t AS608_Timeout = 2000000;
	while (AS608_Timeout--)
	{
		ensure = PS_GetImage();
		if (ensure == 0x00) // 获取图像成功
		{
			ensure = PS_GenChar(CharBuffer1);
			if (ensure == 0x00) // 生成特征成功
			{
				ensure = PS_HighSpeedSearch(CharBuffer1, 0, 99, &seach);
				if (ensure == 0x00) // 搜索成功
				{
					printf("Verify fingerprint successfully\r\n");
					sprintf(str, "ID:%d Scores:%d ", seach.pageID, seach.mathscore);
					printf("%s\r\n", str);
					Delay_ms(500);
					return seach.mathscore;
				}
				else
				{
					printf("Verify failled\r\n");
					Delay_ms(500);
					return 0;
				}
			}
			else
			{
				printf("Please press finger\r\n");
			}
		}
	}
}

/*********************************************************************************
 @name  	Add_FR
 @param 	void
 @note 		Add fingerprint
 @return  	None
**********************************************************************************/
void Add_FR(void)
{
	uint8_t i = 0, ensure, processnum = 0;
	uint8_t ID_NUM = 0;
	while (1)
	{
		switch (processnum)
		{
		case 0:
			i++;
			LCD_Fill(-10, -10, 240, 240, WHITE);
			LCD_ShowString(0, 64, "Logging FP", BLACK, WHITE, LCD_8x16, 0);
			printf("Please Press your finger\r\n");
			ensure = PS_GetImage();
			if (ensure == 0x00)
			{
				ensure = PS_GenChar(CharBuffer1); // 生成特征
				if (ensure == 0x00)
				{
					LCD_Fill(-10, -10, 240, 240, WHITE);
					LCD_ShowString(0, 64, "Normal FP", BLACK, WHITE, LCD_8x16, 0);
					printf("Normal Fingerprint\r\n");
					i = 0;
					processnum = 1; // 跳到第二步
				}
				else
				{
					LCD_Fill(-10, -10, 240, 240, WHITE);
					LCD_ShowString(0, 64, "Error_1", BLACK, WHITE, LCD_8x16, 0);
					ShowErrMessage(ensure);
				}
			}
			else
			{
				LCD_Fill(-10, -10, 240, 240, WHITE);
				LCD_ShowString(0, 64, "Error_1", BLACK, WHITE, LCD_8x16, 0);
				ShowErrMessage(ensure);
			}
			break;
		case 1:
			i++;
			LCD_Fill(-10, -10, 240, 240, WHITE);
			LCD_ShowString(0, 64, "Logging again", BLACK, WHITE, LCD_8x16, 0);
			printf("Press again\r\n");
			ensure = PS_GetImage();
			if (ensure == 0x00)
			{
				ensure = PS_GenChar(CharBuffer2); // 生成特征
				if (ensure == 0x00)
				{

					LCD_Fill(-10, -10, 240, 240, WHITE);
					LCD_ShowString(0, 64, "Normal FP", BLACK, WHITE, LCD_8x16, 0);
					printf("Normal Fingerprint\r\n");
					i = 0;
					processnum = 2; // 跳到第三步
				}
				else
				{
					LCD_Fill(-10, -10, 240, 240, WHITE);
					LCD_ShowString(0, 64, "Error_2", BLACK, WHITE, LCD_8x16, 0);
					ShowErrMessage(ensure);
				}
			}
			else
			{
				LCD_Fill(-10, -10, 240, 240, WHITE);
				LCD_ShowString(0, 64, "Error_2", BLACK, WHITE, LCD_8x16, 0);
				ShowErrMessage(ensure);
			}
			break;

		case 2:
			LCD_Fill(-10, -10, 240, 240, WHITE);
			LCD_ShowString(0, 64, "Contrasting....", BLACK, WHITE, LCD_8x16, 0);
			printf("Contrast with two samples\r\n");
			ensure = PS_Match();
			if (ensure == 0x00)
			{
				LCD_Fill(-10, -10, 240, 240, WHITE);
				LCD_ShowString(0, 64, "Compare right", BLACK, WHITE, LCD_8x16, 0);
				printf("Compare successfully\r\n");
				processnum = 3; // 跳到第四步
			}
			else
			{
				LCD_Fill(-10, -10, 240, 240, WHITE);
				LCD_ShowString(0, 64, "Compare failed", BLACK, WHITE, LCD_8x16, 0);
				printf("Compare failedly\r\n");
				ShowErrMessage(ensure);
				i = 0;
				processnum = 0; // 跳回第一步
			}
			Delay_ms(3000);
			break;

		case 3:
			LCD_Fill(-10, -10, 240, 240, WHITE);
			LCD_ShowString(0, 64, "Generating...", BLACK, WHITE, LCD_8x16, 0);
			printf("Generate the template\r\n");
			Delay_ms(3000);
			ensure = PS_RegModel();
			if (ensure == 0x00)
			{
				LCD_Fill(-10, -10, 240, 240, WHITE);
				LCD_ShowString(0, 64, "Generate right", BLACK, WHITE, LCD_8x16, 0);
				printf("Generate the template successfully\r\n");
				processnum = 4; // 跳到第五步
			}
			else
			{
				LCD_Fill(-10, -10, 240, 240, WHITE);
				LCD_ShowString(0, 64, "Generate failed", BLACK, WHITE, LCD_8x16, 0);
				processnum = 0;
				ShowErrMessage(ensure);
			}
			Delay_ms(3000);
			break;

		case 4:
			printf("Default ID is 6 \r\n");
			ID_NUM = 6;
			ensure = PS_StoreChar(CharBuffer2, ID_NUM); // 储存模板
			if (ensure == 0x00)
			{
				LCD_Fill(-10, -10, 240, 240, WHITE);
				LCD_ShowString(0, 64, "Add successful", BLACK, WHITE, LCD_8x16, 0);
				printf("Add Fingerprint successfully\r\n");
				Delay_ms(3000);
				LCD_Fill(-10, -10, 240, 240, WHITE);
				return;
			}
			else
			{
				LCD_Fill(-10, -10, 240, 240, WHITE);
				LCD_ShowString(0, 64, "Add failed", BLACK, WHITE, LCD_8x16, 0);
				processnum = 0;
				ShowErrMessage(ensure);
			}
			break;
		}
		Delay_ms(1000);
		if (i == 5) // 超过5次没有按手指则退出
		{
			LCD_Fill(-10, -10, 240, 240, WHITE);
			LCD_ShowString(24, 64, "Timeout!", BLACK, WHITE, LCD_8x16, 0);
			Delay_ms(1000);
			LCD_Fill(-10, -10, 240, 240, WHITE);
			break;
		}
	}
}

/*********************************************************************************
 @name  	Del_FR
 @param 	void
 @note 		Delete single fingerprint
 @return  	None
**********************************************************************************/
void Del_FR(void)
{
	uint8_t ensure;
	uint16_t ID_NUM = 0;
	printf("Start to delete single fingerprint\r\n");
	ID_NUM = 1;
	ensure = PS_DeletChar(ID_NUM, 1); // 删除单个指纹
	if (ensure == 0)
	{
		printf("Delete successfully\r\n");
	}
	else
		ShowErrMessage(ensure);
	Delay_ms(1500);
}
/*********************************************************************************
 @name  	Del_FR
 @param 	void
 @note 		Delete gross fingerprint
 @return  	None
**********************************************************************************/
void Del_FR_Lib(void)
{
	uint8_t ensure;
	printf("Start to delete gross fingerprint\r\n");
	ensure = PS_Empty(); // 清空指纹库
	if (ensure == 0)
	{
		printf("Clear successfully\r\n");
	}
	else
		ShowErrMessage(ensure);
	Delay_ms(500);
}
/*********************************************************************************
 @name  	FP_Feedback
 @param 	void
 @note 		Using Fingerprint to open door
 @return  	None
**********************************************************************************/
void FP_Feedback(void)
{
	LCD_Fill(-10, -10, 240, 240, WHITE);
	LCD_ShowString(0, 64, "Please Press:", BLACK, WHITE, LCD_8x16, 0);
	if (Press_FR() >= 100)
	{
		Door_Open();
		LCD_Fill(-10, -10, 240, 240, WHITE);
	}
	else
	{
		Door_Error();
		LCD_Fill(-10, -10, 240, 240, WHITE);
	}
}
/*********************************************************************************
 @name  	HAL_UART_IdleCpltCallback
 @param 	UART_HandleTypeDef *huart UART handle
 @note 		a callback function
 @return  	None
**********************************************************************************/
void HAL_UART2_IdleCpltCallback(UART_HandleTypeDef *huart)
{
	if (__HAL_UART_GET_FLAG(&AS608_UART, UART_FLAG_IDLE) != RESET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&AS608_UART);
		AS608_UART.pRxBuffPtr = AS608_USART_RX_BUF;
		AS608_UART.RxXferCount = sizeof(AS608_USART_RX_BUF);	
		USART2_RX_STA = 1;
		HAL_UART_Receive_IT(&AS608_UART, AS608_USART_RX_BUF, sizeof(AS608_USART_RX_BUF));
	}
}
