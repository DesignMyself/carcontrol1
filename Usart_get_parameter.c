//////////////////////////////**********************************************
/*�Ӵ���2�л�õ��Է����������ݣ�PID)����,PID_Para_thread_entry(void* parameter),����ȼ�15*/
/*�ѻ�õĴ��ڲ���PID_Para[16]ת��ΪPID�е�P,I,Dֵ������ֵ��MOTOR2��PID�ṹ���б�*/
/**�������ô���3��ô����Ļ������������(��������ִ�С���������Ļ����
		EE B1 11 00 00 00 09 11  45 35 58 FF FC FF FF 
		
		����ָ���ʽ�� EE B1 11 Screen_id(00 00) Control_id(00 09) Control_type(11) Strings (45 35 58)FF FC FF FF
���ز����� Screen_id(2 ���ֽ�)��������
Control_id(2 ���ֽ�)���ؼ����
Control_type(1 ���ֽ�)���̶�ֵ 0x11����ʾΪ�ı��ؼ�
Strings(������)����ǰ��ʾ���ı�ֵ
�������������������ư�
			EE B1 A1 A1 A1 A1 A1 11 00 FF FC FF FF;//�Զ�ǰ������ʱ�����Ǵ����Զ�ģʽ�²ſ��ԣ����Զ�ģʽ�Ͽ����Ͳ����Զ�ǰ��
			EE B1 A2 A2 A1 A1 A1 11 00 FF FC FF FF;//�Զ�ǰ������ʱ�����Ǵ����Զ�ģʽ�²ſ��ԣ����Զ�ģʽ�Ͽ����Ͳ����Զ�ǰ��
			EE B1 C1 C1 C1 C1 C1 C1 C1 11 0F FF FF ;//ֹͣ����
			EE B1 C4 C4 C4 C4 C4 C4 C4 11 0F FF FF ;//ֹͣ����
			EE B1 C3 C3 C3 C3 C3 C3 C3 11 0F FF FF ;//��PID
			EE B1 C2 C2 C2 C2 C2 C2 C2 11 0F FF FF ;//�ر�PID
			EE B1 C5 C5 C5 C5 C5 C5 C5 11 0F FF FF ;//��ǰ����
			EE B1 C6 C6 C6 C6 C6 C6 C6 11 0F FF FF ;//��ǰ����
			EE B1 C7 C7 C7 C7 C7 C7 C7 11 0F FF FF ;//X�����
			EE B1 C8 C8 C8 C8 C8 C8 C8 11 0F FF FF ;//X�����
			
**/
/////////////////////////////////


#include "app_uSart.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "HDMI.H"
#include "motor1.h"
#include "PWM.H"
	static rt_thread_t Angle_Get_Data=RT_NULL;//���ȡMPC6550������
	static rt_thread_t Usart_Remote_Data=RT_NULL;//��ȡң�ط�����������
	static rt_thread_t Data_Explain=RT_NULL;//����ң�ط�����������
	extern motor car;
	extern Display D1;
	extern Display D2;
	extern motor motor2;
	extern  motor motor1;
	static rt_sem_t get_data=RT_NULL;//����һ���ź���,������ȷ�������ݺ�����ж�
	
	Usart_get getdata;
	rt_uint8_t PID_Para[16];//����2��õ�PID����
	rt_uint8_t Dis_Para[30];//����3�����Ļ������������
	rt_uint8_t Dis_Para_Copy[30];//���ƴ���3��õ�����
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
				Dis_Para[i]=uart3_getchar();//�������Ѿ������е��Ⱥ�������û����������ʱ��ͣ������һ����ʱ�䵽����ȵ���������
				if(Dis_Para[0]!=0xee)
				{
					
						break;
				}
			
				if(Dis_Para[i]==0xff&Dis_Para[i-1]==0xff)
				{
					getdata.i=i-4;//��¼������ݵĸ���
					for(b=0;b<i;b++)
					{
						getdata.data[b]=Dis_Para[b];//�������
						
					}
					rt_sem_release(get_data);//�ͷ��ź�����ʹ�����ݿ�������һ���߳���
					break;//����for(i=0;i<30;i++)��ѭ��
					
				}
				
			}			
	}
}


void Parameter_Exlpain(void *parameter)
{
	uint8_t Ascii[15];//��������Ҫ��Ϊ���ε�ASCII
	uint8_t j=0;
	while(1)
	{
		
		rt_sem_take(get_data,RT_WAITING_FOREVER);//�ȴ���ȡң�ش����������ͷŵ��ź�
		
			if(getdata.data[10]==0x0f)
			{
							if(getdata.data[4]==0xC6&&getdata.data[5]==0xC6)//�ж��Ƿ�����Ѿ�����
							{
											
										
											PWM_Story+=10;
											if(PWM_Story>=400)
											{
												
												
												PWM_Story=400;
											}
											

							}
					
							if(getdata.data[4]==0xC5&&getdata.data[5]==0xC5)//�ж��Ƿ�����Ѿ�����
							{
											
											
											PWM_Story-=10;
											
											
											if(PWM_Story<=10)
											{
												
												
												PWM_Story=10;
											}
										
							}
			}
			if(getdata.data[4]==0x01)//��һ��ҳ�淢����������
			{
					if(getdata.data[6]==13)//�õ���һ���Ƕ�
					{
							for(j=8;j<getdata.i;j++)
							{
								Ascii[j-8]=getdata.data[j];
								
							}
							car.Angle= Get_AutoRun(Ascii,getdata.i-8);//�Ѵ�ʴ�������ASCII����ת��Ϊ������;JΪASCII������1��KKΪ����
							
					}
				
				
			}
		}
		
		
	}
	
	
	

/*******�����ʼ��********/
int PID_Test_Init(void)
{
  
	get_data=rt_sem_create("get_data",1,RT_IPC_FLAG_FIFO);//��ʼ���ź���
		uart1_open("uart1");
		
				
	
			
    /* ����test�߳� */
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
/* ���������RT_SAMPLES_AUTORUN������뵽��ʼ���߳����Զ����� */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(PID_Test_Init);
#endif
/* ������ msh �����б��� */
MSH_CMD_EXPORT(PID_Test_Init, PID_Test_Init_Sample);
