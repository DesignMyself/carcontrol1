//////////////////////////////**********************************************
/*从串口2中获得电脑发过来的数据（PID)参数,PID_Para_thread_entry(void* parameter),任务等级15*/
/*把获得的串口参数PID_Para[16]转换为PID中的P,I,D值，并赋值给MOTOR2的PID结构体列表*/
/**二、利用串口3获得大彩屏幕发过来的数据(输入的数字大小），大彩屏幕规律
		EE B1 11 00 00 00 09 11  45 35 58 FF FC FF FF 
		
		返回指令格式： EE B1 11 Screen_id(00 00) Control_id(00 09) Control_type(11) Strings (45 35 58)FF FC FF FF
返回参数： Screen_id(2 个字节)：画面编号
Control_id(2 个字节)：控件编号
Control_type(1 个字节)：固定值 0x11，表示为文本控件
Strings(不定长)：当前显示的文本值
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
	static rt_thread_t Angle_Get_Data=RT_NULL;//获获取MPC6550的数据
	static rt_thread_t Usart_Remote_Data=RT_NULL;//获取遥控发过来的数据
	static rt_thread_t Data_Explain=RT_NULL;//解释遥控发过来的数据
	extern motor car;
	extern Display D1;
	extern Display D2;
	extern motor motor2;
	extern  motor motor1;
	static rt_sem_t get_data=RT_NULL;//创建一个信号量,用于正确接收数据后进行判断
	
	Usart_get getdata;
	rt_uint8_t PID_Para[16];//串口2获得的PID参数
	rt_uint8_t Dis_Para[30];//串口3获得屏幕发过来的数据
	rt_uint8_t Dis_Para_Copy[30];//复制串口3获得的数据
	Auto_Run Motor1;
	Angle X;
	Angle Y;
	Angle Z;
	uint16_t PWM_Story=100;
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


	void Usart3_Parameter_Get(void* parameter)
{    
    rt_uint8_t i;
		uint8_t b;
		uint8_t ck;
		uart3_open("uart3");	
			
		 while (1)
	{   
			
			for(i=0;i<30;i++)
		{
				Dis_Para[i]=uart3_getchar();//在里面已经包含有调度函数，当没有数据输入时会停留在这一步，时间到后调度到其他任务
				if(Dis_Para[0]!=0xee)
				{
					
						break;
				}
			
				if(Dis_Para[i]==0xff&Dis_Para[i-1]==0xff)
				{
					getdata.i=i-4;//记录获得数据的个数
					for(b=0;b<i;b++)
					{
						getdata.data[b]=Dis_Para[b];//获得数据
						
					}
					rt_sem_release(get_data);//释放信号量，使得数据可以在下一个线程中
					break;//跳出for(i=0;i<30;i++)的循环
					
				}
				
			}			
	}
}


void Parameter_Exlpain(void *parameter)
{
	uint8_t Ascii[15];//用来储存要变为整形的ASCII
	uint8_t j=0;
	while(1)
	{
		
		rt_sem_take(get_data,RT_WAITING_FOREVER);//等待获取遥控串口线束后释放的信号
		
			if(getdata.data[10]==0x0f)
			{
							if(getdata.data[4]==0xC6&&getdata.data[5]==0xC6)//判断是否接收已经结束
							{
											
										
											PWM_Story+=10;
											if(PWM_Story>=400)
											{
												
												
												PWM_Story=400;
											}
											

							}
					
							if(getdata.data[4]==0xC5&&getdata.data[5]==0xC5)//判断是否接收已经结束
							{
											
											
											PWM_Story-=10;
											
											
											if(PWM_Story<=10)
											{
												
												
												PWM_Story=10;
											}
										
							}
			}
			if(getdata.data[4]==0x01)//第一个页面发过来的数据
			{
					if(getdata.data[6]==13)//得到第一个角度
					{
							for(j=8;j<getdata.i;j++)
							{
								Ascii[j-8]=getdata.data[j];
								
							}
							car.Angle= Get_AutoRun(Ascii,getdata.i-8);//把大彩串口屏的ASCII数据转换为浮点数;J为ASCII个数加1，KK为数组
							
					}
				
				
			}
		}
		
		
	}
	
	
	

/*******任务初始化********/
int PID_Test_Init(void)
{
  
	get_data=rt_sem_create("get_data",1,RT_IPC_FLAG_FIFO);//初始化信号量
		uart1_open("uart1");
		
				
	
			
    /* 创建test线程 */
   Angle_Get_Data = rt_thread_create("Angle_Get_Data",
                    Angle_Get, 
                    RT_NULL,
                    1024, 
										21, 
                    10);
	if (Angle_Get_Data!= RT_NULL)
        rt_thread_startup(Angle_Get_Data);
	
		Usart_Remote_Data=rt_thread_create("Usart_Remote_Data",
													Usart3_Parameter_Get,
													RT_NULL,
													1024,
													20,
													10);

	if (Usart_Remote_Data!= RT_NULL)
				rt_thread_startup(Usart_Remote_Data);
	
	
		Data_Explain=rt_thread_create("Data_Explain",
													Parameter_Exlpain,
													RT_NULL,
													1024,
													19,
													10);
       
		if (Data_Explain!= RT_NULL)
						rt_thread_startup(Data_Explain);
    return 0;
}
/* 如果设置了RT_SAMPLES_AUTORUN，则加入到初始化线程中自动运行 */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(PID_Test_Init);
#endif
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(PID_Test_Init, PID_Test_Init_Sample);
