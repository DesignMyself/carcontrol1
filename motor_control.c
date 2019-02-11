#include "pwm.h"
#include "motor1.h"
#include "HDMI.H"
#include  <stdlib.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
/* Private variables ---------------------------------------------------------*/
 motor motor1;
 motor motor2;
 uint16_t Line_speed=5;
 float speed=0.0;
 float Motor2_Cspeed=0;
 uint8_t sign_speed=1;
 extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim12;
extern float ADC1_value;
extern float ADC2_value;
extern float motor_speed_r_min;
extern 	rt_uint8_t Dis_Para[30];//串口3获得屏幕发过来的数据
extern Display D1;//用于main中的任务，把参数发送给屏幕
extern Display D2;//用于main中的任务，把参数发送给屏幕
PID	Motor1_PID;
PID Motor2_PID;
 

/********电机1PID参数初始化************/
void Motor1_PID_Init(){
	
	Motor1_PID.P_PID=12.0f;
	Motor1_PID.I_PID=2.0f;
	Motor1_PID.D_PID=0.0f;
	Motor1_PID.Last_Error=0.0f;
	Motor1_PID.Current_Error=0.0f;
	Motor1_PID.D_Error=0.0f;
	Motor1_PID.errsum=0.0f;
	Motor1_PID.Inc_PID=0;
	Motor1_PID.PID_OPEN=1;
	
	
}
/********电机2PID参数初始化************/
void Motor2_PID_Init(){
	
	Motor2_PID.P_PID=1.3f;
	Motor2_PID.I_PID=0.02f;
	Motor2_PID.D_PID=0.0f;
	Motor2_PID.Last_Error=0.0f;
	Motor2_PID.Current_Error=0.0f;
	Motor2_PID.errsum=0.0f;
	Motor2_PID.D_Error=0.0f;
	Motor2_PID.Inc_PID=0;
	Motor2_PID.changetime=20;
	Motor2_PID.PID_OPEN=0;
	
}

/********电机2参数初始化*************/
void motor2_Init()
{
	motor2.Ahead_Distance=0;
	motor2.Back_Distance=0;
	motor2.cw=0;
	motor2.pwm=0;
	motor2.state=0;//0——关闭电机;1——开启电机，手动模式;2——开启电机，自动前进一段距离模式;3——开启电机，自动后退一段距离;4——开启电机，先前进一段距离，再后退一段距离，
	motor2.Target_Speed=15;
	motor2.autorun_state=0;
	motor2.summotor_ahead=0;
	motor2.summotor_back=0;
	motor2.open=PB14;
	motor2.brake=PA10;
	motor2.ccw=PB13;
	motor2.state_record=0;
	motor2.direction=0;//0代表没有方向，按键没被按下，1向前，2向后，3两个按键都被按下
	motor2.pwm_nopid=150;
}/********电机1参数初始化*************/
void motor1_Init()
{
	motor1.Ahead_Distance=0;
	motor1.Back_Distance=0;
	motor1.Abs_Distance=0;
	motor1.cw=0;
	motor1.pwm=0;
	motor1.state=0;//0——关闭电机;1——开启电机，手动模式;2——开启电机，自动前进一段距离模式;3——开启电机，自动后退一段距离;4——开启电机，先前进一段距离，再后退一段距离，
	motor1.Target_Speed=2;
	motor1.autorun_state=0;
	motor1.summotor_ahead=0;
	motor1.summotor_back=0;
	motor1.sumotor_abs=0;
	motor1.open=PB0;//左电机
	motor1.brake=PB12;
	motor1.ccw=PB1;
	motor1.state_record=0;
	motor1.direction=0;//0代表没有方向，按键没被按下，1向左，2向右，3两个按键都被按下
	motor1.pwm_nopid=80;
	motor1.zf=0;
	motor1.prode_direction=0;
	
}
/**
  * 函数功能：增量式PID速度环计算
  * 输入参数：Get_Speed    由编码器得到的速度值 
  *           Target_Speed    目标值
  * 返 回 值：经过PID运算得到的增量值
  * 说    明：增量式 PID 速度环控制设计,计算得到的结果仍然是速度值
  */
float PID_Motor_Control(PID  *Motor_PID,float Get_Speed,float Target_Speed,float changetime)//第一个为PID的参数，第二个为测量到的速度，第三个为目标速度，第四个为变化的时间
{
	float IError=0,Increase_PWM=0;
	IError=Target_Speed-Get_Speed;//单位时间内误差
	Motor_PID->errsum=IError*changetime;//单位时间内误差累计
	Motor_PID->D_Error=(IError-Motor_PID->Last_Error)/changetime;//单位时间内误差变化率
	if((IError<0.5f)&&(IError>-0.5f))
	{
		
		IError=0;
//		Motor1_PID.Last_Error=0;
//		Motor1_PID.Pre_Error=0;
	}
	Increase_PWM=(Motor_PID->P_PID*IError)+(Motor_PID->I_PID*Motor_PID->errsum)+(Motor_PID->D_PID*Motor_PID->D_Error);
	Motor_PID->Last_Error=IError;
	return(Increase_PWM);
	
	
	
}
/****************
PC12 上开关
PC4  向上
PC13下开关
PC5向下
自复位键控制车行走*****************/
void key_scan()
{
	if(rt_pin_read(PC12)==0&&rt_pin_read(PC4)==1)//前
	{
		motor1.state=1;
		motor1.pwm=30;
		motor2.pwm=30;
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,motor1.pwm);
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,motor2.pwm);
		rt_pin_write(motor2.ccw,0);//右
				rt_pin_write(motor1.ccw,1);//右
		
	}
		if(rt_pin_read(PC4)==0&&rt_pin_read(PC12)==1)//后
	{
		motor1.state=1;
		motor1.pwm=100;
		motor2.pwm=100;
		
		rt_pin_write(motor2.ccw,1);//右
				rt_pin_write(motor1.ccw,0);//右
		
	}
			if(rt_pin_read(PC13)==0&&rt_pin_read(PC5)==1)//后
	{
		motor1.state=1;
		motor1.pwm=100;
		motor2.pwm=250;
		
		rt_pin_write(motor2.ccw,1);//右
				rt_pin_write(motor1.ccw,0);//右
		
	}
				if(rt_pin_read(PC13)==1&&rt_pin_read(PC5)==0)//后
	{
		motor1.state=1;
		motor1.pwm=250;
		motor2.pwm=100;
		
		rt_pin_write(motor2.ccw,1);//右
				rt_pin_write(motor1.ccw,0);//右
		
	}
//if(rt_pin_read(PC12)==0&&rt_pin_read(PC4)==0)//上行程开关响应，向下按钮没被按下
//	{
//		rt_thread_delay(50);
//		if(rt_pin_read(PC4)==0)
//		{
//					motor1.summotor_back=0;
//					motor1.state=2;
//		}
//		
//	}
	if(rt_pin_read(PC12)==1&&rt_pin_read(PC4)==1&&rt_pin_read(PC13)==1&&rt_pin_read(PC5)==1)//下行程开关响应，向上按钮没被按下
	{
		motor1.state=0;
		
	}


	
	

	}

/*************
* 函数功能：对X轴进行控制
  * 输入参数：
  * 返 回 值：
  * 说    明：
  ***************/

void motor1_control()
{
			/******自动向左输入的距离*********/

				//	Auto_Ahead_Distance_JP(&motor1,&D2,2);//逆时针
						
				/******自动向右输入的距离*********/
				//	Auto_Back_Distance_JP(&motor1,&D2,3);//顺时针
			if(motor1.state==1)//开启电机2
			{
				rt_pin_write(motor1.open,1);//左
				rt_pin_write(motor2.open,1);//右
				
	
				
			}

			if(motor1.state==0)//关闭电机2
			{
				rt_pin_write(motor1.open,0);//close motor 1
					rt_pin_write(motor2.open,0);//close motor 1
			}
			
		
			
			
	
}
static struct rt_thread  * PWM_Para_thread;
static struct rt_thread  * PID_thread;
static void PID_entry(void *parameter)
{
		
		
		
	motor1.pwm=100;
	motor2.pwm=100;
							
	while(1)
	{
						
				
						
						motor1_control();
			__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,motor1.pwm);
		__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,motor2.pwm);
					rt_thread_delay(10);
			}
	
}
static void Motor_Contorl_entry1(void *parameter)
{	
	
	
	while(1)
	{
			
			key_scan();
			rt_thread_delay(50);
		}
		
				
}
int PWM_Init()
{
	
	
		 MX_GPIO_Init();//初始化GPIO时钟
		MX_TIM5_Init();
		MX_TIM4_Init();
	
//		MX_TIM12_Init();//初始化PWM
//		Motor1_PID_Init();
//		Motor2_PID_Init();
//		motor2_Init();
		motor1_Init();
		motor2_Init();
		motor1.pwm=300;
//		motor2.pwm=150;
		HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_2);//启动PWM通道，右电机
		HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);//启动PWM通道，左电机
//		HAL_TIM_PWM_Start(&htim12,TIM_CHANNEL_2);//启动PWM通道
//		
		 PWM_Para_thread= rt_thread_create("PWMLED",//任务的名称
														Motor_Contorl_entry1,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														9,//优先级大小
														10);//时间片，在同一优先级时使用
		if(PWM_Para_thread!= RT_NULL)
		{
			
			rt_thread_startup(PWM_Para_thread);
			
		}
		
	PID_thread= rt_thread_create("PID",//任务的名称
														PID_entry,//任务的主程序入口
														RT_NULL,//任务的传入参数
														1024,//堆栈的大小
														8,//优先级大小
														10);//时间片，在同一优先级时使用
		if(PID_thread!= RT_NULL)
		{
			
			rt_thread_startup(PID_thread);
			
		}
		
		return 0;
	
	
	
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

/* 如果设置了RT_SAMPLES_AUTORUN，则加入到初始化线程中自动运行 */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(PWM_Init);
#endif
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(PWM_Init, PWM_Init_Sample);
