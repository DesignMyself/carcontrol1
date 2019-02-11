//////////////////////////////**********************************************
/*从串口2中获得电脑发过来的数据（PID)参数,PID_Para_thread_entry(void* parameter),任务等级15*/
/*把获得的串口参数PID_Para[16]转换为PID中的P,I,D值，并赋值给MOTOR2的PID结构体列表*/
/**二、利用串口3获得大彩屏幕发过来的数据(输入的数字大小），大彩屏幕规律
		EE B1 11 00 00 00 09 11 00 FF FC FF FF 
		前六位为开始验证码，09表示屏幕中第九个框图键盘发出来的参数，11表面在第一个screen中发过来的参数，后五位是验证码
		EE B1 11 00 00 00 09 11 32 33 2E 36 35 00 FF FC FF FF 
		从11到00之间的数为发过来的参数的ASCII
三、大彩屏发送命令到控制板
			EE B1 A1 A1 A1 A1 A1 11 00 FF FC FF FF;//自动前进，此时必须是处于自动模式下才可以，当自动模式断开，就不再自动前进
			EE B1 A2 A2 A1 A1 A1 11 00 FF FC FF FF;//自动前进，此时必须是处于自动模式下才可以，当自动模式断开，就不再自动前进
			EE B1 C1 C1 C1 C1 C1 C1 C1 11 0F FF FF ;//停止运行
			EE B1 C4 C4 C4 C4 C4 C4 C4 11 0F FF FF ;//停止运行
			EE B1 C3 C3 C3 C3 C3 C3 C3 11 0F FF FF ;//打开PID
			EE B1 C2 C2 C2 C2 C2 C2 C2 11 0F FF FF ;//关闭PID
			EE B1 C5 C5 C5 C5 C5 C5 C5 11 0F FF FF ;//向前加速
			EE B1 C6 C6 C6 C6 C6 C6 C6 11 0F FF FF ;//向前减速
			EE B1 C7 C7 C7 C7 C7 C7 C7 11 0F FF FF ;//X轴加速
			EE B1 C8 C8 C8 C8 C8 C8 C8 11 0F FF FF ;//X轴减速
			
**/
/////////////////////////////////


#include "app_uSart.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "HDMI.H"
#include "motor1.h"
#include "PWM.H"
	
	extern PID Motor1_PID;//电机1PID参数初始化
	extern PID Motor2_PID;//电机2PID参数初始化
	extern Display D1;
	extern Display D2;
	extern motor motor2;
	extern  motor motor1;
	rt_uint8_t PID_Para[16];//串口2获得的PID参数
	rt_uint8_t Dis_Para[30];//串口3获得屏幕发过来的数据
	rt_uint8_t Dis_Para_Copy[30];//复制串口3获得的数据
	Auto_Run Motor1;
	Angle X;
	Angle Y;
	Angle Z;
	
	void Angle_Get(void *parameter)
	{
		uint8_t i;
		uint8_t get_data[6];
		while(1)
		{
				for(i=0;i<6;i++)
				{
					get_data[i]=uart1_getchar();
					
					if(get_data[0]!=0xBB)
						break;
					if(i==5)
					{
						
						if(get_data[1]==0xF1)
						{
							X.data[0]=get_data[2];
							X.data[1]=get_data[3];
							X.data[2]=get_data[4];
							X.data[3]=get_data[5];
						}
						if(get_data[1]==0xF2)
						{
							Y.data[0]=get_data[2];
							Y.data[1]=get_data[3];
							Y.data[2]=get_data[4];
							Y.data[3]=get_data[5];
						}
						if(get_data[1]==0xF3)
						{
							Z.data[0]=get_data[2];
							Z.data[1]=get_data[3];
							Z.data[2]=get_data[4];
							Z.data[3]=get_data[5];
						}
						
					}
					
					
				}
			}
		
		
	}
//	JISUAN Motor2_Sf;
//	void PID_Para_thread_entry(void* parameter)
//{    
//   
//		JISUAN P;
//		JISUAN I;
//		JISUAN D;
//		rt_uint8_t i=0;			
//		 while (1)
//	{   
//		

//		for(i=0;i<16;i++)
//		{
//					PID_Para[i]=uart2_getchar();//在里面已经包含有调度函数，当没有数据输入时会停留在这一步，时间到后调度到其他任务
//					if(PID_Para[0]!=0xAA)
//					{
//						i=0;
//						break;//一定要使用break，不然循环逻辑会出现问题
//					}
//				
//		}
//		if(PID_Para[1]==0x07)
//		{
//			P.x[0]=PID_Para[2];
//			P.x[1]=PID_Para[3];
//			P.x[2]=PID_Para[4];
//			P.x[3]=PID_Para[5];
//			Motor2_PID.P_PID=P.f;
//			I.x[0]=PID_Para[6];
//			I.x[1]=PID_Para[7];
//			I.x[2]=PID_Para[8];
//			I.x[3]=PID_Para[9];
//			Motor2_PID.I_PID=I.f;
//			D.x[0]=PID_Para[10];
//			D.x[1]=PID_Para[11];
//			D.x[2]=PID_Para[12];
//			D.x[3]=PID_Para[13];
//			Motor2_PID.D_PID=D.f;
//			PID_Para[1]=0xff;
//			
//		}
//		rt_thread_delay(30);	
//		
//	}
//}

	void Usart3_Parameter_Control(void* parameter)
{    
    rt_uint8_t i;
		uint8_t b;
		uint8_t ck;
		uart3_open("uart3");	
			
		 while (1)
	{   
			

		
				Dis_Para[i]=uart3_getchar();//在里面已经包含有调度函数，当没有数据输入时会停留在这一步，时间到后调度到其他任务
			
				i++;
		
					if(Dis_Para[i-1]==0xff|i>30|Dis_Para[0]!=0xEE)
					{
						b=i-1;
						i=0;
						
					}
				
					if(Dis_Para[0]==0xee&&Dis_Para[5]==0xC5)//判断是否接收已经结束
					{
									
									motor1.state=1;
									motor1.pwm=100;
									motor2.pwm=200;

					}
					
					if(Dis_Para[0]==0xee&&Dis_Para[5]==0xC6)//判断是否接收已经结束
					{
									
									motor1.state=1;
									motor2.pwm=100;
									motor1.pwm=200;

					}
						if(Dis_Para[0]==0xee&&Dis_Para[5]==0xF1)//判断是否接收已经结束
					{
									
									motor1.state_record=1;

					}
					if(Dis_Para[0]==0xee&&Dis_Para[5]==0xc1)//判断是否接收已经结束
					{
									
									motor1.state_record=0;

					}
	}
}
/*******任务初始化********/
int PID_Test_Init(void)
{
   rt_thread_t PID_Get;//获取PID参数设置的任务
	rt_thread_t PID_Send;//往电脑中发送速度信息
		uart1_open("uart1");
		
				
	
			
    /* 创建test线程 */
    PID_Get = rt_thread_create("PID_GET",
                    Angle_Get, 
                    RT_NULL,
                    1024, 
										11, 
                    10);
	
		PID_Send=rt_thread_create("PID_DIS",
													Usart3_Parameter_Control,
													RT_NULL,
													1024,
													10,
													10);
//    /* 创建成功则启动线程 */
    if (PID_Get!= RT_NULL)
        rt_thread_startup(PID_Get);
    if (PID_Send!= RT_NULL)
        rt_thread_startup(PID_Send);    
    return 0;
}
/* 如果设置了RT_SAMPLES_AUTORUN，则加入到初始化线程中自动运行 */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(PID_Test_Init);
#endif
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(PID_Test_Init, PID_Test_Init_Sample);
