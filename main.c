/*File      : main.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *	float PWM;
	float P;
	float I;
	float Motor1_Speed;
	float Input_Ahead_Distance;
	float Input_Back_Distance;
	float Input_Right_Distance;
	float Input_left_Distance;
	float Input_Motor1_Speed;
	float Input_Motor2_Speed;
 * Change Logs:
 * Date           Author       Notes
 *2017-12-15      DQL          the first version 
 */

#include "app_uSart.h"
#include "pin_def.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "HDMI.H"
#include "PWM.H"
#include "motor1.h"
#include  "ADC.H"
extern PID Motor2_PID;//�����������жϵ��2�Ƿ�ʹ��PIDģʽ
Display D1;//��¼С������Ϣ
Display D2;//��¼X�����Ϣ
	rt_thread_t tid;
	rt_thread_t tid1;
	extern motor motor1;
 extern motor motor2;
	float ADC1_value=0.0;
	float ADC2_value=0.0;
	extern TIM_HandleTypeDef htim3;;
extern	uint16_t PB0_Value;
extern uint16_t Line_speed;//���ڼ�¼ֱ�������ٶ�
extern uint8_t sign_speed;
extern  Auto_Run Motor1;
extern Angle X;
extern	Angle Y;
extern	Angle Z;
void rt_hw_led_init(void)
{
   //	#define 	PB1			27
//#define 	PB13    34
//#define 	PA11    44
//#define 	PC9 		40
//#define 	PB12    33
//#define 	PA10    43
//#define 	PB0 		26
//#define 	PB14    35
//#define 	PA12    45
//#define 	PC8     39
		rt_pin_mode(PB1,PIN_MODE_OUTPUT);//���ڻ���
		rt_pin_mode(PB13,PIN_MODE_OUTPUT);//���ڻ���
		rt_pin_mode(PA11,PIN_MODE_OUTPUT);//���ڻ���
		rt_pin_mode(PC9,PIN_MODE_OUTPUT);//���ڻ���
		rt_pin_mode(PB12,PIN_MODE_OUTPUT);//����ɲ��
		rt_pin_mode(PA10,PIN_MODE_OUTPUT);//����ɲ��
		rt_pin_mode(PB0,PIN_MODE_OUTPUT);//������motor1
		rt_pin_mode(PB14,PIN_MODE_OUTPUT);//�ҵ����motor2
		rt_pin_mode(PA12,PIN_MODE_OUTPUT);//���ڵ������
		rt_pin_mode(PC8,PIN_MODE_OUTPUT);//���ڵ������
	
//	#define 	PC4    	24
//#define 	PC5    	25
//#define  	PC12  	 53
//#define  	PC13  	 2
//#define 	PB3     55
//#define 	PB4     56
//#define 	PB5     57
		rt_pin_mode(PC5,PIN_MODE_INPUT_PULLUP);//��������
		rt_pin_mode(PC4,PIN_MODE_INPUT_PULLUP);//��������
		rt_pin_mode(PC12,PIN_MODE_INPUT_PULLUP);//С����ǰ
		rt_pin_mode(PC13,PIN_MODE_INPUT_PULLUP);//С�����
		rt_pin_mode(PB3,PIN_MODE_INPUT_PULLUP);//���ᵽ���Ҽ���
		rt_pin_mode(PB4,PIN_MODE_INPUT_PULLUP);//�Զ�����ʶ��
		rt_pin_mode(PB5,PIN_MODE_INPUT_PULLUP);//���ᵽ������
//		rt_pin_mode(PB8,PIN_MODE_OUTPUT);
//		rt_pin_mode(PB9,PIN_MODE_OUTPUT);
		rt_pin_write(PB0,0);
		rt_pin_write(PB14,0);
		rt_pin_write(PA12,0);
		rt_pin_write(PC8,0);//�����е����ʼ���ڹر�״̬
		rt_pin_write(motor2.ccw,0);
		rt_pin_write(motor1.ccw,0);//motor1 cw ahead
	}
void Display_Init()
{
	D1.P=0.0F;
	D1.I=0.0F;
	D1. Motor1_Speed=0.0F;
	D1.PWM=0.0F;
	D1.Input_Ahead_Distance=0.0F;
	D1.Input_Back_Distance=0.0F;
	D1.Input_left_Distance=0.0F;
	D1.Input_Motor1_Speed=0.0F;
	D1.Input_Motor2_Speed=0.0F;
	D1.Input_Right_Distance=0.0F;
	D1.Motor2_Speed=0.0F;
	D1.ADC2_Val=0.0F;
	D1.message="��ӭʹ��";
}

void Display2_Init()
{
	D2.P=0.0F;
	D2.I=0.0F;
	D2.Motor1_Speed=0.0F;
	D2.PWM=0.0F;
	D2.Input_Ahead_Distance=3.0F;
	D2.Input_Back_Distance=3.0F;
	D2.Input_left_Distance=0.0F;
	D2.Input_Motor1_Speed=0.0F;
	D2.Input_Motor2_Speed=0.0F;
	D2.Input_Right_Distance=0.0F;
	D2.Motor2_Speed=0.0F;
	D2.ADC2_Val=0.0F;
	D2.message="��ӭʹ��";
}
rt_uint8_t ch[14];
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
/*************�����������������ݷ��͸���ʴ�����******************/
void ADC_thread_entry(void* parameter)
{    
  
		 while (1)
	{   
			
			SetProgress_mess_value(0,11,"",motor1.speed,3);
			SetProgress_mess_value(0,6,"",motor2.speed,3);
			SetProgress_mess_value(0,1,"Y IS:",Y.Angle,3);
			SetProgress_mess_value(0,31,"Z IS:",Z.Angle,3);
			
			rt_thread_delay(30);

	}
}

/***************����Զ�����״̬��ͣ����*******/
extern rt_uint8_t signM1;
extern rt_uint8_t signM2;

//void motor_autorun_entry(void *parameter)
//{
//	while(1)
//		
//	{
//		
//		
//		 rt_thread_delay(50);
//			
//	

//	}
//}


int main(void)
{
     
			Display_Init();
			Display2_Init();
      rt_hw_led_init(); 
			
    /* ����test�߳� */
    tid = rt_thread_create("test",
                    ADC_thread_entry, 
                    RT_NULL,
                    1024, 
										10, 
                    10);
	
//		tid1=rt_thread_create("test2",
//													motor_autorun_entry,
//													RT_NULL,
//													1024,
//													12,
//													10);
//    /* �����ɹ��������߳� */
    if (tid != RT_NULL)
        rt_thread_startup(tid);
//    if (tid1!= RT_NULL)
//        rt_thread_startup(tid1);    
    return 0;
}
