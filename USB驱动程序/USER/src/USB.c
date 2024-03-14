/************************************************
	��΢���ܿƼ������ݣ����޹�˾
	������USB����λ��ͨ������ļ�
	���ߣ�����ΰ
************************************************/


/************************************************
	ͷ�ļ�
************************************************/
#include "USB.h"




/************************************************
	ȫ�ֱ���
************************************************/
uint8_t i = 1 ,j = 1;					

uint8_t H_nmb_max = 0,H_nmb_min = 96;
uint8_t L_nmb_max = 0,L_nmb_min = 96;

float AD_max = 0,AD_min = 2.5;


/************************************************
	��������
************************************************/




/************************************************
	����
************************************************/
/*******************************************************************************
������USB_init
���ܣ�USB��ʼ��
�����������
�����������
���أ���
*******************************************************************************/
void USB_init(void)
{
	delay_ms(1800);
	USB_Port_Set(0); 	//USB�ȶϿ�
	delay_ms(700);
	USB_Port_Set(1);	//USB�ٴ�����
 	Set_USBClock();   
 	USB_Interrupts_Config();    
 	USB_Init();
}


/*******************************************************************************
������USB_send_array
���ܣ�USB��������
���������
	array����Ҫ���͵�����
	length�����鳤��
�����������
���أ���
*******************************************************************************/
void USB_send_array(uint8_t* array,uint8_t length)
{
	for(int i=0 ;i<length ;i++){
		USB_USART_SendData(*array);
		array++;
	}
}

/*******************************************************************************
������clear_array
���ܣ��������
���������
	clear_data����Ҫ���������
	clear_count�����鳤��
���������
	��պ��clear_data����
���أ���
*******************************************************************************/
void clear_array(uint8_t *clear_data ,uint8_t clear_count)
{
	for(int i = 0 ;i < clear_count ;i++){
		clear_data[i] = 0;
	}
}


/*******************************************************************************
������GetCrc
���ܣ�CRCУ��
���������
	pucBuf������
	len�����ݳ���
�����������
���أ�16λCRC
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
������CRC_test
���ܣ��Խ��յ��������CRCУ��
���������
	array������
�����������
���أ���uint8_t����ȷ����1�����󷵻�0
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
������CRC_write
���ܣ��Է��͵�����д��CRC
���������
	array������
���������
	array������CRC������
���أ���
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
������send_array_handle
���ܣ�����������
���������
	function��Э�鹦�ܣ�����鿴ͨ��Э�飩
	... �����ݲ�ͬ�Ĺ����в�ͬ���������
				��0x01���ܣ���һ�������ǵ�ƽ�źţ��ڶ��������ǵ�ѹ�źţ�float���ͣ�����Ϊָ������
���������
	*array������CRC��֡ͷ�����ݵ�����
���أ���
*******************************************************************************/
void send_array_handle(uint8_t *array ,uint8_t function_temporary ,...)
{
	if(function_temporary == 0x00){
		va_list ap;
		void* float_data = 0;
		uint8_t i = 0 ,j = 0;
		va_start(ap, function_temporary);
		*array 			 = 0x55;
		*(array + 1) =0xAA;
		*(array + 2) =0x09;
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
		
		va_end(ap);
		CRC_write(array);
	}
	else if(function_temporary == 0x01){
		va_list ap;
		void* float_data = 0;
		va_start(ap, function_temporary);
		*array 			 = 0x55;
		*(array + 1) =0xAA;
		*(array + 2) =0x07;
		*(array + 3) =0x01;
		float_data = va_arg(ap ,void *);
    for(int i=0; i<4; i++)
    {
        *(array + 4 + i) = ((uint8_t *)float_data)[i];
    }
		va_end(ap);
		CRC_write(array);
	}
}


/*******************************************************************************
������function
���ܣ�����
���������
	array�����յ�������
�����������
���أ���
*******************************************************************************/
void function(uint8_t *array)
{
	if(*(array + 3) == 0x01){
		//��ȡ����
		float foat_data = 0;
		com_choose(*(array + 4) ,*(array + 5));
		foat_data = ADS1256_Read_dat();

		//����,foat_data
		uint8_t send_data[10] = {0};
		send_array_handle(send_data ,0x01 ,&foat_data);
		USB_send_array(send_data ,send_data[2] + 3);
	}
}


/*******************************************************************************
������data_analysis
���ܣ����ݽ���
���������
	array�����յ�������
�����������
���أ���
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


/*******************************************************************************
������updata
���ܣ�����λ����������
�����������
�����������
���أ���
*******************************************************************************/
void updata(void)
{
	float foat_data = 0;
	
	if(j == LIE && i == HANG + 1){
		i = 1;
		j = 1;
		IWDG_Feed();
	}
	else if(i == HANG + 1){
		j++;
		i = 1;
	}
	if(H_nmb_max < i)
		H_nmb_max = i;
	if(H_nmb_min > i)
		H_nmb_min = i;
	if(L_nmb_max < i)
		L_nmb_max = i;
	if(L_nmb_min > i)
		L_nmb_min = i;
	com_choose(i ,j);
	foat_data = ADS1256_Read_dat();
	if(AD_max < foat_data)
		AD_max = foat_data;
	if(AD_min > foat_data)
		AD_min = foat_data;
	//����,foat_data
	uint8_t send_data[12] = {0};
	send_array_handle(send_data ,0x00 ,&foat_data ,&i ,&j);
	USB_send_array(send_data ,send_data[2] + 3);
	i++;
	
}

