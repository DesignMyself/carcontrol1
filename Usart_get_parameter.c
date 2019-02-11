//////////////////////////////**********************************************
/*�Ӵ���2�л�õ��Է����������ݣ�PID)����,PID_Para_thread_entry(void* parameter),����ȼ�15*/
/*�ѻ�õĴ��ڲ���PID_Para[16]ת��ΪPID�е�P,I,Dֵ������ֵ��MOTOR2��PID�ṹ���б�*/
/**�������ô���3��ô����Ļ������������(��������ִ�С���������Ļ����
		EE B1 11 00 00 00 09 11 00 FF FC FF FF 
		ǰ��λΪ��ʼ��֤�룬09��ʾ��Ļ�еھŸ���ͼ���̷������Ĳ�����11�����ڵ�һ��screen�з������Ĳ���������λ����֤��
		EE B1 11 00 00 00 09 11 32 33 2E 36 35 00 FF FC FF FF 
		��11��00֮�����Ϊ�������Ĳ�����ASCII
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
	
	extern PID Motor1_PID;//���1PID������ʼ��
	extern PID Motor2_PID;//���2PID������ʼ��
	extern Display D1;
	extern Display D2;
	extern motor motor2;
	extern  motor motor1;
	rt_uint8_t PID_Para[16];//����2��õ�PID����
	rt_uint8_t Dis_Para[30];//����3�����Ļ������������
	rt_uint8_t Dis_Para_Copy[30];//���ƴ���3��õ�����
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
//					PID_Para[i]=uart2_getchar();//�������Ѿ������е��Ⱥ�������û����������ʱ��ͣ������һ����ʱ�䵽����ȵ���������
//					if(PID_Para[0]!=0xAA)
//					{
//						i=0;
//						break;//һ��Ҫʹ��break����Ȼѭ���߼����������
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
			

		
				Dis_Para[i]=uart3_getchar();//�������Ѿ������е��Ⱥ�������û����������ʱ��ͣ������һ����ʱ�䵽����ȵ���������
			
				i++;
		
					if(Dis_Para[i-1]==0xff|i>30|Dis_Para[0]!=0xEE)
					{
						b=i-1;
						i=0;
						
					}
				
					if(Dis_Para[0]==0xee&&Dis_Para[5]==0xC5)//�ж��Ƿ�����Ѿ�����
					{
									
									motor1.state=1;
									motor1.pwm=100;
									motor2.pwm=200;

					}
					
					if(Dis_Para[0]==0xee&&Dis_Para[5]==0xC6)//�ж��Ƿ�����Ѿ�����
					{
									
									motor1.state=1;
									motor2.pwm=100;
									motor1.pwm=200;

					}
						if(Dis_Para[0]==0xee&&Dis_Para[5]==0xF1)//�ж��Ƿ�����Ѿ�����
					{
									
									motor1.state_record=1;

					}
					if(Dis_Para[0]==0xee&&Dis_Para[5]==0xc1)//�ж��Ƿ�����Ѿ�����
					{
									
									motor1.state_record=0;

					}
	}
}
/*******�����ʼ��********/
int PID_Test_Init(void)
{
   rt_thread_t PID_Get;//��ȡPID�������õ�����
	rt_thread_t PID_Send;//�������з����ٶ���Ϣ
		uart1_open("uart1");
		
				
	
			
    /* ����test�߳� */
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
//    /* �����ɹ��������߳� */
    if (PID_Get!= RT_NULL)
        rt_thread_startup(PID_Get);
    if (PID_Send!= RT_NULL)
        rt_thread_startup(PID_Send);    
    return 0;
}
/* ���������RT_SAMPLES_AUTORUN������뵽��ʼ���߳����Զ����� */
#if defined (RT_SAMPLES_AUTORUN) && defined(RT_USING_COMPONENTS_INIT)
    INIT_APP_EXPORT(PID_Test_Init);
#endif
/* ������ msh �����б��� */
MSH_CMD_EXPORT(PID_Test_Init, PID_Test_Init_Sample);
