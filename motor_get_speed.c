#include "motor1.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "HDMI.H"
#include "app_uSart.h"
#include "pin_def.h"
#include "PWM.H"

TIM_HandleTypeDef htim2;//time2�ؼ���
TIM_HandleTypeDef htim8;//time8�ؼ���
TIM_HandleTypeDef htim3;//time2�ؼ���
extern motor motor1;
extern motor motor2;
extern Display D1;
extern PID	Motor1_PID;
extern PID Motor2_PID;

static void MX_TIM2_Init(void)
{
	
  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;//timƵ�ʷ�Ƶ������ΪAPB1 42M/(0+1)
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFB3;//��λ��,��������������һλ��λ��0
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_RISING ;
  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  sClockSourceConfig.ClockFilter = 0;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	 HAL_TIM_Base_Init(&htim2);	
}

static void MX_TIM8_Init(void)
{
	
    TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;//��Ƶ�������ڴ���ABPB2 ��84M
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 0xFB3;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_ETRF;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
  sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
  sSlaveConfig.TriggerFilter = 4;
  if (HAL_TIM_SlaveConfigSynchronization(&htim8, &sSlaveConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
		D1.message="TIM8 FAIL";
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
		D1.message="TIM8 FAIL2";
  }

	

	
}

static void MX_TIM3_Init(void)
{
	
   TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0xFB3;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_ETRF;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
  sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
  sSlaveConfig.TriggerFilter = 4;
  if (HAL_TIM_SlaveConfigSynchronization(&htim3, &sSlaveConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/*************TIME2 ��ʼ�����������**********************************/
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  
		rt_kprintf("page0.t5.txt=\"error\"");
		
  
  /* USER CODE END Error_Handler_Debug */ 
	
	
	
}


/*******************����ٶ����ݲɼ�����ʾ��������********************/

rt_uint16_t kk1=0;//���1�����ȡ����
rt_uint32_t SumMotor1=0;//���1
rt_uint16_t kk2=0;//���2�����ȡ����
rt_uint32_t SumMotor2=0;//���2
void motor1_Dis_Catch()
{
		kk1=__HAL_TIM_GET_COUNTER(&htim3);//�õ�TIME2����ETR�е�����
		kk2=__HAL_TIM_GET_COUNTER(&htim8);//�õ�TIM8����ETR�е�����
		motor1.summotor=motor1.summotor+kk1;
		motor2.summotor=motor2.summotor+kk2;
	
			__HAL_TIM_SET_COUNTER(&htim3,0);//���TIME8�Ĵ�������ֵ
		__HAL_TIM_SET_COUNTER(&htim8,0);//���TIME2�Ĵ�������ֵ

}

/*************������������һ��������ȡ���ݣ�һ��������ʾ����******************/
/* �̵߳�TCB���ƿ� */
static struct rt_thread *Motor1_thread;
static struct rt_thread *HMI2_thread;
rt_uint32_t motor1_pulse=0;
rt_uint32_t motor2_pulse=0;
	uint8_t PID_Speed2[16];
 JISUAN Motor2_Sf;//��������union��MOTOR2���ٶȷ��͸�����
JISUAN test_speed;
static void Motor1_entry(void *parameter)
{
	rt_thread_delay(1);
	while(1)
	{
		//rt_enter_critical();
		motor1_Dis_Catch();
		
			rt_thread_delay(5);
	}
	
}


/********************����ٶȲ�������*************/
static struct rt_thread  * motor1_speed_thread;

static void motor1_speed_entry(void *parameter)
{

	motor1.speed=0;
	motor2.speed=0;
	rt_thread_delay(1);
	while(1)
	{
		motor1.sumotor_abs=motor1.summotor-motor1.sumotor_abs;
		motor2.sumotor_abs=motor2.summotor-motor2.sumotor_abs;
		motor1.speed=motor1.sumotor_abs*1250*3.14159f/9.0f/139.0f;
		motor2.speed=motor2.sumotor_abs*1250*3.14159f/9.0f/139.0f;
		motor1.sumotor_abs=motor1.summotor;
		motor2.sumotor_abs=motor2.summotor;
		rt_thread_delay(20);//��PID��ʱ����ͬ
		
	}
	
	
}
/******************�����ʼ��*****************/
int  Motor1_Hmi_Init()
{
		MX_GPIO_Init();//��������ʱ��
		MX_TIM3_Init();	//	��ʼ��TIM3
		MX_TIM8_Init();//	��ʼ��TIM8
		HAL_TIM_Base_Start(&htim3);
		HAL_TIM_Base_Start(&htim8);
	__HAL_TIM_SET_COUNTER(&htim8,0);	
	__HAL_TIM_SET_COUNTER(&htim3,0);
	
		motor1.summotor=0;
		motor2.summotor=0;
	/********���1������ȡ������ƿ鶨��**************/
	Motor1_thread= rt_thread_create("Motor1",//���������
														Motor1_entry,//��������������
														RT_NULL,//����Ĵ������
														1024,//��ջ�Ĵ�С
														3,//���ȼ���С
														10);//ʱ��Ƭ����ͬһ���ȼ�ʱʹ��
		if(Motor1_thread!= RT_NULL)
		{
			
			rt_thread_startup(Motor1_thread);
			
		}

		motor1_speed_thread= rt_thread_create("MOTOR1Speed",//���������
														motor1_speed_entry,//��������������
														RT_NULL,//����Ĵ������
														1024,//��ջ�Ĵ�С
														5,//���ȼ���С
														10);//ʱ��Ƭ����ͬһ���ȼ�ʱʹ��
		if(motor1_speed_thread!= RT_NULL)
		{
			
			rt_thread_startup(motor1_speed_thread);
			
		}
		
		return 0;
}
/* ���������RT_SAMPLES_AUTORUN������뵽��ʼ���߳����Զ����� */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(Motor1_Hmi_Init);
#endif
/* ������ msh �����б��� */
MSH_CMD_EXPORT(Motor1_Hmi_Init, Enconder1_Hmi_Sample);
