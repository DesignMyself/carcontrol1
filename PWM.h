
#ifndef __pwm_h__
#define __pwm_h__

#include "pin_def.h"
#include "stm32f4xx_hal.h"
#include <rthw.h>
#include <rtthread.h>


/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define GENERAL_TIMx                        TIM3
#define GENERAL_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM3_CLK_ENABLE()
#define GENERAL_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM3_CLK_DISABLE()
#define GENERAL_TIM_GPIO_RCC_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define GENERAL_TIM_CH1_PORT                GPIOB
#define GENERAL_TIM_CH1_PIN                 PB0
#define DJPWM_TIM_CH1_PIN                	 	PB1

/****************************����IO��****************/
 void MX_TIM4_Init(void);

/* TIM3 init function */
 void MX_TIM3_Init(void);

/* TIM5 init function */
 void MX_TIM5_Init(void);

void MX_TIM12_Init(void);

void MX_GPIO_Init(void);

typedef struct
{
	uint8_t PID_OPEN;
	float Current_Error;
	float Last_Error;
	float D_Error;
	float Inc_PID;
	float P_PID;
	float I_PID;
	float D_PID;
	float speed;//Ŀ���ٶ�
	float errsum;
	uint16_t changetime;
}PID;
// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��84MHz/��GENERAL_TIMx_PRESCALER+1��
#define GENERAL_TIM_PRESCALER               6 // ʵ��ʱ��Ƶ��Ϊ��100KHz

// ���嶨ʱ�����ڣ�����ʱ����ʼ������GENERAL_TIMx_PERIODֵ�Ǹ��¶�ʱ�������ɶ�Ӧ�¼����ж�
#define GENERAL_TIM_PERIOD                 499// ��ʱ�������ж�Ƶ��Ϊ��100KHz/2000=50Hz

/* ��չ���� ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htimx;

/* �������� ------------------------------------------------------------------*/
void GENERAL_TIMx_Init(void);
/* ����궨�� --------------------------------------------------------------------*/


#endif
