/************************************************
	描述：USB与上位机通信相关文件
	作者：南山匿
************************************************/


/************************************************
	头文件
************************************************/
#include "USB.h"

/*协议格式为：
		描述	   帧头		 内容长度①		  功能		 内容			 帧尾			 CRC
		长度	  2字节		  1字节		   	 1字节		N字节			3字节			2字节
		数据	0x55,0xAA	0x00~0xFF  	   0x00~0xFF	 ~	       0xFF 0xFF 0xFF		~
*/


/************************************************
	全局变量
************************************************/
			

/************************************************
	函数声明
************************************************/




/************************************************
	函数
************************************************/
/*******************************************************************************
函数：USB_init
功能：USB初始化
输入参数：无
输出参数：无
返回：无
*******************************************************************************/
void USB_init(void)
{
	delay_ms(1800);
	USB_Port_Set(0); 	//USB先断开
	delay_ms(700);
	USB_Port_Set(1);	//USB再次连接
 	Set_USBClock();   
 	USB_Interrupts_Config();    
 	USB_Init();
}


/*******************************************************************************
函数：USB_send_array
功能：USB发送数组
输入参数：
	array：需要发送的数组
	length：数组长度
输出参数：无
返回：无
*******************************************************************************/
void USB_send_array(uint8_t* array,uint8_t length)
{
	for(int i=0 ;i<length ;i++){
		USB_USART_SendData(*array);
		array++;
	}
}

/*******************************************************************************
函数：clear_array
功能：清空数组
输入参数：
	clear_data：需要清除的数组
	clear_count：数组长度
输出参数：
	清空后的clear_data数组
返回：无
*******************************************************************************/
void clear_array(uint8_t *clear_data ,uint8_t clear_count)
{
	for(int i = 0 ;i < clear_count ;i++){
		clear_data[i] = 0;
	}
}


/*******************************************************************************
函数：GetCrc
功能：CRC校验
输入参数：
	pucBuf：数据
	len：数据长度
输出参数：无
返回：16位CRC
*******************************************************************************/
unsigned short GetCrc(unsigned char *pucBuf, int len) 
{
		unsigned short usCrc = 0xffff; 
		unsigned short shift = 0; 
		int i = 0; int j = 0; 
		for(i = 0; i < len; i++) 
		{ 
			usCrc ^= pucBuf[i]; 
			for(j = 0; j < 8; j++) 
			{
				shift = usCrc & 0x1; 
				usCrc >>= 1; if(shift) 
				{ 
				usCrc ^= 0xa001; 
				} 
			} 
		} 
	return usCrc; 
}


/*******************************************************************************
函数：CRC_test
功能：对接收的数组进行CRC校验
输入参数：
	array：数组
输出参数：无
返回：（uint8_t）正确返回1，错误返回0
*******************************************************************************/
uint8_t CRC_test(uint8_t *array)
{
	uint8_t crc8_2[2];
	crc8_2[0] = (uint8_t)((GetCrc(array ,*(array + 2) + 1) >> 8) & 0x00FF);
	crc8_2[1] = (uint8_t)(GetCrc(array ,*(array + 2) + 1) & 0x00FF);
	if(crc8_2[0] == *(array + *(array + 2) +1) && crc8_2[1] == *(array + *(array + 2) +2))
		return 1;
	else
		return 0;
}


/*******************************************************************************
函数：CRC_write
功能：对发送的数组写入CRC
输入参数：
	array：数组
输出参数：
	array：加了CRC的数组
返回：无
*******************************************************************************/
void CRC_write(uint8_t *array)
{
	uint8_t crc8_2[2];
	crc8_2[0] = (uint8_t)((GetCrc(array ,*(array + 2) + 1) >> 8) & 0x00FF);
	crc8_2[1] = (uint8_t)( GetCrc(array ,*(array + 2) + 1)			 & 0x00FF);
	*(array + (*(array + 2) +1)) = crc8_2[0];
	*(array + (*(array + 2) +2)) = crc8_2[1];
}


/*******************************************************************************
函数：send_array_handle
功能：处理发送数组
输入参数：
	function：协议功能（具体查看通信协议）
	... ：根据不同的功能有不同的输入参数
				①0x01功能：第一个参数是电平信号，第二个参数是电压信号（float类型），都为指针输入
输出参数：
	*array：加了CRC、帧头、数据等的数组
返回：无
*******************************************************************************/
void send_array_handle(uint8_t *array ,uint8_t function_temporary ,...)
{
	/*
	例如
	if(function_temporary == 0x00){
		va_list ap;
		void* float_data = 0;
		uint8_t i = 0 ,j = 0;
		va_start(ap, function_temporary);
		*array 		 = 0x55;
		*(array + 1) =0xAA;
		*(array + 2) =0x0C;
		*(array + 3) =0x00;
		float_data = va_arg(ap ,void *);
		for(int i=0; i<4; i++)
    {
        *(array + 6 + i) = ((uint8_t *)float_data)[i];
    }
		i = *va_arg(ap ,uint8_t *);
		*(array + 4) = i;
		j = *va_arg(ap ,uint8_t *);
		*(array + 5) = j;
		*(array + 10) = 0xFF;
		*(array + 11) = 0xFF;
		*(array + 12) = 0xFF;
		va_end(ap);
		CRC_write(array);
	}
	*/

}


/*******************************************************************************
函数：function
功能：功能
输入参数：
	array：接收到的数组
输出参数：无
返回：无
*******************************************************************************/
void function(uint8_t *array)
{
	/*
	if(*(array + 3) == 0x01){
		//读取数据
		float foat_data = 0;
		com_choose(*(array + 4) ,*(array + 5));
		foat_data = ADS1256_Read_dat();

		//发送,foat_data
		uint8_t send_data[10] = {0};
		send_array_handle(send_data ,0x01 ,&foat_data);
		USB_send_array(send_data ,send_data[2] + 3);
	}
	*/
}


/*******************************************************************************
函数：data_analysis
功能：数据解析
输入参数：
	array：接收到的数组
输出参数：无
返回：无
*******************************************************************************/
void data_analysis(uint8_t *array)
{
		if(*array == 0x55 && *(array + 1) == 0xAA){
			if(CRC_test(array) == 1){
				function(array);
			}
			else
				clear_array(array ,USB_USART_REC_LEN);
		}
		else
			clear_array(array ,USB_USART_REC_LEN);
}


