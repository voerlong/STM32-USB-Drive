/************************************************
	柔微智能科技（苏州）有限公司
	描述：USB.c的头文件
	作者：付佳伟
************************************************/
#ifndef USB_H
#define USB_H


/************************************************
	头文件
************************************************/
#include "usb_lib.h"
#include "delay.h"
#include "SWITCH.h"
#include "AD.h"
#include "wdg.h"
#include <stdio.h>
#include <stdarg.h>


/************************************************
	宏定义
************************************************/
#define HANG  5
#define LIE 5

/************************************************
	外部调用函数声明
************************************************/
void USB_init(void);
void USB_send_array(uint8_t* array,uint8_t length);
void data_analysis(uint8_t *array);
void clear_array(uint8_t *clear_data ,uint8_t clear_count);
void updata(void);
void send_array_handle(uint8_t *array ,uint8_t function_temporary ,...);


/************************************************
	外部调用变量
************************************************/
extern uint8_t H_nmb_max ,H_nmb_min;
extern uint8_t L_nmb_max ,L_nmb_min;

extern float AD_max ,AD_min;


#endif

