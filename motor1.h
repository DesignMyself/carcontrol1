#ifndef __MOTOR1_H__
#define	__MOTOR1_H__
/* 包含头文件 ----------------------------------------------------------------*/
#include "app_uSart.h"
#include "pin_def.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "HDMI.H"
#include "PWM.H"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define MOTOR1_TIMx                        TIM2
#define MOTOR1_TIM_RCC_CLK_ENABLE()        __HAL_RCC_TIM2_CLK_ENABLE()
#define ENCODER_TIM_RCC_CLK_DISABLE()       __HAL_RCC_TIM8_CLK_DISABLE()

#define ENCODER_TIM_GPIO_CLK_ENABLE()       {__HAL_RCC_GPIOC_CLK_ENABLE();__HAL_RCC_GPIOC_CLK_ENABLE();}
#define ENCODER_TIM_CH1_PIN                 GPIO_PIN_6
#define ENCODER_TIM_CH1_GPIO                GPIOC
#define ENCODER_TIM_CH2_PIN                 GPIO_PIN_7
#define ENCODER_TIM_CH2_GPIO                GPIOC

#define ENCODER_TIM_GPIO_AF                 GPIO_AF3_TIM8 

#define TIM_ENCODERMODE_TIx                 TIM_ENCODERMODE_TI12

#define ENCODER_TIM_IRQn                    TIM8_TRG_COM_TIM14_IRQn
#define ENCODER_TIM_IRQHANDLER              TIM8_IRQHandler

// 定义定时器预分频，定时器实际时钟频率为：84MHz/（ENCODER_TIMx_PRESCALER+1）
#define ENCODER_TIM_PRESCALER               0  // 

// 使用32bits 的计数器作为编码器计数,F4系列的TIM2,TIM5
//// 定义定时器周期，当定时器开始计数到ENCODER_TIMx_PERIOD值是更新定时器并生成对应事件和中断
//#define ENCODER_TIM_PERIOD                  0xFFFFFFFF
//#define CNT_MAX                             4294967296
// 使用16bits 的计数器作为编码器计数,F4系列的TIM3,TIM4
#define ENCODER_TIM_PERIOD                0xFFFF
#define CNT_MAX                           65535

#define ENCODER_LINE                        50
#if (TIM_ENCODERMODE_TIx==TIM_ENCODERMODE_TI12)
  #define ENCODER_RESOLUTION                (4*ENCODER_LINE)//4倍频,同时使用CH1,CH2
#else 
  #if ((TIM_ENCODERMODE_TIx==TIM_ENCODERMODE_TI2)||(TIM_ENCODERMODE_TIx==TIM_ENCODERMODE_TI1))
    #define ENCODER_RESOLUTION              (2*ENCODER_LINE)//2倍频,只使用CH1或者CH2
  #else 
    #error " NOT Init @TIM_Encoder:Parameter error"
  #endif
#endif
/* 扩展变量 ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htimx_Encoder;

typedef struct
{
  int16_t pwm;     
                                 
  uint8_t cw;  //方向，0为反，1为正
                              
  float  speed;  
                                 
  float Ahead_Distance; 
	float Back_Distance;
	float Abs_Distance;
	uint8_t  state;//0为关，1为开
	uint8_t autorun_state;
	uint8_t sign;//恢复当前状态
	uint16_t Target_Speed;
	float Target_Angle;//目标角度
  uint32_t summotor_ahead;  
	int32_t sumotor_abs; 
	int32_t summotor;
	float Angle;
	uint8_t open;//开启引脚
	uint8_t brake;//刹车引脚
	uint8_t ccw;//转变引脚
	uint8_t state_record;//记录下运动状态
	uint8_t direction;//记录方向
	uint8_t restrain;//记录行程限制
	uint16_t pwm_nopid;
	uint8_t zf;//正负判断标志
	uint16_t prode_direction;
}motor;

typedef struct{
		
		uint8_t i;//用来记录获得了多少个数据
		uint8_t data[20];//记录数据缓存区
		
	}Usart_get;
	
typedef	union{
		float f;
		uint8_t  x[4];
		uint32_t s;
		
	}JISUAN;
extern int32_t OverflowCount ;//定时器溢出次数
void _Error_Handler(char * file, int line);
uint8_t Auto_Ahead_Distance_JP(motor* motor_num,Display *D_num,uint8_t state_input);//自动运行API,电机参数，输入输出参数，状态记录,状态，状态显示）
uint8_t Auto_Back_Distance_JP(motor *motor_num,Display *D_num,uint8_t state_input);
//void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base);
//void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base);
/* 函数声明 ------------------------------------------------------------------*/
void ENCODER_TIMx_Init(void);

#endif	/* __ENCODER_TIM_H__ */




#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
	 
#ifdef __cplusplus
}
#endif

/**
  * @}
  */ 

/**
  * @}
*/ 
