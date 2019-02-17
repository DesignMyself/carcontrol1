#ifndef __MOTOR1_H__
#define	__MOTOR1_H__
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "app_uSart.h"
#include "pin_def.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "HDMI.H"
#include "PWM.H"

/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
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

// ���嶨ʱ��Ԥ��Ƶ����ʱ��ʵ��ʱ��Ƶ��Ϊ��84MHz/��ENCODER_TIMx_PRESCALER+1��
#define ENCODER_TIM_PRESCALER               0  // 

// ʹ��32bits �ļ�������Ϊ����������,F4ϵ�е�TIM2,TIM5
//// ���嶨ʱ�����ڣ�����ʱ����ʼ������ENCODER_TIMx_PERIODֵ�Ǹ��¶�ʱ�������ɶ�Ӧ�¼����ж�
//#define ENCODER_TIM_PERIOD                  0xFFFFFFFF
//#define CNT_MAX                             4294967296
// ʹ��16bits �ļ�������Ϊ����������,F4ϵ�е�TIM3,TIM4
#define ENCODER_TIM_PERIOD                0xFFFF
#define CNT_MAX                           65535

#define ENCODER_LINE                        50
#if (TIM_ENCODERMODE_TIx==TIM_ENCODERMODE_TI12)
  #define ENCODER_RESOLUTION                (4*ENCODER_LINE)//4��Ƶ,ͬʱʹ��CH1,CH2
#else 
  #if ((TIM_ENCODERMODE_TIx==TIM_ENCODERMODE_TI2)||(TIM_ENCODERMODE_TIx==TIM_ENCODERMODE_TI1))
    #define ENCODER_RESOLUTION              (2*ENCODER_LINE)//2��Ƶ,ֻʹ��CH1����CH2
  #else 
    #error " NOT Init @TIM_Encoder:Parameter error"
  #endif
#endif
/* ��չ���� ------------------------------------------------------------------*/
extern TIM_HandleTypeDef htimx_Encoder;

typedef struct
{
  int16_t pwm;     
                                 
  uint8_t cw;  //����0Ϊ����1Ϊ��
                              
  float  speed;  
                                 
  float Ahead_Distance; 
	float Back_Distance;
	float Abs_Distance;
	uint8_t  state;//0Ϊ�أ�1Ϊ��
	uint8_t autorun_state;
	uint8_t sign;//�ָ���ǰ״̬
	uint16_t Target_Speed;
	float Target_Angle;//Ŀ��Ƕ�
  uint32_t summotor_ahead;  
	int32_t sumotor_abs; 
	int32_t summotor;
	float Angle;
	uint8_t open;//��������
	uint8_t brake;//ɲ������
	uint8_t ccw;//ת������
	uint8_t state_record;//��¼���˶�״̬
	uint8_t direction;//��¼����
	uint8_t restrain;//��¼�г�����
	uint16_t pwm_nopid;
	uint8_t zf;//�����жϱ�־
	uint16_t prode_direction;
}motor;

typedef struct{
		
		uint8_t i;//������¼����˶��ٸ�����
		uint8_t data[20];//��¼���ݻ�����
		
	}Usart_get;
	
typedef	union{
		float f;
		uint8_t  x[4];
		uint32_t s;
		
	}JISUAN;
extern int32_t OverflowCount ;//��ʱ���������
void _Error_Handler(char * file, int line);
uint8_t Auto_Ahead_Distance_JP(motor* motor_num,Display *D_num,uint8_t state_input);//�Զ�����API,����������������������״̬��¼,״̬��״̬��ʾ��
uint8_t Auto_Back_Distance_JP(motor *motor_num,Display *D_num,uint8_t state_input);
//void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base);
//void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base);
/* �������� ------------------------------------------------------------------*/
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
