#include "pwm.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "app_uSart.h"
#include "motor1.h"
#include "HDMI.H"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim4;
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);                                    
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
extern rt_uint8_t ch[14];
uint8_t sign_speed=0
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(htim->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspPostInit 0 */

  /* USER CODE END TIM4_MspPostInit 0 */
  
    /**TIM4 GPIO Configuration    
    PB8     ------> TIM4_CH3
    PB9     ------> TIM4_CH4 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM4_MspPostInit 1 */

  /* USER CODE END TIM4_MspPostInit 1 */
  }

}

static void MX_TIM4_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim4.Instance = TIM4;
  htim4.Init.Prescaler = GENERAL_TIM_PRESCALER ;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = GENERAL_TIM_PERIOD;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
			_Error_Handler(__FILE__, __LINE__);
  }

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 50;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim4);

}

/** Pinout Configuration
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

rt_uint16_t maxpwm=1950;
rt_uint16_t minpwm=1830;
rt_uint16_t DJ_Value=1950;
rt_uint16_t led_Value=1830;

static struct rt_thread  * PWM_Para_thread;
static void PWM_Para_entry(void *parameter)
{	
	while(1)
	{
		
			__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,DJ_Value);
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,led_Value);
		
		HDMI0_2("page0.t50.txt=",(GENERAL_TIM_PERIOD+1  -DJ_Value));
		HDMI0_2("page0.t51.txt=",(GENERAL_TIM_PERIOD+1 -led_Value));
		
			/**********舵机加判断********************/
		if(ch[1]==0x2a)
		{
			 
					DJ_Value+=5;
					if(DJ_Value>=maxpwm)
					{	
								DJ_Value=maxpwm;
						}
					ch[1]=0x00;
		}
		/**********舵机减判断********************/
			if(ch[1]==0x1a)
		{
			 
					DJ_Value-=5;
					if(DJ_Value<=minpwm)
					{	
								DJ_Value=minpwm;
						}
					ch[1]=0x00;
		}
		
		
			/**********舵机加判断********************/
		if(ch[1]==0x4a)
		{
			 
					led_Value+=5;
					if(led_Value>=maxpwm)
					{	
								led_Value=maxpwm;
						}
					ch[1]=0x00;
		}
		/**********舵机减判断********************/
			if(ch[1]==0x3a)
		{
			 
					led_Value-=5;
					if(led_Value<=minpwm)
					{	
								led_Value=minpwm;
						}
					ch[1]=0x00;
		}
		
			rt_thread_delay(5);
			}
			
			
	
}
void PWM_Init()
{
	
	
		 MX_GPIO_Init();
			MX_TIM4_Init();
		HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
		HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
		 PWM_Para_thread= rt_thread_create("PWMLED",//任务的名称
														PWM_Para_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														11,//优先级大小
														10);//时间片，在同一优先级时使用
		if(PWM_Para_thread!= RT_NULL)
		{
			
			rt_thread_startup(PWM_Para_thread);
			
		}
		

	
	
	
}


/* 如果设置了RT_SAMPLES_AUTORUN，则加入到初始化线程中自动运行 */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(PWM_Init);
#endif
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(PWM_Init, PWM_Init_Sample);



