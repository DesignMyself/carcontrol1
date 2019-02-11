
#ifndef __pwm_h__
#define __pwm_h__

#include "pin_def.h"
#include "stm32f4xx_hal.h"
#include <rthw.h>
#include <rtthread.h>


/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define GENERAL_TIMx                        TIM3
#define GENERAL_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM3_CLK_ENABLE()
#define GENERAL_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM3_CLK_DISABLE()
#define GENERAL_TIM_GPIO_RCC_CLK_ENABLE()   __HAL_RCC_GPIOB_CLK_ENABLE()
#define GENERAL_TIM_CH1_PORT                GPIOB
#define GENERAL_TIM_CH1_PIN                 PB0
#define DJPWM_TIM_CH1_PIN                	 	PB1

/****************************配置IO口****************/
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
	float speed;//目标速度
	float errsum;
	uint16_t changetime;
}PID;
// 定义定时器预分频，定时器实际时钟频率为：84MHz/（GENERAL_TIMx_PRESCALER+1）
#define GENERAL_TIM_PRESCALER               6 // 实际时钟频率为：100KHz

// 定义定时器周期，当定时器开始计数到GENERAL_TIMx_PERIOD值是更新定时器并生成对应事件和中断
#define GENERAL_TIM_PERIOD                 499// 定时器产生中断频率为：100KHz/2000=50Hz

/* 扩展变量 ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htimx;

/* 函数声明 ------------------------------------------------------------------*/
void GENERAL_TIMx_Init(void);
/* 舵机宏定义 --------------------------------------------------------------------*/


#endif
