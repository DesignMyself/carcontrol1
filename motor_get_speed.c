#include "motor1.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "HDMI.H"
#include "app_uSart.h"
#include "pin_def.h"
#include "PWM.H"

TIM_HandleTypeDef htim2;//time2控件块
TIM_HandleTypeDef htim8;//time8控件块
TIM_HandleTypeDef htim3;//time2控件块
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
  htim2.Init.Prescaler = 0;//tim频率分频，总线为APB1 42M/(0+1)
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xFB3;//复位数,到计数器到达这一位后复位到0
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
  htim8.Init.Prescaler = 0;//分频数，由于处于ABPB2 ，84M
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

/*************TIME2 初始化错误处理代码**********************************/
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


/*******************电机速度数据采集和显示处理任务********************/

rt_uint16_t kk1=0;//电机1脉冲获取参数
rt_uint32_t SumMotor1=0;//电机1
rt_uint16_t kk2=0;//电机2脉冲获取参数
rt_uint32_t SumMotor2=0;//电机2
void motor1_Dis_Catch()
{
		kk1=__HAL_TIM_GET_COUNTER(&htim3);//得到TIME2——ETR中的脉冲
		kk2=__HAL_TIM_GET_COUNTER(&htim8);//得到TIM8——ETR中的脉冲
		motor1.summotor=motor1.summotor+kk1;
		motor2.summotor=motor2.summotor+kk2;
	
			__HAL_TIM_SET_COUNTER(&htim3,0);//清空TIME8寄存器的数值
		__HAL_TIM_SET_COUNTER(&htim8,0);//清空TIME2寄存器的数值

}

/*************建立两个任务，一个用来收取数据，一个用来显示数据******************/
/* 线程的TCB控制块 */
static struct rt_thread *Motor1_thread;
static struct rt_thread *HMI2_thread;
rt_uint32_t motor1_pulse=0;
rt_uint32_t motor2_pulse=0;
	uint8_t PID_Speed2[16];
 JISUAN Motor2_Sf;//用联合体union把MOTOR2的速度发送给电脑
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


/********************电机速度测试任务*************/
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
		rt_thread_delay(20);//与PID的时间相同
		
	}
	
	
}
/******************任务初始化*****************/
int  Motor1_Hmi_Init()
{
		MX_GPIO_Init();//开启引脚时钟
		MX_TIM3_Init();	//	初始化TIM3
		MX_TIM8_Init();//	初始化TIM8
		HAL_TIM_Base_Start(&htim3);
		HAL_TIM_Base_Start(&htim8);
	__HAL_TIM_SET_COUNTER(&htim8,0);	
	__HAL_TIM_SET_COUNTER(&htim3,0);
	
		motor1.summotor=0;
		motor2.summotor=0;
	/********电机1参数获取任务控制块定义**************/
	Motor1_thread= rt_thread_create("Motor1",//任务的名称
														Motor1_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														3,//优先级大小
														10);//时间片，在同一优先级时使用
		if(Motor1_thread!= RT_NULL)
		{
			
			rt_thread_startup(Motor1_thread);
			
		}

		motor1_speed_thread= rt_thread_create("MOTOR1Speed",//任务的名称
														motor1_speed_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														5,//优先级大小
														10);//时间片，在同一优先级时使用
		if(motor1_speed_thread!= RT_NULL)
		{
			
			rt_thread_startup(motor1_speed_thread);
			
		}
		
		return 0;
}
/* 如果设置了RT_SAMPLES_AUTORUN，则加入到初始化线程中自动运行 */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(Motor1_Hmi_Init);
#endif
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Motor1_Hmi_Init, Enconder1_Hmi_Sample);
